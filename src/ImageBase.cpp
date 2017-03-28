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
      VisNumber(0),
#ifdef CONTROLLED_REFRESH
      timestamp(0),
#endif
      dfp(nullptr),
      refcount(0),
      parent(par),
      title(titleP)
  {
#ifdef CONTROLLED_REFRESH
    if (parent == nullptr)
      tsp = &timestamp;
    else
      {
        // get root of child tree
        while (par->parent != nullptr)
          par = par->parent;

        tsp = & (par->timestamp);
      }
#endif
  }

  ImageBase::~ImageBase()
  {
  }

#define FNAME "Image::set"
  int ImageBase::set(int grayvalue)
  {
    if (grayvalue > maxval)
      {
        ice::Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int y = 0; y < ysize; y++)
      for (int x = 0; x < xsize; x++)
        setP(x, y, grayvalue);

#ifdef CONTROLLED_REFRESH
    needRefresh();
#endif
    return OK;
  }
#undef FNAME
} // namespace
