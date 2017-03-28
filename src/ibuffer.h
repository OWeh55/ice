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
#ifndef IBUFFER_H
#define IBUFFER_H

#include "base.h"

namespace ice
{
// Flags für verschiedene Funktionen
#define IB_CROP 0
#define IB_SCALE 1

#define IB_GRAY 1

#define IB_RGB 2
#define IB_BGR 3
#define IB_RGB32 4
#define IB_BGR32 5
#define IB_RGB_PLANES 6
#define IB_BGR_PLANES 7

#define IB_LSB_FIRST 0
#define IB_MSB_FIRST 1

  typedef struct ibuffer_struct
  {
    int width, height; // Bildgröße in Pixel
    int maxval;                 // maximaler Pixelwert
    int planes;     // 1 - Grau, 3 - RGB
    int linelength; // Zeilenlänge in Byte
    int valuesize;    // bytegröße der Werte
    int packmethod; // Byteanordnungen
    int intensity;  // Invertierung Intensitaet -> Grauwert
    int byteorder;
    bool can_delete; // true wenn *data freigegeben werden kann/muss
    unsigned char* data;
  } ibuffer;

  Image Buffer2Image(ibuffer& ib, const Image& img, int flags = IB_CROP);
  int Buffer2Image(ibuffer& ib, const Image& imgr, const Image& imgg, const Image& imgb,
                   int flags = IB_CROP);

  int Image2Buffer(const Image& ir, const Image& ig, const Image& ib, ibuffer& ibuf);
  int Image2Buffer(const Image& img, ibuffer& ib);

// char * as buffer
  int Buffer2Image(const unsigned char* buffer,
                   const Image& img,
                   bool intensity = true, int lineoffset = 0, int factor = 1, int offset = 0);

  int Buffer2Image(const unsigned char* buffer,
                   const Image& imgr, const Image& imgg, const Image& imgb,
                   bool intensity = true, int packmode = IB_RGB, int lineoffset = 0);

  int Image2Buffer(const Image& img,
                   unsigned char*& buffer, int& size,
                   bool intensity = true,
                   int lineoffset = 0, int factor = 1, int offset = 0);

  int Image2Buffer(const Image& imgr, const Image& imgg, const Image& imgb,
                   unsigned char*& buffer, int& size,
                   bool intensity = true,
                   int packmode = IB_RGB, int lineoffset = 0);

#define GRAYVALUE(r,g,b) ((r*30+g*59+b*11)/100)

}
#endif
