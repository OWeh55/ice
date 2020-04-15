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
#include <limits.h>
#include <iostream>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "util.h"
#include "base.h"
#include "ibuffer.h"

namespace ice
{
  inline unsigned int get1(unsigned char* ptr)
  {
    return *ptr;
  }

  inline unsigned int get2l(unsigned char* ptr)
  {
    unsigned int c1 = *ptr;
    unsigned int c2 = *(ptr + 1);
    return c1 + (c2 << 8);
  }

  inline unsigned int get2h(unsigned char* ptr)
  {
    unsigned int c1 = *ptr;
    unsigned int c2 = *(ptr + 1);
    return c2 + (c1 << 8);
  }

  inline unsigned int get4l(unsigned char* ptr)
  {
    unsigned int c1 = *ptr;
    unsigned int c2 = *(ptr + 1);
    unsigned int c3 = *(ptr + 2);
    unsigned int c4 = *(ptr + 3);
    return c1 + (c2 << 8) + (c3 << 16) + (c4 << 24);
  }

  inline unsigned int get4h(unsigned char* ptr)
  {
    unsigned int c1 = *ptr;
    unsigned int c2 = *(ptr + 1);
    unsigned int c3 = *(ptr + 2);
    unsigned int c4 = *(ptr + 3);
    return c1 + (c2 << 8) + (c3 << 16) + (c4 << 24);
  }

  typedef unsigned int valfunc(unsigned char* ptr);

#define FNAME "Buffer2Image"
  Image Buffer2Image(ibuffer& ib, const Image& img, int flags)
  {
    Image himg;
    int xs, ys;
    int x, y;
    unsigned int val = 0, valr = 0, valg = 0, valb = 0;
    unsigned char* hptr, *rptr = nullptr, *gptr = nullptr, *bptr = nullptr;
    int xm, ym;
    bool norm = false;

    int scal = 1; // keine skalierung
    valfunc* valueFunction;

    if (ib.byteorder == IB_LSB_FIRST)
      {
        switch (ib.valuesize)
          {
          case 1:
            valueFunction = get1;
            break;
          case 2:
            valueFunction = get2l;
            break;
          case 4:
            valueFunction = get4l;
            break;
          default:
            throw IceException(FNAME, M_WRONG_PARAMETER);
            break;
          }
      }
    else
      {
        switch (ib.valuesize)
          {
          case 1:
            valueFunction = get1;
            break;
          case 2:
            valueFunction = get2h;
            break;
          case 4:
            valueFunction = get4h;
            break;
          default:
            throw IceException(FNAME, M_WRONG_PARAMETER);
            break;
          }
      }

    int xsb = ib.width;
    int ysb = ib.height; // Bildgroesse im Puffer

    if (IsImg(img))
      {
        xs = img->xsize;
        ys = img->ysize;
        himg = img;
        norm = (img.maxval != ib.maxval);
      }
    else
      {
        himg = NewImg(xsb, ysb, ib.maxval);
        xs = xsb;
        ys = ysb;

        if (!IsImg(himg))
          {
            throw IceException(FNAME, M_NO_MEM);

            if (ib.can_delete)
              {
                free(ib.data);
              }

            return Image();
          }

        norm = false;
      }

    if (flags & IB_SCALE)
      {
        while ((xsb / scal > xs) || (ysb / scal > ys))
          {
            scal++;
          }
      }

    xm = std::min(xs, xsb / scal);
    ym = std::min(ys, ysb / scal);

    if (ib.planes == 1)
      {
        for (y = 0; y < ym; y++)
          {
            hptr = ib.data + (y * scal) * ib.linelength;

            for (x = 0; x < xm; x++)
              {
                unsigned int val = valueFunction(hptr);
                if (val > (unsigned int)ib.maxval)
                  {
                    val -= ib.maxval + 1;
                  }

                if (ib.intensity)
                  {
                    val = ib.maxval - val;
                  }

                if (norm)
                  {
                    PutVal(himg, x, y, MulDiv(val, img.maxval, ib.maxval));
                  }
                else
                  {
                    PutVal(himg, x, y, val);
                  }

                hptr = hptr + scal * ib.valuesize;
              }
          }
      }

    else if (ib.planes == 3)
      {
        for (y = 0; y < ym; y++)
          {
            switch (ib.packmethod)
              {
              case IB_RGB:
                rptr = ib.data + (y * scal) * ib.linelength;
                gptr = rptr + ib.valuesize;
                bptr = gptr + ib.valuesize;
                break;
              case IB_BGR:
                bptr = ib.data + (y * scal) * ib.linelength;
                gptr = bptr + ib.valuesize;
                rptr = gptr + ib.valuesize;
                break;
              case IB_RGB_PLANES:
                rptr = ib.data + (y * scal) * ib.linelength;
                gptr = rptr + ib.height * ib.linelength;
                bptr = gptr + ib.height * ib.linelength;
                break;
              case IB_BGR_PLANES:
                bptr = ib.data + (y * scal) * ib.linelength;
                gptr = bptr + ib.width * ib.linelength;
                rptr = gptr + ib.width * ib.linelength;
                break;
              }

            for (x = 0; x < xm; x++)
              {
                valr = valueFunction(rptr);
                valg = valueFunction(gptr);
                valb = valueFunction(bptr);

                if (ib.intensity)
                  {
                    val = GRAYVALUE(valr, valg, valb);
                    val = ib.maxval - val;
                  }
                else
                  {
                    valr = ib.maxval - valr;
                    valg = ib.maxval - valg;
                    valb = ib.maxval - valb;
                    val = GRAYVALUE(valr, valg, valb);
                    val = ib.maxval - val;
                  }

                if (norm)
                  {
                    PutVal(himg, x, y,
                           MulDiv(val, img.maxval, ib.maxval));
                  }
                else
                  {
                    PutVal(himg, x, y, val);
                  }

                switch (ib.packmethod)
                  {
                  case IB_RGB:
                  case IB_BGR:
                    rptr = rptr + ib.valuesize * 3 * scal;
                    gptr = gptr + ib.valuesize * 3 * scal;
                    bptr = bptr + ib.valuesize * 3 * scal;
                    break;
                  case IB_RGB_PLANES:
                  case IB_BGR_PLANES:
                    rptr = rptr + ib.valuesize * scal;
                    gptr = gptr + ib.valuesize * scal;
                    bptr = bptr + ib.valuesize * scal;
                    break;
                  }
              }
          }
      }

    if (ib.can_delete)
      {
        free(ib.data);
      }

    return himg;
  }

