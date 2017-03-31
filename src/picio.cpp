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
/************************************************************************/
/* picio.c : Bild E/A                                                   */
/************************************************************************/

#include <stdio.h>

#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif

#include <malloc.h>
#include <string.h>

#include "IceException.h"
#include "macro.h"

#include "util.h"
#include "strtool.h"

#include "picio.h"
#include "tifio.h"
#include "pcxio.h"
#include "jpegio.h"
#include "tgaio.h"
#include "bmpio.h"
#include "pbmio.h"

using namespace std;

namespace ice
{
  unsigned char red[256], green[256], blue[256];

  int ImageFileType(string& filename)
  {
    string ext = "";
    string::size_type pos;
    int type = UNKNOWN_TYP; /* unknown type */

    pos = filename.find(':');

    if (pos != string::npos)
      {
        // for win32 single letter is drive
        if ((pos > 1) && (pos < 5))
          {
            ext = lowercase(Split(filename, ":"));

            if (ext == "tif")
              {
                type = TIF;
              }

            if (ext == "tiff")
              {
                type = TIF;
              }

            if (ext == "jpg")
              {
                type = JPEG;
              }

            if (ext == "jpeg")
              {
                type = JPEG;
              }

            if (ext == "gif")
              {
                type = UNSUPPORTED_TYP;
              }

            if (ext == "pcx")
              {
                type = PCX;
              }

            if (ext == "tga")
              {
                type = TGA;
              }

            if (ext == "bmp")
              {
                type = BMP;
              }

            if (ext == "pgm")
              {
                type = PBM;
              }

            if (ext == "ppm")
              {
                type = PBM;
              }

            if (ext == "pbm")
              {
                type = PBM;
              }

            if (ext == "pnm")
              {
                type = PBM;
              }

            if (ext == "psd")
              {
                type = PSD;
              }

            return type;
          }
      }

    pos = filename.rfind('.');

    if (pos != string::npos)
      {
        ext = lowercase(filename.substr(pos + 1));

        if (ext == "tif")
          {
            type = TIF;
          }

        if (ext == "tiff")
          {
            type = TIF;
          }

        if (ext == "jpg")
          {
            type = JPEG;
          }

        if (ext == "jpeg")
          {
            type = JPEG;
          }

        if (ext == "gif")
          {
            type = UNSUPPORTED_TYP;
          }

        if (ext == "pcx")
          {
            type = PCX;
          }

        if (ext == "tga")
          {
            type = TGA;
          }

        if (ext == "bmp")
          {
            type = BMP;
          }

        if (ext == "pgm")
          {
            type = PBM;
          }

        if (ext == "pnm")
          {
            type = PBM;
          }

        if (ext == "ppm")
          {
            type = PBM;
          }

        if (ext == "psd")
          {
            type = PSD;
          }

        return type;
      }

    return type /*Unknown*/;
  }

  int ImageFileType(string& filename, string& defext)
  {
    int type = ImageFileType(filename);

    defext = "";

    switch (type)
      {
      case TIF:
        defext = "tif";
        break;
      case JPEG:
        defext = "jpg";
        break;
      case PCX:
        defext = "pcx";
        break;
      case TGA:
        defext = "tga";
        break;
      case BMP:
        defext = "bmp";
        break;
      case PBM:
        defext = "ppm";
        break;
      case PSD:
        defext = "psd";
        break;
      }

    return type;
  }

  string ImageTypePrefix(int typ)
  {
    switch (typ)
      {
      case TIF :
        return "TIF:";
      case TGA :
        return "TGA:";
      case JPEG:
        return "JPEG:";
      case PCX :
        return "PCX:";
      case BMP :
        return "BMP:";
      case PBM :
        return "PBM:";
      case PSD :
        return "PSD:";
      }

    return "";
  }

  void bmp8img(unsigned char* bmp, int w, int h, Image& img)
  {
    int x, y;
    int xs, ys;
    int gs = img->maxval + 1;
    xs = img->xsize;

    if (w < xs)
      {
        xs = w;
      }

    ys = img->ysize;

    if (h < ys)
      {
        ys = h;
      }

    for (x = 0; x < xs; x++)
      for (y = 0; y < ys; y++)
        {
          PutVal(img, x, y, bmp[y * w + x]*gs / 256);
        }
  }

  void bmp16img(unsigned short* bmp, int w, int h, Image& img)
  {
    int x, y;
    int xs, ys;
    int gs = img->maxval + 1;
    xs = img->xsize;

    if (w < xs)
      {
        xs = w;
      }

    ys = img->ysize;

    if (h < ys)
      {
        ys = h;
      }

    for (x = 0; x < xs; x++)
      for (y = 0; y < ys; y++)
        {
          PutVal(img, x, y, bmp[y * w + x]*gs / 256 / 256);
        }
  }

