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
#include <stdio.h>
#include <float.h>
#include <string.h>

#include "IceException.h"
#include "defs.h"

#include "vectorarrayfunctions.h"
#include "numbase.h"
#include "rooteig.h"
#include "matrix_function.h"

namespace ice
{
  // private functions
  int rank_matrix(double* m, int row, int col, int* rank, double* det);

  //
  //  Matrixrechnung
  //
  //----------------------------------------------
#define FNAME "MoveMatrix"
  double* MoveMatrix(double* m1, int row, int col, double* m2)
  {
    int i;
    double* mptr;

    if (m1 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m2 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));

        for (i = 0; i < row * col; i++)
          {
            *(mptr + i) = m1[i];
          }

        return mptr;
      }

    for (i = 0; i < row * col; i++)
      {
        m2[i] = m1[i];
      }

    return m2;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "DefMatrix"
  double* DefMatrix(double* m, int row, int col, int option)
  {
    int i, j;

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m == NULL)
      {
        m = (double*)malloc(row * col * sizeof(double));
      }

    switch (option)
      {
      case UNIT:

        for (j = 0; j < row; j++)
          for (i = 0; i < col; i++)
            if (i == j)
              {
                *(m + (j * col) + i) = 1;
              }
            else
              {
                *(m + (i * col) + j) = 0;
              }

        return m;
      case ZERO:

        for (i = 0; i < row * col; i++)
          {
            *(m + i) = 0;
          }

        return m;
      default:
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }
  }
