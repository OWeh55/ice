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
/************************************************************************/
/* tifio.c : tiff io                                                    */
/************************************************************************/

#include <tiffio.h>
#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "macro.h"
#include "IceException.h"
#include "picio.h"

#include "util.h"
#include "tifio.h"

#define OFFSET 256

using namespace std;

namespace ice
{
#define FNAME "ReadTIFImg"
  Image ReadTIFImg(const string& fname, Image& img, int mode)
  {
    TIFF* MyTiff = TIFFOpen(fname.c_str(), "r");
    if (MyTiff)
      {
        //Setup the Buffers and init
        //everything
        int32_t Width, Height;

        TIFFGetField(MyTiff, TIFFTAG_IMAGEWIDTH, &Width);
        TIFFGetField(MyTiff, TIFFTAG_IMAGELENGTH, &Height);

        if (!img.isValid())
          {
            img.create(Width, Height, 255);
          }

        uint32_t* Buffer = new uint32_t[Width * Height];

        TIFFReadRGBAImage(MyTiff, Width, Height, Buffer, 0);

        TIFFClose(MyTiff);
        int xs = img.xsize;
        if (Width < xs)
          {
            xs = Width;
          }
        int ys = img.ysize;
        if (Height < ys)
          {
            ys = Height;
          }

        for (int y = 0; y < ys; ++y)
          for (int x = 0; x < xs; ++x)
            {
              int idx = y * Width + x;
              uint32_t val = Buffer[idx];
              int gray3 = TIFFGetR(val) + TIFFGetG(val) + TIFFGetB(val);
              img.setPixelUnchecked(x, ys - 1 - y,
                                    img.maxval - MulDiv(gray3, img.maxval, 3 * 256));
            }
        delete [] Buffer;
      }
    else
      throw IceException(FNAME, M_NOT_FOUND);
    return img;
  }

  int ReadTIFImg(const std::string& fname, Image& imgr, Image& imgg, Image& imgb, int flags)
  {
    TIFF* MyTiff = TIFFOpen(fname.c_str(), "r");
    if (MyTiff)
      {
        //Setup the Buffers and init
        //everything
        int32_t Width, Height;

        TIFFGetField(MyTiff, TIFFTAG_IMAGEWIDTH, &Width);
        TIFFGetField(MyTiff, TIFFTAG_IMAGELENGTH, &Height);

        if (!imgr.isValid())
          {
            imgr.create(Width, Height, 255);
          }

        if (!imgg.isValid())
          {
            imgg.create(Width, Height, 255);
          }

        if (!imgb.isValid())
          {
            imgb.create(Width, Height, 255);
          }

        int ys;
        int xs;

        RETURN_ERROR_IF_FAILED(MatchImg(imgr, imgg, imgb, xs, ys));

        uint32_t* Buffer = new uint32_t[Width * Height];

        TIFFReadRGBAImage(MyTiff, Width, Height, Buffer, 0);

        TIFFClose(MyTiff);

        if (Width < xs)
          {
            xs = Width;
          }
        if (Height < ys)
          {
            ys = Height;
          }

        for (int y = 0; y < ys; ++y)
          for (int x = 0; x < xs; ++x)
            {
              int idx = y * Width + x;
              uint32_t val = Buffer[idx];
              imgr.setPixel(x, ys - 1 - y,
                            imgr.maxval - TIFFGetR(val)*imgr.maxval / 256);
              imgg.setPixel(x, ys - 1 - y,
                            imgg.maxval - TIFFGetG(val)*imgg.maxval / 256);
              imgb.setPixel(x, ys - 1 - y,
                            imgb.maxval - TIFFGetB(val)*imgb.maxval / 256);
            }
        delete [] Buffer;
        return OK;
      }
    else
      throw IceException(FNAME, M_NOT_FOUND);
  }
#undef FNAME

#define FNAME "InfTIFFile"
  int InfTIFFile(const string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr)
  {
    TIFF* MyTiff = TIFFOpen(fname.c_str(), "r");
    if (MyTiff)
      {
        int32_t Width, Height;

        TIFFGetField(MyTiff, TIFFTAG_IMAGEWIDTH, &Width);
        TIFFGetField(MyTiff, TIFFTAG_IMAGELENGTH, &Height);

        TIFFClose(MyTiff);

        // at the moment read converts all value to 8 bit
        // therefore maxval is limited to 255

        xsize = Width;
        ysize = Height;
        maxval = 255 ;

        nr = 3; // RGB

        return OK;
      }

    throw IceException(FNAME, M_NOT_FOUND);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "WriteTIFImg"
  int WriteTIFImg(const Image& img, const string& fname)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    TIFF* image;

    // Open the TIFF file
    if ((image = TIFFOpen(fname.c_str(), "w")) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    // We need to set some values for basic tags before we can add any data
    TIFFSetField(image, TIFFTAG_IMAGEWIDTH, img.xsize);
    TIFFSetField(image, TIFFTAG_IMAGELENGTH, img.ysize);
    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, img.ysize);

    // TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
    TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
    TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    TIFFSetField(image, TIFFTAG_XRESOLUTION, 150.0);
    TIFFSetField(image, TIFFTAG_YRESOLUTION, 150.0);
    TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

    uint8_t* buffer = new uint8_t[img.xsize * img.ysize];
    int idx = 0;
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        {
          buffer[idx++] = img.getPixel(x, y) * 255 / img.maxval;
        }

    // Write the information to the file
    TIFFWriteEncodedStrip(image, 0, buffer, img.xsize * img.ysize);
    delete [] buffer;

    // Close the file
    TIFFClose(image);
    return OK;
  }

  int WriteTIFImg(const Image& imgr, const Image& imgg, const Image& imgb,
                  const string& fname)
  {
    if (! imgr.isValid() && imgg.isValid() && imgb.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(imgr, imgg, imgb, xs, ys));

    TIFF* image;

    // Open the TIFF file
    if ((image = TIFFOpen(fname.c_str(), "w")) == NULL)
      throw IceException(FNAME, M_FILE_OPEN);

    // We need to set some values for basic tags before we can add any data
    TIFFSetField(image, TIFFTAG_IMAGEWIDTH, xs);
    TIFFSetField(image, TIFFTAG_IMAGELENGTH, ys);
    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, ys);

    // TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
    TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(image, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    TIFFSetField(image, TIFFTAG_XRESOLUTION, 150.0);
    TIFFSetField(image, TIFFTAG_YRESOLUTION, 150.0);
    TIFFSetField(image, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

    uint8_t* buffer = new uint8_t[xs * ys * 3];
    int idx = 0;
    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          buffer[idx++] = 255 - imgr.getPixel(x, y) * 255 / imgb.maxval;
          buffer[idx++] = 255 - imgg.getPixel(x, y) * 255 / imgg.maxval;
          buffer[idx++] = 255 - imgb.getPixel(x, y) * 255 / imgr.maxval;
        }

    // Write the information to the file
    TIFFWriteEncodedStrip(image, 0, buffer, xs * ys * 3);

    delete [] buffer;

    // Close the file
    TIFFClose(image);
    return OK;
  }

#undef FNAME

}
