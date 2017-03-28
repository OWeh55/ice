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
// **************************************************************************
// Simplexmethode
//
//       T
//  max c x
//
//  A x  <= b
//    x >= 0
//
//     A ist eine m mal n Matrix
//     Die Schlupfvariablen werden im Programm selbst erzeugt
//
//****************************************************************************/

//#define DEBUG

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

#include "numbase.h"

#ifdef QuadPrec
#include "Quad.h"
#define double Quad
#else
#define high(x) (x)
#endif


#include "simplex2.h"
#include "simplex3.h"
#include "simplex4.h"

namespace ice
{
#define EPS  1.0e-8
#define EPS1 1.0e-8
#define EPS2 1.0e-12
#define EPS3 1.0e-12
#define MAX_ITER 1000000

  double sdotprod(double* c, double* x_B, int* basics, int m);

  void Nt_times_y(
    int N,
    double* at,
    int* iat,
    int* kat,
    int* basicflag,
    double* y,
    int* iy,
    int ny,
    double* yN,
    int* iyN,
    int* pnyN
  );

  int ratio_test(
    double* dy,
    int*   idy,
    int    ndy,
    double* y
  );

  int pick_neg(int m, double* x);

  int simplex_method(
    int m,    /* number of constraints */
    int n,    /* number of variables */
    int nz,   /* number of nonzeros in sparse constraint matrix */
    int* ia,    /* array row indices */
    int* ka,    /* array of indices into ia and a */
    double* a,    /* array of nonzeros in the constraint matrix */
    double* b,    /* right-hand side */
    double* c,          /* objective coefficients */
    double  f,    /* objective function shift */
    double* x,    /* primal solution (output) */
    double* y,    /* dual solution (output) */
    double* w,    /* primal slacks (output) */
    double* z,    /* dual slacks (output) */
    double* zf          /* Optimal value of the objective function */
  )
  {
    double*  x_B; /* primal basics */
    double*  y_N; /* dual nonbasics */

    double*  dy_N;  /*  dual  basics step direction - values (sparse) */
    int*    idy_N;  /*  dual  basics step direction - row indices */
    int     ndy_N;  /* number of nonz in dy_N */

    double*  dx_B;  /* primal basics step direction - values (sparse) */
    int*    idx_B;  /* primal basics step direction - row indices */
    int     ndx_B;  /* number of nonz in dx_B */

    double* at;         /* sparse data structure for A^T */
    int*    iat;
    int*    kat;

    int*     basics;  /* list of basic variable indices */
    int*     nonbasics; /* list of non-basic variable indices */
    int*     basicflag;   /* neg if nonbasic, pos if basic */

    int     col_in; /* entering column; index in 'nonbasics' */
    int     col_out;  /* leaving column; index in 'basics' */

    int     iter = 0; /* number of iterations */
    int     i, j, k, N;

    double  s, t;
    float   primal_obj;

    double*  vec;
    int*    ivec;
    int     nvec;


    /*******************************************************************
     * For convenience, we put...
     *******************************************************************/

    N = n + m;

    /*******************************************************************
     * Add slack variables.  We assume the calling routine allocated
     * enough memory.
     *******************************************************************/

    i = 0;
    k = ka[n];

    for (j = n; j < N; j++)
      {
        c[j]  = 0.0;
        a[k] = 1.0;
        ia[k] = i;
        i++;
        k++;
        ka[j + 1] = k;
      }

    nz = k;

    /*******************************************************************
     * Read in the Data and initialize the common memory sites.
     *******************************************************************/

    MALLOC(x_B, m, double);
    MALLOC(dx_B, m, double);
    MALLOC(y_N, n, double);
    MALLOC(dy_N, n, double);
    MALLOC(vec, m, double);
    MALLOC(nonbasics, n, int);
    MALLOC(basics, m, int);
    MALLOC(basicflag, N, int);
    MALLOC(idx_B, m,    int);
    MALLOC(idy_N, n,    int);
    MALLOC(ivec, m,    int);
    MALLOC(at, nz,  double);
    MALLOC(iat, nz,   int);
    MALLOC(kat, m + 1,  int);

    /****************************************************************
     *  Initialization.                         *
     ****************************************************************/

    atnum(m, N, ka, ia, a, kat, iat, at);

    for (j = 0; j < n; j++)
      {
        nonbasics[j] = j;
        basicflag[j] = -j - 1;
        y_N[j] = Max(c[j], 1.0); /* to force dual feasibility */
        y_N[j] += RandomD();     /* to ensure nondegeneracy   */
      }

    for (i = 0; i < m; i++)
      {
        basics[i] = n + i;
        basicflag[n + i] = i;
        x_B[i] = b[i];
      }

    lufac(m, ka, ia, a, basics, 0);


    dbsolve(m, x_B);   /* could be done explicitly in terms of bounds/ranges */


    /****************************************************************
     *  Begin Phase I (i.e., dual simplex method)
     ****************************************************************/
#ifdef DEBUG

    printf("m = %d,n = %d,nz = %d\n", m, N, nz);
    printf(
      "---------------------------------------------------------------------------\n"
      "          |   Primal      |        |                           arithmetic  \n"
      "  Iter    |  Obj Value    |   mu   |   nonz(L)     nonz(U)     operations  \n"
      "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n"
    );
    fflush(stdout);
#endif

    /****************************************************************
     *  Main loop                                                   *
     ****************************************************************/

    for (iter = 0; iter < MAX_ITER; iter++)
      {

        /*************************************************************
         * STEP 6: Print statistics and factor/refactor               *
         *************************************************************/

        primal_obj = sdotprod(c, x_B, basics, m) + f;

#ifdef DEBUG
        printf("%8d   %14.7e     NA    ", iter, high(primal_obj));
        fflush(stdout);
#endif

        /*************************************************************
         * STEP 1: Pick most negative basic primal                    *
         *************************************************************/

        col_out = pick_neg(m, x_B);

        if (col_out == -1) break; /* ready for Phase II */

        /*************************************************************
         *                          -1  T                             *
         * STEP 2: Compute dy  = -(B  N) e                            *
         *                   N            i          *
         *         where i = col_out                                  *
         *************************************************************/

        vec[0] = -1.0;
        ivec[0] = col_out;
        nvec = 1;

        btsolve(m, vec, ivec, &nvec);

        Nt_times_y(N, at, iat, kat, basicflag, vec, ivec, nvec,
                   dy_N, idy_N, &ndy_N);

        /*************************************************************
         * STEP 3: Ratio test to find entering column                 *
         *************************************************************/

        col_in = ratio_test(dy_N, idy_N, ndy_N, y_N);

        if (col_in == -1)
          {

            FREE(vec);
            FREE(ivec);
            FREE(x_B);
            FREE(y_N);
            FREE(dx_B);
            FREE(idx_B);
            FREE(dy_N);
            FREE(idy_N);
            FREE(nonbasics);
            FREE(basics);
            FREE(basicflag);
            FREE(at);
            FREE(iat);
            FREE(kat);
            return 2;   /* INFEASIBLE */
          }

        /*************************************************************
         *                        -1                                  *
         * STEP 4: Compute dx  = B  N e                               *
         *                   B         j                              *
         *                                                            *
         *************************************************************/

        j = nonbasics[col_in];

        for (i = 0, k = ka[j]; k < ka[j + 1]; i++, k++)
          {
            dx_B[i] =  a[k];
            idx_B[i] = ia[k];
          }

        ndx_B = i;

        bsolve(m, dx_B, idx_B, &ndx_B);

        /*************************************************************
         *                                                            *
         * STEP 5: Put       t = x /dx                                *
         *                        i   i                               *
         *                   s = y /dy                                *
         *                        j   j                               *
         *************************************************************/

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndx_B; k++) if (idx_B[k] == col_out) break;

        t = x_B[col_out] / dx_B[k];

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndy_N; k++) if (idy_N[k] == col_in) break;

