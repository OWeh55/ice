/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef _VECTOR_ARITH_H
#define _VECTOR_ARITH_H
/*
 * Arithmetik mit vector<T>
 */
#include <vector>
#include "IceException.h"

namespace ice
{
  template<typename T>
  const std::vector<T>& operator+=(std::vector<T>& v1, const std::vector<T>& v2)
  {
    if (v1.size() != v2.size())
      throw IceException("operator+=", "different sizes");
    for (int i = 0; i < v1.size(); ++i)
      {
        v1[i] += v2[i];
      }
    return v1;
  }

  template<typename T>
  std::vector<T> operator+(std::vector<T> v1, const std::vector<T>& v2)
  {
    v1 += v2;
    return v1;
  }

  template<typename T>
  const std::vector<T>& operator-=(std::vector<T>& v1, const std::vector<T>& v2)
  {
    if (v1.size() != v2.size())
      throw IceException("operator-=", "different sizes");
    for (unsigned int i = 0; i < v1.size(); ++i)
      {
        v1[i] -= v2[i];
      }
    return v1;
  }

  template<typename T>
  std::vector<T> operator-(std::vector<T> v1, const std::vector<T>& v2)
  {
    v1 -= v2;
    return v1;
  }

  template<typename T, typename TS>
  const std::vector<T>& operator*=(std::vector<T>& v1, TS f)
  {
    for (unsigned int i = 0; i < v1.size(); ++i)
      {
        v1[i] *= f;
      }
    return v1;
  }

  template<typename T, typename TS>
  std::vector<T> operator*(const std::vector<T>& v1, TS f)
  {
    std::vector<T> res(v1);
    res *= f;
    return res;
  }

  template<typename T, typename TS>
  std::vector<T> operator*(TS f, const std::vector<T>& v1)
  {
    std::vector<T> res(v1);
    res *= f;
    return res;
  }

  template<typename T>
  double operator*(const std::vector<T>& v1, const std::vector<T>& v2)
  {
    double res = 0;
    for (int i = 0; i < v1.size(); ++i)
      {
        res += v1[i] * v2[i];
      }
    return res;
  }

  template<typename T>
  double norm2(const std::vector<T>& v)
  {
    double l = 0;
    for (int i = 0; i < v.size(); ++i)
      {
        l += v[i] * v[i];
      }
    return l;
  }

  template<typename T>
  double norm(const std::vector<T>& v)
  {
    return sqrt(norm2(v));
  }

  template<typename T>
  double length2(const std::vector<T>& v)
  {
    double l = 0;
    for (unsigned int i = 0; i < v.size(); ++i)
      {
        l += v[i] * v[i];
      }
    return l;
  }

  template<typename T>
  double length(const std::vector<T>& v)
  {
    return sqrt(length2(v));
  }

  template<typename T>
  std::vector<T> normalized(const std::vector<T>& v1)
  {
    std::vector<T> res(v1);
    double l = length(v1);
    if (l > 0)
      {
        double f = 1.0 / l;
        for (int i = 0; i < res.size(); ++i)
          {
            res[i] *= f;
          }
      }
    return res;
  }

  template<typename T>
  void normalize(std::vector<T>& v1)
  {
    double l = length(v1);
    if (l > 0)
      {
        double f = 1.0 / l;
        for (int i = 0; i < v1.size(); ++i)
          {
            v1[i] *= f;
          }
      }
  }

  template<typename T>
  void normalize(const std::vector<T>& v1, std::vector<T>& v2)
  {
    v2 = v1;
    double l = length(v1);
    if (l > 0)
      {
        double f = 1.0 / l;
        for (int i = 0; i < v2.size(); ++i)
          {
            v2[i] *= f;
          }
      }
  }

  template<typename T>
  double angle(const std::vector<T>& v1, const std::vector<T>& v2)
  {
    return acos(v1 * v2) / (length(v1) * length(v2));
  }

  template<typename T1, typename T2>
  std::vector<T2> convert(const std::vector<T1>& v1)
  {
    std::vector<T2> res(v1.size());
    for (int i = 0; i < v1.size(); ++i)
      {
        res[i] = static_cast<T2>(v1[i]);
      }
  }
}
#endif
