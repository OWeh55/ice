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
#ifndef _ASSIGMENT_H_
#define _ASSIGMENT_H_

#include "Matrix.h"

namespace ice
{
  typedef int AssignFunction(const Matrix& cost, IMatrix& pairs, int mode);

  // int Hungarian(const Matrix &cost,IMatrix &reference_pairs,double &min_cost);
  // Ungarische Methode mit dummy-Mode (->AssignFunction)

  int Hungarian(const Matrix& cost, IMatrix& reference_pairs, int mode = 0);

  //Reskalierung der Kostenwerte:
  //mode==1: geringste Kosten => 0, zweitgeringste Kosten => 1, Rest entspr.
  int Hungarian(const Matrix& cost, IMatrix& reference_pairs, double& min_cost, int mode = 0);

  // TimeWarping
  // Modi
#define TW_NORMAL 1
#define TW_REDUCED 2
#define TW_MODEMASK 7
#define TW_BIDIRECTIONAL 8
#define TW_SEARCHSTART 16

  int TimeWarp(const Matrix& cost, IMatrix& reference_pairs, int mode = TW_NORMAL);

  double ReferenceCosts(const Matrix& costs, const IMatrix& pairs);

  int ReorderList(Matrix& feat, const IMatrix& ref);
}
#endif
