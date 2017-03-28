/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
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

#include "WindowWalker.h"

namespace ice
{
  WindowWalker::WindowWalker(IPoint center, int sizex, int sizey):
    Walker(center)
  {
    if (sizey < 0)
      sizey = sizex;
    int sx = sizex / 2;
    int sx2 = sizex - 1 - sx;
    int sy = sizey / 2;
    int sy2 = sizey - 1 - sy;
    x1 = center.x - sx;
    x2 = center.x + sx2;
    y1 = center.y - sy;
    y2 = center.y + sy2;
    x = x1;
    y = y1;
  }
}
