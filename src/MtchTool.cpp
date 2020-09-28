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

#include <float.h>
#include <limits.h>
#include "WindowWalker.h"
#include "contlist.h"

#include "MtchTool.h"
#include "DPList.h"
#include "histogram.h"

namespace ice
{
  /*****************************************************************************/
  /*****************************************************************************/
  double Gauss2D(double x, double y,
                 double xm, double ym,
                 double s1, double s2, double s12)
  {
    double f = (1 - s12 * s12);
    double xx = x - xm;
    double yy = y - ym;

    return 1 / ((M_PI + M_PI) * s1 * s2 * sqrt(f)) *
           exp(-1 / (f + f) * ((xx * xx) / (s1 * s1) - 2 * s12 * xx * yy / (s1 * s2) + (yy * yy) / (s2 * s2)));
  }

  /*****************************************************************************/
  /*****************************************************************************/
#define FNAME "getGaussParameters"
  void getGaussParameters(const ImageD& imgd, const Image& imgo,
                          double& x0, double& y0,
                          double& sx, double& sy, double& sxy,
                          double& a, double& b)
  {
    int xsize = imgd.xsize;
    int ysize = imgd.ysize;
    if ((imgo.xsize != xsize) || (imgo.ysize != ysize))
      throw IceException(FNAME, M_SIZES_DIFFER);

    WindowWalker ww(imgd);
    double gmax = -DBL_MAX;
    double gmin = DBL_MAX;
    for (ww.init(); !ww.ready(); ww.next())
      {
        if (imgo.getPixel(ww))
          {
            double g = imgd.getPixel(ww);

            if (gmax < g)
              gmax = g;

            if (gmin > g)
              gmin = g;
          }
      }

    double volume = 0.0;
    for (ww.init(); !ww.ready(); ww.next())
      {
        if (imgo.getPixel(ww))
          {
            volume += imgd.getPixel(ww) - gmin;
          }
      }

    if (x0 == -1 || y0 == -1)   // Mittelwert bestimmen
      {
        if (volume != 0)
          {
            x0 = 0;
            y0 = 0;

            for (ww.init(); !ww.ready(); ww.next())
              {
                if (imgo.getPixel(ww))
                  {
                    double g = imgd.getPixel(ww) - gmin;
                    x0 += g * ww.x;
                    y0 += g * ww.y;
                  }
              }

            x0 /= volume;
            y0 /= volume;
          }
        else
          {
            x0 = imgo.xsize / 2;
            y0 = imgo.ysize / 2;
          }
      }

    sx = sy = sxy = 0;

    for (ww.init(); !ww.ready(); ww.next())
      {
        if (imgo.getPixel(ww))
          {
            double g = imgd.getPixel(ww) - gmin;
            sx += Sqr(ww.x - x0) * g;
            sy += Sqr(ww.y - y0) * g;
            sxy += (ww.x - x0) * (ww.y - y0) * g;
          }
      }

    if (volume != 0)
      {
        sx /= volume;
        sx = sqrt(sx);
        sy /= volume;
        sy = sqrt(sy);
        sxy /= volume * sx * sy;
      }
    else
      {
        sx = DBL_MAX;
        sy = DBL_MAX;
        sxy = 0;
      }

    double max = -DBL_MAX, min = DBL_MAX;

    for (int y = 0; y < imgo.ysize; y++)
      for (int x = 0; x < imgo.xsize; x++)
        {
          if (GetVal(imgo, x, y))
            {
              double f = Gauss2D(x, y, x0, y0, sx, sy, sxy);
              if (max < f)
                {
                  max = f;
                }
              if (min > f)
                {
                  min = f;
                }
            }
        }

    if (max > min)
      {
        a = (gmax - gmin) / (max - min);
      }
    else
      {
        a = 1;
      }

    b = -min * a;
  }

#undef FNAME

  /****************************************************************************/

#define FNAME "EntropyImg"
  double EntropyImg(const Image& img)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_INVALID_IMAGE);

    try
      {
        Histogram h(img);

        double entropy = 0.0;
        for (int i = 0; i < h.nClasses(); i++)
          {
            double p = h.getRelative(i);

            if (p > 0)
              entropy -= p * log2(p);
          }

        return entropy;
      }
    RETHROW;
  }
}
#undef FNAME
