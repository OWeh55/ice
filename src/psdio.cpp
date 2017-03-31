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

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "exfile.h"
#include "picio.h"
#include "psdio.h"
#include "macro.h"

namespace ice
{

#define FNAME "WriteImg"
  int WritePSDImg(const Image& img, const std::string& filename)
  {
    unsigned char head[] =
    {
      '8', 'B', 'P', 'S',   // signature #0
      0, 1,                       // version #4
      0, 0, 0, 0, 0, 0,   // reserved  #6
      0, 1,     // number of channels #12
      0, 0, 0, 0,     // height #14
      0, 0, 0, 0,     // width  #18
      0, 8,     // bit per value #22
      0, 1,     // mode (1=grey) #24
      0, 0, 0, 0,     // color mode data (none) #26
      0, 0, 0, 0,     // image resources (none) #30
      0, 0, 0, 0,     // layer/mask info (none) #34
      0, 0        // no compression #38
    };

    int row, col;
    unsigned int c;

    int width = img->xsize;

    FILE* ofp = fopen(filename.c_str(), FWMODUS);

    head[16] = img->ysize / 256;
    head[17] = img->ysize % 256;
    head[20] = width / 256;
    head[21] = width % 256;

    if (img->maxval < 256)
      {
        head[23] = 8;
      }
    else if (img->maxval < 256 * 256)
      {
        head[23] = 16;
      }
    else
      {
        throw IceException(FNAME, M_HIGHRANGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    fwrite(head, 40, 1, ofp);

    if (head[23] == 16)
      {
        for (row = 0; row < img->ysize; row++)
          {
            for (col = 0; col < width; col++)
              {
                c = ~GetValUnchecked(img, col, row);
                unsigned char cc;
                cc = c / 256;
                fwrite(&cc, 1, 1, ofp);
                cc = c % 256;
                fwrite(&cc, 1, 1, ofp);
              }
          }
      }
    else
      {
        for (row = 0; row < img->ysize; row++)
          {
            for (col = 0; col < width; col++)
              {
                unsigned char cc = ~GetValUnchecked(img, col, row);
                fwrite(&cc, 1, 1, ofp);
              }
          }
      }

    fclose(ofp);
    return OK;
  }

  int WritePSDImg(const Image& imgr, const Image& imgg, const Image& imgb,
                  const std::string& filename)
  {
    char head[] =
    {
      '8', 'B', 'P', 'S',   // signature
      0, 1,                       // version
      0, 0, 0, 0, 0, 0,   // reserved
      0, 3,     // number of channels
      0, 0, 0, 0,     // height
      0, 0, 0, 0,     // width
      0, 16,      // bit per value
      0, 3,     // mode (3=rgb)
      0, 0, 0, 0,     // color mode data (none)
      0, 0, 0, 0,     // image resources (none)
      0, 0, 0, 0,     // layer/mask info (none)
      0, 0        // no compression
    };

    int row, col;
    unsigned int c;

    int width, height;

    RETURN_ERROR_IF_FAILED(MatchImg(imgr, imgg, imgb, width, height));

    int maxval = imgr->maxval;

    if (imgg->maxval > maxval)
      {
        maxval = imgg->maxval;
      }

    if (imgb->maxval > maxval)
      {
        maxval = imgb->maxval;
      }

    FILE* ofp = fopen(filename.c_str(), FWMODUS);

    head[16] = height / 256;
    head[17] = height % 256;
    head[20] = width / 256;
    head[21] = width % 256;

    if (maxval < 256)
      {
        head[23] = 8;
      }
    else if (maxval < 256 * 256)
      {
        head[23] = 16;
      }
    else
      {
        throw IceException(FNAME, M_HIGHRANGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    fwrite(head, 40, 1, ofp);

    if (head[23] == 16)
      {
        for (row = 0; row < height; row++)
          {
            for (col = 0; col < width; col++)
              {
                c = ~GetVal(imgr, col, row);
                unsigned char cc;
                cc = c / 256;
                fwrite(&cc, 1, 1, ofp);
                cc = c % 256;
                fwrite(&cc, 1, 1, ofp);
              }
          }

        for (row = 0; row < height; row++)
          {
            for (col = 0; col < width; col++)
              {
                c = ~GetVal(imgg, col, row);
                unsigned char cc;
                cc = c / 256;
                fwrite(&cc, 1, 1, ofp);
                cc = c % 256;
                fwrite(&cc, 1, 1, ofp);
              }
          }

        for (row = 0; row < height; row++)
          {
            for (col = 0; col < width; col++)
              {
                c = ~GetVal(imgb, col, row);
                unsigned char cc;
                cc = c / 256;
                fwrite(&cc, 1, 1, ofp);
                cc = c % 256;
                fwrite(&cc, 1, 1, ofp);
              }
          }
      }
    else
      {
        for (row = 0; row < height; row++)
          {
            for (col = 0; col < width; col++)
              {
                unsigned char cc = ~GetVal(imgr, col, row);
                fwrite(&cc, 1, 1, ofp);
              }
          }

        for (row = 0; row < height; row++)
          {
            for (col = 0; col < width; col++)
              {
                unsigned char cc = ~GetVal(imgg, col, row);
                fwrite(&cc, 1, 1, ofp);
              }
          }

        for (row = 0; row < height; row++)
          {
            for (col = 0; col < width; col++)
              {
                unsigned char cc = ~GetVal(imgb, col, row);
                fwrite(&cc, 1, 1, ofp);
              }
          }
      }

    fclose(ofp);
    return OK;
  }
#undef FNAME

  unsigned char psd_readc(FILE* fd)
  {
    unsigned char c;
    fread(&c, 1, 1, fd);
    return c;
  }

  unsigned short int psd_reads(FILE* fd)
  {
    int c1 = psd_readc(fd);
    int c2 = psd_readc(fd);
    return (c1 << 8) + c2;
  }

  unsigned int psd_read(FILE* fd)
  {
    int i1 = psd_reads(fd);
    int i2 = psd_reads(fd);
//  Printf("read %d \n",(i1<<16)+i2);
    return (i1 << 16) + i2;
  }

#define FNAME "InfImgFile"

  static bool read_header(FILE* fd, int& width, int& height, int& bits, int& ch)
  {
    if (psd_readc(fd) != '8')
      {
        return false;  // 4 char=signature
      }

    if (psd_readc(fd) != 'B')
      {
        return false;
      }

    if (psd_readc(fd) != 'P')
      {
        return false;
      }

    if (psd_readc(fd) != 'S')
      {
        return false;
      }

    if (psd_reads(fd) != 1)
      {
        return false;  // version==1
      }

    for (int i = 0; i < 6; i++)
      {
        psd_readc(fd);  //ignore reserved
      }

    ch = psd_reads(fd); // channels

    if ((ch != 1) && (ch != 3))
      {
        return false;  // one or three channels
      }

    height = psd_read(fd);
    width = psd_read(fd);
    bits = psd_reads(fd);

    if ((bits != 16) && (bits != 8))
      {
        return false;
      }

    if (psd_reads(fd) != ch)
      {
        return false;  // mode==channel for mode 1, 3
      }

    int skip = psd_read(fd); // color mode data

    for (int i = 0; i < skip; i++)
      {
        psd_readc(fd);
      }

    skip = psd_read(fd); // image resources

    for (int i = 0; i < skip; i++)
      {
        psd_readc(fd);
      }

    skip = psd_read(fd); // layer/mask info

    for (int i = 0; i < skip; i++)
      {
        psd_readc(fd);
      }

    if (psd_reads(fd) != 0)
      {
        return false;  // !no compression
      }

    return true;
  }

  int InfPSDFile(const std::string& filename,
                 int& xsize, int& ysize,
                 int& maxval, int& nr)
  {
    FILE* fd = fopen(filename.c_str(), FRMODUS);

    if ((fd = fopen(filename.c_str(), FRMODUS)) == NULL)
      {
        throw IceException(FNAME, M_FILE_OPEN, FILE_NOT_FOUND);
        return FILE_NOT_FOUND;
      }

    int bits;

    if (!read_header(fd, xsize, ysize, bits, nr))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
        fclose(fd);
        return WRONG_FILE;
      }

    if (bits == 8)
      {
        maxval = 255;  // only 8 or 16 bit supported
      }
    else
      {
        maxval = 256 * 256 - 1;
      }

    fclose(fd);
    return OK;
  }
#undef FNAME
#define FNAME "ReadImg"
  Image ReadPSDImg(const std::string& fname, Image& img, int flag)
  {
    FILE*     fd;

    unsigned char* pic;
    ibuffer ib;
    int xs, ys, bits, ch;

    if ((fd = fopen(fname.c_str(), FRMODUS)) == NULL)
      {
        throw IceException(FNAME, M_FILE_OPEN, FILE_NOT_FOUND);
        return Image();
      }

    if (!read_header(fd, xs, ys, bits, ch))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
        fclose(fd);
        return Image();
      }

    int bsize = (bits / 8) * ch * xs * ys;
    pic = (unsigned char*)malloc(bsize);

    if (pic == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        fclose(fd);
        return Image();
      }

    fread(pic, bsize, 1, fd);

    ib.width = xs;
    ib.height = ys;
    ib.valuesize = (bits / 8);
    ib.maxval = (1 << bits) - 1;
    ib.planes = ch;
    ib.linelength = xs * (bits / 8);
    ib.byteorder = IB_MSB_FIRST;
    ib.intensity = true;
    ib.can_delete = true;
    ib.packmethod = IB_RGB_PLANES; // wird nur bei Farbbildern verwendet
    ib.data = pic;

    if (!IsImg(img))
      {
        img = NewImg(ib.width, ib.height, ib.maxval);
      }

    return Buffer2Image(ib, img, flag);
  }

  int ReadPSDImg(const std::string& fname, Image& imgr, Image& imgg, Image& imgb,
                 int flag)
  {
    FILE*     fd;

    unsigned char* pic;
    ibuffer ib;
    int xs, ys, bits, ch;

    if ((fd = fopen(fname.c_str(), FRMODUS)) == NULL)
      {
        throw IceException(FNAME, M_FILE_OPEN, FILE_NOT_FOUND);
        return FILE_NOT_FOUND;
      }

    if (!read_header(fd, xs, ys, bits, ch))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
        fclose(fd);
        return WRONG_FILE;
      }

    int bsize = (bits / 8) * ch * xs * ys;
    pic = (unsigned char*)malloc(bsize);

    if (pic == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        fclose(fd);
        return NO_MEM;
      }

    fread(pic, bsize, 1, fd);

    ib.width = xs;
    ib.height = ys;
    ib.valuesize = (bits / 8);
    ib.maxval = (1 << bits) - 1;
    ib.planes = ch;
    ib.linelength = xs * (bits / 8);
    ib.byteorder = IB_MSB_FIRST;
    ib.intensity = true;
    ib.can_delete = true;
    ib.packmethod = IB_RGB_PLANES; // wird nur bei Farbbildern verwendet
    ib.data = pic;

    if (!IsImg(imgr))
      {
        imgr = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(imgg))
      {
        imgg = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(imgb))
      {
        imgb = NewImg(ib.width, ib.height, ib.maxval);
      }

    return Buffer2Image(ib, imgr, imgg, imgb, flag);
  }

}