  // RGB
  void Buffer2Image(ibuffer& ib,
                    const Image& imgr, const Image& imgg, const Image& imgb,
                    int flags)
  {
    try
      {
        int xsb = ib.width;
        int ysb = ib.height; // Bildgroesse im Puffer

        int xs, ys, maxval;
        checkImage(imgr, imgg, imgb, xs, ys, maxval);

        bool norm = (maxval != ib.maxval);

        int scal = 1;
        if (flags & IB_SCALE)
          {
            while ((xsb / scal > xs) || (ysb / scal > ys))
              {
                scal++;
              }
          }

        int xm = std::min(xs, xsb / scal);
        int ym = std::min(ys, ysb / scal);

        // spezial: 8Bit RGB
        if (imgr->ImageType() == 1 &&
            ib.valuesize == 1 &&
            scal == 1 &&
            !norm &&
            ib.packmethod == IB_RGB &&
            ib.intensity &&
            ib.planes == 3)
          {
            PixelType1** riptr = (PixelType1**)imgr->getDataPtr();
            PixelType1** giptr = (PixelType1**)imgg->getDataPtr();
            PixelType1** biptr = (PixelType1**)imgb->getDataPtr();
            unsigned char* ptr = nullptr;
            for (int y = 0; y < ym; y++)
              {
                ptr = ib.data + y * ib.linelength;
                for (int x = 0; x < xm; x++)
                  {
                    riptr[y][x] = maxval - *ptr;
                    ptr++;
                    giptr[y][x] = maxval - *ptr;
                    ptr++;
                    biptr[y][x] = maxval - *ptr;
                    ptr++;
                  }
              }
            if (ib.can_delete)
              {
                free(ib.data);
              }
          }
        else
          {
            valfunc* valueFunction;
            if (ib.byteorder == IB_LSB_FIRST)
              {
                switch (ib.valuesize)
                  {
                  case 1:
                    valueFunction = get1;
                    break;
                  case 2:
                    valueFunction = get2l;
                    break;
                  case 4:
                    valueFunction = get4l;
                    break;
                  default:
                    throw IceException(FNAME, M_WRONG_PARAMETER);
                    break;
                  }
              }
            else
              {
                switch (ib.valuesize)
                  {
                  case 1:
                    valueFunction = get1;
                    break;
                  case 2:
                    valueFunction = get2h;
                    break;
                  case 4:
                    valueFunction = get4h;
                    break;
                  default:
                    throw IceException(FNAME, M_WRONG_PARAMETER);
                    break;
                  }
              }

            if (ib.planes == 1)
              {
                // no colorimage in buffer
                if (ib.can_delete)
                  {
                    free(ib.data);
                  }
                throw IceException(FNAME, M_WRONG_FILETYPE);
              }

            if (ib.planes == 3)
              {
                for (int y = 0; y < ym; y++)
                  {
                    unsigned char* rptr = nullptr;
                    unsigned char* gptr = nullptr;
                    unsigned char* bptr = nullptr;
                    switch (ib.packmethod)
                      {
                      case IB_RGB:
                        rptr = ib.data + (y * scal) * ib.linelength;
                        gptr = rptr + ib.valuesize;
                        bptr = gptr + ib.valuesize;
                        break;
                      case IB_BGR:
                        bptr = ib.data + (y * scal) * ib.linelength;
                        gptr = bptr + ib.valuesize;
                        rptr = gptr + ib.valuesize;
                        break;
                      case IB_RGB_PLANES:
                        rptr = ib.data + (y * scal) * ib.linelength;
                        gptr = rptr + ib.height * ib.linelength;
                        bptr = gptr + ib.height * ib.linelength;
                        break;
                      case IB_BGR_PLANES:
                        bptr = ib.data + (y * scal) * ib.linelength;
                        gptr = bptr + ib.width * ib.linelength;
                        rptr = gptr + ib.width * ib.linelength;
                        break;
                      default:
                        throw IceException(FNAME, M_WRONG_PARAMETER);
                      }

                    for (int x = 0; x < xm; x++)
                      {
                        int valr = valueFunction(rptr);
                        int valg = valueFunction(gptr);
                        int valb = valueFunction(bptr);

                        if (ib.intensity)
                          {
                            valr = ib.maxval - valr;
                            valg = ib.maxval - valg;
                            valb = ib.maxval - valb;
                          }

                        if (norm)
                          {
                            valr = MulDiv(valr, imgr.maxval, ib.maxval);
                            valg = MulDiv(valg, imgg.maxval, ib.maxval);
                            valb = MulDiv(valb, imgb.maxval, ib.maxval);
                          }
                        imgr.setPixelLimited(x, y, valr);
                        imgg.setPixelLimited(x, y, valg);
                        imgb.setPixelLimited(x, y, valb);

                        switch (ib.packmethod)
                          {
                          case IB_RGB:
                          case IB_BGR:
                            rptr = rptr + ib.valuesize * 3 * scal;
                            gptr = gptr + ib.valuesize * 3 * scal;
                            bptr = bptr + ib.valuesize * 3 * scal;
                            break;
                          case IB_RGB_PLANES:
                          case IB_BGR_PLANES:
                            rptr = rptr + ib.valuesize * scal;
                            gptr = gptr + ib.valuesize * scal;
                            bptr = bptr + ib.valuesize * scal;
                            break;
                          default:
                            throw IceException(FNAME, M_WRONG_PARAMETER);
                          }
                      }
                  }
              }

            if (ib.can_delete)
              {
                free(ib.data);
              }
          }
      }
    catch (IceException& ex)
      {
        if (ib.can_delete)
          {
            free(ib.data);
          }
        throw IceException(ex, FNAME);
      }
  }

