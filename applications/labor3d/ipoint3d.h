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
  Klasse zur Repräsentation von Punkten der Ebene
*/
#ifndef IPOINT3D_H
#define IPOINT3D_H

//#include <math.h>
//#include "defs.h"

namespace ice
{

  template<class T>
  class point3d
  {
  public:
    T x, y, z;

    point3d() : x(0), y(0), z(0) {};
    point3d(T xs, T ys, T zs) : x(xs), y(ys), z(zs) {};
    point3d(const point3d& p) : x(p.x), y(p.y), z(p.z) {}

    template<class TI>
    explicit point3d(const pointT<TI>& p): x((T)p.x), y((T)p.y), z((T)p.z) {}

    explicit point3d(T d[]): x(d[0]), y(d[1]), z(d[2]) {}

    operator point3d<double>() const
    {
      return point3d<double>(x, y, z);
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

    bool operator==(const point3d& s) const
    {
      return (s.x == x) && (s.y == y) && (s.z == z);
    }

    bool operator!=(const point3d& s) const
    {
      return (s.x != x) || (s.y != y) || (s.z != z);
    }

    void Shift(T dx, T dy, T dz)
    {
      x += dx;
      y += dy;
      z += dz;
    }

    point3d operator+=(const point3d& second)
    {
      x += second.x;
      y += second.y;
      z += second.z;
      return *this;
    }

    point3d operator+ (const point3d& second) const
    {
      point3d res = *this;
      res += second;
      return res;
    }

    point3d operator-=(const point3d& second)
    {
      x -= second.x;
      y -= second.y;
      z -= second.z;
      return *this;
    }

    point3d operator-(const point3d& second) const
    {
      point3d res = *this;
      res -= second;
      return res;
    }

    point3d operator *= (T fac)
    {
      x *= fac;
      y *= fac;
      z *= fac;
      return *this;
    }


    point3d operator*(T fac) const
    {
      point3d res = *this;
      res *= fac;
      return res;
    }

    T operator*(point3d fac) const
    {
      return x * fac.x + y * fac.y + z * fac.z;
    }

    point3d operator/=(T div)
    {
      x /= div;
      y /= div;
      z /= div;
      return *this;
    }

    point3d operator/(T div) const
    {
      point3d res = *this;
      res /= div;
      return res;
    }

  };

  typedef point3d<int> IPoint3D;
}
#endif
