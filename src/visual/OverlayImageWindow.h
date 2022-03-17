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

#ifndef __ICEOVERLAYIMGWIN_H
#define __ICEOVERLAYIMGWIN_H

#include "visual/ImageWindow.h"
#include "ColorTable.h"

namespace ice
{
  class OverlayImageWindow : public ImageWindow
  {
  protected:
    ~OverlayImageWindow() {};
  public:
    OverlayImageWindow(ice::ImageBase* BaseImage,
                       ice::ImageBase* OverlayImage,
                       const std::string& windowname);

    int GetGreyColor(unsigned int Entry,
                     unsigned char& RedVal, unsigned char& GreenVal, unsigned char& BlueVal);
    int SetGreyColor(unsigned int Entry,
                     unsigned char RedVal, unsigned char GreenVal, unsigned char BlueVal);

    int SetGreyLUT(unsigned int First, unsigned int Last);

    int GetOverlayColor(unsigned int Entry,
                        unsigned char& RedVal, unsigned char& GreenVal, unsigned char& BlueVal);
    int SetOverlayColor(unsigned int Entry,
                        unsigned char RedVal, unsigned char GreenVal, unsigned char BlueVal);

    virtual char getVisualizationType() const
    {
      return 2;
    }

  protected:
    // pure virtual implementations
    virtual void PutPixel();
    // The color tables for this window
    ColorTable BaseColorTable, OverlayColorTable;

    // The images that should be displayed in this window. BaseImage will be displayed
    // just like in a grey value window as long as it isn't overlayed by OverlayImage.
    ice::ImageBase* BaseImage;
    ice::ImageBase* OverlayImage;

    DECLARE_EVENT_TABLE()
  };

////////////////////////////////////////////////////////////////////////7
//
//             inline implementations
//

  inline int OverlayImageWindow::GetGreyColor
  (unsigned int Entry, unsigned char& RedVal, unsigned char& GreenVal, unsigned char& BlueVal)
  {
    return BaseColorTable.getColor(Entry, RedVal, GreenVal, BlueVal);
  }

  inline int OverlayImageWindow::GetOverlayColor
  (unsigned int Entry, unsigned char& RedVal, unsigned char& GreenVal, unsigned char& BlueVal)
  {
    return OverlayColorTable.getColor(Entry, RedVal, GreenVal, BlueVal);
  }
}
#endif // #ifndef __ICEOVERLAYIMGWIN_H
