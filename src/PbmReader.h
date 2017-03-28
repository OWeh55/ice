/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef ICE_PBM_READER_H
#define ICE_PBM_READER_H

#include "ibuffer.h"
#include "exfile.h"

namespace ice
{
  class PbmReader
  {
  public:
    PbmReader(): isOpen(false) {}
    PbmReader(const std::string& fn);
    ~PbmReader();

    void open(const std::string& fn);

    void getInfo(int& xs, int& ys, int& mv, int& ch) const;

    void getImage(const Image& r, const Image& g, const Image& b, int flag = IB_SCALE);
    void getImage(const Image& img, int flag = IB_SCALE);

    void getImage();

    void nextImage();

  private:
    unsigned int readNumber(FILE* fd);
    void readInfo();
    void readBuffer();

    bool isOpen;
    ibuffer ib;
    EXFILE fd;
    int xSize, ySize, maxValue, nChannels;
  };
}

#endif
