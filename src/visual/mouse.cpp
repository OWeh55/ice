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

#include "macro.h"
#include "message.h"
#include "visual/visual.h"
#include "visual/ImageWindow.h"
#include "visual/mouse.h"


// using namespace ice;

namespace ice
{
#define FNAME "Mouse"
  int Mouse(ImageWindow& wndw, int& x, int& y)
  {
    return wndw.GetMousePosition(x, y);
  }

  int Mouse(const Image& img, int& x, int& y)
  {
    // check if the parameters are valid
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return WRONG_POINTER;
      }

    Visual p;
    ReturnErrorIfFailed(p = GetVisual(img));
    if (p == NULL)
      {
        Message(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return Mouse(*p, x, y);
  }

  int Mouse(const Image& img, IPoint& p)
  {
    return Mouse(img, p.x, p.y);
  }

  int Mouse(const Image& Img, int* x, int* y)
  {
    if (!IsImg(Img) || x == NULL || y == NULL)
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return WRONG_POINTER;
      }
    return Mouse(Img, *x, *y);
  }
#undef FNAME
}
