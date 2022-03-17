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
#ifndef _PIXELREFERENCE_H
#define _PIXELREFERENCE_H

#include "ImageBase.h"

namespace ice
{
  struct PixelReference
  {
    ImageBase* const img;
    int x;
    int y;

    /* ctor with initialisation */
    PixelReference(ImageBase* img, int x, int y): img(img), x(x), y(y) { }

    /* conversion to int */
    operator int() const
    {
      return img->getP(x, y);
    }

    /* assignment */
    PixelReference& operator=(int v)
    {
      img->setP(x, y, v);
      return *this;
    }

    // increment / decrement
    PixelReference& operator++()
    {
      img->setP(x, y, img->getP(x, y) + 1);
      return *this;
    }
    PixelReference& operator--()
    {
      img->setP(x, y, img->getP(x, y) - 1);
      return *this;
    }

    int operator++(int)
    {
      int h = img->getP(x, y);
      img->setP(x, y, h + 1);
      return h;
    }

    int operator--(int)
    {
      int h = img->getP(x, y);
      img->setP(x, y, h - 1);
      return h;
    }

    friend void swap(PixelReference r1, PixelReference r2)
    {
      int h = r1;
      r1 = (int)r2;
      r2 = h;
    }
  };
}
#endif
