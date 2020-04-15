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
#include "IceException.h"
#include "darith.h"

#include "fourier.h"

namespace ice
{
#define FNAME "ConvolutionFImgD"
  void ConvolutionFImgD(ImageD r1, ImageD i1, ImageD r2, ImageD i2,
                        ImageD r3, ImageD i3)
  {
    try
      {

        int xs, ys;
        MatchImgD(r1, i1, r2);
        MatchImgD(r2, i2, r3);
        MatchImgD(r3, i3, xs, ys);

        double factor  = sqrt(double(xs)) * sqrt(double(ys));
        for (int y = 0; y < r1.ysize; ++y)
          for (int x = 0; x < r1.xsize; ++x)
            {
              double dr1 = GetValD(r1, x, y);
              double di1 = GetValD(i1, x, y);
              double dr2 = GetValD(r2, x, y);
              double di2 = GetValD(i2, x, y);
              double dr3 = (dr1 * dr2 - di1 * di2) * factor;
              double di3 = (dr1 * di2 + di1 * dr2) * factor;
              PutValD(r3, x, y, dr3);
              PutValD(i3, x, y, di3);
            }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "ConvolutionHImgD"
  void ConvolutionHImgD(ImageD i1, ImageD i2, ImageD i3)
  {
    try
      {
        int xs, ys;
        MatchImgD(i1, i2, i3, xs, ys);

        int NEED_TEMP = false;
        ImageD hdest;

        if ((i3 == i1) || (i3 == i2))
          {
            NEED_TEMP = true;
            hdest = NewImgD(xs, ys);
          }
        else
          {
            hdest = i3;
          }

        double factor = sqrt(double(xs)) * sqrt(double(ys)) / 2;

        for (int y = 0; y < i1.ysize; ++y)
          for (int x = 0; x < i1.xsize; ++x)
            {
              double xn = negf(x, xs);
              double yn = negf(y, ys);
              double d1 = GetValD(i1, x, y);
              double dn1 = GetValD(i1, xn, yn);
              double d2 = GetValD(i2, x, y);
              double dn2 = GetValD(i2, xn, yn);
              double   d3 = (d1 * d2 + d1 * dn2 + dn1 * d2 - dn1 * dn2) * factor;
              PutValD(hdest, x, y, d3);
            }

        if (NEED_TEMP)
          {
            for (int y = 0; y < i1.ysize; ++y)
              for (int x = 0; x < i1.xsize; ++x)
                {
                  PutValD(i3, x, y, GetValD(hdest, x, y));
                }
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "InvConvolutionFImgD"
  void InvConvolutionFImgD(ImageD r1, ImageD i1, ImageD r2, ImageD i2,
                           ImageD r3, ImageD i3, double beta)
  {
    try
      {
        if (beta < 0)
          throw IceException(FNAME, M_WRONG_PARAMETER);

        double beta2 = beta * beta;

        int xs, ys;
        MatchImgD(r1, i1, r2);
        MatchImgD(r2, i2, r3);
        MatchImgD(r3, i3, xs, ys);

        double factor = 1 / sqrt(double(xs * ys));

        for (int y = 0; y < r1.ysize; ++y)
          for (int x = 0; x < r1.xsize; ++x)
            {
              double dr1 = GetValD(r1, x, y);
              double di1 = GetValD(i1, x, y);
              double dr2 = GetValD(r2, x, y);
              double di2 = GetValD(i2, x, y);
              double denom = dr1 * dr1 + di1 * di1;

              double dr3, di3;
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
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "InvConvolutionHImgD"
  void InvConvolutionHImgD(ImageD i1, ImageD i2, ImageD i3, double beta)
  {
    try
      {
        if (beta < 0)
          throw IceException(FNAME, M_WRONG_PARAMETER);
        double beta2 = beta * beta;

        int xs, ys;
        MatchImgD(i1, i2, i3, xs, ys);

        ImageD hdest;
        bool NEED_TEMP = false;
        if ((i3 == i1) || (i3 == i2))
          {
            NEED_TEMP = true;
            hdest = NewImgD(xs, ys);
          }
        else
          {
            hdest = i3;
          }

        double factor = 1 / sqrt(double(xs * ys));
        for (int y = 0; y < i1.ysize; ++y)
          for (int x = 0; x < i1.xsize; ++x)
            {
              int xn = negf(x, xs);
              int yn = negf(y, ys);

              double d1 = GetValD(i1, x, y);
              double dn1 = GetValD(i1, xn, yn);
              double dr1 = (d1 + dn1) / 2;
              double di1 = (-d1 + dn1) / 2;

              double d2 = GetValD(i2, x, y);
              double dn2 = GetValD(i2, xn, yn);
              double dr2 = (d2 + dn2) / 2;
              double di2 = (-d2 + dn2) / 2;

              double denom = dr1 * dr1 + di1 * di1;

              double d3;
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
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "CrossCorrelationFImgD"
  void CrossCorrelationFImgD(ImageD r1, ImageD i1, ImageD r2, ImageD i2,
                             ImageD r3, ImageD i3)
  {

    try
      {
        int xs, ys;
        MatchImgD(r1, i1, xs, ys);
        MatchImgD(r1, r2, i2);
        MatchImgD(r1, r3, i3);

        double factor = sqrt(double(xs)) * sqrt(double(ys));

        for (int y = 0; y < r1.ysize; ++y)
          for (int x = 0; x < r1.xsize; ++x)
            {
              double dr1 = GetValD(r1, x, y);
              double di1 = GetValD(i1, x, y);
              double dr2 = GetValD(r2, x, y);
              double di2 = GetValD(i2, x, y);
              double dr3 = (dr1 * dr2 + di1 * di2) * factor;
              double di3 = (-dr1 * di2 + di1 * dr2) * factor;
              PutValD(r3, x, y, dr3);
              PutValD(i3, x, y, di3);
            }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "CrossCorrelationHImgD"
  void CrossCorrelationHImgD(ImageD i1, ImageD i2, ImageD i3)
  {
    try
      {
        int xs, ys;
        MatchImgD(i1, i2, i3, xs, ys);

        int NEED_TEMP = false;
        ImageD hdest;

        if ((i3 == i1) || (i3 == i2))
          {
            NEED_TEMP = true;
            hdest = NewImgD(xs, ys);
          }
        else
          {
            hdest = i3;
          }

        double factor = sqrt(double(xs)) * sqrt(double(ys)) / 2;

        for (int y = 0; y < i1.ysize; ++y)
          for (int x = 0; x < i1.xsize; ++x)
            {
              int xn = negf(x, xs);
              int yn = negf(y, ys);
              double d1 = GetValD(i1, x, y);
              double dn1 = GetValD(i1, xn, yn);
              double d2 = GetValD(i2, x, y);
              double dn2 = GetValD(i2, xn, yn);
              double d3 = (d1 * d2 + d1 * dn2 - dn1 * d2 + dn1 * dn2) * factor;
              PutValD(hdest, x, y, d3);
            }

        if (NEED_TEMP)
          {
            for (int y = 0; y < i3.ysize; ++y)
              for (int x = 0; x < i3.xsize; ++x)
                {
                  PutValD(i3, x, y, GetValD(hdest, x, y));
                }
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "CrossCorrelationImgD"
  void CrossCorrelationImgD(ImageD i1, ImageD i2, ImageD i3)
  {
    try
      {
        int xs, ys;
        MatchImgD(i1, i2, i3, xs, ys);

        ImageD h1 = NewImgD(xs, ys);
        ImageD h2 = NewImgD(xs, ys);

        HartleyImgD(i1, h1);
        HartleyImgD(i2, h2);

        CrossCorrelationHImgD(h1, h2, i3);

        HartleyImgD(i3, i3);
      }
    RETHROW;
  }
#undef FNAME
}
