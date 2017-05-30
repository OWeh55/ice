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
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "IceException.h"
#include "macro.h"

#include "exfile.h"
#include "picio.h"

using namespace std;

namespace ice
{
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef unsigned int uint;

  static uchar tga_bread(FILE* fd);
  static ushort tga_sread(FILE* fd);

  static int tga_bwrite(FILE* fd, uchar b);
  static int tga_swrite(FILE* fd, ushort s);

#define FNAME "ReadTGAImg"
#define MAXIDENTLEN 8000
  int ReadTGAImg(const string& fname, Image& r, Image& g, Image& b, int flag)
  {
    FILE* fd;
    string hname = fname;

    ibuffer ib;

    int colormaporigin, colormaplen, colormapentrysize;
    int xorigin, yorigin;
    char bit, attr, dir, interleave;
    char ident[MAXIDENTLEN];
    int h;
    unsigned char rr = 0, gg = 0, bb = 0, alpha = 0;

    int y1, y2;
    int dy;

    if ((fd = fopen(hname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    int identlen = tga_bread(fd);
    int color_map_typ = tga_bread(fd);
    int typ = tga_sread(fd);

    colormaporigin = tga_bread(fd);
    colormaplen = tga_sread(fd);
    colormapentrysize = tga_bread(fd);

    xorigin = tga_sread(fd);
    yorigin = tga_sread(fd);

    ib.width = tga_sread(fd);
    ib.height = tga_sread(fd);

    bit = tga_bread(fd);
    attr = tga_bread(fd);

    dir = attr & 0x20;
    interleave = attr & 0xc0;

    if ((identlen < MAXIDENTLEN) && (identlen > 0))
      {
        fseek(fd, 18, SEEK_SET);
        fread(ident, 1, identlen, fd);
        ident[identlen] = 0;
        ReadImgFileComment = ident;
      }

    fseek(fd, 18 + identlen, SEEK_SET);

    if (typ != 2)   /* nur RGB und keine Kompression */
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (interleave != 0)   /*nur ohne Interleave */
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (colormaplen != 0)   /*nur ohne Farbtabelle*/
      throw IceException(FNAME, M_WRONG_FILETYPE);

    switch (bit)
      {
      case 32:
      case 24:
        ib.maxval = 255;
        break;
      case 15:
      case 16:
        ib.maxval = 31;
        break;
      default:
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    ib.planes = 3;
    ib.linelength = ib.width * 3;
    ib.valuesize = 1;
    ib.byteorder = IB_LSB_FIRST;
    ib.packmethod = IB_RGB;
    ib.can_delete = true;
    ib.data = (unsigned char*)malloc(ib.linelength * ib.height);
    ib.intensity = true;

    if (!IsImg(r))
      {
        r = NewImg(ib.width, ib.height, ib.maxval);
      }

    if (!IsImg(g))
      {
        g = NewImg(ib.width, ib.height, ib.maxval);
      }

    if (!IsImg(b))
      {
        b = NewImg(ib.width, ib.height, ib.maxval);
      }

    if (!IsImg(r) || !IsImg(g) || !IsImg(b))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (dir != 0)
      {
        y1 = 0;
        y2 = ib.height - 1;
        dy = 1;
      }
    else
      {
        y1 = ib.height - 1;
        y2 = 0;
        dy = -1;
      }

    for (int y = y1; y != (y2 + dy); y += dy)
      {
        for (int x = 0; x < ib.width; x++)
          {
            switch (bit)
              {
              case 32:
                bb = tga_bread(fd);
                gg = tga_bread(fd);
                rr = tga_bread(fd);
                alpha = tga_bread(fd);
                break;
              case 24:
                bb = tga_bread(fd);
                gg = tga_bread(fd);
                rr = tga_bread(fd);
                break;
              case 15:
              case 16:
                h = tga_sread(fd);
                rr = (h >> 10) & 0x1f;
                gg = (h >> 5) & 0x1f;
                bb = (h) & 0x1f;
                break;
              }

            ((unsigned char*)ib.data)[y * ib.linelength + x * 3] = rr;
            ((unsigned char*)ib.data)[y * ib.linelength + x * 3 + 1] = gg;
            ((unsigned char*)ib.data)[y * ib.linelength + x * 3 + 2] = bb;
          }
      }

    fclose(fd);

    if (!IsImg(r))
      {
        r = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(g))
      {
        g = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(b))
      {
        b = NewImg(ib.width, ib.height, ib.maxval);
      }
    Buffer2Image(ib, r, g, b, flag);
    return OK;
  }

  Image ReadTGAImg(const string& fname, Image& img, int flag)
  {
    // z.Z nur: einlesen eines RGB-Bildes als Grauwert-Bild
    FILE* fd;
    string hname = fname;

    ibuffer ib;

    int identlen;
    int color_map_typ;
    int typ;

    int colormaporigin, colormaplen, colormapentrysize;
    int xorigin, yorigin;
    char bit, attr, dir, interleave;
    char ident[MAXIDENTLEN];
    int h;
    unsigned char rr = 0, gg = 0, bb = 0, hh;

    int y1, y2;
    int dy;

    if ((fd = fopen(hname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    identlen = tga_bread(fd);
    color_map_typ = tga_bread(fd);
    typ = tga_sread(fd);

    colormaporigin = tga_bread(fd);
    colormaplen = tga_sread(fd);
    colormapentrysize = tga_bread(fd);

    xorigin = tga_sread(fd);
    yorigin = tga_sread(fd);

    ib.width = tga_sread(fd);
    ib.height = tga_sread(fd);

    bit = tga_bread(fd);
    attr = tga_bread(fd);

    dir = attr & 0x20;
    interleave = attr & 0xc0;

    if ((identlen < MAXIDENTLEN) && (identlen > 0))
      {
        fseek(fd, 18, SEEK_SET);
        fread(ident, 1, identlen, fd);
        ident[identlen] = 0;
        ReadImgFileComment = ident;
      }

    fseek(fd, 18 + identlen, SEEK_SET);

    if (typ != 2)   /* nur RGB und keine Kompression */
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (interleave != 0)   /*nur ohne Interleave */
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (colormaplen != 0)   /*nur ohne Farbtabelle*/
      throw IceException(FNAME, M_WRONG_FILETYPE);

    switch (bit)
      {
      case 32:
      case 24:
        ib.maxval = 255;
        break;
      case 15:
      case 16:
        ib.maxval = 31;
        break;
      default:
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    ib.planes = 3;
    ib.linelength = ib.width * 3;
    ib.valuesize = 1;
    ib.byteorder = IB_LSB_FIRST;
    ib.packmethod = IB_RGB;
    ib.can_delete = true;
    ib.data = (unsigned char*)malloc(ib.linelength * ib.height);
    ib.intensity = true;

    if (dir != 0)
      {
        y1 = 0;
        y2 = ib.height - 1;
        dy = 1;
      }
    else
      {
        y1 = ib.height - 1;
        y2 = 0;
        dy = -1;
      }

    for (int y = y1; y != (y2 + dy); y += dy)
      {
        for (int x = 0; x < ib.width; x++)
          {
            switch (bit)
              {
              case 32:
                bb = tga_bread(fd);
                gg = tga_bread(fd);
                rr = tga_bread(fd);
                hh = tga_bread(fd);
                break;
              case 24:
                bb = tga_bread(fd);
                gg = tga_bread(fd);
                rr = tga_bread(fd);
                break;
              case 15:
              case 16:
                h = tga_sread(fd);
                rr = (h >> 10) & 0x1f;
                gg = (h >> 5) & 0x1f;
                bb = (h) & 0x1f;
                break;
              }

            ((unsigned char*)ib.data)[y * ib.linelength + x * 3] = rr;
            ((unsigned char*)ib.data)[y * ib.linelength + x * 3 + 1] = gg;
            ((unsigned char*)ib.data)[y * ib.linelength + x * 3 + 2] = bb;
          }
      }

    fclose(fd);

    if (!IsImg(img))
      {
        img = NewImg(ib.width, ib.height, ib.maxval);
      }

    return Buffer2Image(ib, img, flag);
  }
#undef FNAME
#define FNAME "WriteTGAImg"
  int WriteTGAImg(const Image& r, const  Image& g, const  Image& b,
                  const string& fname)
  {
    FILE* fd;
    string hname = fname;
    unsigned int identlen;
    unsigned char rr, gg, bb;
    int x, y;
    unsigned int i;

    if ((fd = fopen(hname.c_str(), FWMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    identlen = WriteImgFileComment.length();

    if (identlen > 0)
      {
        tga_bwrite(fd, identlen + 1);
      }
    else
      {
        tga_bwrite(fd, 0);
      }

    tga_bwrite(fd, 0);
    tga_swrite(fd, 2);

    tga_bwrite(fd, 0);
    tga_swrite(fd, 0);
    tga_bwrite(fd, 0);

    tga_swrite(fd, 0);
    tga_swrite(fd, 0);

    tga_swrite(fd, r->xsize);
    tga_swrite(fd, r->ysize);

    tga_bwrite(fd, 24);
    tga_bwrite(fd, 0x20);

    if (identlen > 0)
      {
        for (i = 0; i < identlen; i++)
          {
            tga_bwrite(fd, WriteImgFileComment[i]);
          }

        tga_bwrite(fd, 0);
      }

    for (y = 0; y < r->ysize; y++)
      {
        for (x = 0; x < r->xsize; x++)
          {
            rr = 255 - (GetVal(r, x, y) * 255 / r.maxval);
            gg = 255 - (GetVal(g, x, y) * 255 / g.maxval);
            bb = 255 - (GetVal(b, x, y) * 255 / b.maxval);
            tga_bwrite(fd, bb);
            tga_bwrite(fd, gg);
            tga_bwrite(fd, rr);
          }
      }

    fclose(fd);
    return OK;
  }

  /*******************************************************************/
  /* Hilfsprozeduren zum Lesen von Daten */
  static uchar tga_bread(FILE* fd)
  {
    uchar h;
    fread(&h, 1, 1, fd);
    return h;
  }

  /*******************************************************************/
  static  ushort tga_sread(FILE* fd)
  {
    ushort b1, b2;
    b1 = tga_bread(fd);
    b2 = tga_bread(fd);
    return (b2 << 8) | b1;
  }

  /*******************************************************************/
  static  int tga_bwrite(FILE* fd, uchar b)
  {
    return (fwrite(&b, 1, 1, fd) == 1);
  }

  /*******************************************************************/
  static  int tga_swrite(FILE* fd, ushort s)
  {
    if (!tga_bwrite(fd, (uchar)(s & 0xff)))
      {
        return false;
      }

    return tga_bwrite(fd, (uchar)((s >> 8) & 0xff));
  }

  /*******************************************************************/
#undef FNAME
#define FNAME "InfTGAFile"
  int InfTGAFile(const string& fname,
                 int& xsize, int& ysize, int& maxval, int& nr)
  {
    FILE* fd;
    string hname = fname;

    int identlen;
    int color_map_typ;
    int typ;
    char ident[MAXIDENTLEN];

    int colormaporigin, colormaplen, colormapentrysize;
    int xorigin, yorigin;
    int width, height;
    char bit, attr, dir, interleave;

    if ((fd = fopen(hname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    identlen = tga_bread(fd);
    color_map_typ = tga_bread(fd);
    typ = tga_sread(fd);

    colormaporigin = tga_bread(fd);
    colormaplen = tga_sread(fd);
    colormapentrysize = tga_bread(fd);

    xorigin = tga_sread(fd);
    yorigin = tga_sread(fd);

    width = tga_sread(fd);
    height = tga_sread(fd);

    bit = tga_bread(fd);
    attr = tga_bread(fd);

    dir = attr & 0x20;
    interleave = attr & 0xc0;

    if (typ != 2)   /* nur RGB und keine Kompression */
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (interleave != 0)   /*nur ohne Interleave */
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (colormaplen != 0)   /*nur ohne Farbtabelle*/
      throw IceException(FNAME, M_WRONG_FILETYPE);

    xsize = width;
    ysize = height;
    nr = 3;

    switch (bit)
      {
      case 32:
      case 24:
        maxval = 255;
        break;
      case 15:
      case 16:
        maxval = 31;
        break;
      }

    if ((identlen < MAXIDENTLEN) && (identlen > 0))
      {
        fseek(fd, 18, SEEK_SET);
        fread(ident, 1, identlen, fd);
        ident[identlen] = 0;
        ReadImgFileComment = ident;
      }

    fclose(fd);
    return TGA;
  }
#undef FNAME
}