  /*
   * char * -> Image
   */

  template<class T1, class T2>
  static void getline(T1* dst, int width, T2* src, int factor, int offset)
  {
    src += offset;
    T1* end = dst + width;

    while (dst != end)
      {
        *dst = *src;
        src += factor;
        dst++;
      }
  }

  template<class T1, class T2>
  static void getlinei(T1* dst, int width, T2* src, int factor, int offset)
  {
    src += offset;
    T1* end = dst + width;

    while (dst != end)
      {
        *dst = ~(*src);
        src += factor;
        dst++;
      }
  }

  template<class T1, class T2>
  static  void getimg(T1** dst, int width, int height, T2* src,
                      int lineoffset, int factor, int offset, bool intensity)
  {
    for (int i = 0; i < height; i++)
      {
        if (intensity)
          {
            getlinei(dst[i], width, src, factor, offset);
          }
        else
          {
            getline(dst[i], width, src, factor, offset);
          }

        src += lineoffset;
      }
  }

  int Buffer2Image(const unsigned char* buffer,
                   const Image& img,
                   bool intensity,
                   int lineoffset, int factor, int offset)
  {
    if (!IsImg(img))
      {
        // cerr << "isimg" << endl;
        throw IceException(FNAME, M_WRONG_IMAGE);
      }

    int pt = img->ImageType();

    if ((pt < 0) || (pt > 3))
      {
        // cerr << "img-typ" << endl;
        throw IceException(FNAME, M_WRONG_IMAGE);
      }

    int width = img->xsize;
    int height = img->ysize;

    if (lineoffset == 0)
      {
        lineoffset = width * factor;
      }

    switch (pt)
      {
      case 1:
        getimg((PixelType1**)img->getDataPtr(), width, height,
               (PixelType1*) buffer, lineoffset, factor, offset, intensity);
        break;
      case 2:
        getimg((PixelType2**)img->getDataPtr(), width, height,
               (PixelType2*) buffer, lineoffset, factor, offset, intensity);
        break;
      case 3:
        getimg((PixelType2**)img->getDataPtr(), width, height,
               (PixelType2*) buffer, lineoffset, factor, offset, intensity);
        break;
      }

    return OK;
  }

