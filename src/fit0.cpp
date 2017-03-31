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
  Datenstrukturen für Fitting
*/

#include <vector>
#include "defs.h"
#include "message.h"
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
