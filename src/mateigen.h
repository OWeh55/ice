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
  MatrixStruct Mateigen(MatrixStruct A, double* eval, MatrixStruct evect);
  int SingularValueDcmp(MatrixStruct A, MatrixStruct* U, MatrixStruct* S, MatrixStruct* V);

}

#endif
