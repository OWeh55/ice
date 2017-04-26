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
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"
#include "based.h"
#include "util.h"

using namespace std;

namespace ice
{

#define FNAME "MatchImgD"
  int MatchImgD(const ImageD& i1, const ImageD& i2, int& xs, int& ys)
  {
    if (! i1.isValid() || ! i2.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    xs = i1.xsize;
    ys = i1.ysize;

    if ((xs != i2.xsize) || (ys != i2.ysize))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    return OK;
  }

  int MatchImgD(const ImageD& i1, const ImageD& i2, const ImageD& i3, int& xs, int& ys)
  {
    if (! i1.isValid() || ! i2.isValid() || ! i3.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    xs = i1.xsize;
    ys = i1.ysize;

    if ((xs != i2.xsize) || (ys != i2.ysize) ||
        (xs != i3.xsize) || (ys != i3.ysize))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    return OK;
  }

  int MatchImgD(const ImageD& i1, const ImageD& i2)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImgD(i1, i2, xs, ys));
    return OK;
  }

  int MatchImgD(const ImageD& i1, const ImageD& i2, const ImageD& i3)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImgD(i1, i2, i3, xs, ys));
    return OK;
  }

#undef FNAME

  /*******************************************************************/
  /* Aktualisieren der Eintraege minval, maxval                      */
  /*******************************************************************/
