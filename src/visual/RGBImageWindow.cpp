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
      factor = 255 / redImage->maxval;
    // std::cout << factor<<std::endl;
  }

  void RGBImageWindow::PutPixel()
  {
    *(PixelArrayPtr++) = imgs[0]->getValueShifted(PaintImageX, PaintImageY) * factor;
    *(PixelArrayPtr++) = imgs[1]->getValueShifted(PaintImageX, PaintImageY) * factor;
    *(PixelArrayPtr++) = imgs[2]->getValueShifted(PaintImageX, PaintImageY) * factor;
  }
}
