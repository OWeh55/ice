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
  (old) functions for vector algebra with c-arrays

  Neubauer  5/94
  Ortmann  4/05
********************************************************************/

#ifndef ICE_VECTOR_H
#define ICE_VECTOR_H

#include <stdio.h>

#include "Matrix.h"

namespace ice
{
// The following functions are obsolete and shouldn't be used !!

  /*** 3D-Vektoralgebra ********/
  double  LengthVec(double v[3]);
  double* MoveVec(double v1[3], double v2[3]);
  double* AddVec(double v1[3], double v2[3], double v3[3]);
  double* SubVec(double v1[3], double v2[3], double v3[3]);
  double* NormVec(double v1[3], double v2[3]);
  double* ScaleVec(double v1[3], double fac, double v2[3]);
  double  ScalProdVec(double v1[3], double v2[3]);
  double* CrossProdVec(double v1[3], double v2[3], double v3[3]);
  double  SpatProdVec(double v1[3], double v2[3], double v3[3]);
  double  AngleVec(double v1[3], double v2[3]);

  /*** n-dimensionale Vektoralgebra **********************************/
  int     PrintVecRn(const char* str, double* h, int dim);
  double  LengthVecRn(double* v, int dim);
  double* MoveVecRn(double* v1, int dim, double* v2);
  double* AddVecRn(double* v1, double* v2, int dim, double* v3);
  double* SubVecRn(double* v1, double* v2, int dim, double* v3);
  double* NormVecRn(double* v1, int dim, double v2[3]);
  double* ScaleVecRn(double* v1, int dim, double fac, double v2[3]);
  double  ScalProdVecRn(double* v1, double* v2, int dim);
  double  AngleVecRn(double* v1, double* v2, int dim);
  int     SortVectorRn(double* v, int dim);
  int     ConvVecDI(double* d, int dim, int* i);
  int     ConvVecID(int* i, int dim, double* d);
}
#endif
