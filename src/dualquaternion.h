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

#ifndef DUALQUATERNION_H
#define DUALQUATERNION_H

#include "quaternion.h"

#define M_NO_DUALQUATERNIONINVERSE "keine Inverse vorhanden"
#define NO_DUALQUATERNIONINVERSE (-111)

#include <iostream>

namespace ice
{

  class DualQuaternion
  {
    friend class TrafoDualQuaternion;

    friend bool operator==(const DualQuaternion& a, const DualQuaternion& b);
    friend bool operator!=(const DualQuaternion& a, const DualQuaternion& b);

    friend DualQuaternion operator-(const DualQuaternion& x);
    friend DualQuaternion operator+(const DualQuaternion& x, const DualQuaternion& y);
    friend DualQuaternion operator-(const DualQuaternion& x, const DualQuaternion& y);

    friend DualQuaternion operator*(const DualQuaternion& x, const double y);
    friend DualQuaternion operator*(const double x, const DualQuaternion& y);

    friend DualQuaternion operator*(const DualQuaternion& x, const DualQuaternion& y);

    friend DualQuaternion operator/(const DualQuaternion& y, const double x);

    friend ostream& operator<<(ostream& ost, const DualQuaternion& x);

  protected:
    Quaternion real, dual;

  public:
    DualQuaternion();
    DualQuaternion(const Quaternion& real, const Quaternion& dual);
    DualQuaternion(const DualQuaternion& b): real(b.real), dual(b.dual) {} // copy constructor

    Quaternion getReal()
    {
      return real;
    }
    Quaternion getDual()
    {
      return dual;
    }

    void setReal(const Quaternion& real);
    void setDual(const Quaternion& dual);

    DualQuaternion getNegate();
    DualQuaternion getConjugate();
    DualQuaternion getTilde();
    DualQuaternion getInverse();

    DualQuaternion getQuasiNorm();

    bool isUnitDualQuaternion();

    DualQuaternion& operator=(const DualQuaternion& b)
    {
      real = b.real;
      dual = b.dual;

      return *this;
    }

    DualQuaternion& operator+=(const DualQuaternion& b);
    DualQuaternion& operator-=(const DualQuaternion& b);
    DualQuaternion& operator*=(const double b);
    DualQuaternion& operator/=(const double b);
    DualQuaternion& operator*=(const DualQuaternion& b);

  };

  inline DualQuaternion operator+(const DualQuaternion& x, const DualQuaternion& y)
  {
    DualQuaternion z = x;

    return z += y;
  }

  inline DualQuaternion operator-(const DualQuaternion& x, const DualQuaternion& y)
  {
    DualQuaternion z = x;

    return z -= y;
  }

  inline DualQuaternion operator*(const DualQuaternion& x, const double y)
  {
    DualQuaternion z = x;

    return z *= y;
  }

  inline DualQuaternion operator*(const double x, const DualQuaternion& y)
  {
    DualQuaternion z = y;

    return z *= x;
  }

  inline DualQuaternion operator*(const DualQuaternion& x, const DualQuaternion& y)
  {
    DualQuaternion z = x;

    return z *= y;
  }

  inline DualQuaternion operator/(const DualQuaternion& x, const double y)
  {
    DualQuaternion z = x;

    return z /= y;
  }
}
#endif
