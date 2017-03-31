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
   * 3-dim. and n-dim. vector
   */
  // The following functions are obsolete and shouldn't be used !!
#define FNAME "MoveVec"

  double* MoveVec(double* v1, double* v2)
  {
    if (v1 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (v2 == NULL)
      {
        v2 = new double [3];
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return 0.0;
      }

    double l = 0;

    for (int i = 0; i < 3; i++)
      {
        l += v[i] * v[i];
      }

    return sqrt(l);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AddVec"
  double* AddVec(double* v1, double* v2, double* v3)
  {
    if (v1 == NULL || v2 == NULL)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (v3 == NULL)
      {
        v3 = new double [3];
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return NULL;
      }

    if (v3 == NULL)
      {
        v3 = new double [3];
      }

    v3[0] = v1[0] - v2[0];
    v3[1] = v1[1] - v2[1];
    v3[2] = v1[2] - v2[2];

    return v3;
  }
#undef FNAME
  /*******************************************************************/
#define FNAME "NormVec"
#define EPS 1e-10
  double* NormVec(double* v1, double* v2)
  {
    if (v1 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return (nullptr);
      }

    double l = LengthVec(v1);

    if (l < EPS)
      {
        throw IceException(FNAME, M_ZERO_VECTOR, WRONG_PARAM);
        return nullptr;
      }

    if (v2 == nullptr)
      {
        v2 = new double [3];
      }

    v2[0] = v1[0] / l;
    v2[1] = v1[1] / l;
    v2[2] = v1[2] / l;

    return v2;
  }
#undef FNAME
#undef EPS

  /*******************************************************************/
  double* ScaleVec(double* v1, double fac, double* v2)
#define FNAME "Scale"
  {
    if (v1 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

    if (v2 == nullptr)
      {
        v2 = new double [3];
      }

    v2[0] = v1[0] * fac;
    v2[1] = v1[1] * fac;
    v2[2] = v1[2] * fac;

    return v2;
  }
#undef FNAME

  /*******************************************************************/
  /* #define DEBUG */
#define FNAME "ScalProdVec"
  double ScalProdVec(double v1[3], double v2[3])
  {
    int i;
    double scalar;

    if (v1 == nullptr || v2 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return 0.0;
      }

    scalar = 0;

    for (i = 0; i < 3; i++)
      {
        scalar += v1[i] * v2[i];
#if defined DEBUG
        printf("scalar %f\n", scalar);
        getchar();
#endif
      }

    return scalar;
  }
#undef FNAME
#undef DEBUG

  /*******************************************************************/
#define FNAME "CrossProdVec"
  double* CrossProdVec(double* v1, double* v2, double* v3)
  {
    double vintern[3];

    if (v1 == nullptr || v2 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

    if (v3 == nullptr)
      {
        v3 = new double [3];
      }

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
    double spat;

    if (v1 == nullptr || v2 == nullptr || v3 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return (0);
      }

    CrossProdVec(v1, v2, vintern);
    spat = ScalProdVec(vintern, v3);
    return spat;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AngleVec"
#define EPS 1e-10
  double AngleVec(double* v1, double* v2)
  {
    if (v1 == nullptr || v2 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return 0;
      }

    double l1 = LengthVec(v1);
    double l2 = LengthVec(v2);

    if ((l1 * l2) < EPS)
      {
        throw IceException(FNAME, M_ZERO_VECTOR, ZERO_VECTOR);
        return 0.0;
      }

    return acos(ScalProdVec(v1, v2) / (l1 * l2));
  }
#undef FNAME
#undef EPS

  /*******************************************************************/
#define FNAME "PrintVecRn"
  int PrintVecRn(const char* str, double* h, int dim)
  {
    int i;
    double* dptr;

    printf("*** %s\n", str);
    dptr = h;

    for (i = 0; i < dim; i++)
      {
        printf("%f   ", *dptr++);
      }

    printf("\n\n");
    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "MoveVecRn"
  double* MoveVecRn(double* v1, int dim, double* v2)
  {
    int i;
    double* dptr;

    if (v1 == nullptr)
      {
        throw IceException("FNAME", M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

    if (v2 == nullptr)
      {
        dptr = (double*)malloc(dim * sizeof(double));

        for (i = 0; i < dim; i++)
          {
            *(dptr + i) = v1[i];
          }

        return dptr;
      }

    for (i = 0; i < dim; i++)
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
    int i;
    double l;

    if (v == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return (0);
      }

    l = 0;

    for (i = 0; i < dim; i++)
      {
        l += v[i] * v[i];
      }

    l = sqrt(l);
    return l;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AddVecRn"
  double* AddVecRn(double* v1, double* v2, int dim, double* v3)
  {
    int i;
    double* dptr;

    if (v1 == nullptr || v2 == nullptr)
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

    double l = LengthVecRn(v1, dim);

    if (l < EPS)
      {
        throw IceException(FNAME, M_ZERO_VECTOR, ZERO_VECTOR);
        return nullptr;
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return nullptr;
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return 0.0;
      }

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
      {
        throw IceException(FNAME, M_WRONG_VECTOR, WRONG_VECTOR);
        return 0.0;
      }

    l1 = LengthVecRn(v1, dim);
    l2 = LengthVecRn(v2, dim);

    if ((l1 * l2) < eps)
      {
        throw IceException(FNAME, M_ZERO_VECTOR, ZERO_VECTOR);
        return 0.0;
      }

    return acos(ScalProdVecRn(v1, v2, dim) / (l1 * l2));
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "SortVectorRn"
#define DEBUG 1

  int SortVectorRn(double* ptr, int lng)
  // Sortieren eines Double-Arrays "ptr" der Laenge "lng"
  {
    if ((ptr == nullptr))
      {
        throw IceException(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (lng >= 2)
      {
        std::sort(ptr, ptr + lng);
      }

    return OK;
  }
#undef FNAME
#undef DEBUG


  /*******************************************************************/
#define FNAME "ConvVecDI"
  int ConvVecDI(double* dvec, int dim, int* ivec)
  /*
    Konvertieren eines Double-Vektors in einen Integer-Vektor
  */
  {
    if ((dvec == nullptr) || (ivec == nullptr))
      {
        throw IceException(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int i = 0; i < dim; i++)
      {
        ivec[i] = (int)dvec[i];
      }

    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ConvVecID"
  /*
    Konvertieren eines Integer-Vektors in einen Double-Vektor
  */
  int ConvVecID(int* ivec, int dim, double* dvec)
  {
    int i;

    if ((dvec == nullptr) || (ivec == nullptr))
      {
        throw IceException(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (dim < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (i = 0; i < dim; i++)
      {
        dvec[i] = (double)ivec[i];
      }

    return OK;
  }
#undef FNAME
}
