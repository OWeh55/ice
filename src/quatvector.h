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

#ifndef QUATVECTOR_H
#define QUATVECTOR_H

#include "quaternion.h"

using namespace std;

namespace ice
{
  class QuatVector
  {
  public:
    QuatVector();
    QuatVector(unsigned int dimension);
    QuatVector(const QuatVector& qv);
    ~QuatVector();

    friend QuatVector operator+(const QuatVector&, const QuatVector&);
    friend QuatVector operator-(const QuatVector&, const QuatVector&);
    friend QuatVector operator*(double, const QuatVector&);
    friend QuatVector operator*(const QuatVector&, double);
    friend QuatVector operator*(const QuatVector&, const Quaternion&);
    friend QuatVector operator*(const Quaternion&, const QuatVector&);
    friend bool operator==(const QuatVector& qv1, const QuatVector& qv2);
    friend bool operator!=(const QuatVector& qv1, const QuatVector& qv2);

    friend ostream& operator<<(ostream&, const QuatVector&);

    const Quaternion& operator[](int) const;
    Quaternion& operator[](int);

    QuatVector& operator = (const QuatVector& qv);

    unsigned int getDimension() const;

  protected:
    unsigned int dimension;
    Quaternion* data;
  };
}
#endif
