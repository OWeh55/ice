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

#include "swaptilt.h"

namespace ice
{

  int Mirror(const Image& img)
  {
    int xs = img->xsize;
    int ys = img->ysize;
    int xs2 = xs / 2;

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs2; x++)
        {
          int x1 = xs - 1 - x;
          int h = GetVal(img, x, y);
          PutVal(img, x, y, GetVal(img, x1, y));
          PutVal(img, x1, y, h);
        }

    return OK;
  }

  int Flip(const Image& img)
  {
    int xs = img->xsize;
    int ys = img->ysize;
    int ys2 = ys / 2;

    for (int y = 0; y < ys2; y++)
      {
        int y1 = ys - 1 - y;

        for (int x = 0; x < xs; x++)
          {
            int h = GetVal(img, x, y);
            PutVal(img, x, y, GetVal(img, x, y1));
            PutVal(img, x, y1, h);
          }
      }

    return OK;
  }
}
