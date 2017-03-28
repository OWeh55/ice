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
/* psdio.h
   Function: read and write image files in photoshop format
*/
#ifndef PSDIO_H
#define PSDIO_H

#include <string>
#include "base.h"


namespace ice
{
  Image ReadPSDImg(const std::string& fname, Image& img, int flag = IB_SCALE);
  int ReadPSDImg(const std::string& fname, Image& ir, Image& ig, Image& ib,
                 int flag = IB_SCALE);
  int WritePSDImg(const Image& img, const std::string& fname);
  int WritePSDImg(const Image& ir, const Image& ig, const Image& ib,
                  const std::string& fname);
  int InfPSDFile(const std::string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr);
}

#endif
