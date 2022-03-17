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

#include <iostream>

// for painting into an wxImage prior to painting in an wxBitmap
#include <wx/image.h>

#include "base.h"
#include "ImageBase.h"
#include "visual/ImageMgr.h"
#include "visual/GrayImageWindow.h"
#include "macro.h"  // for min / max templates

namespace ice
{
  BEGIN_EVENT_TABLE(GrayImageWindow, ImageWindow)
    EVT_PAINT(GrayImageWindow::OnPaint)
  END_EVENT_TABLE()

#define FNAME "GrayImageWindow::GrayImageWindow"
  GrayImageWindow::GrayImageWindow(ImageBase* img,
                                   const std::string& windowname)
    : ImageWindow(img, windowname)
  {
  }

  GrayImageWindow::GrayImageWindow(ImageD* img,
                                   const std::string& windowname)
    : ImageWindow(img, windowname)
  {
  }

  void GrayImageWindow::PutPixel()
  {
    int i = imgs[0]->getValueShifted(PaintImageX, PaintImageY);
    *(PixelArrayPtr++) = i;
    *(PixelArrayPtr++) = i;
    *(PixelArrayPtr++) = i;
  }
}