  template<class T>
  static int frombuffer(T** r, T** g, T** b,
                        int width, int height,
                        const unsigned char* buffer,
                        bool intensity,
                        int lineoffset, int packmode)
  {
    int rc = OK;

    switch (packmode)
      {
      case IB_RGB:
        getimg(r, width, height,
               (T*)buffer, lineoffset, 3, 0, intensity);
        getimg(g, width, height,
               (T*)buffer, lineoffset, 3, 1, intensity);
        getimg(b, width, height,
               (T*)buffer, lineoffset, 3, 2, intensity);
        break;
      case IB_BGR:
        getimg(b, width, height,
               (T*)buffer, lineoffset, 3, 0, intensity);
        getimg(g, width, height,
               (T*)buffer, lineoffset, 3, 1, intensity);
        getimg(r, width, height,
               (T*)buffer, lineoffset, 3, 2, intensity);
        break;
      case IB_RGB32:
        getimg(r, width, height,
               (T*)buffer, lineoffset, 4, 0, intensity);
        getimg(g, width, height,
               (T*)buffer, lineoffset, 4, 1, intensity);
        getimg(b, width, height,
               (T*)buffer, lineoffset, 4, 2, intensity);
        break;
      case IB_BGR32:
        getimg(b, width, height,
               (T*)buffer, lineoffset, 4, 0, intensity);
        getimg(g, width, height,
               (T*)buffer, lineoffset, 4, 1, intensity);
        getimg(r, width, height,
               (T*)buffer, lineoffset, 4, 2, intensity);
        break;
      case IB_RGB_PLANES:
        getimg(r, width, height,
               (T*)buffer, lineoffset, 1, 0, intensity);
        getimg(g, width, height,
               (T*)buffer + height * lineoffset, lineoffset, 1, 0, intensity);
        getimg(b, width, height,
               (T*)buffer + 2 * height * lineoffset, lineoffset, 1, 0, intensity);
        break;
      case IB_BGR_PLANES:
        getimg(b, width, height,
               (T*)buffer, lineoffset, 1, 0, intensity);
        getimg(g, width, height,
               (T*)buffer + height * lineoffset, lineoffset, 1, 0, intensity);
        getimg(r, width, height,
               (T*)buffer + 2 * height * lineoffset, lineoffset, 1, 0, intensity);
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
        rc = WRONG_PARAM;
      }

    return rc;
  }

