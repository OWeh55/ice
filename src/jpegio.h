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
/*************************************************************************/
/* jpegio.h                                                               */
/* Funktion: Bild E/A                                                    */
/*************************************************************************/
#ifndef _JPEGIO_H /* verhindern mehrfacher Verarbeitung in einem */
#define _JPEGIO_H /* Programm */

#include <string>

#include "base.h"
#include "ibuffer.h"

namespace ice
{
  Image ReadJPEGImg(const std::string& fname, Image& img, int flag = IB_SCALE);
  int ReadJPEGImg(const std::string& fname, Image& ir, Image& ig, Image& ib,
                  int flag = IB_SCALE);
  int WriteJPEGImg(const Image& img, const std::string& fname);
  int WriteJPEGImg(const Image& ir, const Image& ig, const Image& ib,
                   const std::string& fname);
  int InfJPEGFile(const std::string& fname, int& xsize, int& ysize,
                  int& maxval, int& nr);
}
#endif
