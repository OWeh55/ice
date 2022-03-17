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
#ifndef IMATRIX_H
#define IMATRIX_H

#include "Vector.h"
#include "base.h"

namespace ice
{
  class IMatrix
  {
  public:
    IMatrix();
    IMatrix(int rows, int cols, int init = 0);
    IMatrix(const IMatrix&);  /* Kopier-Konstruktor */
    IMatrix(Image);
    ~IMatrix();

//  void Print() const;

    IVector& operator[](int) const;  /* Element-Operator */
    IVector& at(int) const;  /* Element-Operator */
    IMatrix& operator=(const IMatrix&);   /* Zuweisungs-Operator */
    IMatrix operator()(int, int, int, int) const; /* Teil-Matrix */

    int ExchangeRow(int, int);
    int ExchangeCol(int, int);
    int set(int val);
    int Append(const IVector& v);

    friend IMatrix operator+(const IMatrix&, const IMatrix&);
    friend IMatrix operator-(const IMatrix&, const IMatrix&);
    friend IMatrix operator-(const IMatrix&);

    friend IMatrix operator*(const IMatrix&, const int);
    friend IMatrix operator*(const int, const IMatrix&);

    int rows() const;
    int cols() const;

    int DeleteRow(int n);
    int DeleteRow(int n1, int n2);
    int DeleteCol(int n);
    int DeleteCol(int n1, int n2);

    int MaxVal() const;
  protected:

    int dim;
    int dimv;
    IVector** data;
  };

  IMatrix operator+(const IMatrix&, const IMatrix&);
  IMatrix operator-(const IMatrix&, const IMatrix&);
  IMatrix operator*(const IMatrix&, const int);
  IMatrix operator*(const int, const IMatrix&);
  IMatrix operator*(const IMatrix&, const IMatrix&);
  IVector operator*(const IMatrix&, const IVector&);
  IVector operator*(const IVector&, const IMatrix&);

  IMatrix operator||(const IMatrix&, const IMatrix&);
  IMatrix operator&&(const IMatrix&, const IMatrix&);
  IMatrix operator||(const IVector&, const IMatrix&);
  IMatrix operator&&(const IVector&, const IMatrix&);
  IMatrix operator||(const IMatrix&, const IVector&);
  IMatrix operator&&(const IMatrix&, const IVector&);
  IMatrix operator||(const IVector&, const IVector&);
  IMatrix operator&&(const IVector&, const IVector&);

  IMatrix operator!(const IMatrix&);

  int Determinant(IMatrix m);
}
#endif