        s = y_N[col_in] / dy_N[k];

        /*************************************************************
         * STEP 7: Set y  = y  - s dy                                 *
         *              N    N       N                                *
         *                                                            *
         *             y  = s                                         *
         *              i                                             *
         *                                                            *
         *             x  = x  - t dx                                 *
         *              B    B       B                                *
         *                                                            *
         *             x  = t                                         *
         *              j                                             *
         *************************************************************/

        for (k = 0; k < ndy_N; k++)
          {
            j = idy_N[k];
            y_N[j] -= s * dy_N[k];
          }

        y_N[col_in] = s;

        for (k = 0; k < ndx_B; k++)
          {
            i = idx_B[k];
            x_B[i] -= t * dx_B[k];
          }

        x_B[col_out] = t;

        /*************************************************************
         * STEP 8: Update basis                                       *
         *************************************************************/

        i =    basics[col_out];
        j = nonbasics[col_in];
        basics[col_out]   = j;
        nonbasics[col_in] = i;
        basicflag[i] = -col_in - 1;
        basicflag[j] = col_out;

#ifdef DEBUG
        int     from_scratch = refactor(m, ka, ia, a, basics, col_out, v);

        if (from_scratch)
          {
            printf("\n");
          }
        else
          {
            printf("%c", cr);
          }
#endif

      } /* End of Phase I */

