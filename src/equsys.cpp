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
/* Loesen linearer Gleichungssysteme */

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "equsys.h"
#include "vectorarrayfunctions.h"
#include "matrix_function.h"
#include "derive.h"

namespace ice
{
  /* Nichtoeffentliche Prototypen */
  int equsysr2(double* A, double* b, double* xptr, double* yptr);
  int equsysr3(double* A, double* b, double* x);

  /* Ausgabe der dynamischen Felder */
#if defined(DEBUG)
  void displayequ(int rang, double* dpa, double* dpb)
  {
    double* ph;
    int i, j;
    ph = dpb;

    for (i = 0; i < rang; i++)
      {
        for (j = 0; j < rang; j++)
          {
            printf("%9.2e", *(dpa + (rang * i) + j));
          }

        printf("* %9.2e\n", *(ph++));
      };

    return;
  };
#endif
  /*******************************************************************/
  int TestEquSysMultiple(double A[], double b[], double x[],
                         int row, int col, int rang)
  {
    double sum;
    double eps = 0.1e-5;
    double* hb, *pd;
    int size;
    int i, j, k;
    /*** Anforderung des dynamischen Speichers fuer b*/
    size = row * sizeof(double);
    hb = (double*) malloc(size);

    if (hb == NULL)
      {
        throw IceException("EquSysTest", M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    if (b != NULL)
      {
        memcpy(hb, b, size);
      }
    else
      for (i = 0; i < row; i++)
        {
          hb[i] = 0;
        }

    /* Test des Verschiebungsvektors */
    pd = &x[0];

    for (i = 0; i < row; i++)
      {
        sum = 0;

        for (j = 0; j < col; j++)
          {
            sum += A[i * col + j] * pd[j];
          }

        if (fabs(sum - hb[i]) > eps)
          {
            return ERROR;
          }
      }

    for (k = 0; k < col - rang; k++)
      {
        pd = &x[(k + 1) * col];

        for (i = 0; i < row; i++)
          {
            sum = 0;

            for (j = 0; j < col; j++)
              {
                sum += A[i * col + j] * pd[j];
              }

            if (fabs(sum - hb[i]) > eps)
              {
                return ERROR;
              }
          }
      }

    return OK;
  }
  /********************************************************************
  Loesung von linearen Gleichungssystemen vom Range n
  (Berechnung mit Genauigkeit "double")
  ----------------------------------------------------------------
  rang - Ordnung des Gleichungssystems
  pa   - double pointer auf Koeffizientenmatrix A
  pb   - double pointer auf Imhomogenitaetsvektor b
  px   - double pointer auf Loesungsvektor x
  ----------------------------------------------------------------
     return:   0 - OK (eindeutige Loesung)
        -5 - NO_MEM (nicht genuegend dynamischer Speicherplatz)
        -9 - NO_SOLUTION (keine Loesung)
       -10 - VARIOUS_SOLUTION (Loesungsvielfalt)
       -11 - NUM_INSTABILITY (numerische Instabilitaeten)
  ----------------------------------------------------------------
  ***/

#define FNAME "EquSys"
  int EquSys(double* pa, double* pb, int rang, double* px)
  {
    int c, i, j, col, colh, row, rowh, offs, ret, rc;
    double maxa, max, maxb, fh, fha;
    double epsinst = 1e-15; /* Grenze der Instabilitaet */
    double epsnull = 1e-200; /* Definition 0 */
    double* dpa, *dpb;
    int size_a, size_b;

    maxa = 0;
    size_a = rang * rang * sizeof(double);
    size_b = rang * sizeof(double);

    if (pa == NULL || pb == NULL || px == NULL)
      {
        throw IceException(FNAME, M_WRONG_PTR, ERROR);
        return ERROR;
      };

    if (rang < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      };

    switch (rang)
      {
      case 2:
        rc = equsysr2(pa, pb, &px[0], &px[1]);

        if (rc != OK)
          {
            throw IceException(FNAME, M_ZERO_DET, rc);
            return rc;
          }

        return OK;
      case 3:
        rc = equsysr3(pa, pb, px);

        if (rc != OK)
          {
            throw IceException(FNAME, M_ZERO_DET, rc);
            return rc;
          }

        return OK;
      default:
        /* Anforderung des dynamischen Speichers */
        dpa = (double*) malloc(size_a);

        if (dpa == NULL)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return (NO_MEM);
          };

        dpb = (double*)malloc(size_b);

        if (dpb == NULL)
          {
            free(dpa);
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return (NO_MEM);
          };

        /* kopieren in dynamischen speicherbereich */
        memcpy(dpa, pa, size_a);

        memcpy(dpb, pb, size_b);

        /* transformation des dynamischen A|b */
        offs = 0;

        col = 0;

        ret = OK;

        while (col < rang)
          /* setzen der spalte "col" zu 0 */
          {
            max = 0;
            colh = col - offs;
            rowh = colh;
            /* bestimmen des maximums der spalte "col" */
            row = colh;

            while (row < rang)
              {
                fha = dpa[rang * row + col];
                fh = fabs(fha);

                if (fh > max)
                  {
                    maxa = fha;
                    max = fh;
                    rowh = row;
                  }

                row++;
              }

            /* transformation der zeilen bezueglich Spalte "col" */
            if (rowh != colh)
              /* tauschen der zeilen "rowh" und "colh" */
              {
                for (c = col; c < rang; c++)
                  {
                    fh = dpa[rang * rowh + c];
                    dpa[rang * rowh + c] = dpa[rang * colh + c];
                    dpa[rang * colh + c] = fh;
                  }

                fh = dpb[rowh];
                dpb[rowh] = dpb[colh];
                dpb[colh] = fh;
              }

            /* setzen der spalte "col" zu 0 ab zeile "colh+1" */
            if (fabs(*dpa) < 1e-20)
              {
                throw IceException(FNAME, M_NUM_INSTABILITY, NUM_INSTABILITY);
                free(dpa);
                free(dpb);
                return NUM_INSTABILITY;
              }

            if (fabs(dpa[rang * colh + col] / (*dpa)) < epsinst)
              {
                ret = NUM_INSTABILITY;
              }

            if (fabs(dpa[rang * colh + col]) > epsnull)
              {
                i = colh + 1;

                while (i < rang)
                  {
                    fh = dpa[rang * i + col] / maxa;

                    for (j = col + 1; j < rang; j++)
                      {
                        dpa[rang * i + j] -= dpa[rang * colh + j] * fh;
                      }

                    dpb[i] -= dpb[colh] * fh;
                    i++;
                  }
              }
            else
              {
                offs++;
              }

            col++;
          };

        if (offs == 0)
          {
            /* berechnen des Loesungsvektors */
            for (i = rang - 1; i >= 0; i--)
              {
                fh = 0;
                j = rang - 1;

                while (j > i)
                  {
                    fh += dpa[rang * i + j] * px[j];
                    j--;
                  }

                px[i] = (dpb[i] - fh) / dpa[rang * i + i];
              }
          }
        else
          {
            /* Untersuchung der Loesbarkeit */
            ret = VARIOUS_SOLUTION;
            maxb = 0;

            for (i = 0; i < rang; i++) if (fabs(dpb[i]) > maxb)
                {
                  maxb = fabs(dpb[i]);
                }

            for (i = 0; i < offs; i++)
              if (!(fabs(dpb[rang - 1 - i] / maxb) < epsinst))
                {
                  ret = NO_SOLUTION;
                }
          }

        free(dpb);
        free(dpa);

        switch (ret)
          {
          case NO_SOLUTION:
            throw IceException(FNAME, M_NO_SOLUTION, NO_SOLUTION);
            break;
          case VARIOUS_SOLUTION:
            throw IceException(FNAME, M_SOL_MANIFOLD, VARIOUS_SOLUTION);
            break;
          case NUM_INSTABILITY:
            throw IceException(FNAME, M_NUM_INSTABILITY, NUM_INSTABILITY);
            break;
          }

        return ret;
      }
  }
#undef FNAME
  /*******************************************************************/
#define FNAME "EquSysMultiple"
  int EquSysMultiple(double A[], double b[], double x[],
                     int row, int col, double* pdet, int* prang)
  {
    int i, j, k;
    int ii, jj;
    int size, dim, rang = 0;
    int cont1, cont2;
    double* hA, *hb;
    int* hperm;
    double maxcol;
    int rowmem, colmem;
    double h1, h2;
    int i1;
    double eps = 1e-10;
    rowmem = 0;
    colmem = 0;

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    /* Bearbeitung der Trivialfalles */
    if (row == 1)
      for (i = 0; i < col; i++)
        {
          if (fabs(A[i]) > eps)
            {
              if (col == 1)
                {
                  *pdet = A[0];
                  *prang = 1;
                  return OK;
                }
              else
                {
                  *pdet = 0;
                  *prang = 1;
                  return OK;
                }
            }

          *pdet = 0;
          *prang = 0;
          return OK;
        }

    /*** Anforderung des dynamischen Speichers fuer A*/
    size = row * col * sizeof(double);
    hA = (double*) malloc(size);

    if (hA == NULL)
      {
        throw IceException("EquSysMultiple", M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    memcpy(hA, A, size);
    /*** Anforderung des dynamischen Speichers fuer b*/
    size = row * sizeof(double);
    hb = (double*) malloc(size);

    if (hb == NULL)
      {
        throw IceException("EquSysMultiple", M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    if (b != NULL)
      {
        memcpy(hb, b, size);
      }
    else
      for (i = 0; i < row; i++)
        {
          hb[i] = 0;
        }

    /*** Initialisierung des Loesungsvektors */
    for (i = 0; i < row; i++)
      for (j = 0; j < col; j++)
        {
          x[i * col + j] = 0;
        }

    /*** Initialisierung des Permutationsspeichers */
    size = col * sizeof(double);
    hperm = (int*) malloc(size);

    if (hb == NULL)
      {
        throw IceException("EquSysMultiple", M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    for (i = 0; i < col; i++)
      {
        hperm[i] = i;
      }

    /*** Transformation von (A,b) in Trapezform
       Modifikation des Permutationsvektors */
    rang = 0;
    dim = Min(row, col);
    cont1 = 1;

    for (i = 0; i < dim - 1 && cont1 == 1; i++)
      {
        /* Bestimmen des Maximums der i'ten Restspalte */
        /*
          printf("**** i: %d\n",i);
        */
        maxcol = 0;

        for (ii = i; ii < row; ii++)
          {
            h1 = fabs(hA[ii * col + i]);

            if (h1 > maxcol)
              {
                maxcol = h1;
                rowmem = ii;
              }
          }

        cont2 = 0;

        /*
          printf("maxcol %f rowmem: %d\n",maxcol,rowmem);
          getch();
        */
        if (maxcol < eps)
          {
            /* Bestimmen einer brauchbaren Spalte von rechts beginnend*/
            cont2 = 1;

            for (jj = col - 1; jj > i && cont2 == 1; jj--)
              for (ii = i; ii < row && cont2 == 1; ii++)
                {
                  if (fabs(hA[ii * col + jj]) > eps)
                    {
                      cont2 = 0;
                      colmem = jj;
                    }
                }

            if (cont2 == 1)
              {
                cont1 = 0;
              }
            else
              {
                /* Vertauschen der colmem'ten und i'ten Spalte */
                for (ii = 0; ii < row; ii++)
                  {
                    h1 = hA[ii * col + colmem];
                    hA[ii * col + colmem] = hA[ii * col + i];
                    hA[ii * col + i] = h1;
                  }

                /* Transformation des Permutationsvektors */
                i1 = hperm[colmem];
                hperm[colmem] = hperm[i];
                hperm[i] = i1;
                /* Berechnen des Maximums der i'ten Restspalte */
                maxcol = 0;

                for (ii = i; ii < row; ii++)
                  {
                    h1 = fabs(hA[ii * col + i]);

                    if (h1 > maxcol)
                      {
                        maxcol = h1;
                        rowmem = ii;
                      }
                  }
              }
          }

        if (cont2 == 0)
          {
            /* Vertauschen der i'ten mit der rowmem'ten Zeile */
            if (i != rowmem)
              {
                for (jj = 0; jj < col; jj++)
                  {
                    h1 = hA[i * col + jj];
                    hA[i * col + jj] = hA[rowmem * col + jj];
                    hA[rowmem * col + jj] = h1;
                  }

                if (b != NULL)
                  {
                    h1 = hb[i];
                    hb[i] = hb[rowmem];
                    hb[rowmem] = h1;
                  }
              }

            /* Transformation der Matrix hA */
            for (ii = i + 1; ii < row; ii++)
              {
                h1 = hA[ii * col + i] / hA[i * col + i];

                for (jj = 0; jj < col; jj++)
                  {
                    h2 = hA[i * col + jj] * h1;
                    /*
                      printf("ii: %d jj: %d h1 %f h2 %f\n",ii,jj,h1,h2);
                    */
                    hA[ii * col + jj] -= h2;
                    /*
                      WriteMatrix("hA",hA,row,col);
                    */
                  }

                if (b != NULL)
                  {
                    hb[ii] -= hb[i] * h1;
                  }
              }

            rang++;
          }
      }

    if (fabs(hA[(dim - 1)*col + dim - 1]) > eps)
      {
        rang += 1;
      }

    *prang = rang;

    if (rang < dim)
      {
        *pdet = 0;
      }
    else
      {
        *pdet = 1;

        for (i = 0; i < dim; i++)
          {
            *pdet *= hA[i * col + i];
          }
      }

    if (b != NULL)
      for (i = rang; i < row; i++)
        if (fabs(b[i]) > eps)
          {
            return NO_SOLUTION;
          }

    /* Berechnung des Verschiebungsvektors zur Loesungvielfalt */
    if (rang > 0)
      for (i = col - 1; i >= 0; i--)
        {
          if (i >= rang)
            {
              x[i] = 0;
            }
          else
            {
              h1 = hb[i];

              for (j = i + 1; j < col; j++)
                {
                  h1 -= hA[i * col + j] * x[j];
                }

              x[i] = h1 / hA[i * col + i];
              /*
                printf("i: %d\n",i);
                WriteMatrix("x:",x,col,col);
              */
            }
        }

    /*
      printf("Verschiebung berechnet\n");
      WriteMatrix("x:",x,col,col);
    */
    /* Berechnung der (dim-rang)-dimensionalen Loesungsvielfalt */
    if (rang > 0)
      for (i = 0; i < dim - rang; i++)
        {
          for (j = col - 1; j >= 0; j--)
            if (j >= rang)
              if (j == col - 1 - i)
                {
                  x[(i + 1)*col + j] = 1;
                }
              else
                {
                  x[(i + 1)*col + j] = 0;
                }
            else
              {
                h1 = hb[j];

                for (k = j + 1; k < col; k++)
                  {
                    h1 -= hA[j * col + k] * x[(i + 1) * col + k];
                  }

                x[(i + 1)*col + j] = h1 / hA[j * col + j];
              }

          /*
            printf("Vielfalt i=%d berechnet:\n",i);
            WriteMatrix("x:",x,col,col);
          */
        }

    /* Zuruecknehmen der Permutationen */
    /*
      for (i=0;i<col;i++)
      printf("hperm[%d] %d\n",i,hperm[i]);
    */
    if (rang > 0)
      for (i = 0; i < col - 1; i++)
        for (j = 0; j < col; j++)
          if (hperm[j] == i)
            if (i != j)
              {
                /* Tausch im Permutationsvektor */
                i1 = hperm[i];
                hperm[i] = i;
                hperm[j] = i1;

                /*
                    for (i=0;i<col;i++)
                    printf("hperm[%d] %d\n",i,hperm[i]);
                    getch();
                */
                /* Spaltentauch */
                for (k = 0; k < row; k++)
                  {
                    h1 = x[k * col + i];
                    x[k * col + i] = x[k * col + j];
                    x[k * col + j] = h1;
                  }
              }

    /* Loesungsvielfalt in Orthonormalform bringen */

    if (rang > 0)
      if (rang < dim)
        {
          OrthoMatrix(&x[col], dim - rang, col, &x[col]);
        }

    if (rang < dim)
      {
        return NO_UNIQUE_SOLUTION;
      }

    return OK;
  }
#undef FNAME
  /********************************************************************
  Loesung eines linearen Gleichungssystems vom Range 2
  (Berechnung mit Genauigkeit "double")
  ----------------------------------------------------------------
  a  - Pointer auf Koeffizientenmatrix
  b  - Pointer auf Inhomogenitaetsvektor
  xptr yptr - Pointer auf Loesung
  pdet - Pointer auf Determinante
  ----------------------------------------------------------------
     return:   0 - OK (eindeutige Loesung)
       -11 - NO_UNIQUE_SOLUTION (numerische Instabilitaeten)
  **********************************************************************/
  int equsysr2(double* a, double* b, double* xptr, double* yptr)
  {
    double eps = 1e-80;
    double det;
    det = a[0] * a[3] - a[1] * a[2];

    if (fabs(det) < eps)
      {
        return NUM_INSTABILITY;
      }

    *xptr = (b[0] * a[3] - b[1] * a[1]) / det;
    *yptr = (a[0] * b[1] - a[2] * b[0]) / det;
    return OK;
  }

  /********************************************************************
  Loesung eines linearen Gleichungssystems (Rang 3)
  ----------------------------------------------------------------
  A  - Pointer auf Koeffizientenmatrix
  b  - Pointer auf Inhomogenitaetsvektor
  x -  Pointer auf Loesung
  ----------------------------------------------------------------
     return:   0 - OK (eindeutige Loesung)
       -11 - NO_UNIQUE_SOLUTION (numerische Instabilitaeten)
  ********************************************************************/
  int equsysr3(double* a, double* b, double* x)
  {
    double det;
    double eps = 1e-80;
    det = a[0] * (a[4] * a[8] - a[5] * a[7]) -
          a[1] * (a[3] * a[8] - a[5] * a[6]) +
          a[2] * (a[3] * a[7] - a[4] * a[6]);

    if (fabs(det) < eps)
      {
        return NUM_INSTABILITY;
      }

    x[0] = (b[0] * (a[4] * a[8] - a[5] * a[7]) -
            a[1] * (b[1] * a[8] - a[5] * b[2]) +
            a[2] * (b[1] * a[7] - a[4] * b[2])) / det;

    x[1] = (a[0] * (b[1] * a[8] - a[5] * b[2]) -
            b[0] * (a[3] * a[8] - a[5] * a[6]) +
            a[2] * (a[3] * b[2] - b[1] * a[6])) / det;

    x[2] = (a[0] * (a[4] * b[2] - b[1] * a[7]) -
            a[1] * (a[3] * b[2] - b[1] * a[6]) +
            b[0] * (a[3] * a[7] - a[4] * a[6])) / det;
    return OK;
  }
#undef FNAME
  /*******************************************************************/
  /* Loesung nichtlinearer Gleichungssysteme                          */
  /* neubauer 7/93                                                   */
  /*******************************************************************/
#define DEBUG
#define FNAME "NonLinEquSys"
  int NonLinEquSys(FuncD* f, int dim, int* step, double* eps, double* sol)
  {
    double* a, *b, *x, *hstart, *dptr;
    int size_mat, size_vec;
    int i, j, cont, nbr, rang;
    double heps, det, deriv, ceps = 1e-15;

    size_mat = dim * dim * sizeof(double);
    size_vec = dim * sizeof(double);
    a = (double*)malloc(size_mat);
    b = (double*)malloc(size_vec);
    x = (double*)malloc(size_vec);
    hstart = (double*)malloc(size_vec);
    memcpy(hstart, sol, size_vec);

    cont = 1;
    nbr = 0;

    while (cont)
      {
#if defined DEBUG
        printf("%d: u: %f v: %f ", nbr, hstart[0], hstart[1]);
        getchar();
#endif

        for (i = 0; i < dim; i++)
          for (j = 0; j < dim; j++)
            {
              DeriveFunc(f[i], dim, j + 1, hstart, 0.001, 0, &deriv);
              a[i * dim + j] = deriv;
            }

#if defined DEBUG
        printf("a:\n");
        dptr = a;

        for (i = 0; i < dim * dim; i++)
          {
            printf("%d: %g\n", i, *dptr++);
          }

        getchar();
#endif

        for (i = 0; i < dim; i++)
          {
            b[i] = -f[i](hstart);
          }

#if defined DEBUG
        printf("b:\n");
        dptr = b;

        for (i = 0; i < dim; i++)
          {
            printf("%d: %g\n", i, *dptr++);
          }

        getchar();
#endif
        DetMatrix((double*)a, 2, 2, &det, &rang);
#if defined DEBUG
        printf("det %f rang %d\n", det, rang);
        getchar();
#endif

        if (fabs(det) < ceps)
          {
            throw IceException(FNAME, M_WRONG_START, ERROR);
            return ERROR;
          }

        RETURN_ERROR_IF_FAILED(EquSys(a, b, dim, x));

#if defined DEBUG
        printf("x:\n");
        dptr = x;

        for (i = 0; i < dim; i++)
          {
            printf("%d: %g\n", i, *dptr++);
          }

        getchar();
#endif
        AddVecRn(hstart, x, dim, hstart);
        heps = 0;

        for (i = 0; i < dim; i++)
          {
            heps += Sqr(f[i](hstart));
          }

        heps /= (double)dim;
#if defined DEBUG
        printf(" eps: %G\n", heps);
        getchar();
#endif
        nbr++;

        if (heps < *eps)
          {
            memcpy(sol, hstart, size_vec);
            *step = nbr;
            *eps = heps;
            free(a);
            free(b);
            free(x);
            free(hstart);
            return OK;
          }

        if (nbr >= *step)
          {
            *step = nbr;
            *eps = heps;
            free(a);
            free(b);
            free(x);
            free(hstart);
            return WRONG_PRECISION;
          }
      }

    return (OK);
  }
#undef DEBUG
#undef FNAME

  /*************************************************
   *  Normalengleichungen fuer ueberbestimmtes GS  *
   *   m.schubert 3.94                             *
   *************************************************/
#define FNAME "NormalEqu"
  int NormalEqu(double* m1, double* v1, int row, int col, double* m2, double* v2)
  {
    int i, j, k;
    double* mp1i, *mp1j, *mp2, *vp2;

    if ((m1 == NULL) || (v1 == NULL) || (m2 == NULL) || (v2 == NULL))
      {
        throw IceException(FNAME, M_WRONG_PTR, ERROR);
        return (ERROR);
      }

    vp2 = v2;
    mp2 = m2;

    for (i = 0; i < col; i++)
      {
        for (j = 0; j < col; j++)
          {
            mp1i = m1 + i;
            mp1j = m1 + j;
            *mp2 = 0;

            for (k = 0; k < row; k++)
              {
                (*mp2) += (*mp1i) * (*mp1j);
                mp1i += col;
                mp1j += col;
              }

            mp2++;
          }

        *vp2 = 0;
        mp1i = m1 + i;

        for (k = 0; k < row; k++)
          {
            (*vp2) += (*mp1i) * v1[k];
            mp1i += col;
          }

        vp2++;
      }

    return (OK);
  }
#undef FNAME
  /*************************************************
   *  Ueberbestimmtes Gleichungssystem loesen      *
   *   (Ausgleichsrechnung)                        *
   *   m.schubert 3.94                             *
   *************************************************/
#define FNAME "OverEquSys"
  int OverEquSys(double* m1, double* v1, int row, int col, double* x, double* mse)
  {
    double* m2, *v2, *mp;
    double w;

    m2 = new double[col * col];
    v2 = new double[col];

    NormalEqu(m1, v1, row, col, m2, v2);                   /*Normalengleichungen bestimmen*/
    try
      {
        EquSys(m2, v2, col, x);                      /*Normalengleichungssystem loesen*/
      }
    catch (IceException& ex)
      {
        free(m2);
        free(v2);
        throw IceException(ex, FNAME);
      }

    mp = m1;
    *mse = 0;                                       /*mittleren qudratischen Fehler best.*/

    for (int i = 0; i < row; i++)
      {
        w = 0;

        for (int j = 0; j < col; j++)
          {
            w += *mp++ * x[j];
          }

        w -= v1[i];
        *mse += w * w;
      }

    *mse = sqrt(*mse / row);
    delete m2;
    delete v2;
    return (OK);
  }
#undef FNAME
}
