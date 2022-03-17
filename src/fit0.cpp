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
  Datenstrukturen für Fitting
*/

#include <vector>
#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "fit.h"

namespace ice
{
  void Matrix2pl(const Matrix& m, std::vector<Point>& pl)
  {
    pl.resize(m.rows());

    for (int i = 0; i < m.rows(); i++)
      {
        pl[i] = Point(m[i][0], m[i][1]);
      }
  }

  void Matrix2w(const Matrix& m, std::vector<double>& w)
  {
    w.resize(m.rows());

    if (m.cols() < 3)
      for (int i = 0; i < m.rows(); i++)
        {
          w[i] = 1.0;
        }
    else
      for (int i = 0; i < m.rows(); i++)
        {
          w[i] = m[i][2];
        }
  }

} // namespace ice
