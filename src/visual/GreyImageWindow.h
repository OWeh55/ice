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

#ifndef __ICEGREYIMAGEWINDOW_H
#define __ICEGREYIMAGEWINDOW_H

#include "visual/ImageWindow.h"
#include "ColorTable.h"

namespace ice
{
  //! A window for displaying a grey value image. The intensity values
  //! of the image are mapped to a color for display,
  //! depending on the color table for the window.
  class GreyImageWindow : public ImageWindow
  {
  protected:
    ~GreyImageWindow() {};
  public:
    GreyImageWindow(ice::ImageBase* img,
                    const std::string& windowname);

    GreyImageWindow(ice::ImageD* img,
                    const std::string& windowname);

    virtual char getVisualizationType() const
    {
      return 1;
    }

  protected:
    // implementations of pure virtual methods of ImageWindow
    virtual void PutPixel();

    DECLARE_EVENT_TABLE()
  };
}
#endif // #ifndef __ICEGREYIMAGEWINDOW_H
