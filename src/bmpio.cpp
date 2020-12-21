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
#include "bmpio.h"
/*-----------------------------------------------------------
  Lesen und schreiben von BMP-Files (Windows, OS/2)
  -----------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "exfile.h"
#include "picio.h"
#include "ibuffer.h"

using namespace std;

namespace ice
{
#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2

#define WIN_OS2_OLD 12
#define WIN_NEW     40
#define OS2_NEW     64

#define u_int unsigned int

  static int   loadBMP1(FILE*, unsigned char*, u_int, u_int);
  static int   loadBMP4(FILE*, unsigned char*, u_int, u_int, u_int);
  static int   loadBMP8(FILE*, unsigned char*, u_int, u_int, u_int);
  static int   loadBMP24(FILE*, unsigned char*, u_int, u_int);

  static u_int getshort(FILE*);
  static u_int getint(FILE*);

  static void  putshort(FILE*, int);
  static void  putint(FILE*, int);

  static void  writeBMP1(FILE*, unsigned char*, int, int);
  static void  writeBMP4(FILE*, unsigned char*, int, int);
  static void  writeBMP8(FILE*, unsigned char*, int, int);
  static void  writeBMP24(FILE*, unsigned char*, int, int);

#define FERROR(fp) (ferror(fp) || feof(fp))

  struct BMPInfo
  {
    unsigned int fSize, fOffBits, iSize, iWidth, iHeight, iPlanes;
    unsigned int iBitCount, iCompression, iSizeImage, iXPelsPerMeter;
    unsigned int iYPelsPerMeter, iColorsUsed, iClrImportant;
  };

  static std::string Error;
  static unsigned char r[256], g[256], b[256]; // color table

  int ReadBMPHeader(FILE* fp, BMPInfo& bmi)
  {
    /* read the file type (first two bytes) */
    int c, c1;
    c = getc(fp);
    c1 = getc(fp);

    if (c != 'B' || c1 != 'M')
      {
        Error = "file type != 'BM'";
        return true;
      }

    bmi.fSize = getint(fp);
    getshort(fp);         /* reserved and ignored */
    getshort(fp);

    bmi.fOffBits = getint(fp);
    bmi.iSize = getint(fp);

    bmi.iWidth         = getint(fp);
    bmi.iHeight        = getint(fp);
    bmi.iPlanes        = getshort(fp);
    bmi.iBitCount      = getshort(fp);
    bmi.iCompression   = getint(fp);
    bmi.iSizeImage     = getint(fp);
    bmi.iXPelsPerMeter = getint(fp);
    bmi.iYPelsPerMeter = getint(fp);
    bmi.iColorsUsed    = getint(fp);
    bmi.iClrImportant  = getint(fp);

    if (FERROR(fp))
      {
        fclose(fp);
        Error = "EOF reached in file header";
        return true;
      }

    /* error checking */
    if ((bmi.iBitCount != 1 && bmi.iBitCount != 4 &&
         bmi.iBitCount != 8 && bmi.iBitCount != 24) ||
        bmi.iPlanes != 1 || bmi.iCompression > BI_RLE4)
      {
        Error = string("Bogus BMP File! (bitCount=") + to_string(bmi.iBitCount) +
                string(", Planes=") + to_string(bmi.iPlanes) +
                string(", Compression=") + to_string(bmi.iCompression) + string(")");
        return true;
      }

    if (((bmi.iBitCount == 1 || bmi.iBitCount == 24) && bmi.iCompression != BI_RGB) ||
        (bmi.iBitCount == 4 && bmi.iCompression == BI_RLE8) ||
        (bmi.iBitCount == 8 && bmi.iCompression == BI_RLE4))
      {

        Error = string("Bogus BMP File! (bitCount=") + string(bmi.iBitCount, 0) +
                string(", Compression=") + string(bmi.iCompression, 0) + string(")");
        return true;
      }

    return false;
  }

  int LoadCMap(FILE* fp,  BMPInfo bmi)
  {
    int i, c;
    int bPad = 0;

    if (bmi.iSize != WIN_OS2_OLD)
      {
        /* skip ahead to colormap, using biSize */
        c = bmi.iSize - 40;    /* 40 bytes read from biSize to biClrImportant */

        for (i = 0; i < c; i++)
          {
            getc(fp);
          }

        bPad = bmi.fOffBits - (bmi.iSize + 14);
      }

    /* load up colormap, if any */
    if (bmi.iBitCount != 24)
      {

        int cmaplen;
        cmaplen = (bmi.iColorsUsed) ? bmi.iColorsUsed : 1 << bmi.iBitCount;

        for (i = 0; i < cmaplen; i++)
          {
            b[i] = getc(fp);
            g[i] = getc(fp);
            r[i] = getc(fp);

            if (bmi.iSize != WIN_OS2_OLD)
              {
                getc(fp);
                bPad -= 4;
              }
          }

        if (FERROR(fp))
          {
            return false;
          }
      }

    if (bmi.iSize != WIN_OS2_OLD)
      {
        /* Waste any unused bytes between the colour map (if present)
           and the start of the actual bitmap data. */

        while (bPad > 0)
          {
            getc(fp);
            bPad--;
          }
      }

    return true;
  }

