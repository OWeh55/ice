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
#include <limits.h>
#include <algorithm>

#include "IceException.h"

#include "picio.h"

#include "ImageC.h"
#include "ImageTemplate.h"

using namespace std;

namespace ice
{
#define FNAME "Image::create"
  void Image::create(int SizeX, int SizeY,
                     int MaxValue, const std::string& title)
  {
    if (img != nullptr)
      {
        freeimg();
      }

    if (SizeX < 1 || SizeY < 1 || MaxValue < 1)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    xsize = SizeX;
    ysize = SizeY;
    maxval = MaxValue;

    ImageBase* imag = nullptr;

    if (numeric_limits<PixelType1>::max() >= MaxValue)
      {
        imag = new iceImage1(SizeX, SizeY, MaxValue, title);
      }
    else
      {
        if (numeric_limits<PixelType2>::max() >= MaxValue)
          {
            imag = new iceImage2(SizeX, SizeY, MaxValue, title);
          }
        else
          {
            if (numeric_limits<PixelType3>::max() >= MaxValue)
              {
                imag = new iceImage3(SizeX, SizeY, MaxValue, title);
              }
            else
              throw IceException(FNAME, M_WRONG_PARAMETER);
          }
      }
    assign(imag);
  }

  void Image::create(const Image& src, const std::string& title)
  {
    string newtitle = title.empty() ? src.getTitle() : title ;
    create(src.xsize, src.ysize, src.maxval, newtitle);
  }
#undef FNAME
#define FNAME "Image::copy"
  // copy =
  //   [re-]create image
  //   + deep copy of source data
  void Image::copy(const Image& src)
  {
    if (xsize != src.xsize || ysize != src.ysize || maxval != src.maxval)
      create(src);
    copyData(src);
  }
#undef FNAME
#define FNAME "Image::createImage"
  Image Image::createImage(int SizeX, int SizeY, int MaxValue,
                           const std::string& title)
  {
    Image result;
    result.create(SizeX, SizeY, MaxValue, title);
    return result;
  }

