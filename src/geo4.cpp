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

#include "macro.h"
#include "geo.h"

// simple geometric transformations on images
namespace ice
{
#define FNAME "shift"
  void shift(const Image& simg, const Image& dimg, int dx, int dy, int mode)
  {
    try
      {
        int sx, sy, mv;
        checkImage(simg, dimg, sx, sy, mv);

        Image himg = simg;

        if (simg == dimg)
          {
            himg = NewImg(simg, true);
          }

        for (int y = 0; y < sy; y++)
          {
            int yo = y - dy;

            if (yo < 0)
              {
                yo += sy;
              }
            else
              {
                yo = yo % sy;
              }

            for (int x = 0; x < sx; x++)
              {
                int xo = x - dx;

                if (xo < 0)
                  {
                    xo += sx;
                  }
                else
                  {
                    xo = xo % sx;
                  }

                dimg.setPixel(x, y, himg.getPixel(xo, yo));
              }
          }
      }
    RETHROW;
  }
#undef FNAME

} // namespace ice

