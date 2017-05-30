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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "IceException.h"
#include "macro.h"
#include "defs.h"
#include "matdef.h"
/*****************************************************************************
Fuer eine Matrix a[0..n][0..n] wird durch Vertauschen der Zeilen in eine
Dreiecksmatrix transformiert (Gauss-Verfahren) wobei in den Null-Elementen
der unteren Dreiecksmatrix Zusatzinformationen zur Transformation des
Inhomogenitaetsvektors abgelegt werden (LU-Dekomposition). Input sind a und n.
Output ist die transformierte Matrix a, der Vektor indx, der die
ausgeführten Permutationen aufzeichnet und der Wert d=+/-1 in Abhängigkeit
davon, ob die Zahl der Zeilenvertauschungen gerade oder ungerade
ist. Diese Funktion wird intern zur Lösung von Gleichungssystemen und
zur Matrixinvertierung verwendet. Rückgabewerte sind NO_MEM bei
fehlgeschlagener Speicheranforderung, NUM_INSTABILITY, wenn die
Matrix a singulär ist und OK bei korrekter Ausführung.
******************************************************************************/
namespace ice
{
#if 0
#define TINY 1.0e-20;
  int ludecomp(double** a, int n, int* indx, double* d)
  {
    int i, imax = 0, j, k, rc = OK;
    double big, dum, sum, temp;
    double* vv;
    /*Hilfsvektor*/
    vv = (double*)malloc(n * sizeof(double));

    if (vv == NULL)
      {
        return (NO_MEM);
      }

    *d = 1.0;

    /*Zeilenweise größtes Element*/
    for (i = 0; i < n; i++)
      {
        big = 0.0;

        for (j = 0; j < n; j++)
          if ((temp = fabs(a[i][j])) > big)
            {
              big = temp;
            }

        if (big == 0.0)
          {
            free(vv);
            return (NUM_INSTABILITY);
          }

        vv[i] = 1.0 / big;
      }

    for (j = 0; j < n; j++)
      {
        for (i = 0; i < j; i++)
          {
            sum = a[i][j];

            for (k = 0; k < i; k++)
              {
                sum -= a[i][k] * a[k][j];
              }

            a[i][j] = sum;
          }

        big = 0.0;

        for (i = j; i < n; i++)
          {
            sum = a[i][j];

            for (k = 0; k < j; k++)
              {
                sum -= a[i][k] * a[k][j];
              }

            a[i][j] = sum;

            if ((dum = vv[i] * fabs(sum)) >= big)
              {
                big = dum;
                imax = i;
              }
          }

        if (j != imax)
          {
            for (k = 0; k < n; k++)
              {
                dum = a[imax][k];
                a[imax][k] = a[j][k];
                a[j][k] = dum;
              }

            *d = -(*d);
            vv[imax] = vv[j];
          }

        indx[j] = imax;

        if (a[j][j] == 0.0)
          {
            a[j][j] = TINY;
            rc = NUM_INSTABILITY;
          }

        if (j != (n - 1))
          {
            dum = 1.0 / (a[j][j]);

            for (i = j + 1; i < n; i++)
              {
                a[i][j] *= dum;
              }
          }
      }

    free(vv);
    return (rc);
  }
#undef TINY

