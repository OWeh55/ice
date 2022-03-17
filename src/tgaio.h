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
#ifndef tga_h
#define tga_h

#include "base.h"
#include "ibuffer.h"

namespace ice
{
  int ReadTGAImg(const std::string& str, Image& r, Image& g, Image& b,
                 int flag = IB_SCALE);
  Image ReadTGAImg(const std::string& str, Image& img, int flag = IB_SCALE);

  int WriteTGAImg(const Image& r, const Image& g, const Image& b,
                  const std::string& str);
  int InfTGAFile(const std::string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr);
}
#endif
