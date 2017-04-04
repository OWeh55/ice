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
#ifndef _BASE_H
#define _BASE_H

#include "defs.h"
#include "Window.h"
#include "ImageC.h"

namespace ice
{
  // typedefs for pixel value type
  typedef unsigned char PixelType1;
  typedef unsigned short PixelType2;
  // we use signed int here, because unsigned int would exceed the range
  // in many functions working with int
  // moreover unsigned int would cause an implicit conversion
  // to double in many cases
  typedef int PixelType3;

  // sets all grey values of >Img< to >Value<
  int SetImg(const Image& Img, int Value);
  int ClearImg(const Image& img);

  // Copy an image
  int CopyImg(const Image& source, const Image& destination);

  // Checks, if image is valid

  inline bool IsImg(const Image& Img)
  {
    return Img.isValid();
  }

  // These functions check if 2 or 3 images match == the sizes of the images
  // are equal. Versions with parameters sizeX and sizeY return
  // the windowsize
  // if the images don't match, an error is forced
  int MatchImg(const Image& p1,
               const Image& p2,
               const Image& p3, int& sizeX, int& sizeY);
  int MatchImg(const Image& p1, const Image& p2, int& sizeX, int& sizeY);
  int MatchImg(const Image& p1, const Image& p2, const Image& p3);
  int MatchImg(const Image& p1, const Image& p2);

  // These functions check if 2 or 3 images match == the sizes of the images
  // are equal. Versions with parameters sizeX and sizeY return
  // the size
  // if the images don't match, an exception is thrown
  void checkSizes(const Image& p1,
                  const Image& p2,
                  const Image& p3, int& sizeX, int& sizeY);
  void checkSizes(const Image& p1, const Image& p2, int& sizeX, int& sizeY);
  void checkSizes(const Image& p1, const Image& p2, const Image& p3);
  void checkSizes(const Image& p1, const Image& p2);

  // These functions check if 2 or 3 images match == the sizes of the images
  // are equal. Versions with parameters sizeX and sizeY return
  // the size
  // if the images don't match, an exception is thrown
  void checkImages(const Image& p1,
                   const Image& p2,
                   const Image& p3, int& sizeX, int& sizeY);
  void checkImages(const Image& p1, const Image& p2, int& sizeX, int& sizeY);
  void checkImages(const Image& p1, const Image& p2, const Image& p3);
  void checkImages(const Image& p1, const Image& p2);

  // Interpretes the image Img as a continous grey value function.
  // Returns the grey value at location (x, y). This value is
  // computed by bi-linear interpolation
  // from the grey values of the local neighborhood
  double GetInterpolVal(const Image& Img, double x, double y);
  double GetInterpolVal(const Image& Img, const Point& p);
  bool GetInterpolVal(const Image& Img, double x, double y, double& val);

  // Access to pixels of images, absolut coordinates

#define FNAME "PutVal"
  inline void PutVal(const Image& Img, int PosX, int PosY, int Value)
  {
    Img.setPixel(PosX, PosY, Value);
  }
#undef FNAME
#define FNAME "GetVal"
  inline int GetVal(const Image& Img, int PosX, int PosY)
  {
    return Img.getPixel(PosX, PosY);
  }
#undef FNAME
  inline int GetValUnchecked(const Image& Img, int PosX, int PosY)
  {
    return Img.getPixelUnchecked(PosX, PosY);
  }

  inline void PutValUnchecked(const Image& Img, int PosX, int PosY, int value)
  {
    Img.setPixelUnchecked(PosX, PosY, value);
  }

  // pixel access, returns 0 outside image
  inline int GetValClipped(const Image& Img, int PosX, int PosY)
  {
    return Img.getPixelClipped(PosX, PosY);
  }
#undef FNAME
#define FNAME "GetVal"
  inline int GetVal(const Image& img, double x, double y, int mode)
  {
    switch (mode)
      {
      case DEFAULT:
        return GetVal(img, RoundInt(x), RoundInt(y));
      case INTERPOL:
        return RoundInt(GetInterpolVal(img, x, y));
      }

    return 0;
  }
#undef FNAME

  inline int GetVal(const Image& img, const IPoint& p)
  {
    return img.getPixel(p.x, p.y);
  }

  inline void PutVal(const Image& img, const IPoint& p, int val)
  {
    img.setPixel(p.x, p.y, val);
  }

  inline int GetValUnchecked(const Image& img, const IPoint& p)
  {
    return img.getPixelUnchecked(p.x, p.y);
  }

  inline void PutValUnchecked(const Image& img, const IPoint& p, int val)
  {
    img.setPixelUnchecked(p.x, p.y, val);
  }

  bool Inside(const Image& img, int x, int y);
  bool Inside(const Image& img, const Window& w);
  bool Inside(const Image& img, IPoint p);

  // helper functions
  void setborder(const Image& p, int sx, int sy, int val);
  void setborder(const Image& p, int size, int val);
  void setborder(const Image& p, int val);
}
#endif  /* IFNDEF _BASE_H */

