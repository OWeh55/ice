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

#include <algorithm>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "base.h"
#include "function.h"
#include "util.h"

namespace ice
{
#define FNAME "SetImg"
  void setImg(const Image& img, const Function2d& fn)
  {
    try
      {
        checkImage(img);

        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            {
              int v = RoundInt(fn(x, y));
              PutValUnchecked(img, x, y, std::max(0, limited(v, img)));
            }
      }
    RETHROW;
  }
#undef FNAME
}


