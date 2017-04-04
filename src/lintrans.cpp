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

#include <cmath>

#include <iostream>
#include <vector>

#include "base.h"
#include "macro.h"
#include "IceException.h"
#include "histogram.h"
#include "drawline.h"
#include "WindowWalker.h"
#include "lintrans.h"

using namespace std;

namespace ice
{
#define FNAME "GetGrayLimits"
  int getGrayLimitsQuantile(const vector<double>hi, int& min, int& max,
                            double quantile)
  {
    if (quantile < 0 || quantile >= 0.5)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    int len = hi.size();
    double alq = 0;

    min = 0;
    while (alq <= quantile && min < len)
      {
        alq += hi[min];
        min++;
      }
    min--;

    double arq = 0;

    max = len - 1;
    while (arq <= quantile && max > 0)
      {
        arq += hi[max];
        max--;
      }

    max++;
    return OK;
  }

  // von außen nach innen bis zum jeweils ersten Grauwert
  // mit Häufigkeit >= mittlere H.
  int getGrayLimitsMean(const vector<double>& hi, int& l, int& r)
  {
    int len = hi.size();
    double mean = 1.0 / len;

    for (l = 0; l < len && hi[l] < mean; l++)
      /* nothing */ ;

    for (r = len - 1; r > 0 && hi[r] < mean; r--)
      /* nothing */ ;
    return OK;
  }

  int GetGrayLimits(const Histogram& h, int& l, int& r, int mode,
                    double quantile)
  {
    if (!h.isValid())
      {
        throw IceException(FNAME, M_INVALID_STRUCT, WRONG_PARAM);
      }

    int len = h.nClasses();
    vector<double> hi(len);

    //rel. Häuf. in Array schreiben
    for (int y = 0; y < len; y++)
      {
        hi[y] = h.getRelative(y);
      }

    if (mode == GV_QUANTILE)
      {
        return getGrayLimitsQuantile(hi, l, r, quantile);
      }
    else if (mode == GV_MEAN)
      {
        return getGrayLimitsMean(hi, l, r);
      }
    else
      {
        throw IceException(FNAME, M_WRONG_MODE, WRONG_PARAM);
      }

    return OK;
  }


  int GetGrayLimits(const Image& img, int& l, int& r, int mode,
                    double quantile)
  {
    if (!IsImg(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
      }

    Histogram h(img);

    return GetGrayLimits(h, l, r, mode, quantile);
  }

#undef FNAME

#define FNAME "GrayTransform"
  //g' = a1*g + a0
  int GrayTransform(const Image& src, const Image& tgt, double a1, double a0)
  {
    if (MatchImg(src, tgt) != OK)

      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
      }

    WindowWalker w(src);

    for (w.init(); !w.ready(); w.next())
      {
        tgt.setPixelLimited(w, RoundInt(a1 * src.getPixel(w) + a0));
      }

    return OK;
  }

  int GrayTransform(Image& src, double a1, double a0)
  {
    return GrayTransform(src, src, a1, a0);
  }
#undef FNAME

#define FNAME "GrayTransformLimits"
  //g' = a1*g + a0
  int GrayTransformLimits(const Image& src, const Image& tgt, int min, int max)
  {
    if (max < min)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    double diff = max - min;
    double a1, a0;
    if (diff == 0)
      {
        a1 = 0;
        a0 = tgt->maxval;
      }
    else
      {
        a1 = (tgt->maxval + 1) / diff;
        a0 = -min * a1;
      }

    RETURN_ERROR_IF_FAILED(GrayTransform(src, tgt, a1, a0));

    return OK;
  }

  int GrayTransformLimits(const Image& src, int min, int max)
  {
    return GrayTransformLimits(src, src, min, max);
  }
#undef FNAME

#define FNAME "GrayNormalize"
  int GrayNormalize(const Image& img, const Image& tgt, int mode,
                    double quantile)
  {
    int min, max;
    RETURN_ERROR_IF_FAILED(GetGrayLimits(img, min, max, mode,
                                         quantile));
    RETURN_ERROR_IF_FAILED(GrayTransformLimits(img, tgt, min, max));
    return OK;
  }

  int GrayNormalize(const Image& img, int mode,
                    double quantile)
  {
    return GrayNormalize(img, img, mode, quantile);
  }
#undef FNAME
}