  string ReadImgFileComment = "";
  string WriteImgFileComment = "";

#define FNAME "ReadImg"
  Image ReadImg(const string& fname, Image& img, int flag)
  {
    string filename = fname;
    ReadImgFileComment = "";
    int type = ImageFileType(filename);

    switch (type)
      {
      case TIF:
        return ReadTIFImg(filename, img, flag);
      case JPEG:
        return ReadJPEGImg(filename, img, flag);
      case PCX:
        return ReadPCXImg(filename, img, flag);
      case TGA:
        return ReadTGAImg(filename, img, flag);
      case PBM:
        return ReadPBMImg(filename, img, flag);
      case BMP:
        return ReadBMPImg(filename, img, flag);
      case PSD:
        return ReadPSDImg(filename, img, flag);
      case UNSUPPORTED_TYP:
        throw IceException(FNAME, M_UNSUPPORTED_FILE, WRONG_FILE);
        return Image();
      }

    throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
    return Image();
  }

  Image ReadImg(const std::string& fname)
  {
    Image img;
    return ReadImg(fname, img);
  }

  int ReadImg(const string& fname, Image& ir, Image& ig, Image& ib, int flag)
  {
    string filename = fname;
    ReadImgFileComment = "";
    int type = ImageFileType(filename);

    switch (type)
      {
      //    case TIF: return ReadTIFImg(filename,ir,ig,ib);
      case JPEG:
        return ReadJPEGImg(filename, ir, ig, ib, flag);
      //    case PCX: return ReadPCXImg(filename,ir,ig,ib);
      case TGA:
        return ReadTGAImg(filename, ir, ig, ib, flag);
      case BMP:
        return ReadBMPImg(filename, ir, ig, ib, flag);
      case PBM:
        return ReadPBMImg(filename, ir, ig, ib, flag);
      case PSD:
        return ReadPSDImg(filename, ir, ig, ib, flag);
      case TIF:
        return ReadTIFImg(filename, ir, ig, ib, flag);
      case UNSUPPORTED_TYP:
        throw IceException(FNAME, M_UNSUPPORTED_FILE, WRONG_FILE);
        return WRONG_FILE;
      }

    throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
    return WRONG_FILE;
  }
#undef FNAME
  /**********************************************/
#define FNAME "WriteImg"
  int WriteImg(const Image& img, const string& fname)
  {
    string filename = fname;
    int type = ImageFileType(filename);
    int rc = WRONG_FILE;

    switch (type)
      {
      case TIF:
        rc = WriteTIFImg(img, filename);
        break;
      case JPEG:
        rc = WriteJPEGImg(img, filename);
        break;
      case PCX:
        rc = WritePCXImg(img, filename);
        break;
      case BMP:
        rc = WriteBMPImg(img, filename);
        break;
      case PBM:
        rc = WritePBMImg(img, filename);
        break;
      case PSD:
        rc = WritePSDImg(img, filename);
        break;
      case UNSUPPORTED_TYP:
        throw IceException(FNAME, M_UNSUPPORTED_FILE, WRONG_FILE);
      }

    WriteImgFileComment = "";

    if (rc == WRONG_FILE)
      {
        throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
      }

    return rc;
  }

  int WriteImg(const Image& ir, const Image& ig, const Image& ib,
               const string& fname)
  {
    string filename = fname;
    int type = ImageFileType(filename);
    int rc = WRONG_FILE;

    switch (type)
      {
      case JPEG:
        rc = WriteJPEGImg(ir, ig, ib, filename);
        break;
      case TIF:
        rc = WriteTIFImg(ir, ig, ib, filename);
        break;
      case TGA:
        rc = WriteTGAImg(ir, ig, ib, filename);
        break;
      case BMP:
        rc = WriteBMPImg(ir, ig, ib, filename);
        break;
      case PBM:
        rc = WritePBMImg(ir, ig, ib, filename);
        break;
      case PSD:
        rc = WritePSDImg(ir, ig, ib, filename);
        break;
      case UNSUPPORTED_TYP:
        throw IceException(FNAME, M_UNSUPPORTED_FILE, WRONG_FILE);
        break;
      }

    WriteImgFileComment = "";

    if (rc == WRONG_FILE)
      {
        throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
      }

    return rc;
  }
#undef FNAME
#define FNAME "InfImgFile"
  /*----------------------------------------------------------------*/
  int InfImgFile(const string& filename, int& xsize, int& ysize,
                 int& maxval, int& nr)
  {
    string fname = filename;
    int type = ImageFileType(fname);

    switch (type)
      {
      case TIF:
        return InfTIFFile(fname, xsize, ysize, maxval, nr);
      case PCX:
        return InfPCXFile(fname, xsize, ysize, maxval, nr);
      case JPEG:
        return InfJPEGFile(fname, xsize, ysize, maxval, nr);
      case TGA:
        return InfTGAFile(fname, xsize, ysize, maxval, nr);
      case BMP:
        return InfBMPFile(fname, xsize, ysize, maxval, nr);
      case PBM:
        return InfPBMFile(fname, xsize, ysize, maxval, nr);
      case PSD:
        return InfPSDFile(fname, xsize, ysize, maxval, nr);
      case UNSUPPORTED_TYP:
        throw IceException(FNAME, M_UNSUPPORTED_FILE, WRONG_FILE);
        return WRONG_FILE;
      }

    throw IceException(FNAME, M_WRONG_FILETYPE, WRONG_FILE);
    return WRONG_FILE;
  }

  int InfImgFile(const string& fname, int& xsize, int& ysize,
                 int& maxval)
  {
    int idummy;
    return InfImgFile(fname, xsize, ysize, maxval, idummy);
  }
#undef FNAME
}
