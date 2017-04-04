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
#include "diff.h"
#include <cstdlib>

#include <iostream>

namespace ice
{
  // core functions, no testing here
  template<class T>
  double Diff1(const Image& img1, const Image& img2, int dx, int dy)
  {
    double res = 0.0;
    T** p1 = (T**)img1->getDataPtr();
    T** p2 = (T**)img2->getDataPtr();

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          res += abs(p1[y][x] - p2[y][x]);
        }

    return res / dx / dy;
  }

  template<class T>
  double Diff2(const Image& img1, const Image& img2, int dx, int dy)
  {
    double res = 0.0;
    T** p1 = (T**)img1->getDataPtr();
    T** p2 = (T**)img2->getDataPtr();

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double dv = p1[y][x] - p2[y][x];
          res += dv * dv;
        }

    return res / dx / dy;
  }

  template<class T>
  double Diff3(const Image& img1, const Image& img2, int dx, int dy)
  {
    T** p1 = (T**)img1->getDataPtr();
    T** p2 = (T**)img2->getDataPtr();

    double xm = 0.0;
    double ym = 0.0;

    double x2m = 0.0;
    double y2m = 0.0;

    double xym = 0.0;

    double ct = dx * dy;

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double v1 = p1[y][x];
          double v2 = p2[y][x];
          xm  += v1;
          ym  += v2;
          x2m += v1 * v1;
          y2m += v2 * v2;
          xym += v1 * v2;
        }

    xm = xm / ct;
    ym = ym / ct;

    x2m = x2m / ct;
    y2m = y2m / ct;
    xym = xym / ct;

    double res = xym - xm * ym; // covariance

    double sx2 = x2m - xm * xm;
    double sy2 = y2m - ym * ym;

    return 1.0 - res / sqrt(sx2 * sy2);
  }

#define FNAME "Diff"
  double Diff(const Image& img1, const Image& img2, int mode)
  {
    try {
    double res = 0.0;

    int dx, dy;

    MatchImg(img1, img2, dx, dy);

    int ptype = img1->ImageType();

    if ((ptype > 0) && (ptype < 4))
      {
        if (img2->ImageType() == ptype)
          {
            switch (ptype)
              {
              case 1:

                switch (mode)
                  {
                  case ID_ABS:
                    return Diff1<PixelType1>(img1, img2, dx, dy);
                    break;
                  case ID_SQUARE:
                    return Diff2<PixelType1>(img1, img2, dx, dy);
                    break;
                  case ID_COV:
                    return Diff3<PixelType1>(img1, img2, dx, dy);
                    break;
                  }

              case 2:

                switch (mode)
                  {
                  case ID_ABS:
                    return Diff1<PixelType2>(img1, img2, dx, dy);
                    break;
                  case ID_SQUARE:
                    return Diff2<PixelType2>(img1, img2, dx, dy);
                    break;
                  case ID_COV:
                    return Diff3<PixelType2>(img1, img2, dx, dy);
                    break;
                  }

              case 3:

                switch (mode)
                  {
                  case ID_ABS:
                    return Diff1<PixelType3>(img1, img2, dx, dy);
                    break;
                  case ID_SQUARE:
                    return Diff2<PixelType3>(img1, img2, dx, dy);
                    break;
                  case ID_COV:
                    return Diff3<PixelType3>(img1, img2, dx, dy);
                    break;
                  }
              }
          }
      }

    else
      {
        switch (mode)
          {
          case ID_ABS:
            for (int y = 0; y < dy; ++y)
              for (int x = 0; x < dx; ++x)
                {
                  int v1 = GetVal(img1, x, y);
                  int v2 = GetVal(img2, x, y);
                  res += (abs(v2 - v1));
                }
            break;
          case ID_SQUARE:
            for (int y = 0; y < dy; ++y)
              for (int x = 0; x < dx; ++x)
                {
                  int v = GetVal(img1, x, y);
                  v -= GetVal(img2, x, y);
                  res += v * v;
                }
            break;
          case ID_COV:
            int v1s = 0;
            int v2s = 0;
            for (int y = 0; y < dy; ++y)
              for (int x = 0; x < dx; ++x)
                {
                  v1s += GetVal(img1, x, y);
                  v2s += GetVal(img2, x, y);
                }

            double v1m = (double)v1s / dx / dy;
            double v2m = (double)v2s / dx / dy;

            for (int y = 0; y < dy; ++y)
              for (int x = 0; x < dx; ++x)
                {
                  double v1 = GetVal(img1, x, y) - v1m;
                  double v2 = GetVal(img2, x, y) - v2m;
                  res -= v1 * v2;
                }
            break;
          }

        return res / dx / dy;
      }
    return 0;
    }
    RETHROW;
  }
#undef FNAME
}
