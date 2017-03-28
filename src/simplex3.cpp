/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include "numbase.h"

#ifdef QuadPrec
#include "Quad.h"
#define double Quad
#else
#define high(x) (x)
#endif

#include "defs.h"
#include "simplex4.h"
#include "simplex1.h"
#include "simplex6.h"
#include "simplex2.h"
#include "simplex3.h"

namespace ice
{
#define E_N 200
#define E_NZ 20000
#define LARGE 100000

#define EPS    1.0e-14
#define EPS1   1.0e-14
#define EPSSOL 1.0e-5   /* Zero tolerance for consistent eqns w/dep rows */
#define EPSNUM 1.0e-9
#define NOREORD 0
#define MD  1

  static void Gauss_Eta(
    int m,
    double* dx_B,
    int* idx_B,
    int* pndx_B
  );

  static void Gauss_Eta_T(
    int m,
    double* vec,
    int* ivec,
    int* pnvec
  );

  extern int*     si3_E_d;   /* Eta file; new column location */
  extern double*  si3_E;    /* Eta file - values (sparse matrix) */
  extern int*    si3_iE;    /* Eta file - row indices */
  extern int*    si3_kE;    /* Eta file - column start positions */
  extern int     si3_e_iter; /* number of iterations since last refactorization */
  extern int     si3_enz;


  extern  int     si3_rank;
  extern  int*     si3_kL, *si3_iL ,
          *si3_kLt, *si3_iLt,
          *si3_kU , *si3_iU ,
          *si3_kUt, *si3_iUt;
  extern  int*     si3_colperm, *si3_icolperm, *si3_rowperm, *si3_irowperm;
  extern  double*  si3_L, *si3_Lt, *si3_U, *si3_Ut, *si3_diagU;

  extern  double  si3_cumtime ;
  extern  double  si3_ocumtime;

  struct valind      /* nonzero entry */
  {
    double d;  /* value */
    int    i;  /* row index */
  };
  typedef struct valind VALIND;

  /*-----------------------------------------------------------------+
  | LU factorization.                                                |
  | Input:                                                           |
  |    m          number of rows (= number of columns)               |
  |    kA, iA, A  three array sparse representation of m by n        |
  |               matrix A                                           |
  |    basis      array of m out of n of the column indices of A     |
  |               specify a submatrix B of A                         |
  | Output:                                                          |
  |    static global variables (only visible to other routines in    |
  |    this file:                                                    |
  |                                                                  |
  |    rank       rank of B                                          |
  |    kL, iL, L, three array sparse representation of L             |
  |    kUt,iUt,Ut three array sparse representation of U transpose   |
  |               without its diagonal                               |
  |    diagU      diagonal entries of U                              |
  |    colperm, icolperm, rowperm, irowperm                          |
  |               column and row permutations and their inverses    */

  int refactor(
    int m,
    int* kA,
    int* iA,
    double* A,
    int* basics,
    int col_out,
    int v
  )
  {
    double starttime, endtime;
    double rffactor = 1.0;
    int    from_scratch;
    int    k;

    /*------------------------------------------------------+
    | Check if it is time to refactor from scratch         */

    if (si3_e_iter > 0)
      {
        from_scratch = true;

        for (k = si3_kE[si3_e_iter]; k < si3_kE[si3_e_iter + 1]; k++)
          {
            if (si3_iE[k] == col_out)
              {
                from_scratch = false;
                break;
              }
          }
      }
    else
      {
        from_scratch = false;
      }

    if ((si3_e_iter > 2 && si3_cumtime / (si3_e_iter + 1) >= si3_ocumtime / si3_e_iter)
        || si3_e_iter >= E_N || from_scratch == true)
      {
        /*
        if (e_iter >= 20 || from_scratch == true) {
           */
        si3_ocumtime = 0.0;
        si3_cumtime  = 0.0;
        lufac(m, kA, iA, A, basics, v);
        si3_cumtime  *= rffactor;
        si3_enz = 0;
        si3_e_iter = 0;
        from_scratch = true;
        return from_scratch;
      }

    si3_ocumtime  = si3_cumtime;
    starttime = (double) clock();

    si3_E_d[si3_e_iter] = col_out;
    si3_e_iter++;

    endtime = (double) clock();
    si3_cumtime += endtime - starttime;

    from_scratch = false;
    return from_scratch;
  }

