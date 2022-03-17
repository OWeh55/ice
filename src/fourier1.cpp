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
 * Fourier-Transformation (zweidimensional)
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
  //-----------------------------------------------------
#define FNAME "FourierImgD"
  int FourierImgD(ImageD srcre, ImageD srcim,
                  int option, ImageD re,
                  ImageD im, int mode)
  {
    bool direction;
    switch (option)
      {
      case NORMAL:
        direction = true;
        break;
      case INVERS:
        direction = false;
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }

    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImgD(srcre, srcim, re));
    RETURN_ERROR_IF_FAILED(MatchImgD(srcre, re, im, xs, ys));

    vector<double> vreal(xs);
    vector<double> vimag(xs);

    //    FourierTrafo ft(xs, direction, true);
    FourierTrafo ft(xs, direction, true);

    for (int y = 0; y < ys; ++y)
      {
        for (int x = 0; x < xs; ++x)
          {
            vreal[x] = GetValD(srcre, x, y);
            vimag[x] = GetValD(srcim, x, y);
          }

        if (mode != Y_ONLY)
          {
            ft.setInput(vreal, vimag);
            ft.getResult(vreal, vimag);
          }

        for (int x = 0; x < xs; ++x)
          {
            PutValD(re, x, y, vreal[x]);
            PutValD(im, x, y, vimag[x]);
          }
      }

    if (mode != X_ONLY)
      {
        ft.setParameter(ys, direction);
        vreal.resize(ys);
        vimag.resize(ys);
        for (int x = 0; x < xs; ++x)
          {
            for (int y = 0; y < ys; y++)
              {
                vreal[y] = GetValD(re, x, y);
                vimag[y] = GetValD(im, x, y);
              }

            ft.setInput(vreal, vimag);
            ft.getResult(vreal, vimag);

            for (int y = 0; y < ys; y++)
              {
                PutValD(re, x, y, vreal[y]);
                PutValD(im, x, y, vimag[y]);
              }
          }
      }

    return OK;
  }
#undef FNAME
}
