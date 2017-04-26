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

#include <vector>

#include "vectorfunc.h"

namespace ice
{
#if 0
  template<typename T>
  T Median(const VectorT<T>& v)
  {
    if (v.size() < 3)
      {
        return v[0];
      }

    VectorT<T> iv(v);

    iv.sort();
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

  template<typename T>
  VectorT<T> Normalize(const VectorT<T>& v)
  {
    VectorT<T> z = v;
    T l = 0;

    for (unsigned int i = 0; i < z.dim; i++)
      {
        l += z.data[i] * z.data[i];
      }

    l = sqrt(l);

    if (l == 0)
      {
        vERR("Normalize", "Can't normalize", WRONG_PARAM, z);
      }

    for (unsigned int i = 0; i < z.dim; i++)
      {
        z.data[i] /= l;
      }

    return z;
  }

  template<typename T>
  VectorT<T> Cross(const VectorT<T>& u, const VectorT<T>& v)
  {
    VectorT<T> z(3);

    if ((u.dim != 3) || (v.dim != 3))
      {
        vERR("Cross", M_WRONG_DIM, WRONG_PARAM, z);
      }

    z.data[0] = u.data[1] * v.data[2] - v.data[1] * u.data[2];
    z.data[1] = u.data[2] * v.data[0] - v.data[2] * u.data[0];
    z.data[2] = u.data[0] * v.data[1] - v.data[0] * u.data[1];
    return z;
  }
#endif
}
