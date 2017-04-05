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
#ifndef VECTOR_3D_H
#define VECTOR_3D_H

#include "Vector.h"

namespace ice
{
  template<typename T>
  class vector3d
  {
  public:
    typedef T value_type;
  public:
    T x, y, z;

    vector3d(): x(0), y(0), z(0) {};
    vector3d(T xp, T yp, T zp): x(xp), y(yp), z(zp) {};
    vector3d(const vector3d& p): x(p.x), y(p.y), z(p.z) {};

#define FNAME "Vector3d::Vector3d"
    explicit vector3d(const Vector& v)
    {
      if (v.Size() != 3)
        {
          throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        }

      x = v[0];
      y = v[1];
      z = v[2];
    }
    explicit vector3d(const IVector& v)
    {
      if (v.Size() != 3)
        {
          throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        }

      x = v[0];
      y = v[1];
      z = v[2];
    }
    explicit vector3d(const std::vector<T>& v)
    {
      if (v.size() != 3)
        {
          throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        }

      x = v[0];
      y = v[1];
      z = v[2];
    }
#undef FNAME

    vector3d(T p[]): x(p[0]), y(p[1]), z(p[2]) {};
#if 0
    const T& X() const
    {
      return x;
    }
    const T& Y() const
    {
      return y;
    }
    const T& Z() const
    {
      return z;
    }

    T& X()
    {
      return x;
    }
    T& Y()
    {
      return y;
    }
    T& Z()
    {
      return z;
    }
#endif
#define FNAME "Vector3d::operator[]"
    const T& operator[](int i) const
    {
      switch (i)
        {
        case 0:
          return x;
        case 1:
          return y;
        case 2:
          return z;
        default:
          throw IceException(FNAME, M_WRONG_INDEX, WRONG_PARAM);
        }

      return x;
    }

    T& operator[](int i)
    {
      switch (i)
        {
        case 0:
          return x;
        case 1:
          return y;
        case 2:
          return z;
        default:
          throw IceException(FNAME, M_WRONG_INDEX, WRONG_PARAM);
        }

      return x;
    }

    int size() const
    {
      return 3;
    }
#undef FNAME

    operator Vector() const
    {
      return Vector(x, y, z);
    }

    const vector3d& operator+=(const vector3d& v2)
    {
      x += v2.x;
      y += v2.y;
      z += v2.z;
      return *this;
    }

    friend vector3d operator+(const vector3d& v1, const vector3d& v2)
    {
      vector3d res(v1);
      res += v2;
      return res;
    }

    const vector3d& operator-=(const vector3d& v2)
    {
      x -= v2.x;
      y -= v2.y;
      z -= v2.z;
      return *this;
    }

    friend vector3d operator-(const vector3d& v1, const vector3d& v2)
    {
      vector3d res(v1);
      res -= v2;
      return res;
    }

    vector3d operator-() const
    {
      return vector3d(-x, -y, -z);
    }

    double operator*(const vector3d& v2) const
    {
      return x * v2.x + y * v2.y + z * v2.z;
    }

    const vector3d& operator*=(double f)
    {
      x *= f;
      y *= f;
      z *= f;
      return *this;
    }

    friend vector3d operator*(const vector3d& v, double f)
    {
      vector3d res(v);
      res *= f;
      return res;
    }

    friend vector3d operator*(double f, const vector3d& v)
    {
      vector3d res(v);
      res *= f;
      return res;
    }

    const vector3d& operator/=(double f)
    {
      x /= f;
      y /= f;
      z /= f;
      return *this;
    }

    friend vector3d operator/(const vector3d& v, double f)
    {
      vector3d res(v);
      res /= f;
      return res;
    }

    vector3d Cross(const vector3d& b) const
    {
      return vector3d(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
    }

    double Length() const
    {
      return sqrt(x * x + y * y + z * z);
    }

    double Length2() const
    {
      return x * x + y * y + z * z;
    }

    double length() const
    {
      return sqrt(x * x + y * y + z * z);
    }

    double length2() const
    {
      return x * x + y * y + z * z;
    }

#define FNAME "Vector3d::Normalized"
    vector3d Normalized() const
    {
      vector3d res = *this;
      res.Normalize();
      return res;
    }
#undef FNAME
#define FNAME "Vector3d::Normalize"
    void Normalize()
    {
      double l = Length();

      if (l == 0)
        {
          throw IceException(FNAME, M_ZERO_VECTOR, WRONG_PARAM);
        }
      double fac = 1.0 / l;
      *this *= fac;
    }
#undef FNAME

    double Distance(const vector3d& rhs) const
    {
      return (rhs - *this).Length();
    }

    vector3d& operator = (const vector3d& v)
    {
      x = v.x;
      y = v.y;
      z = v.z;
      return *this;
    }

#define FNAME "Vector3d::operator="
    vector3d& operator = (const Vector& v)
    {
      if (v.Size() != 3)
        {
          throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        }

      x = v[0];
      y = v[1];
      z = v[2];
      return *this;
    }
#undef FNAME
    bool operator==(const vector3d& rhs) const
    {
      return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    bool operator!=(const vector3d& rhs) const
    {
      return x != rhs.x || y != rhs.y || z != rhs.z;
    }

    void Shift(T dx, T dy, T dz)
    {
      x += dx;
      y += dy;
      z += dz;
    }

    friend std::ostream& operator<<(std::ostream& os, const vector3d<T>& v)
    {
      os << "<" << v.x << "," << v.y << "," << v.z << ">";
      return os;
    }

  }; // end class Vector3d

  template<typename T>
  vector3d<T> operator*(double d, const vector3d<T>& v)
  {
    return v * d;
  }

  typedef vector3d<double> Vector3d;
  typedef vector3d<int> IVector3d;
}
#endif