#define FNAME "UpdateLimitImgD"
  int UpdateLimitImgD(ImageD& img)
  {
    double max, min, val;

    if (!img.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    max = img.getPixelUnchecked(0, 0);
    min = max;

    for (int y = 0; y < img.ysize; ++y)
      for (int x = 0; x < img.xsize; ++x)
        {
          val = img.getPixelUnchecked(x, y);
          if (max < val)
            {
              max = val;
            }
          if (min > val)
            {
              min = val;
            }
        }

    if (min == max)
      {
        min -= 1e-150;
        max += 1e-150;
      }

    img.minval = min;
    img.maxval = max;
    return OK;
  }
#undef FNAME

  /*******************************************************************/
  /* Konvertierung Int-Bild in Double-Bild                           */
  /*******************************************************************/
#define FNAME "ConvImgImgD"
  int ConvImgImgD(const Image& inp, ImageD& out, int modus, int sign)
  {
    int sx, sy;

    int goff = 0;
    double factor = 1;

    if (!inp.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!out.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    sx = inp.xsize;
    sy = inp.ysize;

    if (!(out.xsize == sx && out.ysize == sy))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    if (sign == SIGNED)
      {
        goff = (inp.maxval + 1) / 2;
      }

    if (modus == NORMALIZED)
      {
        factor = 4.0 / (inp.maxval + 1);
      }

    sx = Min(sx, out.xsize);
    sy = Min(sy, out.ysize);

    for (int y = 0; y < sy; ++y)
      {
        for (int x = 0; x < sx; ++x)
          {
            out.setPixel(x, y,
                         (inp.getPixelUnchecked(x, y) - goff)*factor);
          }
      }

    UpdateLimitImgD(out);
    return OK;
  }
#undef FNAME

  /*******************************************************************/
  /* Konvertierung Double-Bild in Int-Bild                           */
  /*******************************************************************/
#define FNAME "ConvImgDImg"
  int ConvImgDImg(const ImageD& input, const Image& out, int modus, int sign)
  {

    int outmaxval;
    int xs, ys;
    double inmaxval;
    double factor = 1.0, offset = 0.0;

    if (!IsImgD(input))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if (!IsImg(out))
      throw IceException(FNAME, M_WRONG_IMAGE);

    outmaxval = out.maxval;

    ImageD inp(input); // copy to allow modification of limits

    if (modus == ADAPTIVE)
      {
        UpdateLimitImgD(inp);
      }

    if (sign == SIGNED)
      {
        switch (modus)
          {
          case ADAPTIVE:
            inmaxval = Max(inp.maxval, -inp.minval);
            factor = (outmaxval / 2) / inmaxval;
            offset = (outmaxval + 1) / factor / 2.0;
            break;
          case RAW:
            factor = 1.0;
            offset = (outmaxval + 1) / 2;
            break;
          case NORMALIZED:
            factor = (outmaxval + 1) / 4.0;
            offset = 2.0;
            break;
          }
      }
    else
      {
        switch (modus)
          {
          case ADAPTIVE:
            offset = -inp.minval;
            factor = outmaxval / (inp.maxval - inp.minval);
            break;
          case RAW:
            factor = 1;
            offset = 0;
            break;
          case NORMALIZED:
            factor = (outmaxval + 1) / 4.0;
            offset = 0;
            break;
          }
      }

    xs = inp.xsize;
    ys = inp.ysize;

    if (!(out.xsize == xs && out.ysize == ys))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    for (int y = 0; y < ys; ++y)
      {
        for (int x = 0; x < xs; ++x)
          {
            int ival = RoundInt((inp.getPixelUnchecked(x, y) + offset) * factor);
            out.setPixelLimited(x, y, ival);
          }
      }

    return OK;
  }
#undef FNAME
  //------------------------------------------------------
#define FNAME "LogImgD"
  int LogImgD(const ImageD& src, const ImageD& dst)
  {
    int xs, ys;

    if (! MatchImgD(src, dst, xs, ys))
      throw IceException(FNAME, M_0);

    for (int y = 0; y < ys; ++y)
      for (int x = 0; x < xs; ++x)
        {
          PixelFloatType v = src.getPixel(x, y);

          if (v != 0.0)
            {
              v = log10(v);
            }
          else
            {
              v = -1e12;
            }

          dst.setPixel(x, y, v);
        }

    return OK;
  }
#undef FNAME

  double GetInterpolValD(const ImageD& img, double x, double y)
  {
    // if x and y are to far outside the image border, we simply return zero
    if (x < -0.5 || y < -0.5)
      {
        return 0;
      }

    // Determine the local neighborhood of point (x, y), that means the 4 pixel positions
    // (xi, yi), (xi + 1, yi), (xi + 1, yi + 1), and (xi, yi + 1), that enclose (x, y)
    x = max<double> (x, 0.0);
    y = max<double> (y, 0.0);
    int xi  = (int) x;
    int xi1 = xi + 1;
    int yi  = (int) y;
    int yi1 = yi + 1;

    // check

    if ((xi >= img.xsize) || (yi >= img.ysize))
      {
        return 0.0;
      }

    if ((xi1 == img.xsize) || (yi1 == img.ysize))
      {
        return GetValD(img, xi, yi);  // rechter/unterer Rand
      }

    double dx  = x - (double) xi;
    double dx1 = 1.0 - dx;
    double dy  = y - (double) yi;
    double dy1 = 1.0 - dy;
    return
      dx1 * (dy1 * GetValD(img, xi, yi)
             + dy * GetValD(img, xi, yi1)) +
      + dx  * (dy1 * GetValD(img, xi1, yi)
               + dy * GetValD(img, xi1, yi1));
  }

  bool GetInterpolValD(const ImageD& img, double x, double y, double& val)
  {
    val = 0;

    // if x and y are to far outside the image border, we simply return false
    if (x < -0.5 || y < -0.5)
      {
        return false;
      }

    // Determine the local neighborhood of point (x, y), that means the 4 pixel positions
    // (xi, yi), (xi + 1, yi), (xi + 1, yi + 1), and (xi, yi + 1), that enclose (x, y)
    x = max<double> (x, 0.0);
    y = max<double> (y, 0.0);
    int xi  = (int) x;
    int xi1 = xi + 1;
    int yi  = (int) y;
    int yi1 = yi + 1;

    if ((xi >= img.xsize) || (yi >= img.ysize))
      {
        return 0.0;
      }

    if ((xi1 == img.xsize) || (yi1 == img.ysize))
      {
        return GetValD(img, xi, yi);  // rechter/unterer Rand
      }

    double dx  = x - (double) xi;
    double dx1 = 1.0 - dx;
    double dy  = y - (double) yi;
    double dy1 = 1.0 - dy;
    val = dx1 * (dy1 * GetValD(img, xi, yi) +
                 dy * GetValD(img, xi, yi1))
          +  dx  * (dy1 * GetValD(img, xi1, yi) +
                    dy * GetValD(img, xi1, yi1));
    return true;
  }

#define FNAME "Inside"
  bool Inside(const ImageD& img, int x, int y)
  {
    if (!IsImgD(img))
      throw IceException(FNAME, M_WRONG_IMAGED);
    return img.inside(x, y);
  }

  bool Inside(const ImageD& img, IPoint p)
  {
    return Inside(img, p.x, p.y);
  }

  bool Inside(const ImageD& img, const Window& w)
  {
    return Inside(img, w.p1) && Inside(img, w.p2);
  }

  void setborder(const ImageD& p, int sx, int sy, double val)
  {
    // Rand fuellen
    for (int i = 0; i < p.xsize; i++)
      {
        for (int j = 0; j < sy; j++)
          {
            PutValD(p, i, j, 0.0);
            PutValD(p, i, p.ysize - 1 - j, val);
          }
      }

    for (int i = sy; i < p.ysize - sy; i++)
      {
        for (int j = 0; j < sx; j++)
          {
            PutValD(p, j, i, 0.0);
            PutValD(p, p.xsize - 1 - j, i, val);
          }
      }
  }
#undef FNAME
}