  void lufac(int m, int* kA, int* iA, double* A, int* basis, int v)
  {
    int     kk, kkk, tag, rowdeg, coldeg, row = 0, col, row2, col2;

    int     i, j, k, cnt, lnz, unz, lnzbnd, unzbnd, okey, deg,
            heapnum, cur, method = MD;

    int*     degB = NULL, *degBt = NULL, *hkey = NULL,
             *heap = NULL, *iheap = NULL, *iwork = NULL, *iwork2 = NULL;

    VALIND  tempB, **B = NULL, **Bt = NULL;

    double  narth;

    double starttime, endtime;

    starttime = (double) clock();

    /*---------------------------------------------------------+
      | allocate space for perm and iperm.                      */

    if (si3_colperm == NULL)
      {
        MALLOC(si3_colperm,  m, int);
      }
    else
      {
        REALLOC(si3_colperm,  m, int);
      }

    if (si3_icolperm == NULL)
      {
        MALLOC(si3_icolperm, m, int);
      }
    else
      {
        REALLOC(si3_icolperm, m, int);
      }

    if (si3_rowperm == NULL)
      {
        MALLOC(si3_rowperm,  m, int);
      }
    else
      {
        REALLOC(si3_rowperm,  m, int);
      }

    if (si3_irowperm == NULL)
      {
        MALLOC(si3_irowperm, m, int);
      }
    else
      {
        REALLOC(si3_irowperm, m, int);
      }

    /*---------------------------------------------------------+
      | allocate space for work arrays.                         */

    MALLOC(degB,    m, int);
    MALLOC(degBt,   m, int);
    MALLOC(hkey,    m, int);
    MALLOC(heap,    m, int);
    MALLOC(iheap,   m, int);
    MALLOC(iwork,   m, int);
    MALLOC(iwork2,  m, int);

    heap--;         /* so that indexing starts from 1 */

    /*---------------------------------------------------------+
      | calculate degrees in B and Bt                           */

    for (i = 0; i < m; i++)
      {
        degBt[i] = 0;
      }

    for (i = 0; i < m; i++)
      {
        degB[i] = kA[ basis[i] + 1 ] - kA[ basis[i] ];

        for (k = kA[ basis[i] ]; k < kA[ basis[i] + 1 ]; k++)
          {
            degBt[ iA[k] ]++;
          }
      }

    /*---------------------------------------------------------+
      | calculate initial estimate of number of nonzeros in      |
      | L and Ut                                                */

    lnzbnd = 0;

    for (i = 0; i < m; i++) lnzbnd += degB[i];

    lnzbnd = lnzbnd / 2;
    unzbnd   = lnzbnd;

    /*---------------------------------------------------------+
      | allocate enough space to store L and Ut                  |
      | (without any fillin)                                    */

    if (si3_kL == NULL)
      {
        MALLOC(si3_kL,    m + 1,  int);
      }
    else
      {
        REALLOC(si3_kL,    m + 1,  int);
      }

    if (si3_iL == NULL)
      {
        MALLOC(si3_iL, lnzbnd,  int);
      }
    else
      {
        REALLOC(si3_iL, lnzbnd,  int);
      }

    if (si3_L == NULL)
      {
        MALLOC(si3_L, lnzbnd,  double);
      }
    else
      {
        REALLOC(si3_L, lnzbnd,  double);
      }

    if (si3_kUt == NULL)
      {
        MALLOC(si3_kUt,    m + 1,  int);
      }
    else
      {
        REALLOC(si3_kUt,    m + 1,  int);
      }

    if (si3_iUt == NULL)
      {
        MALLOC(si3_iUt, unzbnd,  int);
      }
    else
      {
        REALLOC(si3_iUt, unzbnd,  int);
      }

    if (si3_Ut == NULL)
      {
        MALLOC(si3_Ut, unzbnd,  double);
      }
    else
      {
        REALLOC(si3_Ut, unzbnd,  double);
      }

    if (si3_diagU == NULL)
      {
        MALLOC(si3_diagU,      m,  double);
      }
    else
      {
        REALLOC(si3_diagU,      m,  double);
      }

    MALLOC(B,  m, VALIND*);
    MALLOC(Bt, m, VALIND*);

    for (i = 0; i < m; i++)
      {
        B[i] = nullptr;
        Bt[i] = nullptr;
        MALLOC(B[i],  degB[i],  VALIND);
        MALLOC(Bt[i], degBt[i], VALIND);
      }

    /*---------------------------------------------------------+
      | initialize B and Bt                                     */

    for (i = 0; i < m; i++)
      {
        iwork[i] = 0;
      }

    for (j = 0; j < m; j++)
      {
        kkk = 0;

        for (k = kA[ basis[j] ]; k < kA[ basis[j] + 1 ]; k++)
          {
            row = iA[k];
            kk  = iwork[row];
            B[j][kkk].i = row;
            B[j][kkk].d = A[k];
            Bt[row][kk].i = j;
            Bt[row][kk].d = A[k];
            iwork[row]++;
            kkk++;
          }
      }

    /*---------------------------------------------------------+
      | miscellaneous initializations.                          */

    for (i = 0; i < m; i++)
      {
        si3_icolperm[i] = -1;
        si3_irowperm[i] = -1;
        iwork[i] = 0;
        iwork2[i] = -1;
      }

    si3_rank = m;
    tag = 0;
    lnz = 0;
    unz = 0;
    si3_kL[0] = 0;
    si3_kUt[0] = 0;

    /*---------------------------------------------------------+
      | hkey encodes the tie-breaking rule - currently the rule  |
      | is somewhat random.  to make it first occuring minimum,  |
      | change the formula to:                                   |
      |       hkey[node] = degree[node]*m + node;                |
      | warning: with this definition of hkey, there is the      |
      | possibility of integer overflow on moderately large      |
      | problems.                                                |
      |                                                         */

    for (j = 0; j < m; j++)
      {
        if (method == MD) hkey[j] = degB[j];
        else              hkey[j] = j;

        if (hkey[j] == 0) hkey[j] = m + 1;
      }

    /*---------------------------------------------------------+
      | set up heap structure for quickly accessing minimum.    */

    heapnum = m;

    for (j = m - 1; j >= 0; j--)
      {
        cur = j + 1;
        iheap[j] = cur;
        heap[cur] = j;
        hfall(heapnum, hkey, iheap, heap, cur);
      }

    /*---------------------------------------------------------+
      | the min degree ordering loop                            */

    for (i = 0; i < m; i++)
      {

        /*------------------------------------------------+
          |  select column with min column degree          */

again:
        col    = heap[1];
        coldeg = degB[col];

        if (coldeg == 0)
          {
#ifdef DEBUG
            printf("singular matrix. rank deficiency = %d\n", m - i);
#endif
            si3_rank = i;
            goto end;
          }

        /*------------------------------------------------+
          |  select pivot element from this column by       |
          |  choosing nonzero whose row is of minimal       |
          |  degree                                        */

        rowdeg = m + 1;

        for (k = 0; k < coldeg; k++)
          {
            if (degBt[ B[col][k].i ] < rowdeg
                && Abs(B[col][k].d) > EPSNUM)
              {
                row    = B[col][k].i;
                rowdeg = degBt[row];
              }
          }

        if (rowdeg == m + 1)
          {
            hkey[col] = m + 2;
            hfall(heapnum, hkey, iheap, heap, iheap[col]);

            if (hkey[heap[1]] == m + 2)
              {
#ifdef DEBUG
                printf("singular matrix. rank deficiency = %d\n", m - i);
#endif
                si3_rank = i;
                goto end;
              }
            else
              {
                goto again;
              }
          }

        /*------------------------------------------------+
          |  update permutation information                */

        si3_colperm[i] = col;
        si3_icolperm[col] = i;

        si3_rowperm[i] = row;
        si3_irowperm[row] = i;

        /*------------------------------------------------+
          |  reallocate space for L, iL, Ut, and iUt as     |
          |        necessary.                               |
          |                                                 |
          |  lnz stores the number of nonzeros in L so far  |
          |  lnzbnd is an estimate of how many will be in L |
          |  unz stores the number of nonzeros in U so far  |
          |  unzbnd is an estimate of how many will be in U*/

        cnt = lnz + coldeg - 1 + coldeg * rowdeg / 2;

        if (cnt > lnzbnd)
          {
            lnzbnd = cnt;
            REALLOC(si3_L, lnzbnd, double);
            REALLOC(si3_iL, lnzbnd, int);
          }

        cnt = unz + rowdeg - 1 + coldeg * rowdeg / 2;

        if (cnt > unzbnd)
          {
            unzbnd = cnt;
            REALLOC(si3_Ut, unzbnd, double);
            REALLOC(si3_iUt, unzbnd, int);
          }

        /*------------------------------------------------+
          |  copy pivot column into L and pivot row into    |
          |  Ut.                                           */

        si3_kL[i + 1] = si3_kL[i] + coldeg - 1;

        for (k = 0; k < coldeg; k++)
          {
            if (B[col][k].i != row)
              {
                si3_iL[lnz] = B[col][k].i;
                si3_L[lnz] = B[col][k].d;
                lnz++;
              }
          }

        si3_kUt[i + 1] = si3_kUt[i] + rowdeg - 1;

        for (k = 0; k < rowdeg; k++)
          {
            if (Bt[row][k].i != col)
              {
                si3_iUt[unz] = Bt[row][k].i;
                si3_Ut[unz] = Bt[row][k].d;
                unz++;
              }
            else
              {
                si3_diagU[i] = Bt[row][k].d;
              }
          }

        /*------------------------------------------------+
          |  remove eliminated elements from B and Bt      */

        for (k = 0; k < coldeg; k++)
          {
            row2 = B[col][k].i;
            degBt[row2]--;

            for (kk = 0; Bt[row2][kk].i != col; kk++) ;

            tempB = Bt[row2][ degBt[row2] ];
            Bt[row2][ degBt[row2] ] = Bt[row2][kk];
            Bt[row2][kk] = tempB;
          }

        for (k = 0; k < rowdeg; k++)
          {
            col2 = Bt[row][k].i;
            degB[col2]--;

            for (kk = 0; B[col2][kk].i != row; kk++) ;

            tempB = B[col2][ degB[col2] ];
            B[col2][ degB[col2] ] = B[col2][kk];
            B[col2][kk] = tempB;
          }

        degB[col] = 0;
        degBt[row] = 0;

        /*------------------------------------------------+
          |  update heap                                   */

        okey = hkey[col];
        heap[1] = heap[heapnum];
        iheap[heap[1]] = 1;
        heapnum--;

        if (okey < hkey[heap[1]])
          hfall(heapnum, hkey, iheap, heap, 1);

        /*------------------------------------------------+
          |  generate fillin and update elements           */

        for (k = si3_kL[i]; k < si3_kL[i + 1]; k++)
          {
            row2 = si3_iL[k];
            tag++;

            for (kk = 0; kk < degBt[row2]; kk++)
              {
                iwork[ Bt[row2][kk].i] = tag; /* tag these columns */
                iwork2[Bt[row2][kk].i] = kk;  /* say where they are */
              }

            for (kk = si3_kUt[i]; kk < si3_kUt[i + 1]; kk++)
              {
                col2 = si3_iUt[kk];

                if (iwork[col2] == tag)
                  {
                    Bt[row2][iwork2[col2]].d -= si3_L[k] * si3_Ut[kk] / si3_diagU[i];
                  }
                else
                  {
                    deg = degBt[row2];
                    REALLOC(Bt[row2], deg + 1, VALIND);
                    Bt[row2][deg].i = col2;
                    Bt[row2][deg].d = -si3_L[k] * si3_Ut[kk] / si3_diagU[i];
                    degBt[row2]++;
                  }
              }
          }

        for (k = si3_kUt[i]; k < si3_kUt[i + 1]; k++)
          {
            col2 = si3_iUt[k];
            tag++;

            for (kk = 0; kk < degB[col2]; kk++)
              {
                iwork[ B[col2][kk].i] = tag; /* tag these rows */
                iwork2[B[col2][kk].i] = kk;  /* say where they are */
              }

            for (kk = si3_kL[i]; kk < si3_kL[i + 1]; kk++)
              {
                row2 = si3_iL[kk];

                if (iwork[row2] == tag)
                  {
                    B[col2][iwork2[row2]].d -= si3_L[kk] * si3_Ut[k] / si3_diagU[i];
                  }
                else
                  {
                    deg = degB[col2];
                    REALLOC(B[col2], deg + 1, VALIND);
                    B[col2][deg].i = row2;
                    B[col2][deg].d = -si3_L[kk] * si3_Ut[k] / si3_diagU[i];
                    degB[col2]++;
                  }
              }
          }

        /*------------------------------------------------+
          |  adjust heap                                   */

        for (k = si3_kUt[i]; k < si3_kUt[i + 1]; k++)
          {
            col2 = si3_iUt[k];

            if (method == MD)
              {
                hkey[col2] = degB[col2];
              }
            else
              {
                hkey[col2] = col2;
              }

            if (hkey[col2] == 0) hkey[col2] = m + 1;

            hrise(hkey, iheap, heap, iheap[col2]);
            hfall(heapnum, hkey, iheap, heap, iheap[col2]);
          }

        /*------------------------------------------------+
          |  free space no longer needed                   */

        /*
          FREE(B[col]);  FREE(Bt[row]);
        */
      }

end:
    /*------------------------------------------------+
      |  process dependent rows/cols                   */

    i = si3_rank;

    for (col = 0; col < m; col++)
      {
        if (si3_icolperm[col] == -1)
          {
            si3_colperm[i] = col;
            si3_icolperm[col] = i;
            i++;
          }
      }

    i = si3_rank;

    for (row = 0; row < m; row++)
      {
        if (si3_irowperm[row] == -1)
          {
            si3_rowperm[i] = row;
            si3_irowperm[row] = i;
            i++;
          }
      }

    for (i = si3_rank; i < m; i++)
      {
        si3_kL[i + 1] = si3_kL[i];
        si3_kUt[i + 1] = si3_kUt[i];
        si3_diagU[i] = 0.0;
      }

    /*------------------------------------------------+
      |  free up space                                 */

    heap++;

    for (i = 0; i < m; i++)
      {
        FREE(B[i]);
        FREE(Bt[i]);
      }

    FREE(degB);
    FREE(degBt);
    FREE(hkey);
    FREE(heap);
    FREE(iheap);
    FREE(iwork);
    FREE(iwork2);
    FREE(B);
    FREE(Bt);

    /*------------------------------------------------+
      |  update "i" arrays to new indices              */

    for (k = 0; k < si3_kL[m]; k++) si3_iL[k] = si3_irowperm[si3_iL[k]];

    for (k = 0; k < si3_kUt[m]; k++) si3_iUt[k] = si3_icolperm[si3_iUt[k]];

    /*------------------------------------------------+
      |  divide each column of L by diagonal           */

    for (i = 0; i < m; i++)
      {
        for (k = si3_kL[i]; k < si3_kL[i + 1]; k++)
          {
            si3_L[k] /= si3_diagU[i];
          }
      }

    /*---------------------------------------------------------+
      | calculate and print statistics.                         */

    narth = 0.0e0;

    for (i = 0; i < m; i++)
      {
        k = si3_kL[i + 1] - si3_kL[i];
        narth += (double) k * k;
        k = si3_kUt[i + 1] - si3_kUt[i];
        narth += (double) k * k;
      }

    narth = narth + 3 * si3_kL[m] + 3 * si3_kUt[m] + 2 * m;

    lnz    = si3_kL[m];
    unz    = si3_kUt[m];
#ifdef DEBUG

    if (v)
      {
        printf("%9d   %9d %15.0f ", lnz, unz, narth);
        fflush(stdout);
      }

#endif

    if (si3_Lt == nullptr)
      {
        MALLOC(si3_Lt, lnz, double);
      }
    else
      {
        REALLOC(si3_Lt, lnz, double);
      }

    if (si3_iLt == nullptr)
      {
        MALLOC(si3_iLt, lnz, int);
      }
    else
      {
        REALLOC(si3_iLt, lnz, int);
      }

    if (si3_kLt == nullptr)
      {
        MALLOC(si3_kLt, m + 1, int);
      }
    else
      {
        REALLOC(si3_kLt, m + 1, int);
      }

    if (si3_U == nullptr)
      {
        MALLOC(si3_U,  unz, double);
      }
    else
      {
        REALLOC(si3_U,  unz, double);
      }

    if (si3_iU == nullptr)
      {
        MALLOC(si3_iU,  unz, int);
      }
    else
      {
        REALLOC(si3_iU,  unz, int);
      }

    if (si3_kU == nullptr)
      {
        MALLOC(si3_kU,  m + 1, int);
      }
    else
      {
        REALLOC(si3_kU,  m + 1, int);
      }

    atnum(m, m, si3_kL, si3_iL, si3_L, si3_kLt, si3_iLt, si3_Lt);
    atnum(m, m, si3_kUt, si3_iUt, si3_Ut, si3_kU, si3_iU, si3_U);

    if (si3_E_d == nullptr)
      {
        MALLOC(si3_E_d, E_N, int);

        MALLOC(si3_E, E_NZ, double);
        MALLOC(si3_iE, E_NZ,    int);
        MALLOC(si3_kE, E_N + 1,    int);
      }

    si3_kE[0] = 0;

    endtime = (double) clock();
    si3_cumtime += endtime - starttime;
  }