#ifdef DEBUG

    printf("End of Phase I \n");

#endif

    /****************************************************************
     *   Restore objective function by setting                       *
     *                -1  T                                          *
     *         y  = (B  N) c  - c                                    *
     *          N           B    N                                   *
     ****************************************************************/

    nvec = 0;

    for (i = 0; i < m; i++)
      {
        if (Abs(c[basics[i]]) > EPS)
          {
            vec[nvec] = c[basics[i]];
            ivec[nvec] = i;
            nvec++;
          }
      }

    btsolve(m, vec, ivec, &nvec);

    Nt_times_y(N, at, iat, kat, basicflag, vec, ivec, nvec,
               dy_N, idy_N, &ndy_N);   /* use sparse vector dy_N */
    /* temporarily */

    for (j = 0; j < n; j++) y_N[j] = -c[nonbasics[j]];

    for (k = 0; k < ndy_N; k++)
      {
        j = idy_N[k];
        y_N[j] += dy_N[k];
      }

    /****************************************************************
     *   Begin Phase II (I.e., primal simplex method)                *
     ****************************************************************/

    for (; iter < MAX_ITER; iter++)
      {

        /*************************************************************
         * STEP 6: Print statistics and factor/refactor               *
         *************************************************************/

        primal_obj = sdotprod(c, x_B, basics, m) + f;
#ifdef DEBUG
        printf("%8d   %14.7e     NA    ", iter, high(primal_obj));
        fflush(stdout);
#endif

        /*************************************************************
         * STEP 1: Pick most negative nonbasic dual                   *
         *************************************************************/

        col_in = pick_neg(n, y_N);

        if (col_in == -1) break;  /* optimal */

        /*************************************************************
         *                        -1                                  *
         * STEP 2: Compute dx  = B  N e                               *
         *                   B         j                              *
         *         where j = col_in                                   *
         *************************************************************/

        j = nonbasics[col_in];

        for (i = 0, k = ka[j]; k < ka[j + 1]; i++, k++)
          {
            dx_B[i] =  a[k];
            idx_B[i] = ia[k];
          }

        ndx_B = i;

        bsolve(m, dx_B, idx_B, &ndx_B);

        /*************************************************************
         * STEP 3: Ratio test to find leaving column                  *
         *************************************************************/

        col_out = ratio_test(dx_B, idx_B, ndx_B, x_B);

        if (col_out == -1)
          {

            FREE(vec);
            FREE(ivec);
            FREE(x_B);
            FREE(y_N);
            FREE(dx_B);
            FREE(idx_B);
            FREE(dy_N);
            FREE(idy_N);
            FREE(nonbasics);
            FREE(basics);
            FREE(basicflag);
            FREE(at);
            FREE(iat);
            FREE(kat);
            return 1; /* UNBOUNDED */
          }

        /*************************************************************
         *                          -1  T                             *
         * STEP 4: Compute dy  = -(B  N) e                            *
         *                   N            i          *
         *         where i = col_out                                  *
         *************************************************************/

        vec[0] = -1.0;
        ivec[0] = col_out;
        nvec = 1;

        btsolve(m, vec, ivec, &nvec);

        Nt_times_y(N, at, iat, kat, basicflag, vec, ivec, nvec,
                   dy_N, idy_N, &ndy_N);

        /*************************************************************
         * STEP 5: Put       t = x /dx                                *
         *                        i   i                               *
         *                   s = y /dy                                *
         *                        j   j                               *
         *************************************************************/

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndx_B; k++)
          if (idx_B[k] == col_out) break;

        t    =    x_B[col_out] / dx_B[k];

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndy_N; k++)
          if (idy_N[k] == col_in) break;

        s    =    y_N[col_in] / dy_N[k];

        /*************************************************************
         * STEP 7: Set y  = y  - s dy                                 *
         *              N    N       N                                *
         *                                                            *
         *             y  = s                                         *
         *              i                                             *
         *                                                            *
         *             x  = x  - t dx                                 *
         *              B    B       B                                *
         *                                                            *
         *             x  = t                                         *
         *              j                                             *
         *************************************************************/

        for (k = 0; k < ndy_N; k++)
          {
            j = idy_N[k];
            y_N[j] -= s * dy_N[k];
          }

        y_N[col_in] = s;

        for (k = 0; k < ndx_B; k++)
          {
            i = idx_B[k];
            x_B[i] -= t * dx_B[k];
          }

        x_B[col_out] = t;

        /*************************************************************
         * STEP 8: Update basis                                       *
         *************************************************************/

        i =    basics[col_out];
        j = nonbasics[col_in];
        basics[col_out]   = j;
        nonbasics[col_in] = i;
        basicflag[i] = -col_in - 1;
        basicflag[j] = col_out;

