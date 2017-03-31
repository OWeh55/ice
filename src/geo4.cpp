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

#include "macro.h"
#include "geo.h"

// simple geometric transformations on images
namespace ice
{
#define FNAME "Shift"
  int Shift(const Image& simg, const Image& dimg, int dx, int dy, int mode)
  {
    int sx, sy;
    RETURN_ERROR_IF_FAILED(MatchImg(simg, dimg, sx, sy));

    Image himg = simg;

    if (simg == dimg)
      himg = NewImg(simg, true);

    for (int y = 0; y < sy; y++)
      {
        int yo = y - dy;

        if (yo < 0) yo += sy;
        else yo = yo % sy;

        for (int x = 0; x < sx; x++)
          {
            int xo = x - dx;

            if (xo < 0) xo += sx;
            else xo = xo % sx;

            PutVal(dimg, x, y, GetVal(himg, xo, yo));
          }
      }

    return OK;
  }

} // namespace ice
#undef FNAME