  /*-----------------------------------------------------------------+
  | Forward/backward solve using LU factorization                    |
  | Input:                                                           |
  |    m          dimension of array y                               |
  |    y          array containing right-hand side                   |
  |                                                                  |
  |    static global variables (assumed setup by lufac()):           |
  |                                                                  |
  |    rank       rank of B                                          |
  |    kL, iL, L, three array sparse representation of L             |
  |    kUt,iUt,Ut three array sparse representation of U transpose   |
  |               without its diagonal                               |
  |    diagU      diagonal entries of U                              |
  |    colperm, icolperm, rowperm, irowperm                          |
  |               column and row permutations and their inverses     |
  | Output:                                                          |
  |                                          -1                      |
  |    y          array containing solution B  y                     |
  |                                                                  |
  |    integer flag indicating whether system is consistent         */

  int     bsolve(
    int m,
    double* sy,
    int* iy,
    int* pny
  )
  {
    int i, ny = *pny;
    int k, row, consistent = true;
    double beta;
    double eps = 0.0;

    extern double* bsolve_y;
    extern int*  bsolve_tag;
    extern int  bsolve_currtag;

    double starttime, endtime;

    starttime = (double) clock();

    if (bsolve_y  == nullptr) CALLOC(bsolve_y, m,   double);

    if (bsolve_tag  == nullptr) CALLOC(bsolve_tag, m,   int);

    for (k = 0; k < ny; k++)
      {
        i = si3_irowperm[iy[k]];
        bsolve_y[i] = sy[k];
        bsolve_tag[i] = bsolve_currtag;
        addtree(i);
      }

    if (si3_rank < m) eps = EPSSOL * maxv(sy, ny);

    /*------------------------------------------------------+
    |               -1                                      |
    |       y  <-  L  y                                    */

    for (i = getfirst(); i < si3_rank && i != -1; i = getnext())
      {
        beta = bsolve_y[i];

        for (k = si3_kL[i]; k < si3_kL[i + 1]; k++)
          {
            row = si3_iL[k];

            if (bsolve_tag[row] != bsolve_currtag)
              {
                bsolve_y[row] = 0.0;
                bsolve_tag[row] = bsolve_currtag;
                addtree(row);
              }

            bsolve_y[row] -= si3_L[k] * beta;
          }
      }

    /*------------------------------------------------------+
    |               -1                                      |
    |       y  <-  U  y                                    */

    for (i = getlast(); i >= si3_rank && i != -1; i = getprev())
      {
        if (Abs(bsolve_y[i]) > eps) consistent = false;

        bsolve_y[i] = 0.0;
      }

    for (; i >= 0; i = getprev())
      {
        beta = bsolve_y[i] / si3_diagU[i];

        for (k = si3_kU[i]; k < si3_kU[i + 1]; k++)
          {
            row = si3_iU[k];

            if (bsolve_tag[row] != bsolve_currtag)
              {
                bsolve_y[row] = 0.0;
                bsolve_tag[row] = bsolve_currtag;
                addtree(row);
              }

            bsolve_y[row] -= si3_U[k] * beta;
          }

        bsolve_y[i] = beta;
      }

    ny = 0;

    for (i = getfirst(); i != -1; i = getnext())
      {
        if (Abs(bsolve_y[i]) > EPS)
          {
            sy[ny] = bsolve_y[i];
            iy[ny] = si3_colperm[i];
            ny++;
          }
      }

    bsolve_currtag++;
    killtree();

    Gauss_Eta(m, sy, iy, &ny);

    *pny = ny;

    /*************************************************************
    * Update E and save col_out in E_d[e_iter]                   *
    *************************************************************/

    REALLOC(si3_E, Max(E_NZ, si3_enz + ny), double);
    REALLOC(si3_iE, Max(E_NZ, si3_enz + ny),    int);

    for (i = 0, k = si3_kE[si3_e_iter]; i < ny; i++, k++)
      {
        si3_E[k] = sy[i];
        si3_iE[k] = iy[i];
      }

    si3_enz = k;
    si3_kE[si3_e_iter + 1] = si3_enz;

    endtime = (double) clock();
    si3_cumtime += endtime - starttime;

    return consistent;
  }

