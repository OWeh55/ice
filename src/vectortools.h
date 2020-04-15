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
#ifndef VECTORTOOLS_H
#define VECTORTOOLS_H

#include <iostream>
#include <iomanip>
#include <vector>

#include "IceException.h"
#include "defs.h"

namespace ice
{
  template<typename T>
  T medianValue(std::vector<T> v) // call by value = copy !!
  {
    if (v.size() < 3)
      {
        return v[0];  // everything is median
      }
    sort(v.begin(), v.end());
    return v[v.size() / 2];
  }

  template<typename T>
  T minValue(const std::vector<T>& v)
  {
    T min = v[0];
    for (auto d : v)
      {
        if (d < min)
          {
            min = d;
          }
      }

    return min;
  }

  template<typename T>
  T maxValue(const std::vector<T>& v)
  {
    double max = v[0];
    for (auto d : v)
      {
        if (d > max)
          {
            max = d;
          }
      }

    return max;
  }

  template<typename T>
  int minIndex(const std::vector<T>& v)
  {
    T min = v[0];
    int res = 0;
    for (unsigned int i = 1; i < v.size(); i++)
      {
        if (v[i] < min)
          {
            min = v[i];
            res = i;
          }
      }
    return res;
  }

  template<typename T>
  int maxIndex(const std::vector<T>& v)
  {
    T max = v[0];
    int res = 0;
    for (unsigned int i = 1; i < v.size(); i++)
      {
        if (v[i] > max)
          {
            max = v[i];
            res = i;
          }
      }

    return res;
  }

  template<typename T>
  std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
  {
    std::locale cLocale("C");
    std::locale oldLocale = os.imbue(cLocale);
    std::streamsize width = os.width(0);
    os << "<" ;
    if (!v.empty())
      {
        for (int k = 0; k < (int)v.size() - 1; k++)
          {
            os << std::setw(width) << v[k] << "," ;
          }

        os << std::setw(width) << v[v.size() - 1];
      }
    os << ">";
    os.imbue(oldLocale);
    return os;
  }

#define FNAME "operator >>"
  template<typename T>
  std::istream& operator>>(std::istream& is, std::vector<T>& v)
  {
    std::locale cLocale("C");
    std::locale oldLocale = is.imbue(cLocale);
    T d;
    char c = 0;
    v.clear();
    is >> c;
    if (is.good())
      {
        if (c != '<')
          {
            throw IceException(FNAME, M_WRONG_FILEFORMAT);
            is.clear();
            return is;
          }

        if (is.peek() == '>')
          {
            // empty VectorT
            is.ignore(1, '\n');
            return is;
          }

        do
          {
            is >> d >> c ;
            v.push_back(d);

            if ((c != ',') && (c != '>'))
              {
                throw IceException(FNAME, M_WRONG_FILEFORMAT);
                is.clear();
                return is;
              }
          }
        while (c != '>');
      }
    is.imbue(oldLocale);
    return is;
  }
#undef FNAME
}
#endif
