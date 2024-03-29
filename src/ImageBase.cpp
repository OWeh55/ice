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

#include "IceException.h"
#include "util.h"     // for MulDiv
#include "macro.h"    // for min/max functions
#include "ImageBase.h"
#include "ImageTemplate.h"

namespace ice
{
  ImageBase::ImageBase(int SizeX, int  SizeY,
                       int MaxIntensity,
                       const std::string& titleP, ImageBase* par)
    : xsize(SizeX),
      ysize(SizeY),
      maxval(MaxIntensity),
      refcount(0),
      parent(par),
      title(titleP)
  {
  }

  ImageBase::~ImageBase()
  {
  }

#define FNAME "Image::set"
  int ImageBase::set(int grayvalue)
  {
    if (grayvalue > maxval)
      throw IceException(FNAME, M_WRONG_VALUE);

    for (int y = 0; y < ysize; y++)
      for (int x = 0; x < xsize; x++)
        {
          setP(x, y, grayvalue);
        }

    return OK;
  }
#undef FNAME

#define FNAME "Image::invert"
  int ImageBase::invert()
  {
    for (int y = 0; y < ysize; y++)
      for (int x = 0; x < xsize; x++)
        {
          setP(x, y, maxval - getP(x, y));
        }

    return OK;
  }
#undef FNAME
} // namespace
