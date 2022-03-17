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
#ifndef _MATEIGEN_H
#define _MATEIGEN_H
//
// Calculation of Eigenvalue and singular value decomposition
//  abraham 11/95
//  Ortmann 7/99
//
#include <vector>
#include <matrixtemplate.h>

#include "matdef.h"
#include "Matrix.h"

namespace ice
{
  // eigenvalues as vector
  int Eigenvalue(const Matrix& a, Vector& eval, Matrix& evect, double eps = 1e-7, int maxIt = 200);
  int Eigenvalue(const matrix<double>& a,
                 std::vector<double>& eval, std::vector<std::vector<double>>& evect,
                 double eps = 1e-7, int maxIt = 200);

  // eigenvector as diagonal matrix
  int Eigenvalue(const Matrix& a, Matrix& eval, Matrix& evect, double eps = 1e-7, int maxIt = 200);

  // singular values as vector
  int SingularValueDcmp(const Matrix& A, Matrix& U, Vector& S, Matrix& V);

  // singular values as diagonal matrix
  int SingularValueDcmp(const Matrix& A, Matrix& U, Matrix& S, Matrix& V);

  // obsolete versions with MatrixStruct
  //MatrixStruct Mateigen(MatrixStruct A, double* eval, MatrixStruct evect);
  //  int SingularValueDcmp(MatrixStruct A, MatrixStruct* U, MatrixStruct* S, MatrixStruct* V);

}

#endif
