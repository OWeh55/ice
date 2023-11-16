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

#include "PointListWalker.h"

namespace ice
{
  void PointListWalker::moveTo(IPoint p)
  {
    IPoint dp = p - referencePoint;
    referencePoint = p;

    x += dp.x;
    y += dp.y;

    for (unsigned int i = 0; i < pl.size(); ++i)
      {
        pl[i] += dp;
      }
    init();
  }

  Neighbor4Walker::Neighbor4Walker(IPoint pp): PointListWalker(pp)
  {
    pl.push_back(IPoint(pp.x + 1, pp.y));
    pl.push_back(IPoint(pp.x,  pp.y + 1));
    pl.push_back(IPoint(pp.x - 1, pp.y));
    pl.push_back(IPoint(pp.x,  pp.y - 1));
    init();
  }

  Neighbor8Walker::Neighbor8Walker(int x, int y): PointListWalker(IPoint(x, y))
  {
    pl.push_back(IPoint(x + 1, y));
    pl.push_back(IPoint(x + 1, y + 1));
    pl.push_back(IPoint(x, y + 1));
    pl.push_back(IPoint(x - 1, y + 1));
    pl.push_back(IPoint(x - 1, y));
    pl.push_back(IPoint(x - 1, y - 1));
    pl.push_back(IPoint(x, y - 1));
    pl.push_back(IPoint(x + 1, y - 1));
    init();
  }

  Neighbor8Walker::Neighbor8Walker(IPoint pp): PointListWalker(pp)
  {
    pl.push_back(IPoint(pp.x + 1, pp.y));
    pl.push_back(IPoint(pp.x + 1, pp.y + 1));
    pl.push_back(IPoint(pp.x, pp.y + 1));
    pl.push_back(IPoint(pp.x - 1, pp.y + 1));
    pl.push_back(IPoint(pp.x - 1, pp.y));
    pl.push_back(IPoint(pp.x - 1, pp.y - 1));
    pl.push_back(IPoint(pp.x, pp.y - 1));
    pl.push_back(IPoint(pp.x + 1, pp.y - 1));
    init();
  }

}
