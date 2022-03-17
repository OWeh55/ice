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

#include "visual/ImageMgr.h"
#include "visual/Overlay2ImageWindow.h"
#include "macro.h"   // for min/max templates

namespace ice
{
  Overlay2ImageWindow::Overlay2ImageWindow(ice::ImageBase* BaseImagep,
      ice::ImageBase* OverlayImagep,
      const std::string& windownamep,
      int transparenz)
    : OverlayImageWindow(BaseImagep, OverlayImagep, windownamep),
      transparenz(transparenz),
      itransparenz(256 - transparenz)
  {
  }

  void Overlay2ImageWindow::PutPixel()
  {
    if (!BaseImage)
      {
        return;
      }
    unsigned int OverlayValue = OverlayImage->getP(PaintImageX, PaintImageY);
    unsigned char ro, go, bo;
    unsigned char rb, gb, bb;
    if (OverlayValue != 0)
      {
        OverlayColorTable.getColor(OverlayValue, ro, go, bo);
        BaseColorTable.getColor(BaseImage->getP(PaintImageX, PaintImageY), rb, gb, bb);
        rb = (rb * transparenz + itransparenz * ro) >> 8;
        gb = (gb * transparenz + itransparenz * go) >> 8;
        bb = (bb * transparenz + itransparenz * bo) >> 8;
      }
    else
      {
        BaseColorTable.getColor(BaseImage->getP(PaintImageX, PaintImageY), rb, gb, bb);
      }

    *(PixelArrayPtr++) = rb;
    *(PixelArrayPtr++) = gb;
    *(PixelArrayPtr++) = bb;
  }
}
