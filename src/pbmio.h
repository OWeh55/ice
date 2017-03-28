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
#ifndef ppm_h
#define ppm_h

#include "base.h"
#include "ibuffer.h"
#include "exfile.h"

using namespace std;

namespace ice
{
  int InfPBMFile(const string& fname, int& xsize, int& ysize, int& maxval, int& nr);

  int ReadPBMImg(const string& str, Image& r, Image& g, Image& b,
                 int flag = IB_SCALE);
  Image ReadPBMImg(const string& str, Image& img, int flag = IB_SCALE);

  // high level write
  int WritePBMImg(const Image& r, const Image& g, const Image& b, const string& str);
  int WritePBMImg(const Image& img, const string& str);

  // low level write
  int WritePBMImg(const Image& r, const Image& g, const Image& b, EXFILE& fd);
  int WritePBMImg(const Image& img, EXFILE& fd);
}
#endif
