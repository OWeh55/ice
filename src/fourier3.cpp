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
/*
 * Routinen im Fourierraum/Hartley-Raum
 * Wolfgang Ortmann, 97, 98
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "macro.h"
#include "defs.h"
#include "message.h"
#include "darith.h"

#include "fourier.h"

namespace ice
{
#define FNAME "ConvolutionFImgD"
  int ConvolutionFImgD(ImageD r1, ImageD i1, ImageD r2, ImageD i2,
                       ImageD r3, ImageD i3)
  {
    int xs, ys;
    double dr1, di1, dr2, di2, dr3, di3;
    double factor;

    RETURN_ERROR_IF_FAILED(MatchImgD(r1, i1, r2));
    RETURN_ERROR_IF_FAILED(MatchImgD(r2, i2, r3));
    RETURN_ERROR_IF_FAILED(MatchImgD(r3, i3, xs, ys));

    factor = sqrt(double(xs)) * sqrt(double(ys));
    for (int y = 0; y < r1.ysize; ++y)
      for (int x = 0; x < r1.xsize; ++x)
        {
          dr1 = GetValD(r1, x, y);
          di1 = GetValD(i1, x, y);
          dr2 = GetValD(r2, x, y);
          di2 = GetValD(i2, x, y);
          dr3 = (dr1 * dr2 - di1 * di2) * factor;
          di3 = (dr1 * di2 + di1 * dr2) * factor;
          PutValD(r3, x, y, dr3);
          PutValD(i3, x, y, di3);
        }
    return OK;
  }
#undef FNAME
#define FNAME "ConvolutionHImgD"
  int ConvolutionHImgD(ImageD i1, ImageD i2, ImageD i3)
  {
    int xn, yn;
    int xs, ys;
    double d1, dn1, d2, dn2, d3;
    double factor;
    int NEED_TEMP = false;
    ImageD hdest;

    RETURN_ERROR_IF_FAILED(MatchImgD(i1, i2, i3, xs, ys));

    if ((i3 == i1) || (i3 == i2))
      {
        NEED_TEMP = true;
        hdest = NewImgD(xs, ys);
      }
    else
      {
        hdest = i3;
      }

    factor = sqrt(double(xs)) * sqrt(double(ys)) / 2;

    for (int y = 0; y < i1.ysize; ++y)
      for (int x = 0; x < i1.xsize; ++x)
        {
          xn = negf(x, xs);
          yn = negf(y, ys);
          d1 = GetValD(i1, x, y);
          dn1 = GetValD(i1, xn, yn);
          d2 = GetValD(i2, x, y);
          dn2 = GetValD(i2, xn, yn);
          d3 = (d1 * d2 + d1 * dn2 + dn1 * d2 - dn1 * dn2) * factor;
          PutValD(hdest, x, y, d3);
        }

    if (NEED_TEMP)
      {
        for (int y = 0; y < i1.ysize; ++y)
          for (int x = 0; x < i1.xsize; ++x)
            {
              PutValD(i3, x, y, GetValD(hdest, x, y));
            }
        FreeImgD(hdest);
      }

    return OK;
  }
#undef FNAME
#define FNAME "InvConvolutionFImgD"
  int InvConvolutionFImgD(ImageD r1, ImageD i1, ImageD r2, ImageD i2,
                          ImageD r3, ImageD i3, double beta)
  {
    int xs, ys;
    double dr1, di1, dr2, di2, dr3, di3;
    double denom;
    double factor;
    double beta2 = beta * beta;

    if (beta < 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(MatchImgD(r1, i1, r2, xs, ys));
    RETURN_ERROR_IF_FAILED(MatchImgD(r2, i2, r3, xs, ys));
    RETURN_ERROR_IF_FAILED(MatchImgD(r3, i3, xs, ys));

    factor = 1 / sqrt(double(xs * ys));

    for (int y = 0; y < r1.ysize; ++y)
      for (int x = 0; x < r1.xsize; ++x)
        {
          dr1 = GetValD(r1, x, y);
          di1 = GetValD(i1, x, y);
          dr2 = GetValD(r2, x, y);
          di2 = GetValD(i2, x, y);
          denom = dr1 * dr1 + di1 * di1;

          if (beta == 0)
            {
              if (denom == 0)
                {
                  dr3 = 0;
                  di3 = 0;
                }
              else
                {
                  dr3 = (dr1 * dr2 + di1 * di2) / denom * factor;
                  di3 = (dr1 * di2 - di1 * dr2) / denom * factor;
                }
            }
          else
            {
              denom = denom + beta2;
              dr3 = (dr1 * dr2 + di1 * di2) / denom * factor;
              di3 = (dr1 * di2 - di1 * dr2) / denom * factor;
            }

          PutValD(r3, x, y, dr3);
          PutValD(i3, x, y, di3);
        }
    return OK;
  }
#undef FNAME
#define FNAME "InvConvolutionHImgD"
  int InvConvolutionHImgD(ImageD i1, ImageD i2, ImageD i3, double beta)
  {
    int xn, yn;
    int xs, ys;
    double d1, dn1, d2, dn2, d3;
    double dr1, di1, dr2, di2;
    int NEED_TEMP = false;
    ImageD hdest;

    //  double denom;
    double factor;
    double beta2 = beta * beta;

    if (beta < 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(MatchImgD(i1, i2, i3, xs, ys));

    if ((i3 == i1) || (i3 == i2))
      {
        NEED_TEMP = true;
        hdest = NewImgD(xs, ys);
      }
    else
      {
        hdest = i3;
      }

    factor = 1 / sqrt(double(xs * ys));
    for (int y = 0; y < i1.ysize; ++y)
      for (int x = 0; x < i1.xsize; ++x)
        {
          xn = negf(x, xs);
          yn = negf(y, ys);

          d1 = GetValD(i1, x, y);
          dn1 = GetValD(i1, xn, yn);
          dr1 = (d1 + dn1) / 2;
          di1 = (-d1 + dn1) / 2;

          d2 = GetValD(i2, x, y);
          dn2 = GetValD(i2, xn, yn);
          dr2 = (d2 + dn2) / 2;
          di2 = (-d2 + dn2) / 2;

          double denom = dr1 * dr1 + di1 * di1;

          if (beta == 0)
            {
              if (denom == 0)
                {
                  d3 = 0;
                }
              else
                {
                  d3 = (dr1 * dr2 + di1 * di2 - dr1 * di2 + di1 * dr2) / denom * factor;
                }
            }
          else
            {
              d3 = (dr1 * dr2 + di1 * di2 - dr1 * di2 + di1 * dr2) / (denom + beta2) * factor;
            }

          PutValD(hdest, x, y, d3);
        }

    if (NEED_TEMP)
      {
        for (int y = 0; y < i3.ysize; ++y)
          for (int x = 0; x < i3.xsize; ++x)
            {
              PutValD(i3, x, y, GetValD(hdest, x, y));
            }
        FreeImgD(hdest);
      }

    return OK;
  }
#undef FNAME
#define FNAME "CrossCorrelationFImgD"
  int CrossCorrelationFImgD(ImageD r1, ImageD i1, ImageD r2, ImageD i2,
                            ImageD r3, ImageD i3)
  {

    int xs, ys;
    double dr1, di1, dr2, di2, dr3, di3;
    double factor;

    RETURN_ERROR_IF_FAILED(MatchImgD(r1, i1, r2, xs, ys));
    RETURN_ERROR_IF_FAILED(MatchImgD(r2, i2, r3, xs, ys));
    RETURN_ERROR_IF_FAILED(MatchImgD(r3, i3, xs, ys));

    factor = sqrt(double(xs)) * sqrt(double(ys));
    for (int y = 0; y < r1.ysize; ++y)
      for (int x = 0; x < r1.xsize; ++x)
        {
          dr1 = GetValD(r1, x, y);
          di1 = GetValD(i1, x, y);
          dr2 = GetValD(r2, x, y);
          di2 = GetValD(i2, x, y);
          dr3 = (dr1 * dr2 + di1 * di2) * factor;
          di3 = (-dr1 * di2 + di1 * dr2) * factor;
          PutValD(r3, x, y, dr3);
          PutValD(i3, x, y, di3);
        }
    return OK;
  }
#undef FNAME
#define FNAME "CrossCorrelationHImgD"
  int CrossCorrelationHImgD(ImageD i1, ImageD i2, ImageD i3)
  {
    int xn, yn;
    int xs, ys;
    double d1, dn1, d2, dn2, d3;
    double factor;
    int NEED_TEMP = false;
    ImageD hdest;

    RETURN_ERROR_IF_FAILED(MatchImgD(i1, i2, i3, xs, ys));

    if ((i3 == i1) || (i3 == i2))
      {
        NEED_TEMP = true;
        hdest = NewImgD(xs, ys);
      }
    else
      {
        hdest = i3;
      }

    factor = sqrt(double(xs)) * sqrt(double(ys)) / 2;

    for (int y = 0; y < i1.ysize; ++y)
      for (int x = 0; x < i1.xsize; ++x)
        {
          xn = negf(x, xs);
          yn = negf(y, ys);
          d1 = GetValD(i1, x, y);
          dn1 = GetValD(i1, xn, yn);
          d2 = GetValD(i2, x, y);
          dn2 = GetValD(i2, xn, yn);
          d3 = (d1 * d2 + d1 * dn2 - dn1 * d2 + dn1 * dn2) * factor;
          PutValD(hdest, x, y, d3);
        }

    if (NEED_TEMP)
      {
        for (int y = 0; y < i3.ysize; ++y)
          for (int x = 0; x < i3.xsize; ++x)
            {
              PutValD(i3, x, y, GetValD(hdest, x, y));
            }
        FreeImgD(hdest);
      }

    return OK;
  }
#undef FNAME
#define FNAME "CrossCorrelationImgD"
  int CrossCorrelationImgD(ImageD i1, ImageD i2, ImageD i3)
  {
    int xs, ys;

    ImageD h1, h2;

    RETURN_ERROR_IF_FAILED(MatchImgD(i1, i2, i3, xs, ys));

    h1 = NewImgD(xs, ys);
    h2 = NewImgD(xs, ys);

    IF_FAILED(HartleyImgD(i1, h1))
    {
      FreeImgD(h1);
      FreeImgD(h2);
      return ERROR;
    }

    IF_FAILED(HartleyImgD(i2, h2))
    {
      FreeImgD(h1);
      FreeImgD(h2);
      return ERROR;
    }

    IF_FAILED(CrossCorrelationHImgD(h1, h2, i3))
    {
      FreeImgD(h1);
      FreeImgD(h2);
      return ERROR;
    }

    IF_FAILED(HartleyImgD(i3, i3))
    {
      FreeImgD(h1);
      FreeImgD(h2);
      return ERROR;
    }

    FreeImgD(h1);
    FreeImgD(h2);

    return OK;
  }
#undef FNAME
}
