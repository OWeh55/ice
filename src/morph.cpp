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

/*
 *   Morphologische Filter
 *
 *   M. Schubert 3/96
 *   W. Ortmann 3/99 ... 4/13
 */

#include "IceException.h"
#include "base.h"
#include "MinMaxHistogram.h"

#include "macro.h"

#include "morph.h"

using namespace std;

namespace ice
{
//--------------------------------------
// Morphologic filters
//--------------------------------------
#define MHISTSIZE (1<<13)
//------------------------------------------------------
// optimized versions for rectangular structuring element
//------------------------------------------------------
#define FNAME "dilateImg"
  void dilateImgO(const Image& pn1, int sx, int sy, const Image& pn2)
  {
    // dilatation in rectangular window
    // optimized with incremental calculation of max with histogram

    if ((pn1.maxval >= MHISTSIZE) ||
        (sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    try
      {

        int sx1 = sx / 2;
        int sy1 = sy / 2;

        int dx, dy;
        checkSizes(pn1, pn2, dx, dy);
        int maxval = pn1.maxval;

        if (pn2.maxval < maxval)
          throw IceException(FNAME, M_WRONG_PARAM);

        MinMaxHistogram mh(maxval);
        matrix<int> tmp(dx, dy);

        // horizontal filtering
        for (int y = 0; y < dy; y++)   // all rows
          {
            int ypn1 = y;
            mh.reset();
            int xr = 0;
            int xrpn1 = 0;

            while (xr < sx1)
              {
                // enter "right" values to histogram
                mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
                xr++;
                xrpn1++;
              }

            int xd = 0;

            while (xd < sx1)
              {
                // enter "right" values to histogram
                // write result
                mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
                tmp[xd][y] = mh.getMax();
                xr++;
                xrpn1++;
                xd++;
              }

            //        int xl = 0;
            int xlpn1 = 0;

            while (xr < dx)
              {
                // enter "right" values to histogram
                // write result
                // delete "left" value from histogram
                mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
                tmp[xd][y] = mh.getMax();
                mh.dec(GetValUnchecked(pn1, xlpn1, ypn1));

                //            xl++;
                xlpn1++;
                xr++;
                xrpn1++;
                xd++;
              }

            while (xd < dx)
              {
                // write result
                // delete "left" value from histogram
                tmp[xd][y] = mh.getMax();
                mh.dec(GetValUnchecked(pn1, xlpn1, ypn1));
                //            xl++;
                xlpn1++;
                xd++;
              }
          }

        // vertical filtering
        for (int x = 0; x < dx; x++)   // all columns
          {
            int xpn2 = x;

            mh.reset();

            int yr = 0;

            while (yr < sy1)
              {

                mh.inc(tmp[x][yr]);
                yr++;
              }

            int yd = 0;
            int ydpn2 = 0;

            while (yd < sy1)
              {
                mh.inc(tmp[x][yr]);
                PutValUnchecked(pn2, xpn2, ydpn2, mh.getMax());
                yr++;
                yd++;
                ydpn2++;
              }

            int yl = 0;

            while (yr < dy)
              {
                mh.inc(tmp[x][yr]);
                PutValUnchecked(pn2, xpn2, ydpn2, mh.getMax());
                mh.dec(tmp[x][yl]);

                yl++;
                yr++;
                yd++;
                ydpn2++;
              }

            while (yd < dy)
              {
                PutValUnchecked(pn2, xpn2, ydpn2, mh.getMax());
                mh.dec(tmp[x][yl]);

                yl++;
                yd++;
                ydpn2++;
              }

          }
      }
    RETHROW;
  }
#undef FNAME
//--------------------------------------------------
#define FNAME "erodeImg"
  void erodeImgO(const Image& pn1, int sx, int sy, const Image& pn2)
  {
    // erosion in rectangular window
    // optimized with incremental calculation of min with histogram
    try
      {
        if (pn1.maxval >= MHISTSIZE)
          throw IceException(FNAME, M_WRONG_PARAM);

        if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
          throw IceException(FNAME, M_WRONG_PARAM);

        int sx1 = sx / 2;
        int sy1 = sy / 2;

        int dx, dy;
        checkSizes(pn1, pn2, dx, dy);

        int maxval = pn1.maxval;

        if (pn2.maxval < maxval)
          throw IceException(FNAME, M_WRONG_PARAM);

        MinMaxHistogram mh(maxval);

        matrix<int> tmp(dx, dy);

        // horizontal filtering
        for (int y = 0; y < dy; y++)   // all rows
          {
            int ypn1 = y;

            mh.reset();
            int xr = 0;
            int xrpn1 = 0;

            while (xr < sx1)
              {
                // enter "right" values to histogram
                mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
                xr++;
                xrpn1++;
              }

            int xd = 0;

            while (xd < sx1)
              {
                // enter "right" values to histogram
                // write result
                mh.inc(pn1.getPixelUnchecked(xrpn1, ypn1));
                tmp[xd][y] = mh.getMin();
                xr++;
                xrpn1++;
                xd++;
              }

            int xlpn1 = 0;

            while (xr < dx)
              {
                // enter "right" values to histogram
                // write result
                // delete "left" value from histogram
                mh.inc(pn1.getPixelUnchecked(xrpn1, ypn1));
                tmp[xd][y] = mh.getMin();
                mh.dec(pn1.getPixelUnchecked(xlpn1, ypn1));
                //            xl++;
                xlpn1++;
                xr++;
                xrpn1++;
                xd++;
              }

            while (xd < dx)
              {
                // write result
                // delete "left" value from histogram
                tmp[xd][y] = mh.getMin();
                mh.dec(GetValUnchecked(pn1, xlpn1, ypn1));
                //            xl++;
                xlpn1++;
                xd++;
              }
          }

        // vertical filtering
        for (int x = 0; x < dx; x++)   // all columns
          {
            int xpn2 = x;
            mh.reset();
            int yr = 0;

            while (yr < sy1)
              {
                mh.inc(tmp[x][yr]);
                yr++;
              }

            int yd = 0;
            int ydpn2 = 0;

            while (yd < sy1)
              {
                mh.inc(tmp[x][yr]);
                PutValUnchecked(pn2, xpn2, ydpn2, mh.getMin());
                yr++;
                yd++;
                ydpn2++;
              }

            int yl = 0;

            while (yr < dy)
              {
                mh.inc(tmp[x][yr]);
                PutValUnchecked(pn2, xpn2, ydpn2, mh.getMin());
                mh.dec(tmp[x][yl]);
                yl++;
                yr++;
                yd++;
                ydpn2++;
              }

            while (yd < dy)
              {
                PutValUnchecked(pn2, xpn2, ydpn2, mh.getMin());
                mh.dec(tmp[x][yl]);
                yl++;
                yd++;
                ydpn2++;
              }

          }
      }
    RETHROW;
  }
#undef FNAME
//--------------------------------------------------
#define FNAME "dilateImg"
  void dilateImg(const Image& sourceImage,
                 const Image& destinationImage,
                 int sx, int sy)
  {
    // Dilatation mit rechteckigen Fenster

    if (sy < 0)
      {
        sy = sx;
      }

    Image imgs = sourceImage;
    int need_temp;

    int x, y;
    int xx, yy;
    int val;

    if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    try
      {
        int dx, dy;
        checkSizes(imgs, destinationImage, dx, dy);

        if (imgs.maxval < MHISTSIZE)
          {
            dilateImgO(imgs, sx, sy, destinationImage);
            return;
          }

        need_temp = (destinationImage == imgs);

        if (need_temp)
          {
            imgs = NewImg(imgs, true);
          }

        // filter rows

        for (y = 0; y < dy; y++)
          {
            // linker Rand auf 0
            for (x = 0; x < sx1; ++x)
              {
                PutVal(destinationImage, x, y, 0);
              }
            // normale Werte
            for (x = sx1; x < dx - sx1; x++)
              {
                val = 0;

                for (xx = x - sx1; xx <= x + sx1; xx++)
                  {
                    val = max(val, GetVal(imgs, xx, y));
                  }

                PutVal(destinationImage, x, y, val);
              }
            // rechter Rand auf 0
            for (x = dx - sx1; x < dx; ++x)
              {
                PutVal(destinationImage, x, y, 0);
              }
          }

        // filter cols
        int* linebuffer = new int[dy];

        for (x = 0; x < dx; x++)
          {
            for (y = 0; y < sy1; y++)
              {
                linebuffer[y] = 0;
              }

            for (y = sy1; y < dy - sy1; y++)
              {
                val = 0;

                for (yy = -sy1; yy <= sy1; yy++)
                  {
                    val = max(val, GetVal(destinationImage, x, y + yy));
                  }

                linebuffer[y] = val;
              }

            for (y = dy - sy1; y < dy; y++)
              {
                linebuffer[y] = 0;
              }

            for (y = 0; y < dy; y++)
              {
                PutVal(destinationImage, x, y, linebuffer[y]);
              }
          }

        delete [] linebuffer;
      }
    RETHROW;
  }
#undef FNAME
//--------------------------------------------------
#define FNAME "erodeImg"
  void erodeImg(const Image& sourceImage, const Image& destinationImage,
                int sx, int sy)
  {
    // Erosion mit rechteckigen Fenster
    if (sy < 0)
      {
        sy = sx;
      }

    Image imgs = sourceImage;
    int need_temp;

    int x, y;
    int xx, yy;
    int val;
    int* buffer = NULL;

    if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    int sx1 = sx / 2;
    int sy1 = sy / 2;
    try
      {
        int dx, dy;
        checkSizes(imgs, destinationImage, dx, dy);

        if (imgs.maxval < MHISTSIZE)
          {
            erodeImgO(imgs, sx, sy, destinationImage);
            return;
          }

        need_temp = (destinationImage == imgs);

        if (need_temp)
          {
            imgs = NewImg(imgs, true);
          }

        // filter rows

        for (y = 0; y < dy; y++)
          {
            for (x = 0; x < sx1; ++x)
              {
                PutVal(destinationImage, x, y, 0);
              }
            for (x = sx1; x < dx - sx1; x++)
              {
                val = imgs.maxval;

                for (xx = -sx1; xx <= sx1; xx++)
                  {
                    val = min(val, GetVal(imgs, x + xx, y));
                  }

                PutVal(destinationImage, x, y, val);
              }
            for (x = dx - sx1; x < dx; ++x)
              {
                PutVal(destinationImage, x, y, 0);
              }
          }

        // filter cols
        buffer = new int[dy];

        for (x = 0; x < dx; x++)
          {
            for (y = 0; y < sy1; y++)
              {
                buffer[y] = 0;
              }

            for (y = sy1; y < dy - sy1; y++)
              {
                val = imgs.maxval;

                for (yy = -sy1; yy <= sy1; yy++)
                  {
                    val = min(val, GetVal(destinationImage, x, y + yy));
                  }

                buffer[y] = val;
              }

            for (y = dy - sy1; y < dy; y++)
              {
                buffer[y] = 0;
              }

            for (y = 0; y < dy; y++)
              {
                PutVal(destinationImage, x, y, buffer[y]);
              }
          }

        delete [] buffer;
      }
    RETHROW;
  }
#undef FNAME

  /* dilate/erode with given mask (as int*) */
#define FNAME "dilateImg"
  void dilateImg(const Image& sourceImage,
                 int nbhx, int nbhy, int* mask,
                 const Image& destinationImage)
  {
    Image imgs = sourceImage;
    Image imgh;
    int dx, dy;

    if (mask == nullptr)
      {
        return dilateImg(imgs, destinationImage, nbhx, nbhy);
      }

    if ((nbhx & 1) == 0 || (nbhy & 1) == 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (destinationImage == imgs)
      {
        imgh = NewImg(imgs, true);
        imgs = imgh;
      }

    RETURN_ERROR_IF_FAILED(MatchImg(imgs, destinationImage, dx, dy));

    int nx2 = nbhx / 2;
    int ny2 = nbhy / 2;

    // use given mask
    for (int y = 0; y < dy ; y++)
      {
        int y1 = y - ny2;
        if (y1 < 0)
          {
            y1 = 0;
          }
        int y2 = y + ny2;
        if (y2 >= dy)
          {
            y2 = dy - 1;
          }

        for (int x = 0; x < dx; x++)
          {
            int x1 = x - nx2;
            if (x1 < 0)
              {
                x1 = 0;
              }
            int x2 = x + nx2;
            if (x2 >= dx)
              {
                x2 = dx - 1;
              }
            int val = 0;
            for (int yy = y1 ; yy <= y2; yy++)
              {
                int my = yy - y + ny2;
                for (int xx = x1; xx <= x2 ; xx++)
                  {
                    int mx = xx - x + nx2;
                    if (mask[mx + nbhx * my] != 0)
                      {
                        int aval = GetValUnchecked(imgs, xx, yy);

                        if (aval > val)
                          {
                            val = aval;
                          }
                      }
                  }
              }
            PutValUnchecked(destinationImage, x, y, val);
          }
      }
  }

  void dilateImg(const Image& sourceImage,
                 int nbh, int* mask,
                 const Image& destinationImage)
  {
    dilateImg(sourceImage, nbh, nbh, mask, destinationImage);
  }

#undef FNAME
#define FNAME "erodeImg"
  void erodeImg(const Image& sourceImage,
                int nbhx, int nbhy, int* mask,
                const Image& destinationImage)
  {
    Image imgs = sourceImage;
    Image imgh;
    int dx, dy;

    if (mask == nullptr)
      {
        return dilateImg(imgs, destinationImage, nbhx, nbhy);
      }

    if ((nbhx & 1) == 0 || (nbhy & 1) == 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (destinationImage == imgs)
      {
        imgh = NewImg(imgs, true);
        imgs = imgh;
      }

    RETURN_ERROR_IF_FAILED(MatchImg(imgs, destinationImage, dx, dy));

    int nx2 = nbhx / 2;
    int ny2 = nbhy / 2;

    // use given mask
    for (int y = 0; y < dy ; y++)
      {
        int y1 = y - ny2;
        if (y1 < 0)
          {
            y1 = 0;
          }
        int y2 = y + ny2;
        if (y2 >= dy)
          {
            y2 = dy - 1;
          }

        for (int x = 0; x < dx; x++)
          {
            int x1 = x - nx2;
            if (x1 < 0)
              {
                x1 = 0;
              }
            int x2 = x + nx2;
            if (x2 >= dx)
              {
                x2 = dx - 1;
              }
            int val = imgs.maxval;
            for (int yy = y1 ; yy <= y2; yy++)
              {
                int my = yy - y + ny2;
                for (int xx = x1; xx <= x2 ; xx++)
                  {
                    int mx = xx - x + nx2;
                    if (mask[mx + nbhx * my] != 0)
                      {
                        int aval = GetValUnchecked(imgs, xx, yy);

                        if (aval < val)
                          {
                            val = aval;
                          }
                      }
                  }
              }
            PutValUnchecked(destinationImage, x, y, val);
          }
      }
  }

  void erodeImg(const Image& sourceImage,
                int nbh, int* mask,
                const Image& destinationImage)
  {
    erodeImg(sourceImage, nbh, nbh, mask, destinationImage);
  }

#undef FNAME

#define FNAME "dilateImg"
  void dilateImg(const Image& sourceImage, const IMatrix& mask, const Image& destinationImage)
  {
    int nx = mask.cols();
    int ny = mask.rows();
    int* imask = new int[nx * ny];

    int i = 0;
    for (int y = 0; y < ny; y++)
      for (int x = 0; x < nx; x++)
        {
          imask[i++] = mask[y][x];
        }
    dilateImg(sourceImage, nx, ny, imask, destinationImage);
    delete [] imask;
  }

  void dilateImg(const Image& sourceImage, const Image& destinationImage, const IMatrix& mask)
  {
    dilateImg(sourceImage, mask, destinationImage);
  }
#undef FNAME

#define FNAME "erodeImg"
  void erodeImg(const Image& sourceImage, const IMatrix& mask, const Image& destinationImage)
  {
    int nx = mask.cols();
    int ny = mask.rows();
    int* imask = new int[nx * ny];

    int i = 0;
    for (int y = 0; y < ny; y++)
      for (int x = 0; x < nx; x++)
        {
          imask[i++] = mask[y][x];
        }
    erodeImg(sourceImage, nx, ny, imask, destinationImage);
    delete [] imask;
  }

  void erodeImg(const Image& sourceImage, const Image& destinationImage, const IMatrix& mask)
  {
    erodeImg(sourceImage, mask, destinationImage);
  }

#undef FNAME

#define FNAME "openingImg"
  void openingImg(const Image& img1, const Image& img2, int nx, int ny)
  {
    if (ny < 0)
      {
        ny = nx;
      }
    try
      {
        erodeImg(img1, img2, nx, ny);
        dilateImg(img2, img2, nx, ny);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "closingImg"
  void closingImg(const Image& img1, const Image& img2, int nx, int ny)
  {
    if (ny < 0)
      {
        ny = nx;
      }
    try
      {
        dilateImg(img1, img2, nx, ny);
        erodeImg(img2, img2, nx, ny);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "MinMaxImg"
  void minmaxcore_std(const Image& pn1,
                      const Image& pn2, const Image& pn3,
                      int sx1, int sy1)
  {
    int x, y;
    int dx = pn1.xsize;
    int dy = pn1.ysize;
    int maxval = pn1.maxval;

    matrix<int> tmp1(dx, dy);
    matrix<int> tmp2(dx, dy);

    MinMaxHistogram mh(maxval);

    // horizontal filtering
    for (y = 0; y < dy; y++)   // all rows
      {
        int ypn1 = y;
        mh.reset();

        int xr = 0;
        int xrpn1 = 0;

        while (xr < sx1)
          {
            // enter "right" values to histogram
            mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
            xr++;
            xrpn1++;
          }

        int xd = 0;

        while (xd < sx1)
          {
            // enter "right" values to histogram
            // write result
            mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
            tmp1[xd][y] = mh.getMin();
            tmp2[xd][y] = mh.getMax();
            xr++;
            xrpn1++;
            xd++;
          }

        //        int xl = 0;
        int xlpn1 = 0;

        while (xr < dx)
          {
            // enter "right" values to histogram
            // write result
            // delete "left" value from histogram
            mh.inc(GetValUnchecked(pn1, xrpn1, ypn1));
            tmp1[xd][y] = mh.getMin();
            tmp2[xd][y] = mh.getMax();
            mh.dec(GetValUnchecked(pn1, xlpn1, ypn1));
            //            xl++;
            xlpn1++;
            xr++;
            xrpn1++;
            xd++;
          }

        while (xd < dx)
          {
            // write result
            // delete "left" value from histogram
            tmp1[xd][y] = mh.getMin();
            tmp2[xd][y] = mh.getMax();
            mh.dec(GetValUnchecked(pn1, xlpn1, ypn1));
            //            xl++;
            xlpn1++;
            xd++;
          }
      }

    // vertical filtering
    for (x = 0; x < dx; x++)   // alle columns (min)
      {
        int xpn2 = x;
        mh.reset();
        int yr = 0;

        while (yr < sy1)
          {
            mh.inc(tmp1[x][yr]);
            yr++;
          }

        int yd = 0;
        int ydpn2 = 0;

        while (yd < sy1)
          {
            mh.inc(tmp1[x][yr]);
            PutValUnchecked(pn2, xpn2, ydpn2, mh.getMin());
            yr++;
            yd++;
            ydpn2++;
          }

        int yl = 0;

        while (yr < dy)
          {
            mh.inc(tmp1[x][yr]);
            PutValUnchecked(pn2, xpn2, ydpn2, mh.getMin());
            mh.dec(tmp1[x][yl]);
            yl++;
            yr++;
            yd++;
            ydpn2++;
          }

        while (yd < dy)
          {
            PutValUnchecked(pn2, xpn2, ydpn2, mh.getMin());
            mh.dec(tmp1[x][yl]);
            yl++;
            yd++;
            ydpn2++;
          }

      }

    for (x = 0; x < dx; x++)   // all columns (max)
      {
        int xpn3 = x;
        mh.reset();
        int yr = 0;

        while (yr < sy1)
          {
            mh.inc(tmp2[x][yr]);
            yr++;
          }

        int yd = 0;
        int ydpn3 = 0;

        while (yd < sy1)
          {
            mh.inc(tmp2[x][yr]);
            PutValUnchecked(pn3, xpn3, ydpn3, mh.getMax());
            yr++;
            yd++;
            ydpn3++;
          }

        int yl = 0;

        while (yr < dy)
          {
            mh.inc(tmp2[x][yr]);
            PutValUnchecked(pn3, xpn3, ydpn3, mh.getMax());
            mh.dec(tmp2[x][yl]);
            yl++;
            yr++;
            yd++;
            ydpn3++;
          }

        while (yd < dy)
          {
            PutValUnchecked(pn3, xpn3, ydpn3, mh.getMax());
            mh.dec(tmp2[x][yl]);
            yl++;
            yd++;
            ydpn3++;
          }

      }

  }

  template<class T>
  void minmaxcore(const Image& pn1,
                  const Image& pn2, const Image& pn3,
                  int sx1, int sy1)
  {
    int x, y;
    int dx = pn1.xsize;
    int dy = pn1.ysize;
    int maxval = pn1.maxval;

    const T** sourcep = (const T**)pn1->getDataPtr();
    T** minp = (T**)pn2->getDataPtr();
    T** maxp = (T**)pn3->getDataPtr();

    matrix<int> tmp1(dx, dy);
    matrix<int> tmp2(dx, dy);
    MinMaxHistogram mh(maxval);

    // horizontal filtering
    for (y = 0; y < dy; y++)   // all rows
      {
        int ypn1 = y;

        mh.reset();

        int xr = 0;
        int xrpn1 = 0;

        while (xr < sx1)
          {
            // enter "right" values to histogram
            mh.inc(sourcep[ypn1][xrpn1]);
            xr++;
            xrpn1++;
          }

        int xd = 0;

        while (xd < sx1)
          {
            // enter "right" values to histogram
            // write result
            mh.inc(sourcep[ypn1][xrpn1]);
            tmp1[xd][y] = mh.getMin();
            tmp2[xd][y] = mh.getMax();
            xr++;
            xrpn1++;
            xd++;
          }

        //        int xl = 0;
        int xlpn1 = 0;

        while (xr < dx)
          {
            // enter "right" values to histogram
            // write result
            // delete "left" value from histogram
            mh.inc(sourcep[ypn1][xrpn1]);
            tmp1[xd][y] = mh.getMin();
            tmp2[xd][y] = mh.getMax();
            mh.dec(sourcep[ypn1][xlpn1]);
            //            xl++;
            xlpn1++;
            xr++;
            xrpn1++;
            xd++;
          }

        while (xd < dx)
          {
            // write result
            // delete "left" value from histogram
            tmp1[xd][y] = mh.getMin();
            tmp2[xd][y] = mh.getMax();
            mh.dec(sourcep[ypn1][xlpn1]);
            //            xl++;
            xlpn1++;
            xd++;
          }
      }

    // vertical filtering
    for (x = 0; x < dx; x++)   // alle columns (min)
      {
        int xpn2 = x;
        mh.reset();
        int yr = 0;

        while (yr < sy1)
          {
            mh.inc(tmp1[x][yr]);
            yr++;
          }

        int yd = 0;
        int ydpn2 = 0;

        while (yd < sy1)
          {
            mh.inc(tmp1[x][yr]);
            minp[ydpn2][xpn2] = mh.getMin();
            yr++;
            yd++;
            ydpn2++;
          }

        int yl = 0;

        while (yr < dy)
          {
            mh.inc(tmp1[x][yr]);
            minp[ydpn2][xpn2] = mh.getMin();
            mh.dec(tmp1[x][yl]);
            yl++;
            yr++;
            yd++;
            ydpn2++;
          }

        while (yd < dy)
          {
            minp[ydpn2][xpn2] = mh.getMin();
            mh.dec(tmp1[x][yl]);
            yl++;
            yd++;
            ydpn2++;
          }

      }

    for (x = 0; x < dx; x++)   // all columns (max)
      {
        int xpn3 = x;
        mh.reset();
        int yr = 0;

        while (yr < sy1)
          {
            mh.inc(tmp2[x][yr]);
            yr++;
          }

        int yd = 0;
        int ydpn3 = 0;

        while (yd < sy1)
          {
            mh.inc(tmp2[x][yr]);
            maxp[ydpn3][xpn3] = mh.getMax();
            yr++;
            yd++;
            ydpn3++;
          }

        int yl = 0;

        while (yr < dy)
          {
            mh.inc(tmp2[x][yr]);
            maxp[ydpn3][xpn3] = mh.getMax();
            mh.dec(tmp2[x][yl]);
            yl++;
            yr++;
            yd++;
            ydpn3++;
          }

        while (yd < dy)
          {
            maxp[ydpn3][xpn3] = mh.getMax();
            mh.dec(tmp2[x][yl]);
            yl++;
            yd++;
            ydpn3++;
          }

      }
  }

  int MinMaxImg(const Image& pn1, int sx, int sy,
                const Image& pn2, const Image& pn3)
  {
    // dilatation and erosion in rectangular window
    // optimized with incremental calculation of max/min with histogram
    int dx, dy;

    if (pn1.maxval >= MHISTSIZE)
      throw IceException(FNAME, M_WRONG_PARAM);

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, pn3, dx, dy));

    if ((sx < 1) || ((sx & 1) != 1) ||
        (sy < 1) || ((sy & 1) != 1) ||
        (sx >= dx) || (sy >= dy))
      throw IceException(FNAME, M_WRONG_PARAM);

    if ((pn1.maxval != pn2.maxval) ||
        (pn2.maxval != pn3.maxval))
      throw IceException(FNAME, M_WRONG_PARAM);

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    int ptyp = pn1->ImageType();

    if ((pn2->ImageType() != ptyp) || (pn3->ImageType() != ptyp))
      {
        ptyp = 0;  // "mixed type" == undefined
      }

    switch (ptyp)
      {
      case 1:
        minmaxcore<PixelType1>(pn1, pn2, pn3, sx1, sy1);
        break;
      case 2:
        minmaxcore<PixelType2>(pn1, pn2, pn3, sx1, sy1);
        break;
      case 3:
        minmaxcore<PixelType3>(pn1, pn2, pn3, sx1, sy1);
        break;
      default:
        minmaxcore_std(pn1, pn2, pn3, sx1, sy1);
        break;
      }

    return OK;
  }
#undef FNAME
}
