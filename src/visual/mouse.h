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
/*************************************************
 *   Mausfunktionen                              *
 *                                               *
 *   M. Schubert 20.11.92                        *
 *   W. Ortmann     09.05                        *
 *                                               *
 *************************************************/

#ifndef _MOUSE_H
#define _MOUSE_H

#include "ImageBase.h"
#include "visual/ImageWindow.h"

namespace ice
{

#define M_LEFT_DOWN       0x001
#define M_RIGHT_DOWN      0x002
#define M_MIDDLE_DOWN     0x004
#define M_LEFT_PRESSED    0x008
#define M_RIGHT_PRESSED   0x010
#define M_MIDDLE_PRESSED  0x020
#define M_LEFT_RELEASED   0x040
#define M_RIGHT_RELEASED  0x080
#define M_MIDDLE_RELEASED 0x100
#define M_MOVED           0x200
#define M_IN_WINDOW       0x400

  int Mouse(const Image& Img, int* x, int* y);
  int Mouse(const Image& Img, int& x, int& y);
  int Mouse(const Image& Img, IPoint& p);
  int Mouse(ImageWindow& wndw, int& x, int& y);
}
#endif
