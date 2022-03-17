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

#include "RegionWalker.h"

namespace ice
{

  void DiskWalker::makeSegments(int px, int py, double rad)
  {
    double rad2 = rad * rad;
    for (int y = py - rad; y <= py + rad; y++)
      {
        int xmin = px;
        int xmax = px;
        for (int x = px - rad; x <= px + rad; x++)
          {
            double xx = x - px;
            double yy = y - py;
            double x2 = xx * xx;
            double y2 = yy * yy;
            if (x2 + y2 <= rad2)
              {
                if (x < xmin)
                  {
                    xmin = x;
                  }
                if (x > xmax)
                  {
                    xmax = x;
                  }
              }
          }
        segments.push_back(RowSegment(xmin, xmax, y));
      }
    referencePoint = IPoint(px, py);
    relativateSegments();
  }
}
