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
#ifndef MATRIXALGEBRA_H
#define MATRIXALGEBRA_H

#include "Vector.h"
#include "matrixtemplate.h"
#include "Matrix.h"

namespace ice
{
  /* special Cholesky decomposition.
     Works in situ m -> m.
     if m is symmetric and positive definite:
         return true and m is Cholesky decomposition
     else
         return false and m is indetermined
  */
  bool Cholesky(matrix<double>& m);

  /*
   * Cholesky decomposition
   * return Cholesky decomposition of m
   * if m is not positive definite throws
   */
  matrix<double> CholeskyDecomposition(const matrix<double>& m);
  Matrix CholeskyDecomposition(const Matrix& m);

  matrix<double> CholeskyInverse(const matrix<double>& mat);
  Matrix CholeskyInverse(const Matrix& m);

  bool   isPositiveDefinite(const matrix<double>& m);
  bool   isPositiveDefinite(const Matrix& m);

  double Determinant(const matrix<double>& m);
  double Determinant(const Matrix& m);

  double CholeskyDeterminant(const matrix<double>& m);
  double CholeskyDeterminant(const Matrix& m);

  matrix<double> Inverse(const matrix<double>& m);
  Matrix Inverse(const Matrix& m);

  // solve Ax = b
  std::vector<double> solveLinearEquation(const matrix<double>& A,
                                          const std::vector<double>& b);
  Vector SolveLinearEquation(const Matrix&, const Vector&);

} // namespace ice
#endif