#ifdef DEBUG
        int from_scratch = refactor(m, ka, ia, a, basics, col_out, v);
        if (from_scratch)
          {
            printf("\n");
          }
        else
          {
            printf("%c", cr);
          }
#endif

      }   /* End of Phase II */

    primal_obj = sdotprod(c, x_B, basics, m) + f;
#ifdef DEBUG
    printf("%8d   %14.7e     NA    \n", iter, high(primal_obj));
    printf("End of Phase II \n");
#endif
    /****************************************************************
     *  Transcribe solution to x vector and dual solution to y      *
     ****************************************************************/
    *zf = primal_obj;

    for (j = 0; j < N; j++) x[j] = 0.0;

    for (i = 0; i < m; i++) x[basics[i]] = x_B[i];

    for (j = 0; j < N; j++) y[j] = 0.0;

    for (i = 0; i < n; i++) y[nonbasics[i]] = y_N[i];

    /****************************************************************
     *  Split out slack variables and shift dual variables.
     ****************************************************************/

    for (j = 0; j < n; j++) z[j] = y[j];

    for (i = 0; i < m; i++)
      {
        y[i] = y[n + i];
        w[i] = x[n + i];
      }

    /****************************************************************
     *  Free work space                                             *
     ****************************************************************/

    FREE(vec);
    FREE(ivec);
    FREE(x_B);
    FREE(y_N);
    FREE(dx_B);
    FREE(idx_B);
    FREE(dy_N);
    FREE(idy_N);
    FREE(nonbasics);
    FREE(basics);
    FREE(basicflag);
    FREE(at);
    FREE(iat);
    FREE(kat);

    return 0;

  }   /* End  */

#ifdef DEBUG
  void Display_Solution(int m, int* basics, double* X)
  {
    int i;

    printf("SOLUTION:\n\n");

    for (i = 0; i < m; i++)
      printf("  X[%d] = %lf\n", basics[i], high(X[i]));
  }
