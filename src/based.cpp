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
  /* Konvertierung Int-Bild in Double-Bild                           */
  /*******************************************************************/
#define FNAME "ConvImgImgD"
  void convImgImgD(const Image& inp, ImageD& out,
                   double factor, double offset)
  {
    int xs = inp.xsize;
    int ys = inp.ysize;

    if (!(out.xsize == xs && out.ysize == ys))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    for (int y = 0; y < ys; ++y)
      for (int x = 0; x < xs; ++x)
        {
          double pVal = (inp.getPixelUnchecked(x, y) - offset) * factor;
          out.setPixel(x, y, pVal);
        }
  }

  void ConvImgImgD(const Image& inp, ImageD& out,
                   int modus, int sign)
  {
    if (!inp.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!out.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    double offset = (sign == SIGNED) ? (inp.maxval + 1) / 2 : 0;
    double factor = (modus == NORMALIZED) ?  4.0 / (inp.maxval + 1) : 1.0;
    convImgImgD(inp, out, factor, offset);
    out.adaptLimits();
  }
#undef FNAME

  /*******************************************************************/
  /* Konvertierung Double-Bild in Int-Bild                           */
  /*******************************************************************/
#define FNAME "ConvImgDImg"
  void convImgDImg(const ImageD& input, const Image& output, double factor, double offset)
  {
    //    cout << factor << " " << offset << endl;
    int xs = input.xsize;
    int ys = input.ysize;

    if (!(output.xsize == xs && output.ysize == ys))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    for (int y = 0; y < ys; ++y)
      for (int x = 0; x < xs; ++x)
        {
          double dval = input.getPixelUnchecked(x, y);
          int ival = RoundInt(dval * factor + offset);
          //    cout << dval << " " << ival << endl;
          output.setPixelLimited(x, y, ival);
        }
  }

  void ConvImgDImg(const ImageD& input, const Image& out, int modus, int sign)
  {
    if (!IsImgD(input))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if (!IsImg(out))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int outmaxval = out.maxval;
    int outnull = (sign == SIGNED) ? (outmaxval + 1) / 2 : 0;
    if (modus != ADAPTIVE)
      {
        double factor = 1.0;
        if (modus == NORMALIZED)
          factor = outmaxval / 4.0;
        convImgDImg(input, out, factor, outnull);
      }
    else
      {
        ImageD inp(input); // copy to allow modification of limits
        inp.adaptLimits();

        if (sign == SIGNED)
          {
            double inmaxval = std::max(inp.maxValue(), -inp.minValue());
            double factor = (outmaxval / 2) / inmaxval;
            convImgDImg(input, out, factor, outnull);
          }
        else
          {
            double factor = outmaxval / (inp.maxValue() - inp.minValue());
            convImgDImg(input, out, factor, -inp.minValue() / factor);
          }
      }
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
