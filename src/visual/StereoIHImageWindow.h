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
#ifndef __ICEStereoIHIMAGEWIN_H
#define __ICEStereoIHIMAGEWIN_H

#include "visual/ImageWindow.h"

namespace ice
{
  class iceStereoIHImageWindow : public ImageWindow
  {
  protected:
    ~iceStereoIHImageWindow() {};
  public:
    iceStereoIHImageWindow(
      ice::ImageBase* RedImageL,
      ice::ImageBase* GreenImageL,
      ice::ImageBase* BlueImageL,
      ice::ImageBase* RedImageR,
      ice::ImageBase* GreenImageR,
      ice::ImageBase* BlueImageR,
      const std::string& windowname);

    virtual char getVisualizationType() const
    {
      return 4;
    }

  protected:
    // pure virtual implementations
    virtual void PutPixel();

    DECLARE_EVENT_TABLE()
  };

}
#endif // #ifndef __ICEStereoIHIMAGEWIN_H