#endif

  void Nt_times_y(
    int n,
    double* at,
    int* iat,
    int* kat,
    int* basicflag,
    double* y,
    int* iy,
    int ny,
    double* yN,
    int* iyN,
    int* pnyN
  )
  {
    int i, j, jj, k, kk;

    extern double* Nt_times_y_a;
    extern int*  Nt_times_y_tag;
    extern int* Nt_times_y_link;
    extern int  Nt_times_y_currtag;

    if (Nt_times_y_a    == NULL) MALLOC(Nt_times_y_a,  n,   double);

    if (Nt_times_y_tag  == NULL) CALLOC(Nt_times_y_tag, n,   int);

    if (Nt_times_y_link == NULL)
      {
        CALLOC(Nt_times_y_link, n + 2, int);
        Nt_times_y_link++;
      }

    jj = -1;

    for (k = 0; k < ny; k++)
      {
        i = iy[k];

        for (kk = kat[i]; kk < kat[i + 1]; kk++)
          {
            j = iat[kk];

            if (basicflag[j] < 0)
              {
                if (Nt_times_y_tag[j] != Nt_times_y_currtag)
                  {
                    Nt_times_y_a[j] = 0.0;
                    Nt_times_y_tag[j] = Nt_times_y_currtag;
                    Nt_times_y_link[jj] = j;
                    jj = j;
                  }

                Nt_times_y_a[j] += y[k] * at[kk];
              }
          }
      }

    Nt_times_y_link[jj] = n;
    Nt_times_y_currtag++;

    k = 0;

    for (jj = Nt_times_y_link[-1]; jj < n; jj = Nt_times_y_link[jj])
      {
        if (Abs(Nt_times_y_a[jj]) > EPS1)
          {
            yN[k] = Nt_times_y_a[jj];
            iyN[k] = -basicflag[jj] - 1;
            k++;
          }
      }

    *pnyN = k;
  }

  int ratio_test(
    double* dy,
    int*   idy,
    int    ndy,
    double* y
  )
  {
    int j, jj = -1, k;
    double min = HUGE_VAL;

    for (k = 0; k < ndy; k++)
      {
        if (dy[k] > EPS1)
          {
            j = idy[k];

            if (y[j] / dy[k] < min)
              {
                min = y[j] / dy[k];
                jj = j;
              }
          }
      }

    return jj;
  }

  double sdotprod(double* c, double* x_B, int* basics, int m)
  {
    int i;
    double prod = 0.0;

    for (i = 0; i < m; i++)
      {
        prod += c[basics[i]] * x_B[i];
      }

    return prod;
  }

  int pick_neg(int m, double* x)
  {
    int i;
    int col = -1;
    double min = -EPS2;

    for (i = 0; i < m; i++)
      {
        if (x[i] < min)
          {
            min = x[i];
            col = i;
          }
      }

    return col;
  }

  int simplex_method_modified(
    int m,    /* number of constraints */
    int n,    /* number of variables */
    int nz,   /* number of nonzeros in sparse constraint matrix */
    int* ia,    /* array row indices */
    int* ka,    /* array of indices into ia and a */
    double* a,    /* array of nonzeros in the constraint matrix */
    double* b,    /* right-hand side */
    double* c,          /* objective coefficients */
    double  f,    /* objective function shift */
    double* x,    /* primal solution (output) */
    double* y,    /* dual solution (output) */
    double* w,    /* primal slacks (output) */
    double* z,    /* dual slacks (output) */
    double* zf,         /* Optimal value of the objective function */
    double* gew         /* Gewichte für Schlupfvaraibale */
  )
  {
    double*  x_B; /* primal basics */
    double*  y_N; /* dual nonbasics */

    double*  dy_N;  /*  dual  basics step direction - values (sparse) */
    int*    idy_N;  /*  dual  basics step direction - row indices */
    int     ndy_N;  /* number of nonz in dy_N */

    double*  dx_B;  /* primal basics step direction - values (sparse) */
    int*    idx_B;  /* primal basics step direction - row indices */
    int     ndx_B;  /* number of nonz in dx_B */

    double*  at;  /* sparse data structure for A^T */
    int*    iat;
    int*    kat;

    int*     basics;  /* list of basic variable indices */
    int*     nonbasics; /* list of non-basic variable indices */
    int*     basicflag; /* neg if nonbasic, pos if basic */

    int     col_in; /* entering column; index in 'nonbasics' */
    int     col_out;  /* leaving column; index in 'basics' */

    int     iter = 0; /* number of iterations */
    int     i, j, k, N;//, v = 0;

    double  s, t;
    float   primal_obj;

    double*  vec;
    int*    ivec;
    int     nvec;

    //    int     from_scratch;

    /*******************************************************************
     * For convenience, we put...
     *******************************************************************/

    N = n + m;

    /*******************************************************************
     * Add slack variables.  We assume the calling routine allocated
     * enough memory.
     *******************************************************************/

    i = 0;
    k = ka[n];

    for (j = n; j < N; j++)
      {
        c[j] = -gew[j - n];
        a[k] = 1.0;
        ia[k] = i;
        i++;
        k++;
        ka[j + 1] = k;
      }

    nz = k;

    /*******************************************************************
     * Read in the Data and initialize the common memory sites.
     *******************************************************************/

    MALLOC(x_B, m, double);
    MALLOC(dx_B, m, double);
    MALLOC(y_N, n, double);
    MALLOC(dy_N, n, double);
    MALLOC(vec, m, double);
    MALLOC(nonbasics, n, int);
    MALLOC(basics, m, int);
    MALLOC(basicflag, N, int);
    MALLOC(idx_B, m,    int);
    MALLOC(idy_N, n,    int);
    MALLOC(ivec, m,    int);
    MALLOC(at, nz,  double);
    MALLOC(iat, nz,   int);
    MALLOC(kat, m + 1,  int);

    /****************************************************************
     *  Initialization.                         *
     ****************************************************************/

    atnum(m, N, ka, ia, a, kat, iat, at);

    for (j = 0; j < n; j++)
      {
        nonbasics[j] = j;
        basicflag[j] = -j - 1;
        y_N[j] = Max(c[j], 1.0); /* to force dual feasibility */
        y_N[j] += RandomD();     /* to ensure nondegeneracy   */
      }

    for (i = 0; i < m; i++)
      {
        basics[i] = n + i;
        basicflag[n + i] = i;
        x_B[i] = b[i];
      }

    lufac(m, ka, ia, a, basics, 0);

    dbsolve(m, x_B);   /* could be done explicitly in terms of bounds/ranges */

    /****************************************************************
     *  Begin Phase I (i.e., dual simplex method)
     ****************************************************************/
#ifdef DEBUG

    printf("m = %d,n = %d,nz = %d\n", m, N, nz);
    printf(
      "---------------------------------------------------------------------------\n"
      "          |   Primal      |        |                           arithmetic  \n"
      "  Iter    |  Obj Value    |   mu   |   nonz(L)     nonz(U)     operations  \n"
      "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n"
    );
    fflush(stdout);
#endif

    /****************************************************************
     *  Main loop                                                   *
     ****************************************************************/

    for (iter = 0; iter < MAX_ITER; iter++)
      {

        /*************************************************************
         * STEP 6: Print statistics and factor/refactor               *
         *************************************************************/

        primal_obj = sdotprod(c, x_B, basics, m) + f;
#ifdef DEBUG
        printf("%8d   %14.7e     NA    ", iter, high(primal_obj));
        fflush(stdout);
#endif

        /*************************************************************
         * STEP 1: Pick most negative basic primal                    *
         *************************************************************/

        col_out = pick_neg(m, x_B);

        if (col_out == -1) break; /* ready for Phase II */

        /*************************************************************
         *                          -1  T                             *
         * STEP 2: Compute dy  = -(B  N) e                            *
         *                   N            i          *
         *         where i = col_out                                  *
         *************************************************************/

        vec[0] = -1.0;
        ivec[0] = col_out;
        nvec = 1;

        btsolve(m, vec, ivec, &nvec);

        Nt_times_y(N, at, iat, kat, basicflag, vec, ivec, nvec,
                   dy_N, idy_N, &ndy_N);

        /*************************************************************
         * STEP 3: Ratio test to find entering column                 *
         *************************************************************/

        col_in = ratio_test(dy_N, idy_N, ndy_N, y_N);

        if (col_in == -1)
          {

            FREE(vec);
            FREE(ivec);
            FREE(x_B);
            FREE(y_N);
            FREE(dx_B);
            FREE(idx_B);
            FREE(dy_N);
            FREE(idy_N);
            FREE(nonbasics);
            FREE(basics);
            FREE(basicflag);
            FREE(at);
            FREE(iat);
            FREE(kat);
            return 2;   /* INFEASIBLE */
          }

        /*************************************************************
         *                        -1                                  *
         * STEP 4: Compute dx  = B  N e                               *
         *                   B         j                              *
         *                                                            *
         *************************************************************/

        j = nonbasics[col_in];

        for (i = 0, k = ka[j]; k < ka[j + 1]; i++, k++)
          {
            dx_B[i] =  a[k];
            idx_B[i] = ia[k];
          }

        ndx_B = i;

        bsolve(m, dx_B, idx_B, &ndx_B);

        /*************************************************************
         *                                                            *
         * STEP 5: Put       t = x /dx                                *
         *                        i   i                               *
         *                   s = y /dy                                *
         *                        j   j                               *
         *************************************************************/

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndx_B; k++) if (idx_B[k] == col_out) break;

        t = x_B[col_out] / dx_B[k];

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndy_N; k++) if (idy_N[k] == col_in) break;

        s = y_N[col_in] / dy_N[k];

        /*************************************************************
         * STEP 7: Set y  = y  - s dy                                 *
         *              N    N       N                                *
         *                                                            *
         *             y  = s                                         *
         *              i                                             *
         *                                                            *
         *             x  = x  - t dx                                 *
         *              B    B       B                                *
         *                                                            *
         *             x  = t                                         *
         *              j                                             *
         *************************************************************/

        for (k = 0; k < ndy_N; k++)
          {
            j = idy_N[k];
            y_N[j] -= s * dy_N[k];
          }

        y_N[col_in] = s;

        for (k = 0; k < ndx_B; k++)
          {
            i = idx_B[k];
            x_B[i] -= t * dx_B[k];
          }

        x_B[col_out] = t;

        /*************************************************************
         * STEP 8: Update basis                                       *
         *************************************************************/

        i =    basics[col_out];
        j = nonbasics[col_in];
        basics[col_out]   = j;
        nonbasics[col_in] = i;
        basicflag[i] = -col_in - 1;
        basicflag[j] = col_out;


