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

#ifndef ROTQUATERNION_H
#define ROTQUATERNION_H

#include "quaternion.h"
#include "Matrix.h"
#include "Vector3d.h"

#define M_CONVERT_NOT_POSSIBLE "Konvertierung nicht möglich"
#define CONVERT_NOT_POSSIBLE  (-110)

using namespace std;

namespace ice
{

  class RotQuaternion
  {
    friend RotQuaternion convertToRotQuaternion(Quaternion& in);
    friend RotQuaternion convertToRotQuaternion(Matrix& in);

    friend bool operator==(const RotQuaternion& a, const RotQuaternion& b);
    friend bool operator!=(const RotQuaternion& a, const RotQuaternion& b);

    friend bool operator==(const RotQuaternion& a, const Quaternion& b);
    friend bool operator==(const Quaternion& a, const RotQuaternion& b);

    friend bool operator!=(const Quaternion& a, const RotQuaternion& b);
    friend bool operator!=(const RotQuaternion& a, const Quaternion& b);

    friend Quaternion operator+(const RotQuaternion& x, const RotQuaternion& y);
    friend Quaternion operator+(const RotQuaternion& x, const Quaternion& y);
    friend Quaternion operator+(const Quaternion& x, const RotQuaternion& y);

    friend Quaternion operator-(const RotQuaternion& x, const RotQuaternion& y);
    friend Quaternion operator-(const RotQuaternion& x, const Quaternion& y);
    friend Quaternion operator-(const Quaternion& x, const RotQuaternion& y);

    friend RotQuaternion operator-(RotQuaternion& x);

    friend Quaternion operator*(const RotQuaternion& x, const double y);
    friend Quaternion operator*(const double x, const RotQuaternion& y);

    friend Quaternion operator/(const RotQuaternion& x, const double y);

    friend RotQuaternion operator*(const RotQuaternion& x, const RotQuaternion& y);
    friend Quaternion operator*(const RotQuaternion& x, const Quaternion& y);
    friend Quaternion operator*(const Quaternion& x, const RotQuaternion& y);

    friend Quaternion qexp(RotQuaternion& x);

    friend ostream& operator<<(ostream& ost, const RotQuaternion& x);

  public:
    RotQuaternion();
    RotQuaternion(double angle, Vector3d& axis);
    RotQuaternion(double angle, double x, double y, double z);
    RotQuaternion(const RotQuaternion& b): rot(b) {} // copy constructor

    ~RotQuaternion();

    Matrix getRotationMatrix();

    double getRotationAngle();
    Vector3d getRotationAxis();

    void setRotationAngle(double angle);
    void setRotationAxis(Vector3d& axis);

    double getSquareNorm();
    double getNorm();
    double getEigenwinkel();

    Quaternion getEigenachse();
    Vector3d getPhases();

    RotQuaternion getNegate();
    RotQuaternion getConjugate();
    RotQuaternion getSquare();
    RotQuaternion getInverse();

    double getReal()
    {
      return rot.real;
    }
    double getI()
    {
      return rot.i;
    }
    double getJ()
    {
      return rot.j;
    }
    double getK()
    {
      return rot.k;
    }

    Vector3d getImaginary()
    {
      return Vector3d(rot.i, rot.j, rot.k);
    };

    RotQuaternion& operator=(const RotQuaternion& b)
    {
      rot = b.rot;
      return *this;
    }

    RotQuaternion& operator*=(const RotQuaternion& b);

    operator Quaternion() const;

  protected:
    Quaternion rot;
  };

  inline bool operator==(const RotQuaternion& a, const RotQuaternion& b)
  {
    return (a.rot == b.rot);
  }

  inline bool operator!=(const RotQuaternion& a, const RotQuaternion& b)
  {
    return !(a.rot == b.rot);
  }


  inline bool operator==(const RotQuaternion& a, const Quaternion& b)
  {
    return (a.rot == b);
  }

  inline bool operator==(const Quaternion& a, const RotQuaternion& b)
  {
    return (a == b.rot);
  }

  inline bool operator!=(const RotQuaternion& a, const Quaternion& b)
  {
    return !(a == b);
  }

  inline bool operator!=(const Quaternion& a, const RotQuaternion& b)
  {
    return !(a == b);
  }

  inline Quaternion operator+(const RotQuaternion& x, const RotQuaternion& y)
  {
    return x.rot + y.rot;
  }

  inline Quaternion operator+(const RotQuaternion& x, const Quaternion& y)
  {
    return x.rot + y;
  }

  inline Quaternion operator+(const Quaternion& x, const RotQuaternion& y)
  {
    return x + y.rot;
  }

  inline Quaternion operator-(const RotQuaternion& x, const RotQuaternion& y)
  {
    return x.rot - y.rot;
  }

  inline Quaternion operator-(const RotQuaternion& x, const Quaternion& y)
  {
    return x.rot - y;
  }

  inline Quaternion operator-(const Quaternion& x, const RotQuaternion& y)
  {
    return x - y.rot;
  }

  inline RotQuaternion operator-(RotQuaternion& x)
  {
    return x.getNegate();
  }

  inline Quaternion operator*(const RotQuaternion& x, const double y)
  {
    return x.rot * y;
  }

  inline Quaternion operator*(const double x, const RotQuaternion& y)
  {
    return x * y.rot;
  }

  inline Quaternion operator/(const RotQuaternion& x, const double y)
  {
    return x.rot / y;
  }

  inline RotQuaternion operator*(const RotQuaternion& x, const RotQuaternion& y)
  {
    RotQuaternion a;
    a.rot = x.rot * y.rot;

    return a;
  }

  inline Quaternion operator*(const RotQuaternion& x, const Quaternion& y)
  {
    return x.rot * y;
  }

  inline Quaternion operator*(const Quaternion& x, const RotQuaternion& y)
  {
    return x * y.rot;
  }

  inline ostream& operator<<(ostream& ost, const RotQuaternion& x)
  {
    return ost << x.rot;
  }

  //inline Quaternion qexp(RotQuaternion& x)
  //  {
  //    return qexp(x);
  //  }

  RotQuaternion convertToRotQuaternion(Quaternion& in);
  RotQuaternion convertToRotQuaternion(Matrix& in);

}
#endif
