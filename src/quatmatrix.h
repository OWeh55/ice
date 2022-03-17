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

/*
 * Author: Alexander Lärz, 2005
 */

#ifndef QUATMATRIX_H
#define QUATMATRIX_H

#include "quaternion.h"
#include "quatvector.h"

using namespace std;

namespace ice
{
  class QuatMatrix
  {
  public:
    QuatMatrix();
    QuatMatrix(unsigned int rows, unsigned int columns);
    QuatMatrix(const QuatMatrix& qm);
    ~QuatMatrix();

    friend QuatMatrix operator+(const QuatMatrix&, const QuatMatrix&);
    friend QuatMatrix operator-(const QuatMatrix&, const QuatMatrix&);
    friend QuatMatrix operator*(double, const QuatMatrix&);
    friend QuatMatrix operator*(const QuatMatrix&, double);
    friend QuatMatrix operator*(const QuatMatrix&, const Quaternion&);
    friend QuatMatrix operator*(const Quaternion&, const QuatMatrix&);

    friend bool operator==(const QuatMatrix& qv1, const QuatMatrix& qv2);
    friend bool operator!=(const QuatMatrix& qv1, const QuatMatrix& qv2);

    friend ostream& operator<<(ostream&, const QuatMatrix&);

    QuatMatrix& operator=(const QuatMatrix& qm);

    const QuatVector& operator[](int) const;
    QuatVector& operator[](int);

    unsigned int getRows() const;
    unsigned int getColumns() const;

  protected:
    unsigned int rows;
    unsigned int columns;
    bool qft;

    QuatVector** data;
  };
}
#endif
