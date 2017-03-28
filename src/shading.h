/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2013 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef SHADING_H
#define SHADING_H

#include "base.h"

namespace ice
{
  /**********************************************************************

      Shadingkorrektur

  ***********************************************************************/

  int correctShading(const Image& img, const Image& imgd, int mode = 2);
  int correctShading(const Image& img, const Image& imgd, Function2dWithFitting& fn);
}
#endif
