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
#ifndef VECTORFUNC_H
#define VECTORFUNC_H

#include "Vector.h"

#define vERR(f,m) { throw IceException("VectorT::" f,m); }

namespace ice
{
  template <typename T>
  T Median(const VectorT<T>& v);
  template <typename T>
  T Min(const VectorT<T>& v);
  template <typename T>
  T Max(const VectorT<T>& v);

  template <typename T>
  VectorT<T> Normalize(const VectorT<T>& v);

  template <typename T>
  VectorT<T> Cross(const VectorT<T>& u, const VectorT<T>& v);

  template<typename T>
  T Median(const VectorT<T>& v)
  {
    if (v.size() < 3)
      {
        return v[0];
      }

    VectorT<T> iv(v);

    iv.Sort();
    return iv[v.size() / 2];
  }

  template<typename T>
  T Min(const VectorT<T>& v)
  {
    T min = v[0];

    for (unsigned int i = 1; i < v.size(); i++)
      {
        if (v[i] < min)
          {
            min = v[i];
          }
      }

    return min;
  }

  template<typename T>
  T Max(const VectorT<T>& v)
  {
    double max = v[0];

    for (unsigned int i = 1; i < v.size(); i++)
      {
        if (v[i] > max)
          {
            max = v[i];
          }
      }

    return max;
  }

#define FNAME "Normalize"
  template<typename T>
  VectorT<T> Normalize(const VectorT<T>& v)
  {
    VectorT<T> z = v;
    T l = 0;

    for (unsigned int i = 0; i < z.size(); i++)
      {
        l += z.data[i] * z.data[i];
      }

    l = sqrt(l);

    if (l == 0)
      {
        vERR("Normalize", M_ZERO_VECTOR);
      }

    for (unsigned int i = 0; i < z.size(); i++)
      {
        z.data[i] /= l;
      }

    return z;
  }
#undef FNAME
#define FNAME "Cross"
  template<typename T>
  VectorT<T> Cross(const VectorT<T>& u, const VectorT<T>& v)
  {
    VectorT<T> z(3);

    if ((u.dim != 3) || (v.dim != 3))
      {
        vERR(FNAME, M_WRONG_DIMENSION);
      }

    z.data[0] = u.data[1] * v.data[2] - v.data[1] * u.data[2];
    z.data[1] = u.data[2] * v.data[0] - v.data[2] * u.data[0];
    z.data[2] = u.data[0] * v.data[1] - v.data[0] * u.data[1];
    return z;
  }
#undef FNAME
}
#endif
