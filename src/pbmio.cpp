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

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>

#include "exfile.h"
#include "macro.h"
#include "IceException.h"

#include "picio.h"
#include "PbmReader.h"

using namespace std;

namespace ice
{
#define FNAME "ReadPBMImg"
  int ReadPBMImg(const string& fname, Image& r, Image& g, Image& b, int flag)
  {
    try
      {
        PbmReader rd(fname);
        int x, y, mv, ch;
        rd.getInfo(x, y, mv, ch);
        if (!IsImg(r))
          {
            r = NewImg(x, y, mv);
          }
        if (!IsImg(g))
          {
            g = NewImg(x, y, mv);
          }
        if (!IsImg(b))
          {
            b = NewImg(x, y, mv);
          }
        rd.getImage(r, g, b, flag);
        return OK;
      }
    RETHROW;
  }

  Image ReadPBMImg(const string& fname, Image& img, int flag)
  {
    try
      {
        PbmReader rd(fname);
        int x, y, mv, ch;
        rd.getInfo(x, y, mv, ch);
        if (!IsImg(img))
          {
            img = NewImg(x, y, mv);
          }
        rd.getImage(img, flag);
        return img;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "WritePBMImg"
  int WritePBMImg(const Image& r, const Image& g, const Image& b,
                  EXFILE& fd)
  {
    int maxval = r.maxval;

    fprintf(fd.fd, "P6\n%d %d\n%d\n", r->xsize, r->ysize, maxval);

    if (maxval < 256)
      {
        for (int y = 0; y < r.ysize; ++y)
          for (int x = 0; x < r.xsize; ++x)
            {
              unsigned char rb = maxval - r.getPixelUnchecked(x, y);
              unsigned char gb = maxval - g.getPixelUnchecked(x, y);
              unsigned char bb = maxval - b.getPixelUnchecked(x, y);
              fputc(rb, fd.fd);
              fputc(gb, fd.fd);
              fputc(bb, fd.fd);
            }
      }
    else
      {
        for (int y = 0; y < r.ysize; ++y)
          for (int x = 0; x < r.xsize; ++x)
            {
              short rw = maxval - r.getPixelUnchecked(x, y);
              short gw = maxval - g.getPixelUnchecked(x, y);
              short bw = maxval - b.getPixelUnchecked(x, y);
              fputc(rw >> 8, fd.fd);
              fputc(rw & 0xff, fd.fd);
              fputc(gw >> 8, fd.fd);
              fputc(gw & 0xff, fd.fd);
              fputc(bw >> 8, fd.fd);
              fputc(bw & 0xff, fd.fd);
            }
      }
    return OK;
  }

  int WritePBMImg(const Image& r, const Image& g, const Image& b,
                  const string& fname)
  {
    EXFILE fd = exopen(fname, FWMODUS);

    if (fd.fd == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    int rc = WritePBMImg(r, g, b, fd);

    exclose(fd);
    return rc;
  }

  int WritePBMImg(const Image& img, const string& fname)
  {
    FILE* fd;
    unsigned char ib;
    short int iw;
    int maxval;

    if ((fd = fopen(fname.c_str(), FWMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    maxval = img.maxval;
    fprintf(fd, "P5\n%d %d %d\n", img->xsize, img->ysize, maxval);

    if (maxval < 256)
      {
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            {
              ib = maxval - GetVal(img, x, y);
              fputc(ib, fd);
            }
      }
    else
      {
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            {
              iw = maxval - GetVal(img, x, y);
              fputc(iw >> 8, fd);
              fputc(iw & 0xff, fd);
            }
      }

    fclose(fd);
    return OK;
  }
#undef FNAME
#define FNAME "InfPBMFile"
  int InfPBMFile(const string& fname,
                 int& xsize, int& ysize, int& maxval, int& nr)
  {
    PbmReader rd(fname);
    rd.getInfo(xsize, ysize, maxval, nr);
    return OK;
  }
#undef FNAME
}
