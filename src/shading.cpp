/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2013 FSU Jena, Digital Image Processing Group
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

#include "macro.h"
#include "fitfn.h"
#include "shading.h"

/**********************************************************************
    Shadingkorrektur
***********************************************************************/

namespace ice
{
#define FNAME "Shading"
  int correctShading(const Image& img, const Image& imgd, Function2dWithFitting& fn)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(img, imgd, xs, ys));

    fn.Fit(img);

    for (int y = 0; y < ys; ++y)
      for (int x = 0; x < xs; ++x)
        {
          int val = img.getPixel(x, y);
          val = RoundInt(val - fn(x, y));
          imgd.setPixelLimited(x, y, val + imgd.maxval / 2);
        }
    return OK;
  }

  int correctShading(const Image& img, const Image& imgd, int mode)
  {
    Function2dWithFitting* fn;

    if (mode < 0)
      {
        throw IceException(FNAME, M_WRONG_MODE, WRONG_PARAM);
      }

    switch (mode)
      {
      case 0:
        fn = new Constant2d();
        break;
      case 1:
        fn = new Polynom2d1o();
        break;
      case 2:
        fn = new Polynom2d2o();
        break;

      default:
        fn = new Polynom2d(mode);
        break;
      }

    int rc = correctShading(img, imgd, *fn);

    delete fn;
    return rc;
  }
#undef FNAME
}
