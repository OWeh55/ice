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
#ifndef VECTORARITH_H
#define VECTORARITH_H

#include <iostream>
#include <iomanip>
#include <vector>

#include "message.h"
#include "defs.h"

namespace ice
{
  template<typename T>
  std::vector<T> operator+(const std::vector<T>& v1, const std::vector<T>& v2)
  {
    std::vector<T> result(v1);
    for (unsigned int i = 0; i < v2.size(); ++i)
      result[i] += v2[i];
    return result;
  }

  template<typename T>
  std::vector<T> operator-(const std::vector<T>& v1, const std::vector<T>& v2)
  {
    std::vector<T> result(v1);
    for (unsigned int i = 0; i < v2.size(); ++i)
      result[i] -= v2[i];
    return result;
  }

  template<typename T>
  double norm2(const std::vector<T>& v)
  {
    double sum2 = 0.0;
    for (unsigned int i = 0; i < v.size(); ++i)
      sum2 += v[i] * v[i];
    return sum2;
  }

  template<typename T>
  double norm(const std::vector<T>& v)
  {
    return sqrt(norm2(v));
  }

}
#endif
