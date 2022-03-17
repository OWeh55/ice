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

