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

#ifndef __ICEOVERLAY2IMGWIN_H
#define __ICEOVERLAY2IMGWIN_H

#include "visual/OverlayImageWindow.h"
#include "ColorTable.h"

namespace ice
{
  class Overlay2ImageWindow : public OverlayImageWindow
  {
  protected:
    ~Overlay2ImageWindow() {};
  public:
    Overlay2ImageWindow(ice::ImageBase* BaseImage,
                        ice::ImageBase* OverlayImage,
                        const std::string& windowname,
                        int transparency = 128);

    virtual char getVisualizationType() const
    {
      return 5;
    }

  protected:
    virtual void PutPixel();

    int transparenz;
    int itransparenz;
  };

}
#endif // #ifndef __ICEOVERLAY2IMGWIN_H
