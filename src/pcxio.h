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
#ifndef _PCXIO_H
#define _PCXIO_H

#include "base.h"
#include "ibuffer.h"

namespace ice
{

  using namespace std;

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