  /*-----------------------------------------------------------------+
  | Forward/backward solve using LU factorization                    |
  | Input:                                                           |
  |    m          dimension of array y                               |
  |    y          array containing right-hand side                   |
  |                                                                  |
  |    static global variables (assumed setup by lufac()):           |
  |                                                                  |
  |    rank       rank of B                                          |
  |    kL, iL, L, three array sparse representation of L             |
  |    kUt,iUt,Ut three array sparse representation of U transpose   |
  |               without its diagonal                               |
  |    diagU      diagonal entries of U                              |
  |    colperm, icolperm, rowperm, irowperm                          |
  |               column and row permutations and their inverses     |
  | Output:                                                          |
  |                                          -T                      |
  |    y          array containing solution B  y                     |
  |                                                                  |
  |    integer flag indicating whether system is consistent         */

  int     btsolve(
    int m,
    double* sy,
    int* iy,
    int* pny
  )
  {
    int i, ny = *pny;
    int k, row, consistent = true;
    double beta;
    double eps = 0.0;

    extern double* btsolve_y;
    extern int*    btsolve_tag;
    extern int    btsolve_currtag;

    double starttime, endtime;

    starttime = (double) clock();

    if (btsolve_y == nullptr) CALLOC(btsolve_y, m, double);

    if (btsolve_tag == nullptr) CALLOC(btsolve_tag, m, int);




    Gauss_Eta_T(m, sy, iy, &ny);




    for (k = 0; k < ny; k++)
      {
        i = si3_icolperm[iy[k]];
        btsolve_y[i] = sy[k];
        btsolve_tag[i] = btsolve_currtag;
        addtree(i);
      }

    if (si3_rank < m) eps = EPSSOL * maxv(sy, ny);

    /*------------------------------------------------------+
    |               -T                                      |
    |       y  <-  U  y                                    */

    for (i = getfirst(); i < si3_rank && i != -1; i = getnext())
      {
        beta = btsolve_y[i] / si3_diagU[i];

        for (k = si3_kUt[i]; k < si3_kUt[i + 1]; k++)
          {
            row = si3_iUt[k];

            if (btsolve_tag[row] != btsolve_currtag)
              {
                btsolve_y[row] = 0.0;
                btsolve_tag[row] = btsolve_currtag;
                addtree(row);
              }

            btsolve_y[row] -= si3_Ut[k] * beta;
          }

        btsolve_y[i] = beta;
      }

    for (i = getlast(); i >= si3_rank && i != -1; i = getprev())
      {
        if (Abs(btsolve_y[i]) > eps) consistent = false;

        btsolve_y[i] = 0.0;
      }

    /*------------------------------------------------------+
    |               -T                                      |
    |       y  <-  L  y                                    */

    for (; i >= 0; i = getprev())
      {
        beta = btsolve_y[i];

        for (k = si3_kLt[i]; k < si3_kLt[i + 1]; k++)
          {
            row = si3_iLt[k];

            if (btsolve_tag[row] != btsolve_currtag)
              {
                btsolve_y[row] = 0.0;
                btsolve_tag[row] = btsolve_currtag;
                addtree(row);
              }

            btsolve_y[row] -= si3_Lt[k] * beta;
          }
      }

    ny = 0;

    for (i = getfirst(); i != -1; i = getnext())
      {
        if (Abs(btsolve_y[i]) > EPS)
          {
            sy[ny] = btsolve_y[i];
            iy[ny] = si3_rowperm[i];
            ny++;
          }
      }

    *pny = ny;

    btsolve_currtag++;
    killtree();

    endtime = (double) clock();
    si3_cumtime += endtime - starttime;

    return consistent;

  }

