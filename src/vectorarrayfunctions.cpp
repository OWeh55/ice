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

#include <algorithm>

#include "macro.h"
#include "IceException.h"
#include "matrix_function.h"
#include "numbase.h"

#include "Vector.h"
#include "Matrix.h"
#include "vectorarrayfunctions.h"

namespace ice
{
  /*
   * 3-dim vector as double*
   */
  // The following functions are obsolete and shouldn't be used !!
#define FNAME "MoveVec"
  double* MoveVec(double* v1, double* v2)
  {
    if (v1 == NULL)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v2 == NULL)
      v2 = new double [3];

    v2[0] = v1[0];
    v2[1] = v1[1];
    v2[2] = v1[2];

    return v2;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "LengthVec"
  double LengthVec(double* v)
  {
    if (v == NULL)
      throw IceException(FNAME, M_WRONG_VECTOR);

    double l = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

    return sqrt(l);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AddVec"
  double* AddVec(double* v1, double* v2, double* v3)
  {
    if (v1 == NULL || v2 == NULL)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v3 == NULL)
      v3 = new double [3];

    v3[0] = v1[0] + v2[0];
    v3[1] = v1[1] + v2[1];
    v3[2] = v1[2] + v2[2];

    return v3;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "SubVec"
  double* SubVec(double* v1, double* v2, double* v3)
  {
    if (v1 == NULL || v2 == NULL)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v3 == NULL)
      v3 = new double [3];

    v3[0] = v1[0] - v2[0];
    v3[1] = v1[1] - v2[1];
    v3[2] = v1[2] - v2[2];

    return v3;
  }
#undef FNAME
  /*******************************************************************/
#define FNAME "NormVec"
  double* NormVec(double* v1, double* v2)
  {
    if (v1 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    double l = LengthVec(v1);

    if (l <= 0.0)
      throw IceException(FNAME, M_ZERO_VECTOR);

    if (v2 == nullptr)
      v2 = new double [3];

    v2[0] = v1[0] / l;
    v2[1] = v1[1] / l;
    v2[2] = v1[2] / l;

    return v2;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ScaleVec"
  double* ScaleVec(double* v1, double fac, double* v2)
  {
    if (v1 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v2 == nullptr)
      v2 = new double [3];

    v2[0] = v1[0] * fac;
    v2[1] = v1[1] * fac;
    v2[2] = v1[2] * fac;

    return v2;
  }
#undef FNAME

  /*******************************************************************/

#define FNAME "ScalProdVec"
  double ScalProdVec(double v1[3], double v2[3])
  {
    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "CrossProdVec"
  double* CrossProdVec(double* v1, double* v2, double* v3)
  {
    double vintern[3];

    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v3 == nullptr)
      v3 = new double [3];

    vintern[0] = v1[1] * v2[2] - v1[2] * v2[1];
    vintern[1] = v1[2] * v2[0] - v1[0] * v2[2];
    vintern[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return MoveVec(vintern, v3);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "SpatProdVec"
  double SpatProdVec(double v1[3], double v2[3], double v3[3])
  {
    double vintern[3];

    if (v1 == nullptr || v2 == nullptr || v3 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    CrossProdVec(v1, v2, vintern);
    return ScalProdVec(vintern, v3);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AngleVec"

  double AngleVec(double* v1, double* v2)
  {
    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    double l1 = LengthVec(v1);
    double l2 = LengthVec(v2);

    if (l1 == 0.0 || l2 == 0.0)
      throw IceException(FNAME, M_ZERO_VECTOR);

    return acos(ScalProdVec(v1, v2) / (l1 * l2));
  }
#undef FNAME

  /*
   * n-dim vector as double *
   */
#define FNAME "PrintVecRn"
  void PrintVecRn(const char* str, double* h, int dim)
  {
    double* dptr;

    printf("*** %s\n", str);
    dptr = h;

    for (int i = 0; i < dim; i++)
      {
        printf("%f   ", *dptr++);
      }

    printf("\n\n");
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "MoveVecRn"
  double* MoveVecRn(double* v1, int dim, double* v2)
  {
    if (v1 == nullptr)
      throw IceException("FNAME", M_WRONG_VECTOR);

    if (v2 == nullptr)
      v2 = (double*)malloc(dim * sizeof(double));

    for (int i = 0; i < dim; i++)
      {
        v2[i] = v1[i];
      }

    return v2;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "LengthVecRn"
  double LengthVecRn(double* v, int dim)
  {
    if (v == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    double l2 = 0;

    for (int i = 0; i < dim; i++)
      {
        l2 += v[i] * v[i];
      }

    return sqrt(l2);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AddVecRn"
  double* AddVecRn(double* v1, double* v2, int dim, double* v3)
  {
    int i;
    double* dptr;

    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v2 == nullptr)
      {
        dptr = (double*)malloc(dim * sizeof(double));

        for (i = 0; i < dim; i++)
          {
            *(dptr + i) = v1[i] + v2[i];
          }

        return dptr;
      }

    for (i = 0; i < dim; i++)
      {
        v3[i] = v1[i] + v2[i];
      }

    return v3;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "SubVecRn"
  double* SubVecRn(double* v1, double* v2, int dim, double* v3)
  {
    int i;
    double* dptr;

    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (v3 == nullptr)
      {
        dptr = (double*)malloc(dim * sizeof(double));

        for (i = 0; i < dim; i++)
          {
            *(dptr + i) = v1[i] - v2[i];
          }

        return dptr;
      }

    for (i = 0; i < dim; i++)
      {
        v3[i] = v1[i] - v2[i];
      }

    return v3;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "NormVevRn"
#define EPS 1e-30
  double* NormVecRn(double* v1, int dim, double* v2)
  {
    int i;
    double* dptr = v2;

    if (v1 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    double l = LengthVecRn(v1, dim);

    if (l < EPS)
      throw IceException(FNAME, M_ZERO_VECTOR);

    if (dptr == nullptr)
      {
        dptr = new double [dim];
      }

    for (i = 0; i < dim; i++)
      {
        dptr[i] = v1[i] / l;
      }

    return dptr;
  }
#undef FNAME
#undef EPS

  /*******************************************************************/
#define FNAME "ScaleVecRn"
  double* ScaleVecRn(double* v1, int dim, double fac, double* v2)
  {
    double* dptr = v2;

    if (v1 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    if (dptr == nullptr)
      {
        dptr = new double [dim];
      }

    for (int i = 0; i < dim; i++)
      {
        dptr[i] = v1[i] * fac;
      }

    return dptr;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ScalProdVecRn"
  double ScalProdVecRn(double* v1, double* v2, int dim)
  {
    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    double scalar = 0;

    for (int i = 0; i < dim; i++)
      {
        scalar += v1[i] * v2[i];
      }

    return scalar;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AngleVecRn"
  double AngleVecRn(double* v1, double* v2, int dim)
  {
    double l1, l2;
    double eps = 1e-10;

    if (v1 == nullptr || v2 == nullptr)
      throw IceException(FNAME, M_WRONG_VECTOR);

    l1 = LengthVecRn(v1, dim);
    l2 = LengthVecRn(v2, dim);

    if ((l1 * l2) < eps)
      throw IceException(FNAME, M_ZERO_VECTOR);

    return acos(ScalProdVecRn(v1, v2, dim) / (l1 * l2));
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "SortVectorRn"
  void SortVectorRn(double* ptr, int lng)
  // Sortieren eines Double-Arrays "ptr" der Laenge "lng"
  {
    if (ptr == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    if (lng >= 2)
      {
        std::sort(ptr, ptr + lng);
      }
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ConvVecDI"
  void ConvVecDI(double* dvec, int dim, int* ivec)
  /*
    Konvertieren eines double-Vektors in einen Integer-Vektor
  */
  {
    if ((dvec == nullptr) || (ivec == nullptr))
      throw IceException(FNAME, M_WRONG_PTR);

    if (dim < 1)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    for (int i = 0; i < dim; i++)
      {
        ivec[i] = (int)dvec[i];
      }
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ConvVecID"
  /*
    Konvertieren eines Integer-Vektors in einen Double-Vektor
  */
  void ConvVecID(int* ivec, int dim, double* dvec)
  {
    if ((dvec == nullptr) || (ivec == nullptr))
      throw IceException(FNAME, M_WRONG_PTR);

    if (dim < 1)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    for (int i = 0; i < dim; i++)
      {
        dvec[i] = (double)ivec[i];
      }
  }
#undef FNAME
}
