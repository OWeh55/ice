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
#ifndef _ICE_RGB_IMAGE_WIN_H
#define _ICE_RGB_IMAGE_WIN_H

#include "visual/ImageWindow.h"
#include "visual/ImageMgr.h"

namespace ice
{
  class RGBImageWindow : public ImageWindow
  {
  public:
    RGBImageWindow(ice::ImageBase* RedImage,
                   ice::ImageBase* GreenImage,
                   ice::ImageBase* BlueImage,
                   const std::string& windowname);

    virtual ~RGBImageWindow() {};

    virtual char getVisualizationType() const
    {
      return 3;
    }

  protected:
    // renorm values to 0..255
    int factor;

    // implementation of abstract methods of base class
    virtual void PutPixel();

    DECLARE_EVENT_TABLE()
  };

}
#endif