#define FNAME "InfBMPFile"
  int InfBMPFile(const string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr)
  {
    FILE* fp;
    BMPInfo bmi;
    string hname = fname;

    if ((fp = fopen(hname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    if (ReadBMPHeader(fp, bmi))
      {
        fclose(fp);
        throw IceException(FNAME, Error);
      }
    fclose(fp);

    xsize = bmi.iWidth;
    ysize = bmi.iHeight;
    maxval = 255; /* max. value in Colormap */

    if (bmi.iBitCount == 24)
      {
        nr = 3;
      }
    else
      {
        nr = 1;
      }

    return BMP;
  }

#undef FNAME
#define FNAME "ReadBMPImg"
  /*******************************************/
  Image ReadBMPImg(const string& fname, Image& img, int flag)
  /*******************************************/
  {
    FILE*         fp;
    unsigned int i;
    unsigned int rv = 1;
    unsigned int cmaplen;
    BMPInfo bmi;
    ibuffer ib;

    string hname = fname;

    if ((fp = fopen(hname.c_str(), FRMODUS)) == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    if (ReadBMPHeader(fp, bmi))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    if (!LoadCMap(fp, bmi))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    if (bmi.iBitCount != 24)
      {
        // convert ColorMap to Gray (if available)
        cmaplen = (bmi.iColorsUsed) ? bmi.iColorsUsed : 1 << bmi.iBitCount;

        for (unsigned int i = 0; i < cmaplen; ++i)
          {
            r[i] = GRAYVALUE(r[i], g[i], b[i]);
          }
      }




    ib.width = bmi.iWidth;
    ib.height = bmi.iHeight;
    ib.valuesize = 1;
    ib.byteorder = IB_LSB_FIRST;
    ib.intensity = true;
    ib.packmethod = IB_RGB; // wird nur bei Farbbildern verwendet

    if (bmi.iBitCount == 24)
      {
        ib.alloc(bmi.iWidth * bmi.iHeight * 3);
      }
    else
      {
        ib.alloc(bmi.iWidth * bmi.iHeight);
      }

    unsigned char* const data = ib.getData();

    /* load up the image */
    switch (bmi.iBitCount)
      {
      case 1:
        rv = loadBMP1(fp, data, bmi.iWidth, bmi.iHeight);
        ib.maxval = 1;
        ib.planes = 1;
        break;
      case 4:
        rv = loadBMP4(fp, data, bmi.iWidth, bmi.iHeight, bmi.iCompression);
        ib.maxval = 15;
        ib.planes = 1;
        break;
      case 8:
        rv = loadBMP8(fp, data, bmi.iWidth, bmi.iHeight, bmi.iCompression);
        ib.maxval = 255;
        ib.planes = 1;
        break;
      case 24:
        rv = loadBMP24(fp, data, bmi.iWidth, bmi.iHeight);
        ib.maxval = 255;
        ib.planes = 3;
        break;
      }

    ib.linelength = ib.width * ib.planes;

    fclose(fp);

    if (bmi.iBitCount != 24)
      {
        for (i = 0; i < bmi.iWidth * bmi.iHeight; i++)
          {
            data[i] = r[data[i]];
          }
      }

    if (rv)
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (!IsImg(img))
      {
        img = NewImg(ib.width, ib.height, ib.maxval);
      }

    return Buffer2Image(ib, img, flag);

  }
  /*******************************************/
  int ReadBMPImg(const string& fname,
                 Image& red, Image& green, Image& blue,
                 int flag)
  {
    FILE*         fp;
    int          rv = 1;
    ibuffer ib;

    BMPInfo bmi;

    string hname = fname;

    if ((fp = fopen(hname.c_str(), FRMODUS)) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    if (ReadBMPHeader(fp, bmi))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    if (!LoadCMap(fp, bmi))
      {
        throw IceException(FNAME, M_WRONG_FILETYPE);
      }

    ib.width = bmi.iWidth;
    ib.height = bmi.iHeight;
    ib.valuesize = 1;
    ib.byteorder = IB_LSB_FIRST;
    ib.intensity = true;
    ib.packmethod = IB_RGB; // wird nur bei Farbbildern verwendet

    if (bmi.iBitCount == 24)
      {
        ib.alloc(bmi.iWidth * bmi.iHeight * 3);
      }
    else
      {
        ib.alloc(bmi.iWidth * bmi.iHeight);
      }
    unsigned char* data = ib.getData();

    /* load up the image */
    switch (bmi.iBitCount)
      {
      case 1:
        rv = loadBMP1(fp, data, bmi.iWidth, bmi.iHeight);
        ib.maxval = 1;
        ib.planes = 1;
        break;
      case 4:
        rv = loadBMP4(fp, data, bmi.iWidth, bmi.iHeight, bmi.iCompression);
        ib.maxval = 15;
        ib.planes = 1;
        break;
      case 8:
        rv = loadBMP8(fp, data, bmi.iWidth, bmi.iHeight, bmi.iCompression);
        ib.maxval = 255;
        ib.planes = 1;
        break;
      case 24:
        rv = loadBMP24(fp, data, bmi.iWidth, bmi.iHeight);
        ib.maxval = 255;
        ib.planes = 3;
        break;
      }

    ib.linelength = ib.width * ib.planes;

    fclose(fp);

    if (rv)
      throw IceException(FNAME, M_WRONG_FILETYPE);

    if (!IsImg(red))
      {
        red = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(green))
      {
        green = NewImg(ib.width, ib.height, ib.maxval);
      }
    if (!IsImg(blue))
      {
        blue = NewImg(ib.width, ib.height, ib.maxval);
      }

    Buffer2Image(ib, red, green, blue, flag);
    return OK;
  }

  /*******************************************/
  static int loadBMP1(FILE* fp, unsigned char* pic8, u_int w, u_int h)
  {
    int  i, j, c, bitnum, padw;
    unsigned char* pp;

    c = 0;
    padw = ((w + 31) / 32) * 32; /* 'w', padded to be a multiple of 32 */

    for (i = h - 1; i >= 0; i--)
      {
        pp = pic8 + (i * w);

        for (j = bitnum = 0; j < padw; j++, bitnum++)
          {
            if ((bitnum & 7) == 0)   /* read the next byte */
              {
                c = getc(fp);
                bitnum = 0;
              }

            if (j < (int)w)
              {
                *pp++ = (c & 0x80) ? 1 : 0;
                c <<= 1;
              }
          }

        if (FERROR(fp))
          {
            break;
          }
      }

    return (FERROR(fp));
  }

  /*******************************************/
  static int loadBMP4(FILE* fp, unsigned char* pic8, u_int w, u_int h, u_int comp)
  {
    int   i, j, c, c1, x, y, nybnum, padw, rv;
    unsigned char* pp;

    rv = 0;
    c = c1 = 0;

    if (comp == BI_RGB)     /* read uncompressed data */
      {
        padw = ((w + 7) / 8) * 8; /* 'w' padded to a multiple of 8pix (32 bits) */

        for (i = h - 1; i >= 0; i--)
          {
            pp = pic8 + (i * w);

            for (j = nybnum = 0; j < padw; j++, nybnum++)
              {
                if ((nybnum & 1) == 0)   /* read next byte */
                  {
                    c = getc(fp);
                    nybnum = 0;
                  }

                if (j < (int)w)
                  {
                    *pp++ = (c & 0xf0) >> 4;
                    c <<= 4;
                  }
              }

            if (FERROR(fp))
              {
                break;
              }
          }
      }

    else if (comp == BI_RLE4)    /* read RLE4 compressed data */
      {
        x = y = 0;
        pp = pic8 + x + (h - y - 1) * w;

        while (y < (int)h)
          {
            c = getc(fp);

            if (c == EOF)
              {
                rv = 1;
                break;
              }

            if (c)                                     /* encoded mode */
              {
                c1 = getc(fp);

                for (i = 0; i < c; i++, x++, pp++)
                  {
                    *pp = (i & 1) ? (c1 & 0x0f) : ((c1 >> 4) & 0x0f);
                  }
              }

            else      /* c==0x00  :  escape codes */
              {
                c = getc(fp);

                if (c == EOF)
                  {
                    rv = 1;
                    break;
                  }

                if (c == 0x00)                           /* end of line */
                  {
                    x = 0;
                    y++;
                    pp = pic8 + x + (h - y - 1) * w;
                  }

                else if (c == 0x01)
                  {
                    break;  /* end of pic8 */
                  }

                else if (c == 0x02)                      /* delta */
                  {
                    c = getc(fp);
                    x += c;
                    c = getc(fp);
                    y += c;
                    pp = pic8 + x + (h - y - 1) * w;
                  }

                else                                     /* absolute mode */
                  {
                    for (i = 0; i < c; i++, x++, pp++)
                      {
                        if ((i & 1) == 0)
                          {
                            c1 = getc(fp);
                          }

                        *pp = (i & 1) ? (c1 & 0x0f) : ((c1 >> 4) & 0x0f);
                      }

                    if (((c & 3) == 1) || ((c & 3) == 2))
                      {
                        getc(fp);  /* read pad byte */
                      }
                  }
              }  /* escape processing */

            if (FERROR(fp))
              {
                break;
              }
          }  /* while */
      }

    else
      {
        fprintf(stderr, "unknown BMP compression type 0x%0x\n", comp);
      }

    if (FERROR(fp))
      {
        rv = 1;
      }

    return rv;
  }

  /*******************************************/
  static int loadBMP8(FILE* fp, unsigned char* pic8, u_int w, u_int h, u_int comp)
  {
    int i, j, c1, padw, x, y, rv;
    int c;
    unsigned char* pp;

    rv = false;

    if (comp == BI_RGB)     /* read uncompressed data */
      {
        padw = ((w + 3) / 4) * 4; /* 'w' padded to a multiple of 4pix (32 bits) */

        for (i = h - 1; i >= 0; i--)
          {
            pp = pic8 + (i * w);

            for (j = 0; j < padw; j++)
              {
                c = getc(fp);

                if (c == EOF)
                  {
                    rv = true;
                  }

                if (j < (int) w)
                  {
                    *pp++ = c;
                  }
              }

            if (FERROR(fp))
              {
                break;
              }
          }
      }

    else if (comp == BI_RLE8)    /* read RLE8 compressed data */
      {
        x = y = 0;
        pp = pic8 + x + (h - y - 1) * w;

        while (y < (int)h)
          {
            c = getc(fp);

            if (c == EOF)
              {
                rv = 1;
                break;
              }

            if (c)                                     /* encoded mode */
              {
                c1 = getc(fp);

                for (i = 0; i < c; i++, x++, pp++)
                  {
                    *pp = c1;
                  }
              }

            else      /* c==0x00  :  escape codes */
              {
                c = getc(fp);

                if (c == EOF)
                  {
                    rv = 1;
                    break;
                  }

                if (c == 0x00)                           /* end of line */
                  {
                    x = 0;
                    y++;
                    pp = pic8 + x + (h - y - 1) * w;
                  }

                else if (c == 0x01)
                  {
                    break;  /* end of pic8 */
                  }

                else if (c == 0x02)                      /* delta */
                  {
                    c = getc(fp);
                    x += c;
                    c = getc(fp);
                    y += c;
                    pp = pic8 + x + (h - y - 1) * w;
                  }

                else                                     /* absolute mode */
                  {
                    for (i = 0; i < c; i++, x++, pp++)
                      {
                        c1 = getc(fp);
                        *pp = c1;
                      }

                    if (c & 1)
                      {
                        getc(fp);  /* odd length run: read an extra pad byte */
                      }
                  }
              }  /* escape processing */

            if (FERROR(fp))
              {
                break;
              }
          }  /* while */
      }

    else
      {
        fprintf(stderr, "unknown BMP compression type 0x%0x\n", comp);
      }

    if (FERROR(fp))
      {
        rv = true;
      }

    return rv;
  }

  /*******************************************/
  static int loadBMP24(FILE* fp, unsigned char* pic24, u_int w, u_int h)
  {
    int   i, j, padb, rv;
    unsigned char* pp;

    rv = 0;

    padb = (4 - ((w * 3) % 4)) & 0x03; /* # of pad bytes to read at EOscanline */

    for (i = h - 1; i >= 0; i--)
      {
        pp = pic24 + (i * w * 3);

        for (j = 0; j < (int)w; j++)
          {
            pp[2] = getc(fp);   /* blue */
            pp[1] = getc(fp);   /* green */
            pp[0] = getc(fp);   /* red */
            pp += 3;
          }

        for (j = 0; j < padb; j++)
          {
            getc(fp);
          }

        rv = (FERROR(fp));

        if (rv)
          {
            break;
          }
      }

    return rv;
  }

  /*******************************************/
  static unsigned int getshort(FILE* fp)
  {
    int c, c1;
    c = getc(fp);
    c1 = getc(fp);
    return ((unsigned int) c) + (((unsigned int) c1) << 8);
  }

  /*******************************************/
  static unsigned int getint(FILE* fp)
  {
    int c, c1, c2, c3;
    c = getc(fp);
    c1 = getc(fp);
    c2 = getc(fp);
    c3 = getc(fp);
    return ((unsigned int) c) +
           (((unsigned int) c1) << 8) +
           (((unsigned int) c2) << 16) +
           (((unsigned int) c3) << 24);
  }

  /*******************************************/
  static void putshort(FILE* fp, int i)
  {
    int c, c1;

    c = ((unsigned int) i) & 0xff;
    c1 = (((unsigned int) i) >> 8) & 0xff;
    putc(c, fp);
    putc(c1, fp);
  }

  /*******************************************/
  static void putint(FILE* fp, int i)
  {
    int c, c1, c2, c3;
    c  = ((unsigned int) i)      & 0xff;
    c1 = (((unsigned int) i) >> 8)  & 0xff;
    c2 = (((unsigned int) i) >> 16) & 0xff;
    c3 = (((unsigned int) i) >> 24) & 0xff;

    putc(c, fp);
    putc(c1, fp);
    putc(c2, fp);
    putc(c3, fp);
  }

#undef FNAME

  /*******************************************/
#define PIC8 0
#define PIC24 1

#define F_GREYSCALE 10
#define F_REDUCED 11
#define F_FULLCOLOR 12

#define FNAME "WriteBMPImg"
  int WriteBMPImg(const Image& ir, const Image& ig, const Image& ib,
                  const string& fname)
  {
    int i, j, bperlin;
    int w, h;
    unsigned char* pp;
    unsigned char* rgb;
    string hname = fname;
    FILE* fp;

    if (!IsImg(ir) || !IsImg(ig) || !IsImg(ib))
      throw IceException(FNAME, M_WRONG_IMAGE);

    RETURN_ERROR_IF_FAILED(MatchImg(ir, ig, ib, w, h));

    bperlin = ((w * 24 + 31) / 32) * 4;   /* # bytes written per line */

    if ((fp = fopen(hname.c_str(), FWMODUS)) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    putc('B', fp);
    putc('M', fp);           /* BMP file magic number */

    /* compute filesize and write it */
    i = 14 +                /* size of bitmap file header */
        40 +                /* size of bitmap info header */
        bperlin * h;        /* size of image data */

    putint(fp, i);
    putshort(fp, 0);        /* reserved1 */
    putshort(fp, 0);        /* reserved2 */
    putint(fp, 14 + 40);    /* offset from BOfile to BObitmap */

    putint(fp, 40);         /* biSize: size of bitmap info header */
    putint(fp, w);          /* biWidth */
    putint(fp, h);          /* biHeight */
    putshort(fp, 1);        /* biPlanes:  must be '1' */
    putshort(fp, 24);       /* biBitCount: 1,4,8, or 24 */
    putint(fp, BI_RGB);     /* biCompression:  BI_RGB, BI_RLE8 or BI_RLE4 */
    putint(fp, bperlin * h);/* biSizeImage:  size of raw image data */
    putint(fp, 75 * 39);    /* biXPelsPerMeter: (75dpi * 39 inch per meter) */
    putint(fp, 75 * 39);    /* biYPelsPerMeter: (75dpi * 39 inc per meter) */
    putint(fp, 0);          /* biColorsUsed: # of colors used in cmap */
    putint(fp, 0);          /* biClrImportant: same as above */

    rgb = (unsigned char*)malloc(h * w * 3);

    if (rgb == NULL)
      {
        fclose(fp);
        throw IceException(FNAME, M_NO_MEM);
      }

    for (j = 0; j < h; j++)
      {
        pp = rgb + j * w * 3;

        for (i = 0; i < w; i++)
          {
            *(pp++) = (GetVal(ir, i, j) * 255 / ir.maxval) ^ 255;
            *(pp++) = (GetVal(ig, i, j) * 255 / ig.maxval) ^ 255;
            *(pp++) = (GetVal(ib, i, j) * 255 / ib.maxval) ^ 255;
          }
      }

    /* write out the image */
    writeBMP24(fp, rgb, w, h);
    free(rgb);

    if (FERROR(fp))
      {
        fclose(fp);
        throw IceException(FNAME, M_WRONG_WRITE);
      }

    fclose(fp);
    return OK;
  }

  int WriteBMPImg(const Image& img, const string& fname)
  {
    unsigned int i, j, nc, nbits, bperlin;
    unsigned int w, h;
    unsigned char* pic, *dd;
    unsigned int graymax;
    string hname = fname;
    FILE* fp;

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    w = img->xsize;
    h = img->ysize;
    graymax = img.maxval;

    if ((fp = fopen(hname.c_str(), FWMODUS)) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    nc = graymax + 1;

    if (graymax < 2)
      {
        nbits = 1;
      }
    else if (graymax < 16)
      {
        nbits = 4;
      }
    else
      {
        nbits = 8;
      }

    if (nc > 256)
      {
        nc = 256;
      }

    bperlin = ((w * nbits + 31) / 32) * 4;   /* # bytes written per line */

    putc('B', fp);
    putc('M', fp);           /* BMP file magic number */

    /* compute filesize and write it */
    i = 14 +                /* size of bitmap file header */
        40 +                /* size of bitmap info header */
        (nc * 4) +          /* size of colormap */
        bperlin * h;        /* size of image data */

    putint(fp, i);
    putshort(fp, 0);        /* reserved1 */
    putshort(fp, 0);        /* reserved2 */
    putint(fp, 14 + 40 + (nc * 4));  /* offset from BOfile to BObitmap */

    putint(fp, 40);         /* biSize: size of bitmap info header */
    putint(fp, w);          /* biWidth */
    putint(fp, h);          /* biHeight */
    putshort(fp, 1);        /* biPlanes:  must be '1' */
    putshort(fp, nbits);    /* biBitCount: 1,4,8, or 24 */
    putint(fp, BI_RGB);     /* biCompression:  BI_RGB, BI_RLE8 or BI_RLE4 */
    putint(fp, bperlin * h);/* biSizeImage:  size of raw image data */
    putint(fp, 75 * 39);    /* biXPelsPerMeter: (75dpi * 39 inch per meter) */
    putint(fp, 75 * 39);    /* biYPelsPerMeter: (75dpi * 39 inch per meter) */
    putint(fp, nc);         /* biColorsUsed: # of colors used in cmap */
    putint(fp, nc);         /* biClrImportant: same as above */

    /* write out the colormap */
    for (i = 0; i < nc; i++)
      {
        putc((char)(i * 255 / (nc - 1) ^ 255), fp);
        putc((char)(i * 255 / (nc - 1) ^ 255), fp);
        putc((char)(i * 255 / (nc - 1) ^ 255), fp);
        putc(0, fp);
      }

    /* create bitmap */
    pic = (unsigned char*)malloc(h * w);

    if (nc - 1 != graymax)
      {
        for (i = 0; i < h; i++)
          {
            dd = &pic[i * w];

            for (j = 0; j < w; j++)
              {
                *(dd++) = GetVal(img, j, i) * (nc - 1) / graymax;
              }
          }
      }
    else
      {
        for (i = 0; i < h; i++)
          {
            dd = &pic[i * w];

            for (j = 0; j < w; j++)
              {
                *(dd++) = GetVal(img, j, i);
              }
          }
      }

    /* write out the image */
    switch (nbits)
      {
      case 1:
        writeBMP1(fp, pic, w, h);
        break;
      case 4:
        writeBMP4(fp, pic, w, h);
        break;
      case 8:
        writeBMP8(fp, pic, w, h);
        break;
      case 24:
        writeBMP24(fp, pic, w, h);
        break;
      }

    free(pic);

    if (FERROR(fp))
      {
        fclose(fp);
        throw IceException(FNAME, M_WRONG_WRITE);
      }

    fclose(fp);
    return OK;
  }

  /*******************************************/
  static void writeBMP1(FILE* fp, unsigned char* pic8, int w, int h)
  {
    int   i, j, c, bitnum, padw;
    unsigned char* pp;

    padw = ((w + 31) / 32) * 32; /* 'w', padded to be a multiple of 32 */

    for (i = h - 1; i >= 0; i--)
      {
        pp = pic8 + (i * w);

        for (j = bitnum = c = 0; j <= padw; j++, bitnum++)
          {
            if (bitnum == 8)   /* write the next byte */
              {
                putc(c, fp);
                bitnum = c = 0;
              }

            c <<= 1;

            if (j < w)
              {
                c |= (*pp++ & 0x01);
              }
          }
      }
  }

  /*******************************************/
  static void writeBMP4(FILE* fp, unsigned char* pic8, int w, int h)
  {
    int   i, j, c, nybnum, padw;
    unsigned char* pp;

    padw = ((w + 7) / 8) * 8; /* 'w' padded to a multiple of 8pix (32 bits) */

    for (i = h - 1; i >= 0; i--)
      {
        pp = pic8 + (i * w);

        for (j = nybnum = c = 0; j <= padw; j++, nybnum++)
          {
            if (nybnum == 2)   /* write next byte */
              {
                putc((c & 0xff), fp);
                nybnum = c = 0;
              }

            c <<= 4;

            if (j < w)
              {
                c |= *pp++ & 0x0f;
                pp++;
              }
          }
      }
  }

  /*******************************************/
  static void writeBMP8(FILE* fp, unsigned char* pic8, int w, int h)
  {
    int   i, j, padw;
    unsigned char* pp;

    padw = ((w + 3) / 4) * 4; /* 'w' padded to a multiple of 4pix (32 bits) */

    for (i = h - 1; i >= 0; i--)
      {
        pp = pic8 + (i * w);

        for (j = 0; j < w; j++)
          {
            putc(*pp++, fp);
          }

        for (; j < padw; j++)
          {
            putc(0, fp);
          }
      }
  }

  /*******************************************/
  static void writeBMP24(FILE* fp, unsigned char* pic24, int w, int h)
  {
    int   i, j, padb;
    unsigned char* pp;

    padb = (4 - ((w * 3) % 4)) & 0x03; /* # of pad bytes to write at EOscanline */

    for (i = h - 1; i >= 0; i--)
      {
        pp = pic24 + (i * w * 3);

        for (j = 0; j < w; j++)
          {
            putc(pp[2], fp);
            putc(pp[1], fp);
            putc(pp[0], fp);
            pp += 3;
          }

        for (j = 0; j < padb; j++)
          {
            putc(0, fp);
          }
      }
  }
#undef FNAME
}
