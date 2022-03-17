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

#ifndef QUATERNION_H
#define QUATERNION_H

#define M_NO_QUATERNIONEIGENWINKEL "kein Eigenwinkel vorhanden"
#define M_NO_QUATERNIONEIGENACHSE "keine Eigenachse vorhanden"
#define M_NO_QUATERNIONINVERSE "keine Inverse vorhanden"
#define M_NO_QUATERNIONPHASES "keine Phasen vorhanden"
#define M_NOT_NORMALIZEABLE "nicht normalisierbar"

#define NO_QUATERNIONEIGENWINKEL (-100)
#define NO_QUATERNIONEIGENACHSE (-101)
#define NO_QUATERNIONINVERSE (-102)
#define NO_QUATERNIONPHASES (-103)
#define NOT_NORMALIZEABLE (-104)

#include <iostream>

#include "Vector3d.h"

namespace ice
{
  using namespace std;

  class Quaternion
  {
    friend class RotQuaternion;

    friend bool operator==(const Quaternion& a, const Quaternion& b);
    friend bool operator!=(const Quaternion& a, const Quaternion& b);

    friend Quaternion operator-(Quaternion& x);
    friend Quaternion operator+(const Quaternion& x, const Quaternion& y);
    friend Quaternion operator-(const Quaternion& x, const Quaternion& y);

    friend Quaternion operator*(const Quaternion& x, const double y);
    friend Quaternion operator*(const double x, const Quaternion& y);
    friend Quaternion operator*(const Quaternion& x, const Quaternion& y);

    friend Quaternion operator/(const Quaternion& x, const double y);

    friend Quaternion qexp(const Quaternion& x);
    friend ostream& operator<<(ostream& ost, const Quaternion& x);

  private:
    double myatan2(double x, double y);

  protected:
    double real, i, j, k;

  public:
    Quaternion();
    Quaternion(double real, double i, double j, double k);
    Quaternion(double real, Vector3d& imaginary);
    Quaternion(const Quaternion& b): real(b.real), i(b.i), j(b.j), k(b.k) {} // copy constructor

    ~Quaternion();

    double getReal() const
    {
      return real;
    }
    double getI() const
    {
      return i;
    }
    double getJ() const
    {
      return j;
    }
    double getK() const
    {
      return k;
    }

    Vector3d getImaginary() const
    {
      return Vector3d(i, j, k);
    };

    void setReal(double real);
    void setI(double i);
    void setJ(double j);
    void setK(double k);

    void setImaginary(Vector3d& imaginary);

    Quaternion getNegate();
    Quaternion getConjugate();
    Quaternion getSquare();
    Quaternion getInverse();
    Quaternion getNormalize();

    double getSquareNorm();
    double getNorm();
    double getEigenwinkel();

    Quaternion getEigenachse();
    Vector3d getPhases();

    bool isUnitQuaternion();

    Quaternion& operator=(const Quaternion& b)
    {
      real = b.real;
      i = b.i;
      j = b.j;
      k = b.k;

      return *this;
    }

    Quaternion& operator+=(const Quaternion& b);
    Quaternion& operator-=(const Quaternion& b);
    Quaternion& operator*=(const double b);
    Quaternion& operator*=(const Quaternion& b);
    Quaternion& operator/=(const double b);
  };

  inline Quaternion operator+(const Quaternion& x, const Quaternion& y)
  {
    Quaternion z = x;

    return z += y;
  }

  inline Quaternion operator-(const Quaternion& x, const Quaternion& y)
  {
    Quaternion z = x;

    return z -= y;
  }

  inline Quaternion operator*(const Quaternion& x, const double y)
  {
    Quaternion z = x;

    return z *= y;
  }

  inline Quaternion operator*(const double x, const Quaternion& y)
  {
    Quaternion z = y;

    return z *= x;
  }

  inline Quaternion operator/(const Quaternion& x, const double y)
  {
    Quaternion z = x;

    return z /= y;
  }

  inline Quaternion operator*(const Quaternion& x, const Quaternion& y)
  {
    Quaternion z = x;
    return z *= y;
  }
}
#endif
