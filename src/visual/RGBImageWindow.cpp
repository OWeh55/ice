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

#include "visual/RGBImageWindow.h"

namespace ice
{

  BEGIN_EVENT_TABLE(RGBImageWindow, ImageWindow)
    EVT_PAINT(RGBImageWindow::OnPaint)
  END_EVENT_TABLE()

  RGBImageWindow::RGBImageWindow(ice::ImageBase* redImage,
                                 ice::ImageBase* greenImage,
                                 ice::ImageBase* blueImage,
                                 const std::string& windowname)
    : ImageWindow(redImage, windowname)
  {
    addImage(greenImage);
    addImage(blueImage);
    factor = 1;
    if (redImage->maxval < 128)
      {
        factor = 255 / redImage->maxval;
      }
    // std::cout << factor<<std::endl;
  }

  void RGBImageWindow::PutPixel()
  {
    *(PixelArrayPtr++) = imgs[0]->getValueShifted(PaintImageX, PaintImageY) * factor;
    *(PixelArrayPtr++) = imgs[1]->getValueShifted(PaintImageX, PaintImageY) * factor;
    *(PixelArrayPtr++) = imgs[2]->getValueShifted(PaintImageX, PaintImageY) * factor;
  }
}
