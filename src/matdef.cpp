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
/********************************************************************
  funktionen zur matrixverwaltung

  abraham 11/95
********************************************************************/

#include <malloc.h>
#include <stdio.h>

#include "IceException.h"
#include "macro.h"
#include "defs.h"
#include "matdef.h"

namespace ice
{
  static MatrixStruct first_mat = NULL, last_mat = NULL;

  /* Verwaltung der Matrizen */
  /* Anlegen einer Matrix A[0..rsize-1][0..csize-1]*/
#define FNAME "NewMatrix"
  MatrixStruct NewMatrix(int type, int rsize, int csize)
  {
    MatrixStruct mat;
    double* dptr;
    int* iptr, i;
    unsigned char* cptr;

    if (csize < 1 || rsize < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return (nullptr);
      }

    mat = (MatrixStruct)malloc(sizeof(struct MatrixStruct_));
    /* Daten eintragen */
    mat->csize = csize;
    mat->rsize = rsize;
    mat->type = type;

    if (mat == nullptr)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return (nullptr);
      }

    if (last_mat == nullptr)
      {
        first_mat = mat;
        last_mat = mat;
        mat->prev = mat->next = nullptr;
      }
    else
      {
        last_mat->next = mat;
        mat->prev = last_mat;
        last_mat = mat;
        mat->next = nullptr;
      }

    /* den Speicher fuer die Matrixelemente */
    switch (type)
      {
      case MAT_DOUBLE: /* Double */
        mat->data = (double**)malloc(rsize * sizeof(double*));

        if (mat->data == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return (nullptr);
          }

        mat->data[0] = (double*)malloc(csize * rsize * sizeof(double));

        if (mat->data[0] == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            free(mat->data);
            free(mat);
            return (nullptr);
          }

        for (dptr = mat->data[0], i = 1; i < rsize; i++)
          {
            mat->data[i] = dptr += csize;
          }

        mat->datai = nullptr;
        mat->datac = nullptr;
        break;
      case MAT_INT: /* Integer */
        mat->datai = (int**)malloc(rsize * sizeof(int*));

        if (mat->datai == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return (nullptr);
          }

        mat->datai[0] = (int*)malloc(csize * rsize * sizeof(int));

        if (mat->datai[0] == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            free(mat->datai);
            free(mat);
            return (nullptr);
          }

        for (iptr = mat->datai[0], i = 1; i < rsize; i++)
          {
            mat->datai[i] = iptr += csize;
          }

        mat->data = nullptr;
        mat->datac = nullptr;
        break;
      case MAT_CHAR: /* char */
        mat->datac = (unsigned char**)malloc(rsize * sizeof(char*));

        if (mat->datac == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return (nullptr);
          }

        mat->datac[0] = (unsigned char*)malloc(csize * rsize * sizeof(char));

        if (mat->datac[0] == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            free(mat->datac);
            free(mat);
            return (nullptr);
          }

        for (cptr = mat->datac[0], i = 1; i < rsize; i++)
          {
            mat->datac[i] = cptr += csize;
          }

        mat->data = nullptr;
        mat->datai = nullptr;
        break;
      default:
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        free(mat);
        return (nullptr);
      }

    return (mat);
  }
#undef FNAME

  /* Test ob Pointer in der Liste */
  int IsMatrix(MatrixStruct mat)
  {
    MatrixStruct tmat = first_mat;

    while (tmat != nullptr)
      {
        if (mat == tmat)
          {
            return (true);
          }

        tmat = tmat->next;
      }

    return (false);
  }

  /* Freigeben einer Matrix */
#define FNAME "FreeMatrix"
  int FreeMatrix(MatrixStruct mat)
  {
    if (IsMatrix(mat) == false)
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return (WRONG_MATRIX);
      }

    switch (mat->type)
      {
      case MAT_DOUBLE:
        free(mat->data[0]);
        free(mat->data);
        break;
      case MAT_INT:
        free(mat->datai[0]);
        free(mat->datai);
        break;
      case MAT_CHAR:
        free(mat->datac[0]);
        free(mat->datac);
        break;
      }

    /* Aushaengen aus Kette */
    if (first_mat == mat)
      {
        first_mat = mat->next;
      }

    if (last_mat == mat)
      {
        last_mat = mat->prev;
      }

    if (mat->next != nullptr)
      {
        mat->next->prev = mat->prev;
      }

    if (mat->prev != nullptr)
      {
        mat->prev->next = mat->next;
      }

    free(mat);
    return (OK);
  }
#undef FNAME

  /* Transponieren einer Matrix */
