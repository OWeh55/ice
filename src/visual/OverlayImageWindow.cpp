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
#include "visual/OverlayImageWindow.h"
#include "macro.h"   // for min/max templates

namespace ice
{

  BEGIN_EVENT_TABLE(OverlayImageWindow, ImageWindow)
    EVT_PAINT(OverlayImageWindow::OnPaint)
  END_EVENT_TABLE()

  OverlayImageWindow::OverlayImageWindow(ice::ImageBase* BaseImagep,
                                         ice::ImageBase* OverlayImagep,
                                         const std::string& windownamep)
    : ImageWindow(OverlayImagep, windownamep),
      BaseColorTable(BaseImagep->maxval),
      OverlayColorTable(BaseImagep->maxval),
      BaseImage(BaseImagep),
      OverlayImage(OverlayImagep)
  {
    // if a base image is given, we have to check, whether the sizes of the base image and the
    // overlay image match each other. If they don't, we can't create
    // the window and throw an exception.
    if (!(BaseImage->xsize == OverlayImage->xsize && BaseImage->ysize == OverlayImage->ysize))
      {
        return;
      }

    addImage(BaseImagep);

    // initialize color table for the base image, if one is provided
    BaseColorTable.setLinear(0, BaseImage->maxval);

    // initialize color table for the overlay image
    OverlayColorTable.setTable(0);

    // set the image status
    BaseImage->startVis(DestroyWindows);
    OverlayImage->startVis(DestroyWindows);
  }

  bool OverlayImageWindow::Destroy()
  {
    BaseImage->stopVis();
    OverlayImage->stopVis();
    return ImageWindow::Destroy();
  }

  int OverlayImageWindow::SetGreyColor(unsigned int Entry,
                                       unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue)
  {
    int RetVal = BaseColorTable.setColor(Entry, RedValue, GreenValue, BlueValue);
    if (RetVal != ERROR)
      {
        Refresh();
      }
    return RetVal;
  }

  int OverlayImageWindow::SetGreyLUT(unsigned int First, unsigned int Last)
  {
    int RetVal = BaseColorTable.setLinear(First, Last);
    if (RetVal != ERROR)
      {
        Refresh();
      }
    return RetVal;
  }

  int OverlayImageWindow::SetOverlayColor(unsigned int Entry, unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue)
  {
    int RetVal = OverlayColorTable.setColor(Entry, RedValue, GreenValue, BlueValue);
    if (RetVal != ERROR)
      {
        Refresh();
      }
    return RetVal;
  }

  void OverlayImageWindow::PutPixel()
  {
    if (!BaseImage)
      {
        return;
      }
    unsigned int OverlayValue = OverlayImage->getP(PaintImageX, PaintImageY);
    unsigned char r, g, b;
    if (OverlayValue != 0)
      {
        OverlayColorTable.getColor(OverlayValue, r, g, b);
      }
    else
      {
        BaseColorTable.getColor(BaseImage->getP(PaintImageX, PaintImageY), r, g, b);
      }
    *(PixelArrayPtr++) = r;
    *(PixelArrayPtr++) = g;
    *(PixelArrayPtr++) = b;
  }
  /*
  bool OverlayImageWindow::ShowsImage(ImageBase *img) const
  {
    std::cout << "OverlayWindow::showsImage" << std::endl;
    return img == BaseImage || img == OverlayImage;
  }
  */
}

