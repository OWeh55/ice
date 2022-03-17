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

#include <wx/wx.h>

#include "../macro.h"
#include "../defs.h"

#include "App.h"

#include "visual.h"
#include "ImageMgr.h"

#include "visual/ColorImage_vis.h"

using namespace std;
namespace ice
{
#define FNAME "Show"
  Visual Show(int mode, const ColorImage& img, string wname)
  {
    if (!img.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (wname.empty())
      {
        wname = img.getTitle();
      }

    switch (mode)
      {
      case ON:
        return wxGetApp().GetImageManager()->Show(_RGB,
               img.redImage().Img(),
               img.greenImage().Img(),
               img.blueImage().Img(), wname);

      case OFF:
        return wxGetApp().GetImageManager()->Show(OFF, getVisual(img));
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }
    return OK;
  }
#undef FNAME

} /* namespace ice */