#define FNAME "TranspMat"
  MatrixStruct TranspMat(MatrixStruct m1, MatrixStruct m2)
  {
    MatrixStruct dmat;
    int i, j;

    /* Test der Matrizen */
    if (IsMatrix(m1) == false)
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return (nullptr);
      }

    if (m2 != nullptr)
      {
        if (IsMatrix(m2) == false)
          {
            throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
            return (nullptr);
          }
        else
          {
            if (m1->type != m2->type || m1->csize != m2->rsize || m1->rsize != m2->csize)
              {
                throw IceException(FNAME, M_MAT_NO_COMPAT, MAT_NO_COMPAT);
                return (nullptr);
              }

            dmat = m2;
          }
      }
    else
      {
        dmat = NewMatrix(m1->type, m1->csize, m1->rsize);
      }

    /* jetzt endlich passierts */
    switch (m1->type)
      {
      case MAT_DOUBLE:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m1->csize; j++)
            {
              dmat->data[j][i] = m1->data[i][j];
            }

        break;
      case MAT_INT:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m1->csize; j++)
            {
              dmat->datai[j][i] = m1->datai[i][j];
            }

        break;
      case MAT_CHAR:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m1->csize; j++)
            {
              dmat->datac[j][i] = m1->datac[i][j];
            }

        break;
      }

    return (dmat);
  }
#undef FNAME

  /* Kopieren einer Matrix */
#define FNAME "MoveMat"
  MatrixStruct MoveMat(MatrixStruct m1, MatrixStruct m2)
  {
    MatrixStruct dmat;
    int i, j;

    /* Test der Matrizen */
    if (IsMatrix(m1) == false)
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return (nullptr);
      }

    if (m2 != nullptr)
      {
        if (IsMatrix(m2) == false)
          {
            throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
            return (nullptr);
          }
        else
          {
            if (m1->type != m2->type || m1->csize != m2->csize || m1->rsize != m2->rsize)
              {
                throw IceException(FNAME, M_MAT_NO_COMPAT, MAT_NO_COMPAT);
                return (nullptr);
              }

            dmat = m2;
          }
      }
    else
      {
        dmat = NewMatrix(m1->type, m1->rsize, m1->csize);
      }

    /* jetzt endlich passierts */
    switch (m1->type)
      {
      case MAT_DOUBLE:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m1->csize; j++)
            {
              dmat->data[i][j] = m1->data[i][j];
            }

        break;
      case MAT_INT:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m1->csize; j++)
            {
              dmat->datai[i][j] = m1->datai[i][j];
            }

        break;
      case MAT_CHAR:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m1->csize; j++)
            {
              dmat->datac[i][j] = m1->datac[i][j];
            }

        break;
      }

    return (dmat);
  }
#undef FNAME

  /*Matrizen Multiplizieren*/
#define FNAME "MulMat"
  MatrixStruct MulMat(MatrixStruct m1, MatrixStruct m2, MatrixStruct m3)
  {
    int i, j, k;

    /*Parametertest*/
    if (!IsMatrix(m1) || !IsMatrix(m2))
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return (nullptr);
      }

    if (m1->type != m2->type || m1->csize != m2->rsize)
      {
        throw IceException(FNAME, M_MAT_NO_COMPAT, MAT_NO_COMPAT);
        return (nullptr);
      }

    if (m3 != nullptr)
      {
        if (!IsMatrix(m3))
          {
            throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
            return (nullptr);
          }

        if (m1->type != m3->type || m3->rsize != m1->rsize || m3->csize != m2->csize)
          {
            throw IceException(FNAME, M_MAT_NO_COMPAT, MAT_NO_COMPAT);
            return (nullptr);
          }
      }
    /*Zielmatrix anfordern*/
    else
      {
        m3 = NewMatrix(m1->type, m1->rsize, m2->csize);

        if (m3 == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return (nullptr);
          }
      }

    /*hier geht's los*/
    switch (m1->type)
      {
      case MAT_DOUBLE:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m2->csize; j++)
            for (k = 0, m3->data[i][j] = 0; k < m1->csize; k++)
              {
                m3->data[i][j] += m1->data[i][k] * m2->data[k][j];
              }

        break;
      case MAT_INT:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m2->csize; j++)
            for (k = 0, m3->datai[i][j] = 0; k < m1->csize; k++)
              {
                m3->datai[i][j] += m1->datai[i][k] * m2->datai[k][j];
              }

        break;
      case MAT_CHAR:

        for (i = 0; i < m1->rsize; i++)
          for (j = 0; j < m2->csize; j++)
            for (k = 0, m3->datac[i][j] = 0; k < m1->csize; k++)
              {
                m3->datac[i][j] += m1->datac[i][k] * m2->datac[k][j];
              }

        break;
      }

    return (m3);
  }
#undef FNAME

  /*Multiplikation einer Matrix mit einem Vektor*/
#define FNAME "MulMatVec"
  int MulMatVec(MatrixStruct A, double* b, double* x)
  {
    int i, j;

    if (!IsMatrix(A))
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return (WRONG_MATRIX);
      }

    switch (A->type)
      {
      case MAT_DOUBLE:

        for (i = 0; i < A->rsize; i++)
          for (j = 0, x[i] = 0; j < A->csize; j++)
            {
              x[i] += A->data[i][j] * b[j];
            }

        break;
      case MAT_INT:

        for (i = 0; i < A->rsize; i++)
          for (j = 0, x[i] = 0; j < A->csize; j++)
            {
              x[i] += A->datai[i][j] * b[j];
            }

        break;
      case MAT_CHAR:

        for (i = 0; i < A->rsize; i++)
          for (j = 0, x[i] = 0; j < A->csize; j++)
            {
              x[i] += A->datac[i][j] * b[j];
            }

        break;
      }

    return (OK);
  }
#undef FNAME
}
