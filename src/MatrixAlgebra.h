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
#ifndef MATRIXALGEBRA_H
#define MATRIXALGEBRA_H

#include "Vector.h"
#include "matrixtemplate.h"
#include "Matrix.h"

namespace ice
{
  Vector SolveLinEqu(const Matrix&, const Vector&);

  int FindPivot(const Matrix&, int i, int j);

  Matrix CholeskyInverse(const Matrix& m);
  matrix<double> CholeskyInverse(const matrix<double>& mat);

  matrix<double> CholeskyDecomposition(const matrix<double>& m);
  Matrix CholeskyDecomposition(const Matrix& m);

  bool   isPositivDefinit(const Matrix& m);
  bool   isPositivDefinit(const matrix<double>& m);

  bool   hasInverse(const matrix<double>& m);
  bool   hasInverse(const Matrix& m);

  double Determinant(const Matrix& m);
  double Determinant(const matrix<double>& m);

  double CholeskyDeterminant(const matrix<double>& m);
  double CholeskyDeterminant(const Matrix& m);

  Matrix Inverse(const Matrix& m);
  matrix<double> Inverse(const matrix<double>& m);

  // solve Ax = b
  std::vector<double> solveLinearEquation(const matrix<double>& A,
                                          const std::vector<double>& b);
} // namespace ice
#endif
