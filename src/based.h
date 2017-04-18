/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002..2014 FSU Jena, Digital Image Processing Group
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
      throw IceException(FNAME, M_WRONG_PARAM);
    img.create(xsize, ysize, minval, maxval);
    return img;
  }

  inline   ImageD NewImgD(const ImageD& im, bool copy = false)
  {
    ImageD res;

    if (! im.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);

    res.create(im.xsize, im.ysize, im.minval, im.maxval);

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

  inline   ImageD NewImgD(const Image& img, bool copy = false)
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
  /* Freigeben eines Bildes */
  inline   void FreeImgD(ImageD& img)
  {
    img.destroy();
  }

  /* Kopieren Image ---> ImageD */
  int ConvImgImgD(const Image& imgs, ImageD& imgd,
                  int mode = RAW, int sign = UNSIGNED);

  /* Kopieren ImageD ---> Image */
  int ConvImgDImg(const ImageD& imgs, const Image& imgd,
                  int mode = ADAPTIVE, int sign = UNSIGNED);

  /* Aktualisieren der Eintraege minval, maxval nach dem Bildinhalt*/
  int UpdateLimitImgD(ImageD& img);

  /* Test auf gueltiges Bild */
  inline   int IsImgD(const ImageD& img)
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
    PutValD(i, p.x, p.y, val);
  }
  inline   double GetValD(const ImageD& i, IPoint p)
  {
    return GetValD(i, p.x, p.y);
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