  void lu_clo()
  {
    FREE(si3_rowperm);
    FREE(si3_irowperm);
    FREE(si3_colperm);
    FREE(si3_icolperm);
    FREE(si3_L);
    FREE(si3_iL);
    FREE(si3_kL);
    FREE(si3_Ut);
    FREE(si3_iUt);
    FREE(si3_kUt);
    FREE(si3_diagU);
    FREE(si3_E_d);
    FREE(si3_E);
    FREE(si3_iE);
    FREE(si3_kE);
  }

  /*-----------------------------------------------------------------+
  | Forward/backward solve using LU factorization                    |
  | Input:                                                           |
  |    m          dimension of array y                               |
  |    y          array containing right-hand side                   |
  |                                                                  |
  |    static global variables (assumed setup by lufac()):           |
  |                                                                  |
  |    rank       rank of B                                          |
  |    kL, iL, L, three array sparse representation of L             |
  |    kUt,iUt,Ut three array sparse representation of U transpose   |
  |               without its diagonal                               |
  |    diagU      diagonal entries of U                              |
  |    colperm, icolperm, rowperm, irowperm                          |
  |               column and row permutations and their inverses     |
  | Output:                                                          |
  |                                          -1                      |
  |    y          array containing solution B  y                     |
  |                                                                  |
  |    integer flag indicating whether system is consistent         */