  int Buffer2Image(const unsigned char* buffer,
                   const Image& imgr, const Image& imgg, const Image& imgb,
                   bool intensity,
                   int packmode, int lineoffset)
  {
    if (!IsImg(imgr) || !IsImg(imgg) || !IsImg(imgb))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int width;
    int height;

    RETURN_ERROR_IF_FAILED(MatchImg(imgr, imgg, imgb, width, height));

    int pt = imgr->ImageType();

    if ((pt < 0) || (pt > 3) || imgg->ImageType() != pt || imgb->ImageType() != pt)
      throw IceException(FNAME, M_WRONG_IMAGE);

    int factor = 3;

    if (packmode == IB_RGB32 || packmode == IB_BGR32)
      {
        factor = 4;
      }

    if (lineoffset == 0)
      {
        lineoffset = width * factor;
      }

    int rc = ERROR;

    switch (pt)
      {
      case 1:
        rc = frombuffer((PixelType1**)imgr->getDataPtr(),
                        (PixelType1**)imgg->getDataPtr(),
                        (PixelType1**)imgb->getDataPtr(),
                        width, height,
                        buffer, intensity,
                        lineoffset, packmode);
        break;

      case 2:
        rc = frombuffer((PixelType2**)imgr->getDataPtr(),
                        (PixelType2**)imgg->getDataPtr(),
                        (PixelType2**)imgb->getDataPtr(),
                        width, height,
                        buffer, intensity,
                        lineoffset, packmode);
        break;

      case 3:
        rc = frombuffer((PixelType3**)imgr->getDataPtr(),
                        (PixelType3**)imgg->getDataPtr(),
                        (PixelType3**)imgb->getDataPtr(),
                        width, height,
                        buffer, intensity,
                        lineoffset, packmode);
        break;
      }

    return rc;
  }
#undef FNAME

#define FNAME "Image2Buffer"
  int Image2Buffer(const Image& img, ibuffer& ib)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    ib.width = img->xsize;
    ib.height = img->ysize;
    ib.planes = 1;
    ib.maxval = img.maxval;

    ib.valuesize = img->getBytesPerPoint();
    ib.linelength = img->xsize * ib.valuesize;

    ib.intensity = false;
    ib.packmethod = IB_GRAY;
    ib.can_delete = true;

    ib.data = (unsigned char*) malloc(ib.linelength * ib.height);

    if (ib.data == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    if (ib.valuesize == 1)
      {
        unsigned char* bptr = (unsigned char*) ib.data;

        for (int y = 0; y < img->ysize; y++)
          for (int x = 0; x < img->xsize; x++)
            {
              *(bptr++) = img.getPixel(x, y);
            }
      }
    else
      {
        unsigned int* bptr = (unsigned int*) ib.data;

        for (int y = 0; y < img->ysize; y++)
          for (int x = 0; x < img->xsize; x++)
            {
              *(bptr++) = img.getPixel(x, y);
            }
      }

    return OK;
  }

