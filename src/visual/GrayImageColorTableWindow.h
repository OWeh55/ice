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

#ifndef __ICEGRAYIMAGEWINDOWCT_H
#define __ICEGRAYIMAGEWINDOWCT_H

#include "visual/ImageWindow.h"
#include "ColorTable.h"

namespace ice
{
  //! A window for displaying a gray value image. The intensity values
  //! of the image are mapped to a color for display,
  //! depending on the color table for the window.
  class GrayImageColorTableWindow : public ImageWindow
  {
  protected:
    ~GrayImageColorTableWindow() {};
  public:
    GrayImageColorTableWindow(ice::ImageBase* Img,
                              const std::string& windowname);

    virtual char getVisualizationType() const
    {
      return 1;
    }

    int GetGrayColor(unsigned int Entry, unsigned char& RedValue, unsigned char& GreenValue, unsigned char& BlueValue);
    int SetGrayColor(unsigned int Entry, unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue);
    int GetOverlayColor(unsigned int Entry, unsigned char& RedValue, unsigned char& GreenValue, unsigned char& BlueValue);
    int SetOverlayColor(unsigned int Entry, unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue);
    int SetGrayLUT(unsigned int First, unsigned int Last);
    int SetTable(int type = 0)
    {
      return colorTable.setTable(type);
    }

  protected:
    // implementations of pure virtual methods of ImageWindow
    virtual void PutPixel();

    //! The color table for this window.
    ColorTable colorTable;

    DECLARE_EVENT_TABLE()
  };

  ////////////////////////////////////////////////////////////////////////7
  //
  //             inline implementations
  //

  inline int GrayImageColorTableWindow::GetGrayColor(unsigned int Entry,
      unsigned char& RedValue, unsigned char& GreenValue, unsigned char& BlueValue)
  {
    return colorTable.getColor(Entry, RedValue, GreenValue, BlueValue);
  }

  inline int GrayImageColorTableWindow::GetOverlayColor(unsigned int Entry,
      unsigned char& RedValue, unsigned char& GreenValue, unsigned char& BlueValue)
  {
    return colorTable.getColor(Entry, RedValue, GreenValue, BlueValue);
  }
}
#endif // #ifndef __ICEGRAYIMAGEWINDOW_H