  int     dbsolve(int m, double* y)
  {
    int i;
    int k, row, consistent = true;
    double beta, *dwork;
    double eps = 0.0;

    double starttime, endtime;

    starttime = (double) clock();

    MALLOC(dwork, m, double);

    if (si3_rank < m) eps = EPSSOL * maxv(y, m);

    for (i = 0; i < m; i++) dwork[i] = y[i];

    for (i = 0; i < m; i++) y[si3_irowperm[i]] = dwork[i];

    /*------------------------------------------------------+
    |               -1                                      |
    |       y  <-  L  y                                    */

    for (i = 0; i < si3_rank; i++)
      {
        beta = y[i];

        for (k = si3_kL[i]; k < si3_kL[i + 1]; k++)
          {
            row = si3_iL[k];
            y[row] -= si3_L[k] * beta;
          }
      }

    /*------------------------------------------------------+
    |               -1                                      |
    |       y  <-  U  y                                    */

    for (i = m - 1; i >= si3_rank; i--)
      {
        if (Abs(y[i]) > eps) consistent = false;

        y[i] = 0.0;
      }

    for (i = si3_rank - 1; i >= 0; i--)
      {
        beta = y[i];

        for (k = si3_kUt[i]; k < si3_kUt[i + 1]; k++)
          {
            beta -= si3_Ut[k] * y[si3_iUt[k]];
          }

        y[i] = beta / si3_diagU[i];
      }

    for (i = 0; i < m; i++) dwork[i] = y[i];

    for (i = 0; i < m; i++) y[si3_colperm[i]] = dwork[i];

    FREE(dwork);

    endtime = (double) clock();
    si3_cumtime += endtime - starttime;

    return consistent;
  }

