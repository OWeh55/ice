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
//
//   SelPoint
// interactive selection of points
//

#ifndef _POINTSEL_H
#define _POINTSEL_H

#include "base.h"
#include "Vector.h"
#include "geo_ob.h"
#include "ColorImage.h"

namespace ice
{
  enum windowSelectionMode {select, shiftupperleft, shiftlowerright, setupperleft, setlowerright};
  int SelPoint(int option, const Image& img, int p[2]);// Compatibility
  IPoint SelPoint(int option, const Image& img);
  IPoint SelPoint(int option, const Image& img, int& rc);
  IPoint SelPoint(const Image& img);
  IPoint SelPoint(const Image& img, int& rc);

  /**
   * function to wait for a click on the visualization window of img
   * and return the coordinates of the point
   *
   * @param img the shown image, where the point is to be selected
   * @return the position where the mouse click is made
   */
  IPoint SelPoint(const ColorImage& img);

  IVector SelVector(int option, const Image& img);
  IVector SelVector(int option, const Image& img, int& rc);
  IVector SelVector(const Image& img);
  IVector SelVector(const Image& img, int& rc);
}
#endif
