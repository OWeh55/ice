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

#include <limits.h>

#include "message.h"  // for printing error messages
#include "util.h"     // for MulDiv
#include "macro.h"    // for min/max functions

#include "picio.h"

#include "ImageC.h"
#include "ImageTemplate.h"

namespace ice
{
  /*
  bool operator==(const Image::const_iterator &lhs,
           const Image::const_iterator& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }

  bool operator!=(const Image::const_iterator &lhs,
           const Image::const_iterator& rhs)
  {
    return lhs.x != rhs.x || lhs.y != rhs.y;
  }
  */
#define FNAME "Image::create"
  void Image::create(int SizeX, int SizeY,
                     int MaxValue, const std::string& title)
  {
    if (img != nullptr)
      freeimg();

    xsize = SizeX;
    ysize = SizeY;
    maxval = MaxValue;

    ImageBase* imag = nullptr;
    if (UCHAR_MAX >= MaxValue)
      {
        imag = new iceImage1(SizeX, SizeY, MaxValue, title);
      }
    else
      {
        if (USHRT_MAX >= MaxValue)
          {
            imag = new iceImage2(SizeX, SizeY, MaxValue, title);
          }
        else
          {
            if (INT_MAX >= MaxValue)
              {
                imag = new iceImage3(SizeX, SizeY, MaxValue, title);
              }
            else
              {
                Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
              }
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
  //   recreate if image size changes
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

  Image Image::createImage(const Image& src, bool copy,
                           const std::string& title)
  {
    Image result;
    result.create(src, title);
    if (copy)
      result.copyData(src);
    return result;
  }
#undef FNAME
#define FNAME "Image::Image"
  Image::Image(const Image& i, const Window& w, const std::string& title)
  {
    ImageBase* imag = nullptr;

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
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    xsize = imag->xsize;
    ysize = imag->ysize;
    maxval = imag->maxval;
    assign(imag);
  }
#undef FNAME

#define FNAME "Image::match"
  int Image::match(const Image& img2) const
  {
    if (!isValid() || !img2.isValid())
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((xsize != img2->xsize) || (ysize != img2->ysize))
      {
        Message(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return OK;
  }

  int Image::match(const Image& img2, const Image& img3) const
  {
    if (match(img2) != OK)
      return WRONG_PARAM;
    return match(img3);
  }
#undef FNAME
#define FNAME "NewImg"
  Image NewImg(int SizeX, int SizeY, int MaxValue, const std::string& title)
  {
    if (SizeX <= 0 || SizeY <= 0 || MaxValue <= 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return Image();
      }

    Image Img;
    Img.create(SizeX, SizeY, MaxValue, title);
    return Img;
  }

  Image NewImg(const ice::Image& Img, bool ShallCopyContents, const std::string& title)
  {
    if (!IsImg(Img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return Image();
      }

    Image result;
    result.create(Img, title); // create with given size..
    // result.setTitle(title); // rename to given name

    if (ShallCopyContents) // copy content if neccesary
      result.copyData(Img);

    return result;
  }

  Image NewImg(const ice::Image& imgp,
               const Window& w,
               const std::string& title)
  {
    if (!IsImg(imgp))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return Image();
      }

    Image result(imgp, w, title);

    return result;
  }
#undef FNAME
#define FNAME "FreeImg"
  int FreeImg(Image& Img)
  {
    if (!IsImg(Img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Img.destroy();
    return OK;
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
      return ReadImg(filename, *this).isValid();
    else
      {
        *this = ReadImg(filename);
        return isValid();
      }
  }

  double Image::getPixelInterpol(double x, double y) const
  {
    // if x and y are too far outside the image border, we simply return zero
    // check before conversion to int to avoid overflow
    if (x < -0.5 || y < -0.5 || x > xsize || y > ysize)
      return 0;

    // Determine the local neighborhood of point(x, y), that means the 4 pixel positions
    //(xi, yi),(xi + 1, yi),(xi + 1, yi + 1), and(xi, yi + 1), that enclose(x, y)
    x = ice::max<double>(x, 0.0);
    y = ice::max<double>(y, 0.0);
    int xi  = (int) x;
    int xi1 = xi + 1;
    int yi  = (int) y;
    int yi1 = yi + 1;

    // check if the local neighborhood is still inside the image
    if ((xi >= xsize) || (yi >= ysize)) return 0;

    if ((xi1 == xsize) || (yi1 == ysize)) return getPixelUnchecked(xi, yi); // rechter/unterer Rand

    double dx  = x - (double) xi;
    double dx1 = 1.0 - dx;
    double dy  = y - (double) yi;
    double dy1 = 1.0 - dy;
    return dx1 * (dy1 * img->getP(xi, yi)
                  + dy * img->getP(xi, yi1))
           + dx  * (dy1 * img->getP(xi1, yi)
                    + dy * img->getP(xi1, yi1));

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
    x = ice::max<double>(x, 0.0);
    y = ice::max<double>(y, 0.0);
    int xi  = (int) x;
    int xi1 = xi + 1;
    int yi  = (int) y;
    int yi1 = yi + 1;

    if ((xi >= xsize) || (yi >= ysize))
      {
        val = 0;
        return false;
      }

    if ((xi1 == xsize) || (yi1 == ysize)) // rechter/unterer Rand
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
