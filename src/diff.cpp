/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002-2019 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019-2021 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
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

    double xs = 0.0;
    double ys = 0.0;

    double x2s = 0.0;
    double y2s = 0.0;

    double xys = 0.0;

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double v1 = p1[y][x];
          double v2 = p2[y][x];
          xs  += v1;
          ys  += v2;
          x2s += v1 * v1;
          y2s += v2 * v2;
          xys += v1 * v2;
        }

    double ct = dx * dy;

    double xm = xs / ct;
    double ym = ys / ct;

    double x2m = x2s / ct;
    double y2m = y2s / ct;
    double xym = xys / ct;

    // covariance
    double sym = xym - xm * ym;
    double sx2 = x2m - xm * xm;
    double sy2 = y2m - ym * ym;

    return 1.0 - sym / sqrt(sx2 * sy2);
  }


  // core functions, no testing here
  double Diff1(const Image& img1, const Image& img2, int dx, int dy)
  {
    double res = 0.0;
    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          res += abs(img1.getPixel(x, y) - img2.getPixel(x, y));
        }

    return res / dx / dy;
  }

  double Diff2(const Image& img1, const Image& img2, int dx, int dy)
  {
    double res = 0.0;

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double dv = img1.getPixel(x, y) - img2.getPixel(x, y);
          res += dv * dv;
        }

    return res / dx / dy;
  }

  double Diff3(const Image& img1, const Image& img2, int dx, int dy)
  {
    double xs = 0.0;
    double ys = 0.0;

    double x2s = 0.0;
    double y2s = 0.0;

    double xys = 0.0;

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double v1 = img1.getPixel(x, y);
          double v2 = img2.getPixel(x, y);
          xs  += v1;
          ys  += v2;
          x2s += v1 * v1;
          y2s += v2 * v2;
          xys += v1 * v2;
        }

    double ct = dx * dy;

    double xm = xs / ct;
    double ym = ys / ct;

    double x2m = x2s / ct;
    double y2m = y2s / ct;
    double xym = xys / ct;

    // covariance
    double sym = xym - xm * ym;
    double sx2 = x2m - xm * xm;
    double sy2 = y2m - ym * ym;

    return 1.0 - sym / sqrt(sx2 * sy2);
  }

#define FNAME "Diff"
  double Diff(const Image& img1, const Image& img2, int mode)
  {
    try
      {
        int dx, dy;
        MatchImg(img1, img2, dx, dy);

        int ptype = img1->ImageType();
        if ((ptype > 0) && (ptype < 4) && (img2->ImageType() == ptype))
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
	else
          {
            switch (mode)
              {
              case ID_ABS:
                return Diff1(img1, img2, dx, dy);

              case ID_SQUARE:
                return Diff2(img1, img2, dx, dy);

              case ID_COV:
                return Diff3(img1, img2, dx, dy);
              }
          }
      }
    RETHROW;
  }

  // Diff for ImageD

  // core functions, no testing here
  double Diff1(const ImageD& img1, const ImageD& img2, int dx, int dy)
  {
    double res = 0.0;
    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          res += abs(img1.getPixel(x, y) - img2.getPixel(x, y));
        }

    return res / dx / dy;
  }

  double Diff2(const ImageD& img1, const ImageD& img2, int dx, int dy)
  {
    double res = 0.0;

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double dv = img1.getPixel(x, y) - img2.getPixel(x, y);
          res += dv * dv;
        }

    return res / dx / dy;
  }

  double Diff3(const ImageD& img1, const ImageD& img2, int dx, int dy)
  {
    double xs = 0.0;
    double ys = 0.0;

    double x2s = 0.0;
    double y2s = 0.0;

    double xys = 0.0;

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          double v1 = img1.getPixel(x, y);
          double v2 = img2.getPixel(x, y);
          xs  += v1;
          ys  += v2;
          x2s += v1 * v1;
          y2s += v2 * v2;
          xys += v1 * v2;
        }

    double ct = dx * dy;

    double xm = xs / ct;
    double ym = ys / ct;

    double x2m = x2s / ct;
    double y2m = y2s / ct;
    double xym = xys / ct;

    // covariance
    double sym = xym - xm * ym;
    double sx2 = x2m - xm * xm;
    double sy2 = y2m - ym * ym;

    return 1.0 - sym / sqrt(sx2 * sy2);
  }

  double Diff(const ImageD& img1, const ImageD& img2, int mode)
  {
    try
      {
        int dx, dy;
        MatchImgD(img1, img2, dx, dy);

        switch (mode)
          {
          case ID_ABS:
            return Diff1(img1, img2, dx, dy);

          case ID_SQUARE:
            return Diff2(img1, img2, dx, dy);

          case ID_COV:
            return Diff3(img1, img2, dx, dy);
          }
      }
    RETHROW;
  }

#undef FNAME
}

