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
