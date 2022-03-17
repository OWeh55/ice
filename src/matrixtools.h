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
#ifndef _MAT_TOOLS_H
#define _MAT_TOOLS_H
/*
 * tools for matrix<T>
 */
#include <vector>
#include "matrixtemplate.h"

namespace ice
{
  template<typename T>
  std::ostream& operator<<(std::ostream& os, const ice::matrix<T>& m)
  {
    std::streamsize width = os.width(0);
    os << "<" << std::endl ;

    for (int i = 0; i < m.rows(); i++)
      {
        os << " <";
        for (int k = 0; k < m.cols() - 1; k++)
          {
            os << std::setw(width) << m[i][k] << "," ;
          }

        os << std::setw(width) << m[i][m.cols() - 1] << ">" << std::endl ;
      }

    os << ">" << std::endl ;
    return os;
  }
}
#endif