  Image Image::createImage(const Image& src, // bool copy,
                           const std::string& title)
  {
    Image result;
    result.create(src, title);
    /*
    if (copy)
      {
        result.copyData(src);
      }
    */
    return result;
  }
#undef FNAME
#define FNAME "Image::Image"
  Image::Image(const Image& i, const Window& w, const std::string& title)
  {
    ImageBase* imag = nullptr;
    try
      {
        switch (i->getBytesPerPoint())
          {
          case sizeof(PixelType1):
            imag = new iceImage1((iceImage1*)i.img, w, title);
            break;
          case sizeof(PixelType2):
            imag = new iceImage2((iceImage2*)i.img, w, title);
            break;
          case sizeof(PixelType3):
            imag = new iceImage3((iceImage3*)i.img, w, title);
            break;
          default:
            throw IceException(FNAME, M_WRONG_PARAMETER);
          }
      }
    RETHROW;
    xsize = imag->xsize;
    ysize = imag->ysize;
    maxval = imag->maxval;
    assign(imag);
  }
#undef FNAME

#define FNAME "Image::checkSizes"
  void Image::checkSizes(const Image& img2) const
  {
    if (!isValid() || !img2.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((xsize != img2->xsize) || (ysize != img2->ysize))
      throw IceException(FNAME, M_WRONG_IMGSIZE);
  }
#undef FNAME
#define FNAME "Image::checkImage"
  void Image::checkImage(const Image& img2) const
  {
    try
      {
        checkSizes(img2);
        if (maxval != img2.maxval)
          throw IceException(FNAME, M_WRONG_RANGE);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "NewImg"
  Image NewImg(int sizeX, int sizeY, int maxValue, const std::string& title)
  {
    return Image::createImage(sizeX, sizeY, maxValue, title);
  }

  Image NewImg(const ice::Image& Img, bool ShallCopyContents, const std::string& title)
  {
    if (!IsImg(Img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    Image result;
    result.create(Img, title); // create with given size..

    if (ShallCopyContents)   // copy content if necesary
      {
        result.copyData(Img);
      }

    return result;
  }

  Image NewImg(const ice::Image& imgp,
               const Window& w,
               const std::string& title)
  {
    if (!IsImg(imgp))
      throw IceException(FNAME, M_WRONG_IMAGE);

    return Image(imgp, w, title);
  }
#undef FNAME
#define FNAME "Image::write"
  int Image::write(const std::string& filename) const
  {
    return WriteImg(*this, filename);
  }
#undef FNAME
#define FNAME "Image::read"
  int Image::read(const std::string& filename)
  {
    if (isValid())
      {
        return ReadImg(filename, *this).isValid();
      }
    else
      {
        *this = ReadImg(filename);
        return isValid();
      }
  }
#undef FNAME
  double Image::getPixelInterpol(Point p) const
  {
    return getPixelInterpol(p.x, p.y);
  }

  double Image::getPixelInterpol(double x, double y) const
  {
    // if x and y are too far outside the image border, we simply return zero
    // check before conversion to int to avoid overflow
    if (x < -0.5 || y < -0.5 || x > xsize || y > ysize)
      {
        return 0;
      }

    // Determine the local neighborhood of point(x, y), that means the 4 pixel positions
    //(xi, yi),(xi + 1, yi),(xi + 1, yi + 1), and(xi, yi + 1), that enclose(x, y)
    x = max<double>(x, 0.0);
    y = max<double>(y, 0.0);

    int xi  = (int) x;
    int xi1 = xi + 1;
    int yi  = (int) y;
    int yi1 = yi + 1;

    // check if the local neighborhood is still inside the image
    if ((xi >= xsize) || (yi >= ysize))
      {
        return 0;
      }

    if ((xi1 == xsize) || (yi1 == ysize))
      {
        return getPixelUnchecked(xi, yi);  // rechter/unterer Rand
      }

    double dx  = x - (double) xi;
    double dx1 = 1.0 - dx;
    double dy  = y - (double) yi;
    double dy1 = 1.0 - dy;
    return dx1 * (dy1 * img->getP(xi, yi) + dy * img->getP(xi, yi1))
           + dx  * (dy1 * img->getP(xi1, yi) + dy * img->getP(xi1, yi1));

  }

  bool Image::getPixelInterpol(double x, double y, double& val) const
  {
    // if x and y are to far outside the image border, we simply return false
    // check before conversion to int to avoid overflow
    if (x < -0.5 || y < -0.5  || x > xsize || y > ysize)
      {
        val = 0;
        return false;
      }

    // Determine the local neighborhood of point(x, y), that means the 4 pixel positions
    //(xi, yi),(xi + 1, yi),(xi + 1, yi + 1), and(xi, yi + 1), that enclose(x, y)
    x = max<double>(x, 0.0);
    y = max<double>(y, 0.0);
    int xi  = (int) x;
    int xi1 = xi + 1;
    int yi  = (int) y;
    int yi1 = yi + 1;

    if ((xi >= xsize) || (yi >= ysize))
      {
        val = 0;
        return false;
      }

    if ((xi1 == xsize) || (yi1 == ysize))   // rechter/unterer Rand
      {
        val = getPixelUnchecked(xi, yi);
        return true;
      }

    double dx  = x - (double) xi;
    double dx1 = 1.0 - dx;
    double dy  = y - (double) yi;
    double dy1 = 1.0 - dy;
    val = dx1 * (dy1 * getPixelUnchecked(xi, yi) +
                 dy * getPixelUnchecked(xi, yi1))
          +  dx  * (dy1 * getPixelUnchecked(xi1, yi) +
                    dy * getPixelUnchecked(xi1, yi1));
    return true;
  }
} // namespace
