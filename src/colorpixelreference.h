/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef _PIXELREFERENCE_H
#define _PIXELREFERENCE_H

#include "ColorImage.h"

namespace ice
{
  struct ColorPixelReference
  {
    ColorImage* const img;
    int x;
    int y;

    /* ctor with initialisation */
    PixelReference(ColorImage* img, int x, int y): img(img), x(x), y(y) { }

    /* conversion to int */
    operator ColorValue() const
    {
      return img->getPixelUnchecked(x, y);
    }

    /* assignment */
    ColorPixelReference& operator=(ColorValue v)
    {
      img->setPixelUnchecked(x, y, v);
      return *this;
    }

    // increment / decrement
    ColorPixelReference& operator++()
    {
      img->setPixelUnchecked(x, y, img->getPixelUnchecked(x, y) + 1);
      return *this;
    }
    ColorPixelReference& operator--()
    {
      img->setPixel(x, y, img->getPixelUnchecked(x, y) - 1);
      return *this;
    }

    ColorValue operator++(int)
    {
      ColorValue h = img->getPixelUnchecked(x, y);
      img->setPixelUnchecked(x, y, h + 1);
      return h;
    }

    ColorValue operator--(int)
    {
      ColorValue h = img->getPixelUnchecked(x, y);
      img->setPixelUnchecked(x, y, h - 1);
      return h;
    }

    friend void swap(ColorPixelReference r1, ColorPixelReference r2)
    {
      ColorValue h = r1;
      r1 = (int)r2;
      r2 = h;
    }
  };
}
#endif
