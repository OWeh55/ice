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
#ifndef _PCXIO_H
#define _PCXIO_H

#include "base.h"
#include "ibuffer.h"

namespace ice
{
  struct HeaderPCX
  {
    unsigned char Signature;
    unsigned char Version;
    unsigned char Encoding;
    unsigned char BitsPerPixel;
    short int MinX;
    short int MinY;
    short int MaxX;
    short int MaxY;
    short int Hdpi;
    short int Vdpi;
    unsigned char ColorMap[48];
    unsigned char Reserved;
    unsigned char Nplanes;
    short int BytesPerLine ;
    short int PaletteInfo;
    short int ScreenSixeH;
    short int ScreenSizeV;
    unsigned char dummy[54];
  };

  Image ReadPCXImg(const std::string& fname, Image& img, int flags = IB_SCALE);
  int WritePCXImg(const Image& img, const std::string& fname);
  int InfPCXFile(const std::string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr);

}
#endif

