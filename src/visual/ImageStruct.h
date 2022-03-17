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
#ifndef ICE_IMAGE_STRUCT_H
#define ICE_IMAGE_STRUCT_H

#include "util.h"
#include "base.h"
#include "based.h"

namespace ice
{
  class ImageStruct
  {
  public:
    virtual ~ImageStruct() {}

    virtual int ImageType() const = 0;
    virtual int getReferenceCount() const = 0;
    virtual unsigned char getValueShifted(int x, int y) const = 0;
    virtual int getPixel(int x, int y) const = 0;
  };

  class ImageStructInt: public ImageStruct
  {
  private:
    ImageBase* img;
    unsigned int shift;
  public:
    ImageStructInt(ImageBase* imgp): img(imgp)
    {
      imgp->refcount++;
      shift = 0;
      while ((imgp->maxval >> shift) > 255)
        {
          shift++;
        }
    }

    virtual ~ImageStructInt()
    {
      img->refcount--;
      if (img->refcount == 0)
        {
          delete img;
        }
    };

    virtual int ImageType() const
    {
      return 1;
    }

    virtual int getReferenceCount() const
    {
      return img->refcount;
    }

    const ImageBase* Img() const
    {
      return img;
    }

    unsigned char getValueShifted(int x, int y) const
    {
      return (img->getP(x, y) >> shift) ^ 255;
    }

    int getPixel(int x, int y) const
    {
      return img->getP(x, y);
    }
  };

  class ImageStructDouble: public ImageStruct
  {
  private:
    ImageD* img;
  public:
    ImageStructDouble(ImageD* imgp): img(imgp)
    {
      img->getMatrix().refcount++;
    }

    virtual ~ImageStructDouble()
    {
      img->getMatrix().refcount--;
      if (img->getMatrix().refcount == 0)
        delete img;
    };

    virtual int ImageType() const
    {
      return 2;
    }

    const ImageD* Img() const
    {
      return img;
    }

    virtual int getReferenceCount() const
    {
      return img->getMatrix().refcount;
    }

    unsigned char getValueShifted(int x, int y) const
    {
      double max = img->getMatrix().maxValue;
      double min = img->getMatrix().minValue;
      double range = max - min;
      double shiftedValue = max - img->getPixel(x, y);
      double normalizedValue = shiftedValue / range * 255 ;
      return limited(normalizedValue, 0, 255);
    }

    int getPixel(int x, int y) const
    {
      return getValueShifted(x, y);
    }
  };
}
#endif
