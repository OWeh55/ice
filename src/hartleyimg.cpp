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
 * Hartley-Transformation (zweidimensional)
 * Wolfgang Ortmann, 1997, 1998, 2013
 */

#include <vector>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "darith.h"
#include "fourier.h"

#include "FourierTrafo.h"

using namespace std;

namespace ice
{
#define FNAME "HartleyImgD"
  void HartleyImgD(ImageD src, ImageD res, int mode)
  {
    try
      {
        int x, y, nx, ny;
        int xs, ys;
        int xsh, ysh;

        double* fr;
        double v1, v2, v3, v4, d;

        MatchImgD(src, res, xs, ys);

        xsh = xs / 2;
        ysh = ys / 2;

        if ((mode == XY) || (mode == X_ONLY))
          {
            fr = new double[xs];

            if (fr == NULL)
              throw IceException(FNAME, M_NO_MEM);

            for (y = 0; y < ys; ++y)
              {
                for (x = 0; x < xs; ++x)
                  {
                    fr[x] = GetValD(src, (x + xsh) % xs, y);
                  }

                HartleyD(fr, xs, fr);

                for (x = 0; x < xs; ++x)
                  {
                    PutValD(res, (x + xsh) % xs, y, fr[x]);
                  }
              }

            delete [] fr;
          }
        else
          for (y = 0; y < ys; ++y)
            for (x = 0; x < xs; ++x)
              {
                PutValD(res, x, y, GetValD(src, x, y));
              }

        if ((mode == XY) || (mode == Y_ONLY))
          {
            fr = new double[ys];

            if (fr == NULL)
              throw IceException(FNAME, M_NO_MEM);

            for (x = 0; x < xs; ++x)
              {
                for (y = 0; y < ys; ++y)
                  {
                    fr[y] = GetValD(res, x, (y + ysh) % ys);
                  }

                HartleyD(fr, ys, fr);

                for (y = 0; y < ys; ++y)
                  {
                    PutValD(res, x, (y + ysh) % ys, fr[y]);
                  }
              }

            delete [] fr;
          }

        if (mode == XY)
          {
            // row / column transform is not 2D Hartley transform !!
            // correction is necessary
            for (y = 0; y < ysh; y++)
              {
                ny = negf(y, ys);

                for (x = 0; x < xsh; x++)
                  {
                    nx = negf(x, xs);
                    v1 = GetValD(res, x, y);
                    v2 = GetValD(res, nx, y);
                    v3 = GetValD(res, x, ny);
                    v4 = GetValD(res, nx, ny);
                    d = (v1 - v2 - v3 + v4) / 2;
                    PutValD(res, x, y, v1 - d);
                    PutValD(res, nx, y, v2 + d);
                    PutValD(res, x, ny, v3 + d);
                    PutValD(res, nx, ny, v4 - d);
                  }
              }
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "HartleyImg"
  void HartleyImg(const Image& src, ImageD dst, int dir)
  {
    try
      {
        ImageD srcd = NewImgD(src);
        ConvImgImgD(src, srcd, NORMALIZED, SIGNED);
        HartleyImgD(srcd, dst, dir);
      }
    RETHROW;
  }

#undef FNAME
}
