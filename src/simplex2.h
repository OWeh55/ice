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
#ifndef _SIMPLEX2_H_
#define _SIMPLEX2_H_

namespace ice
{
  double dotprod(double* x, double* y, int n);
  void bmx(int m, double* a, int* ka, int* ia, int* basis, double* x, double* y);
  void btmx(int m, double* a, int* ka, int* ia, int* basis, double* x, double* y);
  void smx(int m, int n, double* a, int* ka, int* ia, double* x, double* y);
  void atnum(int m, int n, int* ka, int* ia, double* a,
             int* kat, int* iat, double* at
            );
  double maxv(double* x, int n);
}
#endif