#undef FNAME
//----------------------------------------------
#define FNAME "SetMatrix"
  double* SetMatrix(double* m1, int row, int col, double val)
  {
    int i;
    double* mptr;

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m1 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));

        for (i = 0; i < row * col; i++)
          {
            *(mptr + i) = val;
          }

        return mptr;
      }

    for (i = 0; i < row * col; i++)
      {
        m1[i] = val;
      }

    return m1;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "ScaleMatrix"
  double* ScaleMatrix(double* m1, int row, int col, double fac, double* m2)
  {
    int i;
    double* mptr;

    if (m1 == NULL)
      {
        throw IceException(FNAME , M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME , M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m2 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));

        for (i = 0; i < row * col; i++)
          {
            *(mptr + i) = fac * m1[i];
          }

        return mptr;
      }

    for (i = 0; i < row * col; i++)
      {
        m2[i] = fac * m1[i];
      }

    return m2;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "AddMatrix"
  double* AddMatrix(double* m1, double* m2, int row, int col, double* m3)
  {
    int i;
    double* mptr;

    if (m1 == NULL || m2 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m3 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));

        for (i = 0; i < row * col; i++)
          {
            *(mptr + i) = m1[i] + m2[i];
          }

        return mptr;
      }

    for (i = 0; i < row * col; i++)
      {
        m3[i] = m1[i] + m2[i];
      }

    return m3;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "SubMatrix"
  double* SubMatrix(double* m1, double* m2, int row, int col, double* m3)
  {
    int i;
    double* mptr;

    if (m1 == NULL || m2 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m3 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));

        for (i = 0; i < row * col; i++)
          {
            *(mptr + i) = m1[i] - m2[i];
          }

        return mptr;
      }

    for (i = 0; i < row * col; i++)
      {
        m3[i] = m1[i] - m2[i];
      }

    return m3;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "TranspMatrix"
  double* TranspMatrix(double* m1, int row, int col, double* m2)
  {
    int i, j;
    double* mh, *mptr;

    if (m1 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m2 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));

        for (i = 0; i < row; i++)
          for (j = 0; j < col; j++)
            {
              mptr[j * row + i] = m1[i * col + j];
            }

        return mptr;
      }

    mh = (double*) malloc(row * col * sizeof(double));

    if (mh == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NULL;
      }

    for (i = 0; i < row; i++)
      for (j = 0; j < col; j++)
        {
          mh[j * row + i] = m1[i * col + j];
        }

    MoveMatrix((double*)mh, col, row, (double*)m2);
    free(mh);
    return m2;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "MulMatrix"
  double* MulMatrix(double* m1, double* m2, int row1, int col1, int col2, double* m3)
  {
    int i, j, k;
    double* mh, *mptr;

    if (m1 == NULL || m2 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (row1 < 1 || col1 < 1 || col2 < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (m3 == NULL)
      {
        mptr = (double*)malloc(row1 * col2 * sizeof(double));

        for (i = 0; i < row1; i++)
          {
            for (j = 0; j < col2; j++)
              {
                mptr[i * col2 + j] = 0;

                for (k = 0; k < col1; k++)
                  {
                    mptr[i * col2 + j] += m1[i * col1 + k] * m2[k * col2 + j];
                  }
              }
          }

        return mptr;
      }

    mh = (double*) malloc(row1 * col2 * sizeof(double));

    if (mh == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NULL;
      }

    for (i = 0; i < row1; i++)
      {
        for (j = 0; j < col2; j++)
          {
            mh[i * col2 + j] = 0;

            for (k = 0; k < col1; k++)
              {
                mh[i * col2 + j] += m1[i * col1 + k] * m2[k * col2 + j];
              }
          }
      }

    MoveMatrix((double*)mh, row1, col2, (double*)m3);
    free(mh);
    return m3;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "InvertMatrix"
  double* InvertMatrix(double* m1, int dim, double* m2)
  {
    double* mptr = NULL;
    int c, i, j, k, col, colh, row, rowh, offs, ret;
    double maxa, max, fh, fha;
    double epsinst = 1e-100; // Grenze der Instabilitॊt
    double epsnull = 1e-200; // Definition 0
    double* dpa, *dpc;
    int size_a;

    col = 0;
    row = 0;
    maxa = 0;

    if (m1 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (m2 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));
      }
    else
      {
        mptr = m2;
      }

    size_a = dim * dim * sizeof(double);
    // Anforderung des dynamischen Speichers
    dpa = (double*) malloc(size_a);

    if (dpa == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);

        if (m2 == NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    dpc = (double*)malloc(size_a);

    if (dpc == NULL)
      {
        free(dpa);

        if (m2 == NULL)
          {
            free(mptr);
          }

        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NULL;
      }

    // kopieren in dynamischen speicherbereich
    memcpy(dpa, m1, size_a);

    // füllen der "Einheitsmatrix " und normalisieren dpa
    for (i = 0; i < dim * dim; i++)
      {
        dpc[i] = 0;
      }

#if 0

    for (i = 0; i < dim; i++)
      {
        fh = 0;

        for (j = 0; j < dim; j++)
          {
            fh += fabs(dpa[dim * i + j]);
          }

        fh /= dim;
        fh = sqrt(fh);

        for (j = 0; j < dim; j++)
          {
            dpa[dim * i + j] /= fh;
          }

        dpc[i * dim + i] = 1 / fh;
      }

#else

    for (i = 0; i < dim; i++)
      {
        dpc[i * dim + i] = 1;
      }

#endif

    // transformation des dynamischen A|C
    offs = 0;
    col = 0;
    ret = OK;

    while ((col < dim) && (!offs))
      // setzen der spalte "col" zu 0
      {
        max = 0;
        colh = col - offs;
        rowh = colh;
        // bestimmen des maximums der spalte "col"
        row = colh;

        while (row < dim)
          {
            fha = dpa[dim * row + col];
            fh = fabs(fha);

            if (fh > max)
              {
                maxa = fha;
                max = fh;
                rowh = row;
              }

            row++;
          }

        // transformation der zeilen bezüglich Spalte "col"
        if (rowh != colh)
          // tauschen der zeilen "rowh" und "colh"
          {
            for (c = col; c < dim; c++)
              {
                fh = dpa[dim * rowh + c];
                dpa[dim * rowh + c] = dpa[dim * colh + c];
                dpa[dim * colh + c] = fh;
              }

            for (c = 0; c < dim; c++)
              {
                fh = dpc[dim * rowh + c];
                dpc[dim * rowh + c] = dpc[dim * colh + c];
                dpc[dim * colh + c] = fh;
              }
          }

        // setzen der spalte "col" zu 0 ab zeile "colh+1"
        if (fabs(dpa[dim * colh + col] / (*dpa)) < epsinst)
          {
            ret = NUM_INSTABILITY;
          }

        if (fabs(dpa[dim * colh + col]) > epsnull)
          {
            i = colh + 1;

            while (i < dim)
              {
                fh = dpa[dim * i + col] / maxa;
                dpa[dim * i + col] = 0;

                for (j = col + 1; j < dim; j++)
                  {
                    dpa[dim * i + j] -= dpa[dim * colh + j] * fh;
                  }

                for (k = 0; k < dim; k++)
                  {
                    dpc[dim * i + k] -= dpc[colh * dim + k] * fh;
                  }

                i++;
              }
          }
        else
          {
            offs = 1;
          }

        col++;
      };

    if (offs == 0)
      {
        // berechnen der Lösungsvektoren
        for (k = 0; k < dim; k++)
          {
            for (i = dim - 1; i >= 0; i--)
              {
                fh = 0;
                j = dim - 1;

                while (j > i)
                  {
                    fh += dpa[dim * i + j] * mptr[j * dim + k];
                    j--;
                  }

                mptr[dim * i + k] = (dpc[i * dim + k] - fh) / dpa[dim * i + i];
              }
          }
      }
    else
      {
        free(dpa);
        free(dpc);

        if (m2 == NULL)
          {
            free(mptr);
          }

        throw IceException(FNAME, M_NO_INVERSE, NO_SOLUTION);
        return NULL;
      }

    free(dpa);
    free(dpc);

    if (ret == NUM_INSTABILITY)
      {
        if (m2 == NULL)
          {
            free(mptr);
          }

        throw IceException(FNAME, M_NUM_INSTABILITY, NUM_INSTABILITY);
        return NULL;
      }

    return mptr;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "OrthoMatrix"
  /* Orthonormalisierung von "row" Zeilenvektoren */
  double* OrthoMatrix(double* m1, int row, int col, double* m2)
  {
    int rang = 0;
    double det;
    double* hx, *hb1, *hb2;
    double d1, d2;
    int i, k;
    int size1, size2;
    double* mptr = NULL;

    /* Test Parmeterbelegung */
    if (m1 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (m2 == NULL)
      {
        mptr = (double*)malloc(row * col * sizeof(double));
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);

        if (mptr != NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    if (row > col)
      {
        throw IceException(FNAME, M_VEC_DEPEND, WRONG_PARAM);

        if (mptr != NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    rank_matrix(m1, row, col, &rang, &det);

    if (rang < row)
      {
        throw IceException(FNAME, M_VEC_DEPEND, WRONG_PARAM);

        if (mptr != NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    size1 = row * col * sizeof(double);
    /* Anforderung des dynamischen Speichers */
    hx = (double*) malloc(size1);

    if (hx == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);

        if (mptr != NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    memcpy(hx, m1, size1);

    /* Anforderung der Hilfsvektoren hb1, hb2 */
    size2 = col * sizeof(double);
    hb1 = (double*) malloc(size2);

    if (hb1 == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        free(hx);

        if (mptr != NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    hb2 = (double*) malloc(size2);

    if (hb2 == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        free(hx);
        free(hb1);

        if (mptr != NULL)
          {
            free(mptr);
          }

        return NULL;
      }

    /* Berechnung des Orthogonalsystems */
    NormVecRn(hx, col, hx);

    for (k = 1; k < row; k++)
      {
        /* Löschen des Hilfsvektors */
        for (i = 0; i < row; i++)
          {
            hb2[i] = 0;
          }

        for (i = 0; i < k; i++)
          {
            d1 = ScalProdVecRn(&hx[k * col], &hx[i * col], col);
            d2 = ScalProdVecRn(&hx[i * col], &hx[i * col], col);
            MoveVecRn(&hx[i * col], col, hb1);
            ScaleVecRn(hb1, col, d1 / d2, hb1);
            AddVecRn(hb1, hb2, col, hb2);
          }

        SubVecRn(&hx[k * col], hb2, col, &hx[k * col]);
        NormVecRn(&hx[k * col], col, &hx[k * col]);

      }

    if (m2 == NULL)
      {
        memcpy(mptr, hx, size1);
        free(hx);
        free(hb1);
        free(hb2);
        return mptr;
      }

    memcpy(m2, hx, size1);
    free(hx);
    free(hb1);
    free(hb2);
    return m2;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "GenOrthoSystem"
  int GenOrthoSystem(double n[3], double m[3][3])
  {
    int i;
    double ah[3][3];
    double v[3];
    double unit[3][3] =
    {
      {1, 0, 0},
      {0, 1, 0},
      {0, 0, 1}
    };
    double eps = 1e-3;
    /* Berechnung eines Koordinatensystem OUVW */
    MoveVec(n, ah[0]);
    i = 1;
    CrossProdVec(n, unit[0], v);

    if (LengthVec(v) > eps)
      {
        MoveVec(unit[0], ah[i++]);
      }

    CrossProdVec(n, unit[1], v);

    if (LengthVec(v) > eps)
      {
        MoveVec(unit[1], ah[i++]);
      }

    if (i < 3)
      {
        MoveVec(unit[2], ah[2]);
      }

    OrthoMatrix((double*)ah, 3, 3, (double*)m);
    return OK;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "rank_matrix"
  int rank_matrix(double* m, int row, int col, int* rank, double* det)
  {
    int i;
    int ii, jj;
    int size, dim, rang = 0;
    int cont1, cont2;
    double* hA;
    double maxcol;
    int rowmem, colmem;
    double h1, h2;
    double eps = 1e-8;
    int flag;
    rowmem = 0;
    colmem = 0;

    if (m == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return (WRONG_VECTOR);
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    /* Test auf Trivialfall */
    flag = 1;

    if (row == 1)
      {
        for (i = 0; i < col; i++)
          {
            if (fabs(m[i]) > eps)
              {
                if (col == 1)
                  {
                    *det = m[0];
                    *rank = 1;
                    return OK;
                  }
                else
                  {
                    *det = 0;
                    *rank = 1;
                    return OK;
                  }
              }
          }

        *det = 0;
        *rank = 0;
        return OK;
      }

    /*** Anforderung des dynamischen Speichers für m*/
    size = row * col * sizeof(double);
    hA = (double*) malloc(size);

    if (hA == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    memcpy(hA, m, size);
    /*** Transformation von (m,b) in Trapezform */
    rang = 0;
    dim = Min(row, col);
    cont1 = 1;

    for (i = 0; i < dim - 1 && cont1 == 1; i++)
      {
        /* Bestimmen des Maximums der i'ten Restspalte */
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
                flag *= -1;

                for (jj = 0; jj < col; jj++)
                  {
                    h1 = hA[i * col + jj];
                    hA[i * col + jj] = hA[rowmem * col + jj];
                    hA[rowmem * col + jj] = h1;
                  }
              }

            /* Transformation der Matrix hA */
            for (ii = i + 1; ii < row; ii++)
              {
                h1 = hA[ii * col + i] / hA[i * col + i];

                for (jj = 0; jj < col; jj++)
                  {
                    h2 = hA[i * col + jj] * h1;
                    hA[ii * col + jj] -= h2;
                  }
              }

            rang++;
          }
      }

    if (fabs(hA[(dim - 1)*col + dim - 1]) > eps)
      {
        rang += 1;
      }

    *rank = rang;

    if (rang < dim)
      {
        *det = 0;
      }
    else
      {
        *det = 1;

        for (i = 0; i < dim; i++)
          {
            *det *= hA[i * col + i];
          }
      }

    *det *= (double)flag;
    free(hA);
    return (OK);
  }
#undef FNAME
//---------------------------------------------------
#define FNAME "NormMatrix"
  int NormMatrix(double* m, int row, int col, int mode, double* norm)
  /*...................................................................
  Von der auf Gleitkomma-Matrix "a" werden in Abhaengigkeit von
  mode einzelne Normen auf norm bereitgestellt.

  mode = 0  - (DEFAULT) Wurzel aus der Summe der Koeffizientenquadrate
  mode = 1  - Maximum aus den Summen der Absolutwerte der Zeilen
  mode = 2  - Maximum aus den Summen der Absolutwerte der Spalten
  mode = 3  - Maximum der Absolutwerte der Eigenwerte
        (fuer symmetrische Matrizen)
  ....................................................................*/
  {
    int i, j;
    double val;
    double* eval, *evec;

    double eps = 1e-5;
    i = 0;

    if (m == NULL)
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    switch (mode)
      {
      case DEFAULT:
        val = 0;

        for (i = 0; i < row; i++)
          for (j = 0; j < col; j++)
            {
              val += Sqr(*(m + i * col + j));
            }

        *norm = sqrt(val);
        return OK;
      case 1:
        *norm = 0;

        for (i = 0; i < row; i++)
          {
            val = 0;

            for (j = 0; j < col; j++)
              {
                val += m[i * col + j];
              }

            if (val > *norm)
              {
                *norm = val;
              }
          }

        return OK;
      case 2:
        *norm = 0;

        for (j = 0; j < col; j++)
          {
            val = 0;

            for (j = 0; j < row; j++)
              {
                val += m[j * col + i];
              }

            if (val > *norm)
              {
                *norm = val;
              }
          }

        return OK;
      case 3:

        if (row != col)
          {
            throw IceException(FNAME, M_NO_SQUARE, WRONG_PARAM);
            return WRONG_PARAM;
          }

        if (IsMatrixSymm(m, row, &eps) != true)
          {
            throw IceException(FNAME, M_NO_SYMM, WRONG_PARAM);
            return WRONG_PARAM;
          }

        eval = (double*)malloc(row * sizeof(double));
        evec = (double*)malloc(row * row * sizeof(double));

        IF_FAILED(EigenVal(m, row, eval, evec))
        {
          free(eval);
          free(evec);
          throw IceException(FNAME, M_NO_REGULAR, WRONG_PARAM);
        }

        *norm = eval[row - 1];
        free(eval);
        free(evec);
        return OK;
      default:
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }
  }
#undef FNAME
//--------------------------------------------------------
#define FNAME "DetMatrix"
  int DetMatrix(double* a, int row, int col, double* det, int* rank)
  {
    int r, i, ii, jj, rowh, colh;
    int perm;
    double d1, max, eps;
    double* ha; // Hilfsfeld fuer Transformation

    rowh = 0;
    colh = 0;

    if (a == NULL)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    /* Anforderung des dynamischen Speichers */
    ha = (double*) malloc(row * col * sizeof(double));

    if (ha == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        free(ha);
        return (NO_MEM);
      }

    /* Kopieren des Orginalbereiches in Hilfsbereich */
    memcpy(ha, a, row * col * sizeof(double));

    r = Min(row, col);
    perm = 0;

    if (NormMatrix(ha, row, col, DEFAULT, &eps) != OK)
      {
        free(ha);
        throw IceException(FNAME, M_0, ERROR);
      }

    eps = eps * 1e-60;

    /* Transformation der Rest-Nullspalten */
    for (i = 0; i < r; i++)
      {
        max = -HUGE_VAL;

        /* Such nach Maximum in der Restmatrix */
        for (ii = i; ii < col; ii++)
          for (jj = i; jj < row; jj++)
            {
              d1 = fabs(*(ha + jj * col + ii));

              if (d1 > max)
                {
                  max = d1;
                  rowh = jj;
                  colh = ii;
                }
            }

        if (max <= eps)
          {
            *rank = i;
            *det = 0;
            free(ha);
            return OK;
          }

        /* Tausch der i-ten mit der rowh-ten Zeile */
        if (i != rowh)
          {
            perm++; /* Zeilentausch negiert Determinante - spaetere Auswertung */

            ChangeMatrixRow(ha, i, rowh, col);
          }

        /* Tausch der i-ten mit der colh-ten Spalte */
        if (i != colh)
          {
            perm++; /* Spaltentausch negiert Determinante - spaetere Auswertung */

            ChangeMatrixCol(ha, i, colh, row, col);
          }

        /* Erzeugen der Rest-Nullspalte */
        if (i < r - 1)
          for (jj = i + 1; jj < row; jj++)
            {
              d1 = *(ha + jj * col + i) / (*(ha + i * col + i));
              *(ha + jj * col + i) = 0;

              for (ii = i + 1; ii < col; ii++)
                {
                  *(ha + jj * col + ii) -= *(ha + i * col + ii) * d1;
                }
            }
      }

    *rank = r;

    if (r < col)
      {
        *det = 0;
      }
    else
      {
        *det = *ha;

        for (i = 1; i < r; i++)
          {
            *det *= *(ha + i * col + i);
          }

        if (perm % 2 == 1)
          {
            *det = -*det;
          }
      }

    free(ha);
    return OK;
  }
#undef FNAME
//-------------------------------
#define FNAME "ChangeMatrixRow"
  double* ChangeMatrixRow(double* m, int row1, int row2, int col)
  {
    int i;

    if (m == NULL)
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_PARAM);
        return NULL;
      }

    if (row1 < 0 || row2 < 0 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (row1 != row2)
      for (i = 0; i < col; i++)
        {
          std::swap(*(m + row1 * col + i), *(m + row2 * col + i));
        }

    return m;
  }
#undef FNAME
//----------------------------------------------------
#define FNAME "ChangeMatrixCol"
  double* ChangeMatrixCol(double* m, int col1, int col2, int row, int col)
  {
    int i;

    if (m == NULL)
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_PARAM);
        return NULL;
      }

    if (col1 < 0 || col2 < 0 || row < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (col1 != col2)
      for (i = 0; i < row; i++)
        {
          std::swap(*(m + i * col + col1), *(m + i * col + col2));
        }

    return m;
  }
#undef FNAME
//----------------------------------------------------
#define FNAME "IsMatrixZero"
  int IsMatrixZero(double* m, int row, int col, double* eps)
  {
    int i, j;
    double val;

    if (row < 1 || col < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    val = 0;

    for (i = 0; i < row; i++)
      for (j = 0; j < col; j++)
        {
          val = Max(val, fabs(m[i * col + j]));
        }

    if (val < *eps)
      {
        *eps = val;
        return true;
      }

    *eps = val;
    return false;
  }
#undef FNAME
//--------------------------------------------------------
#define FNAME "IsMatrixUnit"
  int IsMatrixUnit(double* m, int dim, double* eps)
  {
    int i, j;
    double val;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    val = 0;

    for (i = 0; i < dim; i++)
      for (j = 0; j < dim; j++)
        if (i == j)
          {
            val = Max(val, fabs(m[i * dim + j] - 1));
          }
        else
          {
            val = Max(val, fabs(m[i * dim + j]));
          }

    if (val < *eps)
      {
        *eps = val;
        return true;
      }

    *eps = val;
    return false;
  }
#undef FNAME
//-----------------------------------------
#define FNAME "IsMatrixOrtho"
  int IsMatrixOrtho(double* m, int dim, double* eps)
  {
    double* m1, *m2;
    int flag;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    m1 = (double*)malloc(dim * dim * sizeof(double));
    m2 = (double*)malloc(dim * dim * sizeof(double));
    TranspMatrix(m, dim, dim, m1);
    MulMatrix(m, m1, dim, dim, dim, m2);
    flag = IsMatrixOrtho(m2, dim, eps);
    free(m1);
    free(m2);

    if (flag == true)
      {
        return true;
      }
    else
      {
        return false;
      }
  }
#undef FNAME
//------------------------------------------------
#define FNAME "IsMatrixDiag"
  int IsMatrixDiag(double* m, int dim, double* eps)
  {
    int i, j;
    double val;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    val = 0;

    for (i = 0; i < dim; i++)
      for (j = 0; j < dim; j++)
        if (i != j)
          {
            val = Max(val, fabs(m[i * dim + j]));
          }

    if (val < *eps)
      {
        *eps = val;
        return true;
      }

    *eps = val;
    return false;
  }
#undef FNAME
//--------------------------------------------------
#define FNAME "IsMatrixSymm"
  int IsMatrixSymm(double* m, int dim, double* eps)
  {
    int i, j;
    double val;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    val = 0;

    for (i = 0; i < dim; i++)
      for (j = i + 1; j < dim; j++)
        if (i != j)
          {
            val = Max(val, fabs(m[i * dim + j] - m[j * dim + i]));
          }

    if (val < *eps)
      {
        *eps = val;
        return true;
      }

    *eps = val;
    return false;
  }
#undef FNAME
//--------------------------------------------
#define FNAME "IsMatrixAnti"
  int IsMatrixAnti(double* m, int dim, double* eps)
  {
    int i, j;
    double val;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    val = 0;

    for (i = 0; i < dim; i++)
      for (j = i + 1; j < dim; j++)
        if (i != j)
          {
            val = Max(val, fabs(m[i * dim + j] + m[j * dim + i]));
          }

    if (val < *eps)
      {
        *eps = val;
        return true;
      }

    *eps = val;
    return false;
  }
#undef FNAME
//-----------------------------------------
#define FNAME "IsMatrixRegular"
  int IsMatrixRegular(double* m, int dim, double* eps)
  {
    double val;
    int rank;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (*eps < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    DetMatrix(m, dim, dim, &val, &rank);

    if (fabs(val) > *eps)
      {
        *eps = val;
        return true;
      }
    else
      {
        *eps = val;
        return false;
      }
  }
#undef FNAME
//----------------------------------------------
#define FNAME "IsMatrixPosDef"
  int IsMatrixPosDef(double* m, int dim)
  {
    double* eval, *evec;
    int i;

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    eval = (double*)malloc(dim * sizeof(double));
    evec = (double*)malloc(dim * dim * sizeof(double));

    i = EigenVal(m, dim, eval, evec);

    if (i != OK)
      {
        free(eval);
        free(evec);
        throw IceException(FNAME, M_NO_REGULAR, WRONG_PARAM);
      }

    free(evec);

    for (i = 0; i < dim; i++)
      if (eval[i] <= 0)
        {
          free(eval);
          return false;
        }

    free(eval);
    return true;
  }
#undef FNAME
}