  /****************************************************************************
   * Für die Lösung eines Gleichungssystems Ax = b wird die Rücksubstitution
   * durchgeführt, wobei A die LU-Dekomposition der eigentlichen Koeffizienten-
   * matrix ist.
   ****************************************************************************/
  int lubacksub(double** a, double* b, int n, int* indx, double* x)
  {
    int i, ii = -1, ip, j;
    double sum;

    if (x != b) for (i = 0; i < n; i++)
        {
          x[i] = b[i];
        }

    for (i = 0; i < n; i++)
      {
        ip = indx[i];
        sum = x[ip];
        x[ip] = x[i];

        if (ii >= 0)
          for (j = ii; j < i; j++)
            {
              sum -= a[i][j] * x[j];
            }
        else if (sum)
          {
            ii = i;
          }

        x[i] = sum;
      }

    for (i = n - 1; i >= 0; i--)
      {
        sum = x[i];

        for (j = i + 1; j < n; j++)
          {
            sum -= a[i][j] * x[j];
          }

        x[i] = sum / a[i][i];
      }

    return (OK);
  }
  /****************************************************************************
   * Es wird ein lineares Gleichungssystem der Form A*x = b gelöst. Dazu wird
   * fuer die Matrix A die LU-Dekomposition durchgeführt. Die Matrix A und der
   * Vektor b werden nicht verändert.
   *
   * Literatur : "Numerical recipes in C" Page 44.
   ****************************************************************************/
#define FNAME "EquationSys"
  int EquationSys(MatrixStruct A, double* b, double* x)
  {
    int n, *indx, rc;
    double** a, d;
    MatrixStruct Ah;

    /*Parametertestung*/
    if (!IsMatrix(A))
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (A->type != MAT_DOUBLE)
      throw IceException(FNAME, M_WRONG_MATRIXTYPE);

    if (A->rsize != A->csize)
      throw IceException(FNAME, M_NO_SQUARE);

    /*Initialisierungen*/
    n = A->rsize;
    indx = (int*)malloc(n * sizeof(int));

    if (indx == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    Ah = MoveMat(A, nullptr);

    if (Ah == nullptr)
      {
        free(indx);
        throw IceException(FNAME, M_NO_MEM);
      }

    a = Ah->data;
    /*LU-Dekomposition*/
    rc = ludecomp(a, n, indx, &d);

    if (rc != OK)
      {
        free(indx);
        FreeMatrix(Ah);

        if (rc == NO_MEM)
          throw IceException(FNAME, M_NO_MEM);

        if (rc == NUM_INSTABILITY)
          throw IceException(FNAME, M_MATRIX_SINGULAR);

        return (rc);
      }

    /*Rücksubstitution*/
    rc = lubacksub(a, b, n, indx, x);
    free(indx);
    FreeMatrix(Ah);
    return (OK);
  }
#undef FNAME
  /****************************************************************************
  Es wird ein überbestimmtes lineares Gleichungssystem der Form A*x = b im Sinne
  der kleinsten Quadrate gelöst (A^TAx = A^Tb)
  ****************************************************************************/
#define FNAME "NormalEquationSys"
  int NormalEquationSys(MatrixStruct A, double* b, double* x)
  {
    int i, j, k, n, *indx, rc;
    double** a, d;
    MatrixStruct Ah;

    /*Parametertestung*/
    if (!IsMatrix(A))
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (A->type != MAT_DOUBLE)
      throw IceException(FNAME, M_WRONG_MATRIXTYPE);

    if (A->rsize < A->csize)
      throw IceException(FNAME, M_MATRIX_SINGULAR);

    /*Initialisierungen*/
    n = A->csize;
    indx = (int*)malloc(n * sizeof(int));

    if (indx == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    Ah = NewMatrix(MAT_DOUBLE, n, n);

    if (Ah == nullptr)
      {
        free(indx);
        throw IceException(FNAME, M_NO_MEM);
      }

    /*A^T * A */
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        for (k = 0, Ah->data[i][j] = 0; k < A->rsize; k++)
          {
            Ah->data[i][j] += A->data[k][i] * A->data[k][j];
          }

    a = Ah->data;
    /*LU-Dekomposition*/
    rc = ludecomp(a, n, indx, &d);

    if (rc != OK)
      {
        free(indx);
        FreeMatrix(Ah);

        if (rc == NO_MEM)
          throw IceException(FNAME, M_NO_MEM);

        if (rc == NUM_INSTABILITY)
          throw IceException(FNAME, M_MATRIX_SINGULAR);

        return (rc);
      }

    /* A^T*b */
    for (i = 0; i < n; i++)
      for (j = 0, x[i] = 0; j < A->rsize; j++)
        {
          x[i] += A->data[j][i] * b[j];
        }

    /*Rücksubstitution*/
    rc = lubacksub(a, x, n, indx, x);
    free(indx);
    FreeMatrix(Ah);
    return (OK);
  }
#undef FNAME
  /**************************************************************************
   * Die quadratische Matrix A wird invertiert indem die n  Gleichungssysteme
   * AB = I gelöst werden. Input ist die Matrix A, Output die Invertierte
   * Matrix B. A bleibt  unverändert.
   **************************************************************************/
#define FNAME "InvertMat"
  MatrixStruct InvertMat(MatrixStruct A, MatrixStruct B)
  {
    int i, j, *indx, n, rc;
    MatrixStruct Ah;
    double* icol, *bcol, d, **a, **b;

    /*Parametertestung*/
    if (!IsMatrix(A))
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (A->type != MAT_DOUBLE)
      throw IceException(FNAME, M_WRONG_MATRIXTYPE);

    if (A->rsize != A->csize)
      throw IceException(FNAME, M_NO_SQUARE);

    if (B != nullptr)
      {
        if (!IsMatrix(B))
          throw IceException(FNAME, M_WRONG_MATRIX);

        if (B->type != MAT_DOUBLE || B->rsize != A->rsize || B->csize != A->csize)
          throw IceException(FNAME, M_WRONG_MATRIXTYPE);
      }
    else
      {
        B = NewMatrix(A->type, A->rsize, A->csize);
      }

    /*Initialisierungen*/
    n = A->rsize;
    icol = (double*)malloc(n * sizeof(double));  /*Spaltenvektor der Einheitsmatrix*/
    bcol = (double*)malloc(n * sizeof(double));  /*Spaltenvektor der inversen Matrix*/
    indx = (int*)malloc(n * sizeof(int));       /*Indexvektor für Dekomposition*/
    Ah = MoveMat(A, nullptr);                      /*Hilfsmatrix für Dreiecksmatrix*/

    if (icol == nullptr || bcol == nullptr || indx == nullptr || Ah == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    a = Ah->data;
    b = B->data;
    /*LU-Dekomposition*/
    rc = ludecomp(a, n, indx, &d);

    if (rc != OK)
      {
        free(indx);
        free(icol);
        free(bcol);
        FreeMatrix(Ah);

        if (rc == NO_MEM)
          throw IceException(FNAME, M_NO_MEM);

        if (rc == NUM_INSTABILITY)
          throw IceException(FNAME, M_MATRIX_SINGULAR);

        return (nullptr);
      }

    /*Rücksubstitution fuer jede Spalte der Einheitsmatrix*/
    for (i = 0; i < n; i++)
      {
        icol[i] = 0;
      }

    for (i = 0; i < n; i++)
      {
        icol[i] = 1;
        rc = lubacksub(a, icol, n, indx, bcol);

        for (j = 0; j < n; j++)
          {
            b[j][i] = bcol[j];
          }

        icol[i] = 0;
      }

    free(icol);
    free(bcol);
    free(indx);
    FreeMatrix(Ah);
    return (B);
  }
#undef FNAME
  /**************************************************************************
  Von der quadratischen Matrix A wird die Determinate berechnet. Dazu wird A
  in eine Dreiecksmatrix umgeformt und anschließend das Produkt der
  Hauptdiagonalelemente berechnet.
  **************************************************************************/
#define FNAME "DetMat"
  double DetMat(MatrixStruct A)
  {
    int i, *indx, n, rc;
    MatrixStruct Ah;
    double d, **a;

    /*Parametertestung*/
    if (!IsMatrix(A))
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (A->type != MAT_DOUBLE)
      throw IceException(FNAME, M_WRONG_MATRIXTYPE);

    if (A->rsize != A->csize)
      throw IceException(FNAME, M_NO_SQUARE);

    /*Initialisierungen*/
    n = A->rsize;
    indx = (int*)malloc(n * sizeof(int));       /*Indexvektor für Dekomposition*/
    Ah = MoveMat(A, nullptr);                      /*Hilfsmatrix für Dreiecksmatrix*/

    if (indx == nullptr || Ah == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    a = Ah->data;
    /*LU-Dekomposition*/
    rc = ludecomp(a, n, indx, &d);

    if (rc != OK)
      {
        free(indx);
        FreeMatrix(Ah);

        if (rc == NO_MEM)
          throw IceException(FNAME, M_NO_MEM);

        if (rc == NUM_INSTABILITY)
          throw IceException(FNAME, M_MATRIX_SINGULAR);

        return (0);
      }

    for (i = 0; i < n; i++)
      {
        d *= a[i][i];
      }

    free(indx);
    FreeMatrix(Ah);
    return (d);
  }
#undef FNAME
#endif
}
