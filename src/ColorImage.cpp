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
#include <stdio.h>
#include <stdlib.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "picio.h"
#include "ColorImage.h"

/************************************************************************
"ColorImage.cpp" - ortmann 02/2013
************************************************************************/

using namespace std;

namespace ice
{

#define FNAME "ColorImage::ColorImage"

  ColorImage::ColorImage(const ColorImage& src, const Window& window, const std::string& newtitle)
  {
    red = src.red(window);
    green = src.green(window);
    blue = src.blue(window);

    makeValid();

    title = newtitle;

    if (title.empty())
      {
        title = src.title;
      }
    makeValid();
  }

  ColorImage::ColorImage(const Image& rot, const Image& gruen, const Image& blau, const std::string& title)
  {
    if ((!IsImg(rot)) || (!IsImg(gruen)) || (!IsImg(blau)))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }
    else
      {
        // bilder (smart pointer) original übernehmen
        red = rot;
        green = gruen;
        blue = blau;

        makeValid();

        if (!title.empty())
          {
            this->title = title;
          }
        else
          {
            this->title = rot->getTitle() + "/" + gruen->getTitle() + "/" + blau->getTitle();
          }
      }
  }
#undef FNAME

  // [re-]creation of real colorimage
  // given sizes
#define FNAME "ColorImage::create"
  void ColorImage::create(int sizeX, int sizeY, int maxValue, const std::string& title_p)
  {
    xsize = sizeX;
    ysize = sizeY;
    maxval = maxValue;
    title = title_p;
    if (!title.empty())
      {
        title += " - ";
      }
    try {
      red.create(xsize, ysize, maxval, title + "red");
    green.create(xsize, ysize, maxval, title + "green");
    blue.create(xsize, ysize, maxval, title + "blue");
    }
    RETHROW;
  }

  // given template colorimage
  void ColorImage::create(const ColorImage& templateImg, const std::string& title)
  {
    string newTitle = title.empty() ? templateImg.getTitle() : title;
    create(templateImg.xsize, templateImg.ysize, templateImg.maxval, newTitle);
  }

  // given template image
  void ColorImage::create(const Image& templateImg, const std::string& title)
  {
    string newTitle = title.empty() ? templateImg.getTitle() + " - rgb" : title;
    create(templateImg.xsize, templateImg.ysize, templateImg.maxval, newTitle);
  }
#undef FNAME

  // copy = recreate with new image size and deep copy of source data
  void ColorImage::copy(const ColorImage& src)
  {
    red.copy(src.red);
    green.copy(src.green);
    blue.copy(src.blue);
  }

  void ColorImage::copy(const Image& src)
  {
    red.copy(src);
    green.copy(src);
    blue.copy(src);
  }

  // named constructors
  ColorImage ColorImage::createColorImage(int sizeX, int sizeY, int maxValue, const std::string& title)
  {
    ColorImage result;
    result.create(sizeX, sizeY, maxValue, title);
    return result;
  }

  ColorImage ColorImage::createColorImage(const ColorImage& src, const std::string& title, bool copy)
  {
    ColorImage result;
    result.create(src, title);
    if (copy)
      {
        result.copy(src);
      }
    return result;
  }

  ColorImage ColorImage::createColorImage(const Image& src, const std::string& title, bool copy)
  {
    ColorImage result;
    result.create(src, title);
    if (copy)
      {
        result.copy(src);
      }
    return result;
  }

  bool ColorImage::makeValid()
  {
    if (!red.isValid())
      {
        return false;
      }
    xsize = red.xsize;
    ysize = red.ysize;
    maxval = red.maxval;
    return isValid();
  }

  bool ColorImage::isValid() const
  {
    //    std::cout << "isValid" << std::endl;
    if (!(red.isValid() && green.isValid() && blue.isValid()))
      {
        return false;
      }

    //    std::cout << "isValid2" << std::endl;
    if (red == green || green == blue || red == blue)
      {
        return false;
      }

    //    std::cout << "isValid (ne)" << std::endl;
    //    std::cout << xsize << " " << ysize << " " << maxval << std::endl;
    //    std::cout << red.xsize << " " << red.ysize << " " << red.maxval << std::endl;
    //    std::cout << green.xsize << " " << green.ysize << " " << green.maxval << std::endl;
    //    std::cout << blue.xsize << " " << blue.ysize << " " << blue.maxval << std::endl;
    return xsize == red.xsize &&
           ysize == red.ysize &&
           maxval == red.maxval &&
           xsize == green.xsize &&
           ysize == green.ysize &&
           maxval == green.maxval &&
           xsize == blue.xsize &&
           ysize == blue.ysize &&
           maxval == blue.maxval;
  }

#define FNAME "ColorImage::checkSizes"
  void ColorImage::checkSizes(const ColorImage& img2) const
  {
    if (!isValid() || !img2.isValid())
      throw IceException(FNAME, M_INVALID, WRONG_PARAM);
    try
      {
        red.checkSizes(img2.red);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "ColorImage::checkImages"
  void ColorImage::checkImages(const ColorImage& img2) const
  {
    if (maxval != img2.maxval)
      throw IceException(FNAME, M_WRONG_RANGE);
    try
      {
        checkSizes(img2);
      }
    RETHROW;
  }
#undef FNAME

  int ColorImage::read(const std::string& filename)
  {
    if (!isValid())
      {
        int xs, ys, mv, ch;
        InfImgFile(filename, xs, ys, mv, ch);
        create(xs, ys, mv);
      }
    return ReadImg(filename, red, green, blue);
  }

  int ColorImage::write(const std::string& filename) const
  {
    return WriteImg(red, green, blue, filename);
  }

#define FNAME "ColorImage::set"
  int ColorImage::set(ColorValue cv) const // Set all pixel to value
  {
    RETURN_ERROR_IF_FAILED(SetImg(red, maxval - cv.red));
    RETURN_ERROR_IF_FAILED(SetImg(green, maxval - cv.green));
    RETURN_ERROR_IF_FAILED(SetImg(blue, maxval - cv.blue));
    return OK;
  }
  int ColorImage::set(int r, int g, int b) const // Set all pixel to value
  {
    RETURN_ERROR_IF_FAILED(SetImg(red, r));
    RETURN_ERROR_IF_FAILED(SetImg(green, g));
    RETURN_ERROR_IF_FAILED(SetImg(blue, b));
    return OK;
  }
#undef FNAME

  ColorImage newColorImage(int sizeX, int sizeY, int maxValue, const std::string& title)
  {
    return ColorImage::createColorImage(sizeX, sizeY, maxValue, title);
  }

  ColorImage newColorImage(const ColorImage& src, const std::string& title, bool copy)
  {
    return ColorImage::createColorImage(src, title, copy);
  }

  ColorImage newColorImage(const Image& src, const std::string& title, bool copy)
  {
    return ColorImage::createColorImage(src, title, copy);
  }
} // namespace ice
