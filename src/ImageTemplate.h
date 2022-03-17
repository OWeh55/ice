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
#ifndef __IMAGE_TEMPLATE_H
#define __IMAGE_TEMPLATE_H

#include <cstring>
#include <string>
#include <list>
#include <limits>
#include <iostream>

#include "base.h"
#include "ImageBase.h"

namespace ice
{
  template<typename ValueType>
  class iceImage : public ImageBase
  {
    friend class ImageManager;

  protected:
    void alloc();

  public:
    iceImage(int SizeX,  int  SizeY,
             int MaxIntensity, const std::string& title);

    iceImage(int SizeX,  int  SizeY,
             int MaxIntensity, const std::string& title, ValueType* data);

    iceImage(iceImage<ValueType>* img,
             const Window& w,
             const std::string& title);

    virtual ~iceImage();

    // implementation of all the virtual methods
    // methods are virtual (and must be virtual)

    virtual void setP(int PosX, int PosY, int Value)
    {
      Pixel[PosY][PosX] = Value;
    }

    virtual int getP(int PosX, int PosY) const
    {
      return Pixel[PosY][PosX];
    }

    virtual int getBytesPerPoint() const;
    virtual int ImageType() const;
    virtual void* getDataPtr()
    {
      return Pixel;
    }
    virtual const void* getDataPtr() const
    {
      return Pixel;
    }

    virtual int set(int val);

  protected:
    virtual ValueType* PixelAddress(int PosX, int PosY);

    ValueType* Pixelarray;
    ValueType** Pixel;

    bool can_delete;

    virtual void copyData(ImageBase* source);
  };

  typedef iceImage<PixelType1> iceImage1;
  typedef iceImage<PixelType2> iceImage2;
  typedef iceImage<PixelType3> iceImage3;

  /*
   *
   *   implementations
   *
   */
  template<typename ValueType>
  inline void iceImage<ValueType>::alloc()
  {
    Pixel = new ValueType*[ysize];
    Pixelarray = new ValueType[xsize * ysize];

    for (int y = 0; y < ysize; y++)
      {
        Pixel[y] = &Pixelarray[y * xsize];
      }

    can_delete = true;
  }

// neues Bild mit Größenangaben
  template<typename ValueType>
  inline iceImage<ValueType>::iceImage(int SizeX,  int  SizeY,
                                       int MaxIntensity, const std::string& titleP)
    : ImageBase(SizeX, SizeY, MaxIntensity, titleP, nullptr)
  {
    alloc();
  }

  // neues bild als SubImage eines gegebenen Bildes
  template<typename ValueType>
  inline iceImage<ValueType>::iceImage(iceImage<ValueType>* imgPtr,
                                       const Window& w,
                                       const std::string& titleP)
    : ImageBase(w.Width(), w.Height(), imgPtr->maxval, titleP, imgPtr)
  {
    if (w.p1.x < 0 || w.p1.y < 0 ||
        w.p2.x >= imgPtr->xsize || w.p2.y >= imgPtr->ysize)
      throw IceException("iceImage<ValueType>::iceImage()", M_INVALID_WINDOW);
    Pixel = new ValueType*[ysize];
    Pixelarray = nullptr;
    can_delete = false;

    for (int y = 0; y < ysize; y++)
      {
        Pixel[y] = imgPtr->PixelAddress(w.p1.x, w.p1.y + y);
      }
  }

  template<typename ValueType>
  inline iceImage<ValueType>::~iceImage()
  {
    if (can_delete)
      {
        delete [] Pixelarray;
      }

    delete [] Pixel;
  }

  template<typename ValueType>
  inline int iceImage<ValueType>::getBytesPerPoint() const
  {
    return sizeof(ValueType);
  }

  template<typename ValueType>
  inline int iceImage<ValueType>::ImageType() const
  {
    return 0; // undefined, if unknown ValueType
  }

  template<>
  inline int iceImage<PixelType1>::ImageType() const
  {
    return 1;
  }

  template<>
  inline int iceImage<PixelType2>::ImageType() const
  {
    return 2;
  }

  template<>
  inline int iceImage<PixelType3>::ImageType() const
  {
    return 3;
  }

#define FNAME "PixelAddress"
  template<typename ValueType>
  inline ValueType* iceImage<ValueType>::PixelAddress(int PosX, int PosY)
  {
    if ((int)PosX >= (int)xsize)
      throw IceException(FNAME, M_XTOOLARGE);

    if ((int)PosY >= (int)ysize)
      throw IceException(FNAME, M_YTOOLARGE);

    return Pixel[PosY] + PosX;
  }
#undef FNAME

  template<typename ValueType>
  inline void iceImage<ValueType>::copyData(ImageBase* source)
  {
    // We assume that >Source< has the same type as we.
    // now we can copy the Pixels line per line

    for (int y = 0; y < ysize; y++)
      {
        memcpy(Pixel[y], ((iceImage<ValueType>*)source)->Pixel[y], sizeof(ValueType)*xsize);
      }
  }

  template<typename ValueType>
  inline int iceImage<ValueType>::set(int val)
  {
    for (int y = 0; y < ysize; y++)
      {
        ValueType* pptr = Pixel[y];

        for (int x = 0; x < xsize; x++)
          {
            *(pptr++) = val;
          }
      }

    return OK;
  }
}
#endif