#ifdef DEBUG
        int from_scratch = refactor(m, ka, ia, a, basics, col_out, v);
        if (from_scratch)
          {
            printf("\n");
          }
        else
          {
            printf("%c", cr);
          }
#endif

      } /* End of Phase I */

#ifdef DEBUG

    printf("End of Phase I \n");

#endif

    /****************************************************************
     *   Restore objective function by setting                       *
     *                -1  T                                          *
     *         y  = (B  N) c  - c                                    *
     *          N           B    N                                   *
     ****************************************************************/

    nvec = 0;

    for (i = 0; i < m; i++)
      {
        if (Abs(c[basics[i]]) > EPS)
          {
            vec[nvec] = c[basics[i]];
            ivec[nvec] = i;
            nvec++;
          }
      }

    btsolve(m, vec, ivec, &nvec);

    Nt_times_y(N, at, iat, kat, basicflag, vec, ivec, nvec,
               dy_N, idy_N, &ndy_N);   /* use sparse vector dy_N */
    /* temporarily */

    for (j = 0; j < n; j++) y_N[j] = -c[nonbasics[j]];

    for (k = 0; k < ndy_N; k++)
      {
        j = idy_N[k];
        y_N[j] += dy_N[k];
      }

    /****************************************************************
     *   Begin Phase II (I.e., primal simplex method)                *
     ****************************************************************/

    for (; iter < MAX_ITER; iter++)
      {

        /*************************************************************
         * STEP 6: Print statistics and factor/refactor               *
         *************************************************************/

        primal_obj = sdotprod(c, x_B, basics, m) + f;
#ifdef DEBUG
        printf("%8d   %14.7e     NA    ", iter, high(primal_obj));
        fflush(stdout);
#endif

        /*************************************************************
         * STEP 1: Pick most negative nonbasic dual                   *
         *************************************************************/

        col_in = pick_neg(n, y_N);

        if (col_in == -1) break;  /* optimal */

        /*************************************************************
         *                        -1                                  *
         * STEP 2: Compute dx  = B  N e                               *
         *                   B         j                              *
         *         where j = col_in                                   *
         *************************************************************/

        j = nonbasics[col_in];

        for (i = 0, k = ka[j]; k < ka[j + 1]; i++, k++)
          {
            dx_B[i] =  a[k];
            idx_B[i] = ia[k];
          }

        ndx_B = i;

        bsolve(m, dx_B, idx_B, &ndx_B);

        /*************************************************************
         * STEP 3: Ratio test to find leaving column                  *
         *************************************************************/

        col_out = ratio_test(dx_B, idx_B, ndx_B, x_B);

        if (col_out == -1)
          {

            FREE(vec);
            FREE(ivec);
            FREE(x_B);
            FREE(y_N);
            FREE(dx_B);
            FREE(idx_B);
            FREE(dy_N);
            FREE(idy_N);
            FREE(nonbasics);
            FREE(basics);
            FREE(basicflag);
            FREE(at);
            FREE(iat);
            FREE(kat);
            return 1; /* UNBOUNDED */
          }

        /*************************************************************
         *                          -1  T                             *
         * STEP 4: Compute dy  = -(B  N) e                            *
         *                   N            i          *
         *         where i = col_out                                  *
         *************************************************************/

        vec[0] = -1.0;
        ivec[0] = col_out;
        nvec = 1;

        btsolve(m, vec, ivec, &nvec);

        Nt_times_y(N, at, iat, kat, basicflag, vec, ivec, nvec,
                   dy_N, idy_N, &ndy_N);

        /*************************************************************
         * STEP 5: Put       t = x /dx                                *
         *                        i   i                               *
         *                   s = y /dy                                *
         *                        j   j                               *
         *************************************************************/

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndx_B; k++) if (idx_B[k] == col_out) break;

        t    =    x_B[col_out] / dx_B[k];

        /* this is inefficient - it should be fixed */
        for (k = 0; k < ndy_N; k++) if (idy_N[k] == col_in) break;

        s    =    y_N[col_in] / dy_N[k];

        /*************************************************************
         * STEP 7: Set y  = y  - s dy                                 *
         *              N    N       N                                *
         *                                                            *
         *             y  = s                                         *
         *              i                                             *
         *                                                            *
         *             x  = x  - t dx                                 *
         *              B    B       B                                *
         *                                                            *
         *             x  = t                                         *
         *              j                                             *
         *************************************************************/

        for (k = 0; k < ndy_N; k++)
          {
            j = idy_N[k];
            y_N[j] -= s * dy_N[k];
          }

        y_N[col_in] = s;

        for (k = 0; k < ndx_B; k++)
          {
            i = idx_B[k];
            x_B[i] -= t * dx_B[k];
          }

        x_B[col_out] = t;

        /*************************************************************
         * STEP 8: Update basis                                       *
         *************************************************************/

        i =    basics[col_out];
        j = nonbasics[col_in];
        basics[col_out]   = j;
        nonbasics[col_in] = i;
        basicflag[i] = -col_in - 1;
        basicflag[j] = col_out;

