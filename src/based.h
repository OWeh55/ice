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
#ifndef _BASED_H
#define _BASED_H

#include "ImageF.h"
#include "base.h"

namespace ice
{
  // type definition for pixels in images with floating point values
  typedef double PixelFloatType;
  // type definition for images with floating point values
  typedef ImageF<PixelFloatType> ImageD;

  /*
   * create the real image - function wrapper for c'tor + create
   */
#define FNAME "NewImgD"
  inline ImageD NewImgD(int xsize, int ysize,
                        double minval = std::numeric_limits<double>::min(),
                        double maxval = std::numeric_limits<double>::max())
  {
    ImageD img;
    if ((xsize <= 0) || (ysize <= 0) || (minval > maxval))
      throw IceException(FNAME, M_WRONG_PARAMETER);
    img.create(xsize, ysize, minval, maxval);
    return img;
  }

  inline ImageD NewImgD(const ImageD& im, bool copy = false)
  {
    ImageD res;

    if (! im.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    res.create(im.xsize, im.ysize, im.minValue(), im.maxValue());

    if (copy)
      {
        for (int y = 0; y < im.ysize; y++)
          for (int x = 0; x < im.xsize; x++)
            {
              res.setPixelUnchecked(x, y, im.getPixelUnchecked(x, y));
            }
      }

    return res;
  }

  inline ImageD NewImgD(const Image& img, bool copy = false)
  {
    ImageD res;

    if (!img.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    res.create(img.xsize, img.ysize, 0, img.maxval);

    if (copy)
      {
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            {
              res.setPixelUnchecked(x, y, img.getPixelUnchecked(x, y));
            }
      }

    return res;
  }
#undef FNAME

  /* convert Image ---> ImageD */
  void convImgImgD(const Image& inp, ImageD& out,
                   double factor, double offset);
  void ConvImgImgD(const Image& imgs, ImageD& imgd,
                   int mode = RAW, int sign = UNSIGNED);

  /* convert ImageD ---> Image */
  void convImgDImg(const ImageD& input, const Image& output,
                   double factor, double offset);
  void ConvImgDImg(const ImageD& imgs, const Image& imgd,
                   int mode = ADAPTIVE, int sign = UNSIGNED);

  /* update minval and maxval from pixel values */
  int UpdateLimitImgD(ImageD& img);

  /* valid Image ? */
  inline int IsImgD(const ImageD& img)
  {
    return img.isValid();
  }

  // access to double-images

  inline   void PutValD(const ImageD& img, int x, int y, double wert)
  {
    img.setPixel(x, y, wert);
  }
  inline   double GetValD(const ImageD& img, int x, int y)
  {
    return img.getPixel(x, y);
  }

  inline   void PutValD(const ImageD& i, IPoint p, double val)
  {
    i.setPixel(p, val);
  }
  inline   double GetValD(const ImageD& i, IPoint p)
  {
    return i.getPixel(p);
  }

//
  double GetInterpolValD(const ImageD& img, double x, double y);
  bool GetInterpolValD(const ImageD& Img, double x, double y, double& val);

  int MatchImgD(const ImageD& i1, const ImageD& i2);
  int MatchImgD(const ImageD& i1, const ImageD& i2, const ImageD& i3);

  int MatchImgD(const ImageD& i1, const ImageD& i2, int& xs, int& ys);
  int MatchImgD(const ImageD& i1, const ImageD& i2, const ImageD& i3,
                int& xs, int& ys);

  //
  bool Inside(const ImageD& img, int x, int y);
  bool Inside(const ImageD& img, IPoint p);

  bool Inside(const ImageD& img, const Window& w);

  //
  void setborder(const ImageD& p, int sx, int sy, double val);

  // pixelwise logarithm
  int LogImgD(const ImageD& src, const ImageD& dst);

} // namespace ice
#endif  /* IFNDEF _BASED_H */
