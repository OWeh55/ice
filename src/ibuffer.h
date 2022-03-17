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

  struct ibuffer
  {
    int width, height; // Bildgröße in Pixel
    int maxval;        // maximaler Pixelwert
    int planes;        // 1 - Grau, 3 - RGB
    int linelength;    // Zeilenlänge in Byte
    int valuesize;     // bytegröße der Werte
    int packmethod;    // Byteanordnungen
    int intensity;     // Invertierung Intensitaet -> Grauwert
    int byteorder;

    ibuffer(): data(nullptr) {}
    ~ibuffer()
    {
      delete data;
    }
    void alloc(int size)
    {
      delete data;
      data = new unsigned char [size];
    }
    unsigned char* const getData()
    {
      return data;
    }
  private:
    unsigned char* data;
  };

  Image Buffer2Image(ibuffer& ib, const Image& img, int flags = IB_CROP);
  void Buffer2Image(ibuffer& ib, const Image& imgr, const Image& imgg, const Image& imgb,
                    int flags = IB_CROP);

  void Image2Buffer(const Image& ir, const Image& ig, const Image& ib, ibuffer& ibuf);
  void Image2Buffer(const Image& img, ibuffer& ib);

// char * as buffer
  void Buffer2Image(const unsigned char* buffer,
                    const Image& img,
                    bool intensity = true, int lineoffset = 0, int factor = 1, int offset = 0);

  void Buffer2Image(const unsigned char* buffer,
                    const Image& imgr, const Image& imgg, const Image& imgb,
                    bool intensity = true, int packmode = IB_RGB, int lineoffset = 0);

  void Image2Buffer(const Image& img,
                    unsigned char*& buffer, int& size,
                    bool intensity = true,
                    int lineoffset = 0, int factor = 1, int offset = 0);

  void Image2Buffer(const Image& imgr, const Image& imgg, const Image& imgb,
                    unsigned char*& buffer, int& size,
                    bool intensity = true,
                    int packmode = IB_RGB, int lineoffset = 0);

#define GRAYVALUE(r,g,b) ((r*30+g*59+b*11)/100)

}
#endif
