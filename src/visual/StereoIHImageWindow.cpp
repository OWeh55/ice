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

#include <wx/image.h>

#include "visual/StereoIHImageWindow.h"

namespace ice
{

  BEGIN_EVENT_TABLE(iceStereoIHImageWindow, ImageWindow)
    EVT_PAINT(iceStereoIHImageWindow::OnPaint)
  END_EVENT_TABLE()

  iceStereoIHImageWindow::iceStereoIHImageWindow(
    ice::ImageBase* redImageL,
    ice::ImageBase* greenImageL,
    ice::ImageBase* blueImageL,
    ice::ImageBase* redImageR,
    ice::ImageBase* greenImageR,
    ice::ImageBase* blueImageR,
    const std::string& windowname)
    : ImageWindow(redImageL, windowname)
  {
    addImage(greenImageL);
    addImage(blueImageL);
    addImage(redImageR);
    addImage(greenImageR);
    addImage(blueImageR);
  }

  void iceStereoIHImageWindow::PutPixel()
  {
    int i = 0;
    if (PaintWindowY % 2 != 0)
      {
        i = 3;
      }

    *(PixelArrayPtr++) = imgs[i]->getValueShifted(PaintImageX, PaintImageY);
    *(PixelArrayPtr++) = imgs[i + 1]->getValueShifted(PaintImageX, PaintImageY);
    *(PixelArrayPtr++) = imgs[i + 2]->getValueShifted(PaintImageX, PaintImageY);
  }

}
