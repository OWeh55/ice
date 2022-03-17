/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "IceException.h"

#include "fit.h"
#include "matdef.h"
#include "sort.h"

namespace ice
{
#define FNAME "SortPointList"
  double* sortarray;
  int plsort_Compare(const void* i1, const void* i2)
  {
    if (sortarray[*((const int*)i1)] < sortarray[*((const int*)i2)])
      {
        return -1;
      }

    if (sortarray[*((const int*)i1)] > sortarray[*((const int*)i2)])
      {
        return  1;
      }

    return 0;
  }

  PointList SortPointList(PointList pl, int col, int mode)
  {
    int i, j, *ix;
    PointList pls;

    /*zu sortierendes Feld auswählen*/
    switch (col)
      {
      case 0:
        sortarray = pl->xptr;
        break;
      case 1:
        sortarray = pl->yptr;
        break;
      case 2:
        sortarray = pl->wptr;
        break;
      default:
        throw IceException(FNAME, M_WRONG_PARAMETER);
      }

    /*Indexfeld anlegen*/
    ix = (int*)malloc(pl->lng * sizeof(int));

    if (ix == NULL)
      throw IceException(FNAME, M_NO_MEM);

    for (i = 0; i < pl->lng; i++)
      {
        ix[i] = i;
      }

    /*Indexfeld sortieren*/
    qsort(ix, pl->lng, sizeof(int), plsort_Compare);
    /*neue Punktliste anlegen*/
    pls = NewPointList(pl->lng);

    if (pls == NULL)
      {
        free(ix);
        throw IceException(FNAME, M_NO_MEM);
      }

    /*Punkte in Reihenfolge übertragen*/
    if (mode >= 0)
      {
        for (i = 0; i < pl->lng; i++)
          {
            PutPoint(pls, i, pl->xptr[ix[i]], pl->yptr[ix[i]], pl->wptr[ix[i]]);
          }
      }
    else
      {
        for (i = 0, j = pl->lng - 1; i < pl->lng; i++, j--)
          {
            PutPoint(pls, j, pl->xptr[ix[i]], pl->yptr[ix[i]], pl->wptr[ix[i]]);
          }
      }

    free(ix);
    return pls;
  }
#undef FNAME
#define FNAME "SortMatrix"
  int sortcol;
  double**          sortdata_d;
  int**             sortdata_i;
  unsigned char**   sortdata_c;

  int sortmat_Compare_d(const void* i1, const void* i2)
  {
    if (sortdata_d[*((const int*)i1)][sortcol] < sortdata_d[*((const int*)i2)][sortcol])
      {
        return -1;
      }

    if (sortdata_d[*((const int*)i1)][sortcol] > sortdata_d[*((const int*)i2)][sortcol])
      {
        return  1;
      }

    return 0;
  }

  int sortmat_Compare_i(const void* i1, const void* i2)
  {
    if (sortdata_i[*((const int*)i1)][sortcol] < sortdata_i[*((const int*)i2)][sortcol])
      {
        return -1;
      }

    if (sortdata_i[*((const int*)i1)][sortcol] > sortdata_i[*((const int*)i2)][sortcol])
      {
        return  1;
      }

    return 0;
  }

  int sortmat_Compare_c(const void* i1, const void* i2)
  {
    if (sortdata_c[*((const int*)i1)][sortcol] < sortdata_c[*((const int*)i2)][sortcol])
      {
        return -1;
      }

    if (sortdata_c[*((const int*)i1)][sortcol] > sortdata_c[*((const int*)i2)][sortcol])
      {
        return  1;
      }

    return 0;
  }

  MatrixStruct SortMatrix(MatrixStruct mat, int col, int mode)
  {
    int i, j, k, *ix;
    MatrixStruct mats;

    if (!IsMatrix(mat))
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (col >= mat->csize)
      throw IceException(FNAME, M_WRONG_INDEX);

    /*zu sortierendes Feld auswählen*/
    sortcol = col;
    /*Indexfeld anlegen*/
    ix = (int*)malloc(mat->rsize * sizeof(int));

    if (ix == NULL)
      throw IceException(FNAME, M_NO_MEM);

    for (i = 0; i < mat->rsize; i++)
      {
        ix[i] = i;
      }

    /*Zielmatrix anlegen*/
    mats = NewMatrix(mat->type, mat->rsize, mat->csize);

    if (mats == NULL)
      {
        free(ix);
        throw IceException(FNAME, M_NO_MEM);
      }

    /*Matrix sortieren */
    switch (mat->type)
      {
      case MAT_DOUBLE:
        sortdata_d = mat->data;
        qsort(ix, mat->rsize, sizeof(int), sortmat_Compare_d);

        if (mode >= 0)
          for (i = 0; i < mat->rsize; i++)
            for (j = 0; j < mat->csize; j++)
              {
                mats->data[i][j] = mat->data[ix[i]][j];
              }
        else
          for (i = 0, k = mat->rsize - 1; i < mat->rsize; i++, k--)
            for (j = 0; j < mat->csize; j++)
              {
                mats->data[k][j] = mat->data[ix[i]][j];
              }

        break;

      case MAT_INT:
        sortdata_i = mat->datai;
        qsort(ix, mat->rsize, sizeof(int), sortmat_Compare_i);

        if (mode >= 0)
          for (i = 0; i < mat->rsize; i++)
            for (j = 0; j < mat->csize; j++)
              {
                mats->datai[i][j] = mat->datai[ix[i]][j];
              }
        else
          for (i = 0, k = mat->rsize - 1; i < mat->rsize; i++, k--)
            for (j = 0; j < mat->csize; j++)
              {
                mats->datai[k][j] = mat->datai[ix[i]][j];
              }

        break;

      case MAT_CHAR:
        sortdata_c = mat->datac;
        qsort(ix, mat->rsize, sizeof(int), sortmat_Compare_c);

        if (mode >= 0)
          for (i = 0; i < mat->rsize; i++)
            for (j = 0; j < mat->csize; j++)
              {
                mats->datac[i][j] = mat->datac[ix[i]][j];
              }
        else
          for (i = 0, k = mat->rsize - 1; i < mat->rsize; i++, k--)
            for (j = 0; j < mat->csize; j++)
              {
                mats->datac[k][j] = mat->datac[ix[i]][j];
              }

        break;
      }

    free(ix);
    return mats;
  }
#undef FNAME
}
