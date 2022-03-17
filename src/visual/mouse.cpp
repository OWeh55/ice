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

#include "macro.h"
#include "IceException.h"
#include "visual/visual.h"
#include "visual/ImageWindow.h"
#include "visual/mouse.h"

// using namespace ice;

namespace ice
{
#define FNAME "Mouse"
  int Mouse(ImageWindow& wndw, int& x, int& y)
  {
    return wndw.getMousePosition(x, y);
  }

  int Mouse(const Image& img, int& x, int& y)
  {
    try
      {
        Visual p;
        p = getVisual(img);
        if (p == nullptr)
          throw IceException(FNAME, M_NOT_VIS);

        return Mouse(*p, x, y);
      }
    RETHROW;
  }

  int Mouse(const Image& img, IPoint& p)
  {
    return Mouse(img, p.x, p.y);
  }

  int Mouse(const Image& Img, int* x, int* y)
  {
    if (!IsImg(Img) || x == NULL || y == NULL)
      throw IceException(FNAME, M_WRONG_IMAGE);
    return Mouse(Img, *x, *y);
  }
#undef FNAME
}