  int Image2Buffer(const Image& RedImage, const Image& GreenImage, const Image& BlueImage,
                   ibuffer& ImageBuffer)
  {
    // consistency checks at first
    if (!IsImg(RedImage) || !IsImg(GreenImage) || !IsImg(BlueImage))
      throw IceException(FNAME, M_WRONG_IMAGE);

    // some more consistency checks
    if (RedImage->xsize != GreenImage->xsize || RedImage->ysize != GreenImage->ysize ||
        RedImage->xsize != BlueImage->xsize  || RedImage->ysize != BlueImage->ysize ||
        RedImage.maxval != GreenImage.maxval || RedImage.maxval != BlueImage.maxval)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    // copy values from the image to the image buffer
    ImageBuffer.width = RedImage->xsize;
    ImageBuffer.height = RedImage->ysize;
    ImageBuffer.planes = 3;
    ImageBuffer.maxval = RedImage.maxval;

    ImageBuffer.valuesize = RedImage->getBytesPerPoint();
    ImageBuffer.linelength = RedImage->xsize * ImageBuffer.valuesize * 3;

    ImageBuffer.intensity = false;
    ImageBuffer.packmethod = IB_RGB;
    ImageBuffer.can_delete = true;

    ImageBuffer.data = (unsigned char*)malloc(ImageBuffer.linelength * ImageBuffer.height);

    if (ImageBuffer.data == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    // copy the pixel values
    if (ImageBuffer.valuesize == 1)
      {
        unsigned char* bptr = (unsigned char*) ImageBuffer.data;

        for (int y = 0; y < RedImage->ysize; y++)
          for (int x = 0; x < RedImage->xsize; x++)
            {
              *(bptr++) = RedImage.getPixel(x, y);
              *(bptr++) = GreenImage.getPixel(x, y);
              *(bptr++) = BlueImage.getPixel(x, y);
            }
      }
    else
      {
        unsigned int* bptr = (unsigned int*) ImageBuffer.data;

        for (int y = 0; y < RedImage->ysize; y++)
          for (int x = 0; x < RedImage->xsize; x++)
            {
              *(bptr++) = RedImage.getPixel(x, y);
              *(bptr++) = GreenImage.getPixel(x, y);
              *(bptr++) = BlueImage.getPixel(x, y);
            }
      }

    return OK;
  }

  /*
   * Image -> char *
   */

  template<class T1, class T2>
  static void storeline(T1* src, int width, T2* dst, int factor, int offset)
  {
    dst += offset;
    T1* end = src + width;

    while (src != end)
      {
        *dst = *src;
        dst += factor;
        src++;
      }
  }

  template<class T1, class T2>
  static void storelinei(T1* src, int width, T2* dst, int factor, int offset)
  {
    dst += offset;
    T1* end = src + width;

    while (src != end)
      {
        *dst = ~(*src);
        dst += factor;
        src++;
      }
  }

  template<class T1, class T2>
  static  void storeimg(T1** src, int width, int height, T2* dst,
                        int lineoffset, int factor, int offset, bool intensity)
  {
    for (int i = 0; i < height; i++)
      {
        if (intensity)
          {
            storelinei(src[i], width, dst, factor, offset);
          }
        else
          {
            storeline(src[i], width, dst, factor, offset);
          }

        dst += lineoffset;
      }
  }

  int Image2Buffer(const Image& img,
                   unsigned char*& buffer, int& size,
                   bool intensity, int lineoffset, int factor, int offset)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int pt = img->ImageType();

    if ((pt < 0) || (pt > 3))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int width = img->xsize;
    int height = img->ysize;

    if (lineoffset == 0)
      {
        lineoffset = width;
      }

    size = 0;

    if (buffer == nullptr)
      {
        switch (pt)
          {
          case 1:
            buffer = (unsigned char*)new PixelType1[width * height * factor];
            size = sizeof(PixelType1) * width * height * factor;
            break;
          case 2:
            buffer = (unsigned char*)new PixelType2[width * height * factor];
            break;
            size = sizeof(PixelType2) * width * height * factor;
            break;
          case 3:
            buffer = (unsigned char*)new PixelType3[width * height * factor];
            break;
            size = sizeof(PixelType3) * width * height * factor;
            break;
          }
      }

    switch (pt)
      {
      case 1:
        storeimg((PixelType1**)img->getDataPtr(), width, height,
                 (PixelType1*) buffer, lineoffset, factor, offset, intensity);
        break;
      case 2:
        storeimg((PixelType2**)img->getDataPtr(), width, height,
                 (PixelType2*) buffer, lineoffset, factor, offset, intensity);
        break;
      case 3:
        storeimg((PixelType2**)img->getDataPtr(), width, height,
                 (PixelType2*) buffer, lineoffset, factor, offset, intensity);
        break;
      }

    return OK;
  }