  /*****************************************************************
  *  Gaussian elimination for Eta transformations which will solve *
  * each system E ... E  d = a for d.        *
  *              1     s                                           *
  *****************************************************************/

  static void Gauss_Eta(
    int m,
    double* dx_B,
    int* idx_B,
    int* pndx_B
  )
  {
    int i, j, k, col, kcol = 0, ii, ndx_B = *pndx_B;
    double temp;
    extern double* Gauss_Eta_a;
    extern int*  Gauss_Eta_tag;
    extern int* Gauss_Eta_link;
    extern int  Gauss_Eta_currtag;

    if (Gauss_Eta_a  == nullptr) CALLOC(Gauss_Eta_a,  m,   double);

    if (Gauss_Eta_tag == nullptr) CALLOC(Gauss_Eta_tag, m,   int);

    if (Gauss_Eta_link == nullptr)
      {
        CALLOC(Gauss_Eta_link, m + 2, int);
        Gauss_Eta_link++;
      }

    if (si3_e_iter <= 0) return;

    ii = -1;

    for (k = 0; k < ndx_B; k++)
      {
        i = idx_B[k];
        Gauss_Eta_a[i] = dx_B[k];
        Gauss_Eta_tag[i] = Gauss_Eta_currtag;
        Gauss_Eta_link[ii] = i;
        ii = i;
      }

    for (j = 0; j < si3_e_iter; j++)
      {
        col = si3_E_d[j];

        for (k = si3_kE[j]; k < si3_kE[j + 1]; k++)
          {
            i = si3_iE[k];

            if (Gauss_Eta_tag[i] != Gauss_Eta_currtag)
              {
                Gauss_Eta_a[i] = 0.0;
                Gauss_Eta_tag[i] = Gauss_Eta_currtag;
                Gauss_Eta_link[ii] = i;
                ii = i;
              }

            if (i == col) kcol = k;
          }

        temp = Gauss_Eta_a[col] / si3_E[kcol];

        if (temp != 0.0)
          {
            for (k = si3_kE[j]; k < kcol; k++)
              {
                i = si3_iE[k];
                Gauss_Eta_a[i] -= si3_E[k] * temp;
              }

            Gauss_Eta_a[col] = temp;

            for (k = kcol + 1; k < si3_kE[j + 1]; k++)
              {
                i = si3_iE[k];
                Gauss_Eta_a[i] -= si3_E[k] * temp;
              }
          }
      }

    Gauss_Eta_link[ii] = m;
    Gauss_Eta_currtag++;

    k = 0;

    for (i = Gauss_Eta_link[-1]; i < m; i = Gauss_Eta_link[i])
      {
        if (Abs(Gauss_Eta_a[i]) > EPS1)
          {
            dx_B[k] = Gauss_Eta_a[i];
            idx_B[k] = i;
            k++;
          }
      }

    *pndx_B = k;
  }


