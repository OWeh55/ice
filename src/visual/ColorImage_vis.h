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

#ifndef _COLORIMAGEVIS_H
#define _COLORIMAGEVIS_H

#include <string>
#include "visual/ImageMgr.h"

#include "ColorImage.h"

namespace ice
{
  /**
   * this function displays the given ColorImage in a new RGBImageWindow
   * if wname is not given, the imageTitle is taken as window title
   *
   * ATTENTION: if the image is a subImage it cannot be displayed (visual.cpp prevents this)
   *            maybe this should be extended one day...
   *
   * available modes:
   *    ON: the image will be visualized
   *    OFF: the image will be hidden
   *    DISABLE: the image will be removed from the autorefresh list (only prevents autorefresh
   *          if the image window is overlayed by another window and the getting visible,
   *          refresh will be made anyway)
   *    ENABLE: the image will be put on the autorefresh list
   *
   * @param mode defines how to display the image (available modes listed above)
   * @param img the ColorImage to be displayed
   * @param wname the title of the window, where the image will be displayed
   * @return pointer to the imageWindow that contains img
   */
  Visual Show(int mode, const ColorImage& img, std::string wname = "");
}

#endif  /* IFNDEF _COLORIMAGEVIS_H */