  template<class T>
  static int tobuffer(T** r, T** g, T** b,
                      int width, int height,
                      unsigned char* buffer,
                      bool intensity,
                      int lineoffset, int packmode)
  {
    int rc = OK;

    switch (packmode)
      {
      case IB_RGB:
        storeimg(r, width, height,
                 (T*)buffer, lineoffset, 3, 0, intensity);
        storeimg(g, width, height,
                 (T*)buffer, lineoffset, 3, 1, intensity);
        storeimg(b, width, height,
                 (T*)buffer, lineoffset, 3, 2, intensity);
        break;
      case IB_BGR:
        storeimg(b, width, height,
                 (T*)buffer, lineoffset, 3, 0, intensity);
        storeimg(g, width, height,
                 (T*)buffer, lineoffset, 3, 1, intensity);
        storeimg(r, width, height,
                 (T*)buffer, lineoffset, 3, 2, intensity);
        break;
      case IB_RGB32:
        storeimg(r, width, height,
                 (T*)buffer, lineoffset, 4, 0, intensity);
        storeimg(g, width, height,
                 (T*)buffer, lineoffset, 4, 1, intensity);
        storeimg(b, width, height,
                 (T*)buffer, lineoffset, 4, 2, intensity);
        break;
      case IB_BGR32:
        storeimg(b, width, height,
                 (T*)buffer, lineoffset, 4, 0, intensity);
        storeimg(g, width, height,
                 (T*)buffer, lineoffset, 4, 1, intensity);
        storeimg(r, width, height,
                 (T*)buffer, lineoffset, 4, 2, intensity);
        break;
      case IB_RGB_PLANES:
        storeimg(r, width, height,
                 (T*)buffer, lineoffset, 1, 0, intensity);
        storeimg(g, width, height,
                 (T*)buffer + height * lineoffset, lineoffset, 1, 0, intensity);
        storeimg(b, width, height,
                 (T*)buffer + 2 * height * lineoffset, lineoffset, 1, 0, intensity);
        break;
      case IB_BGR_PLANES:
        storeimg(b, width, height,
                 (T*)buffer, lineoffset, 1, 0, intensity);
        storeimg(g, width, height,
                 (T*)buffer + height * lineoffset, lineoffset, 1, 0, intensity);
        storeimg(r, width, height,
                 (T*)buffer + 2 * height * lineoffset, lineoffset, 1, 0, intensity);
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
        rc = WRONG_PARAM;
      }

    return rc;
  }

  int Image2Buffer(const Image& imgr, const Image& imgg, const Image& imgb,
                   unsigned char*& buffer, int& size,
                   bool intensity, int packmode, int lineoffset)
  {
    if (!IsImg(imgr) || !IsImg(imgg) || !IsImg(imgb))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int width;
    int height;

    RETURN_ERROR_IF_FAILED(MatchImg(imgr, imgg, imgb, width, height));

    int pt = imgr->ImageType();

    if ((pt < 0) || (pt > 3) || imgg->ImageType() != pt || imgb->ImageType() != pt)
      throw IceException(FNAME, M_WRONG_IMAGE);

    int factor = 3;

    if (packmode == IB_RGB32 || packmode == IB_BGR32)
      {
        factor = 4;
      }

    if (lineoffset == 0)
      {
        lineoffset = width * factor;
      }

    size = 0;

    if (buffer == nullptr)
      {
        switch (pt)
          {
          case 1:
            buffer = (unsigned char*)new PixelType1[width * height * factor];
            size = sizeof(PixelType1) * width * height * factor;
            break;
          case 2:
            buffer = (unsigned char*)new PixelType2[width * height * factor];
            size = sizeof(PixelType2) * width * height * factor;
            break;
          case 3:
            buffer = (unsigned char*)new PixelType3[width * height * factor];
            size = sizeof(PixelType3) * width * height * factor;
            break;
          }
      }

    switch (pt)
      {
      case 1:
        return tobuffer((PixelType1**)imgr->getDataPtr(),
                        (PixelType1**)imgg->getDataPtr(),
                        (PixelType1**)imgb->getDataPtr(),
                        width, height,
                        buffer, intensity,
                        lineoffset, packmode);
        break;

      case 2:
        return tobuffer((PixelType2**)imgr->getDataPtr(),
                        (PixelType2**)imgg->getDataPtr(),
                        (PixelType2**)imgb->getDataPtr(),
                        width, height,
                        buffer, intensity,
                        lineoffset, packmode);
        break;

      case 3:
        return tobuffer((PixelType3**)imgr->getDataPtr(),
                        (PixelType3**)imgg->getDataPtr(),
                        (PixelType3**)imgb->getDataPtr(),
                        width, height,
                        buffer, intensity,
                        lineoffset, packmode);
        break;
      }

    return ERROR;
  }
#undef FNAME
}