#ifdef DEBUG
        int from_scratch = refactor(m, ka, ia, a, basics, col_out, v);
        //if (from_scratch) {printf("\n");} else {printf("%c", cr);}
#endif

      }   /* End of Phase II */

    primal_obj = sdotprod(c, x_B, basics, m) + f;
#ifdef DEBUG
    printf("%8d   %14.7e     NA    \n", iter, high(primal_obj));
    printf("End of Phase II \n");
#endif
    /****************************************************************
     *  Transcribe solution to x vector and dual solution to y      *
     ****************************************************************/
    *zf = primal_obj;

    for (j = 0; j < N; j++) x[j] = 0.0;

    for (i = 0; i < m; i++) x[basics[i]] = x_B[i];

    for (j = 0; j < N; j++) y[j] = 0.0;

    for (i = 0; i < n; i++) y[nonbasics[i]] = y_N[i];

    /****************************************************************
     *  Split out slack variables and shift dual variables.
     ****************************************************************/

    for (j = 0; j < n; j++) z[j] = y[j];

    for (i = 0; i < m; i++)
      {
        y[i] = y[n + i];
        w[i] = x[n + i];
      }

    /****************************************************************
     *  Free work space                                             *
     ****************************************************************/

    FREE(vec);
    FREE(ivec);
    FREE(x_B);
    FREE(y_N);
    FREE(dx_B);
    FREE(idx_B);
    FREE(dy_N);
    FREE(idy_N);
    FREE(nonbasics);
    FREE(basics);
    FREE(basicflag);
    FREE(at);
    FREE(iat);
    FREE(kat);

    return 0;

  }   /* End of solver */
}
