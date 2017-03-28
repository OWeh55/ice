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

#include <climits>
#include <stdexcept>
#include "macro.h"
#include "PbmReader.h"

using namespace std;

namespace ice
{
  unsigned int PbmReader::readNumber(FILE* fd)
  {
    int c;
    bool isComment = false;
    do
      {
        c = fgetc(fd);
        if (c == EOF)
          throw runtime_error("PbmReader - EOF reading Number");
        if (c == '#')
          isComment = true;
        else if (isComment && c == 0x0a)
          isComment = false;
      }
    while (isComment || isspace(c));

    unsigned int n = 0;

    while (isdigit(c))
      {
        n = n * 10 + (c - '0');
        c = getc(fd);
      }

    return n;
  }

  PbmReader::PbmReader(const string& fn): isOpen(false)
  {
    open(fn);
  }

  PbmReader::~PbmReader()
  {
    if (isOpen)
      {
        exclose(fd);
        delete ib.data;
      }
  }

  void PbmReader::readInfo()
  {
    int c = fgetc(fd.fd);

    if (c != 'P')
      {
        if (c == EOF)
          throw runtime_error("PbmReader - Empty pbm file");
        else
          throw runtime_error("PbmReader - No pbm file");
      }

    c = fgetc(fd.fd);

    switch (c)
      {
      case '5':
        nChannels = 1;
        break;
      case '6':
        nChannels = 3;
        break;
      default:
      {
        throw runtime_error("PbmReader - Not supported pbm file type");
      }
      }

    xSize = readNumber(fd.fd);
    ySize = readNumber(fd.fd);

    unsigned int maxuval = readNumber(fd.fd);

    if (maxuval > INT_MAX)
      maxValue = INT_MAX;     // ice limit for max. value
    else
      maxValue = maxuval;
  }

  void PbmReader::open(const string& fn)
  {
    if (isOpen)
      throw logic_error("PbmReader - Already opened");

    fd = exopen(fn, FRMODUS);

    if (fd.fd == nullptr)
      throw runtime_error("PbmReader - Cannot open file " + fn);

    readInfo();

    isOpen = true;
    ib.data = nullptr;
  }

  void PbmReader::nextImage()
  {
    delete ib.data;
    ib.data = nullptr;
    readInfo();
  }

  void PbmReader::getInfo(int& xs, int& ys, int& mv, int& ch) const
  {
    if (!isOpen)
      throw logic_error("PbmReader - Not opened");
    xs = xSize;
    ys = ySize;
    mv = maxValue;
    ch = nChannels;
  }

  void PbmReader::readBuffer()
  {
    if (!isOpen)
      throw logic_error("PbmReader - Not opened");
    if (ib.data == nullptr) // if already read - ignore
      {
        int valsize;

        if (maxValue >= (1 << 16))
          valsize = 4;
        else if (maxValue >= (1 << 8))
          valsize = 2;
        else
          valsize = 1;

        int pixelsize = valsize * nChannels;

        ib.width = xSize;
        ib.height = ySize;

        ib.maxval = maxValue;

        ib.planes = nChannels;

        ib.linelength = ib.width * pixelsize;
        ib.valuesize = valsize;
        ib.byteorder = IB_MSB_FIRST;
        ib.packmethod = IB_RGB;
        ib.can_delete = false;
        ib.data = new unsigned char [ib.linelength * ib.height];
        ib.intensity = true;
        int nReadValues = fread(ib.data, pixelsize, ib.width * ib.height, fd.fd);
        if (nReadValues !=  ib.width * ib.height)
          throw runtime_error("PbmReader - Error reading pbm file data");
      }
  }

  void PbmReader::getImage(const Image& r, const Image& g, const Image& b, int flag)
  {
    readBuffer();
    IfFailed(MatchImg(r, g, b))
    throw length_error("PbmReader - Images do not match");
    Buffer2Image(ib, r, g, b, flag);
  }

  void PbmReader::getImage(const Image& img, int flag)
  {
    readBuffer();
    Buffer2Image(ib, img, flag);
  }

  void PbmReader::getImage()
  {
    readBuffer();
  }
}
