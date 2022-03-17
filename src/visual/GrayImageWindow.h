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

#ifndef __ICEGRAYIMAGEWINDOW_H
#define __ICEGRAYIMAGEWINDOW_H

#include "visual/ImageWindow.h"
#include "ColorTable.h"

namespace ice
{
  //! A window for displaying a gray value image. The intensity values
  //! of the image are mapped to a color for display,
  //! depending on the color table for the window.
  class GrayImageWindow : public ImageWindow
  {
  protected:
    ~GrayImageWindow() {};
  public:
    GrayImageWindow(ice::ImageBase* img,
                    const std::string& windowname);

    GrayImageWindow(ice::ImageD* img,
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
#endif // #ifndef __ICEGRAYIMAGEWINDOW_H