  /*****************************************************************
  *  Gaussian elimination for Eta transformations which will solve *
  * each system B y = c for y                                *
  *****************************************************************/

  static void Gauss_Eta_T(
    int m,
    double* vec,
    int* ivec,
    int* pnvec
  )
  {
    int i, j, k, kk = 0, kkk = 0, col, nvec = *pnvec;
    double temp;
    extern double* Gauss_Eta_T_a;
    extern int*  Gauss_Eta_T_tag;
    extern int  Gauss_Eta_T_currtag;

    if (Gauss_Eta_T_a   == nullptr) CALLOC(Gauss_Eta_T_a,   m,   double);

    if (Gauss_Eta_T_tag == nullptr) CALLOC(Gauss_Eta_T_tag, m,   int);

    for (j = si3_e_iter - 1; j >= 0; j--)
      {
        col = si3_E_d[j];

        for (k = 0; k < nvec; k++)
          {
            i = ivec[k];

            if (i == col) kk = k;

            Gauss_Eta_T_a[i] = vec[k];
            Gauss_Eta_T_tag[i] = Gauss_Eta_T_currtag;
          }

        if (Gauss_Eta_T_tag[col] != Gauss_Eta_T_currtag)
          {
            vec[nvec] = 0.0;
            ivec[nvec] = col;
            kk = nvec;
            nvec++;
            Gauss_Eta_T_a[col] = 0.0;
            Gauss_Eta_T_tag[col] = Gauss_Eta_T_currtag;
          }

        temp = vec[kk];

        for (k = si3_kE[j]; k < si3_kE[j + 1]; k++)
          {
            i = si3_iE[k];

            if (i == col) kkk = k;

            if (Gauss_Eta_T_tag[i] == Gauss_Eta_T_currtag)
              {
                if (i != col)
                  {
                    temp -= si3_E[k] * Gauss_Eta_T_a[i];
                  }
              }
          }

        Gauss_Eta_T_currtag++;

        vec[kk] = temp / si3_E[kkk];
        *pnvec = nvec;
      }
  }
}
