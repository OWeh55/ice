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

#include <iostream>

// for painting into an wxImage prior to painting in an wxBitmap
#include <wx/image.h>

#include "base.h"
#include "ImageBase.h"
#include "visual/ImageMgr.h"
#include "visual/GreyImageColorTableWindow.h"
#include "macro.h"  // for min / max templates

namespace ice
{
  BEGIN_EVENT_TABLE(GreyImageColorTableWindow, ImageWindow)
    EVT_PAINT(GreyImageColorTableWindow::OnPaint)
  END_EVENT_TABLE()

#define FNAME "GreyImageColorTableWindow::GreyImageColorTableWindow"
  GreyImageColorTableWindow::GreyImageColorTableWindow(ImageBase* img,
      const std::string& windowname)
    : ImageWindow(img, windowname),
      colorTable(img->maxval)
  {
  }

  int GreyImageColorTableWindow::SetGreyColor(unsigned int Entry,
      unsigned char RedValue,
      unsigned char GreenValue,
      unsigned char BlueValue)
  {
    int RetVal = colorTable.setColor(Entry, RedValue, GreenValue, BlueValue);
    if (RetVal != ERROR)
      {
        Refresh();
      }
    return RetVal;
  }

  int GreyImageColorTableWindow::SetOverlayColor(unsigned int Entry,
      unsigned char RedValue,
      unsigned char GreenValue,
      unsigned char BlueValue)
  {
    int RetVal = colorTable.setColor(Entry, RedValue, GreenValue, BlueValue);
    if (RetVal != ERROR)
      {
        Refresh();
      }
    return RetVal;
  }

  int GreyImageColorTableWindow::SetGreyLUT(unsigned int First, unsigned int Last)
  {
    int RetVal = colorTable.setLinear(First, Last);
    if (RetVal != ERROR)
      {
        Refresh();
      }
    return RetVal;
  }

  void GreyImageColorTableWindow::PutPixel()
  {
    int i = imgs[0]->getPixel(PaintImageX, PaintImageY);
    unsigned char r, g, b;
    colorTable.getColor(i, r, g, b);
    *(PixelArrayPtr++) = r;
    *(PixelArrayPtr++) = g;
    *(PixelArrayPtr++) = b;
  }
}
