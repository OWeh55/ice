#ifndef LUDECOMP_H
#define LUDECOMP_H
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

/*
 * Functions for LU-Decomposition of matrices
 * and solving linear equation systems
 */

#include "Matrix.h"
#include "matrixtemplate.h"

namespace ice
{
  void LUDecompositionPacked(const Matrix& a, Matrix& res,
                            IVector& indx, bool pivot = true);
  void LUDecompositionPacked(const Matrix& a, Matrix& res);
  void LUDecomposition(const Matrix& a, Matrix& L, Matrix& U);
  Vector LUSolve(const Matrix& LU, const IVector& indx, const Vector& b);

  void LUDecompositionPacked(const matrix<double>& a, matrix<double>& res,
                            std::vector<int>& index, bool pivot = true);
  void LUDecompositionPacked(const matrix<double>& a, matrix<double>& res);
  void LUDecomposition(const matrix<double>& a,
                      matrix<double>& L, matrix<double>& U);
  std::vector<double> LUSolve(const matrix<double>& LU,
                              const std::vector<int>& index,
                              const std::vector<double>& b);
}

#endif
