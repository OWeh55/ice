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
  Klasse zur Repräsentation von Punkten der Ebene
*/
#ifndef POINT_H
#define POINT_H

#include <cmath>
#include <iostream>
#include <sstream>

#include "defs.h"

namespace ice
{
  /**
   * class template poinT describes point by coordinates of type T.
   * ice typically uses pointT<int> as IPoint and pointT<double> as Point
   */
  template<typename T>
  class pointT
  {
  public:
    typedef T value_type;

    /**
     * the coordinates.
     * these are public for fast (and easy) access.
     */
    T x, y;

    /**
     * standard c'tor.
     * creates point in origin.
     */
    pointT() : x(0), y(0) {};

    /**
     *  c'tor with given coordinates.
     */
    pointT(T xs, T ys) : x(xs), y(ys) {};

    /**
     * type conversion and copy c'tor.
     */
    template<typename TI>
    explicit pointT(const pointT<TI>& p): x((T)p.x), y((T)p.y) {}

    /**
     * c'tor for type conversion from c array.
     */
    explicit pointT(T d[]): x(d[0]), y(d[1]) {}

    /**
     * implicite conversion to pointT<double>
     */
    operator pointT<double>() const
    {
      return pointT<double>(x, y);
    }

    /**
     * reference to coordinate x.
     */
    T& X()
    {
      return x;
    }
    /**
     * reference to coordinate y.
     */
    T& Y()
    {
      return y;
    }

    /**
     * const reference to coordinate x.
     */
    const T& X() const
    {
      return x;
    }

    /**
     * const reference to coordinate y.
     */
    const T& Y() const
    {
      return y;
    }

    bool operator==(const pointT& s) const
    {
      return (s.x == x) && (s.y == y);
    }

    bool operator!=(const pointT& s) const
    {
      return (s.x != x) || (s.y != y);
    }

    bool operator<(const pointT& s) const
    {
      if (x == s.x)
        {
          return y < s.y;
        }
      else
        {
          return x < s.x;
        }
    }

    friend void swap(pointT<T>& first, pointT<T>& second)
    {
      using std::swap;
      swap(first.x, second.x);
      swap(first.y, second.y);
    }

    int shift(T dx, T dy)
    {
      x += dx;
      y += dy;
      return OK;
    }

    const pointT& operator+=(const pointT& second)
    {
      x += second.x;
      y += second.y;
      return *this;
    }

    pointT operator+(const pointT& second) const
    {
      pointT res = *this;
      res += second;
      return res;
    }

    const pointT& operator-=(const pointT& second)
    {
      x -= second.x;
      y -= second.y;
      return *this;
    }

    pointT operator-(const pointT& second) const
    {
      pointT res = *this;
      res -= second;
      return res;
    }

    pointT operator-() const
    {
      return pointT(-x, -y);
    }

    const pointT& operator*=(T fac)
    {
      x *= fac;
      y *= fac;
      return *this;
    }

    pointT operator*(T fac) const
    {
      pointT res = *this;
      res *= fac;
      return res;
    }

    T operator*(pointT fac) const
    {
      return x * fac.x + y * fac.y;
    }

    const pointT& operator/=(T div)
    {
      x /= div;
      y /= div;
      return *this;
    }

    pointT operator/(T div) const
    {
      pointT res = *this;
      res /= div;
      return res;
    }

    T Cross(pointT fac) const
    {
      return x * fac.y - y * fac.x;
    }

    pointT normalized() const
    {
      pointT res = *this;
      double l = x * x + y * y;

      if (l == 0)
        {
          return res;
        }

      l = sqrt(l);
      return res / l;
    }

    double r() const
    {
      return sqrt(x * x + y * y);
    }

    double Length() const
    {
      return sqrt(x * x + y * y);
    }

    double length() const
    {
      return sqrt(x * x + y * y);
    }

    double abs() const
    {
      return sqrt(x * x + y * y);
    }

    double r2() const
    {
      return x * x + y * y;
    }

    double Length2() const
    {
      return x * x + y * y;
    }

    double length2() const
    {
      return x * x + y * y;
    }

    double abs2() const
    {
      return x * x + y * y;
    }

    double absL1() const
    {
      return std::fabs(x) + std::fabs(y);
    }

    double phi() const
    {
      return atan2(y, x);
    }

    friend std::ostream& operator<<(std::ostream& os, pointT p)
    {
      os << "(" << p.x << "," << p.y << ")";
      return os;
    }

    friend std::string to_string(pointT p)
    {
      std::ostringstream os;
      os << p;
      return os.str();
    }

  };

  template<typename T>
  pointT<T> operator*(double fac, pointT<T> p)
  {
    return pointT<T>(fac * p.x, fac * p.y);
  }

  typedef pointT<int> IPoint;
  typedef pointT<double> Point;
}
#endif
