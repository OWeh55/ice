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

#include "message.h"
#include "base.h"
#include "macro.h"

#include "filter.h"

namespace ice
{
//--------------------------------------
// Morphologic filters
//--------------------------------------

  class MinMaxHistogram
  {
    // very simple and fast histogram
    // used for incremental calculation of min and max

#define MHISTSIZE (1<<13)

    int* data;
    int size;
    int min, max;

  public:
    MinMaxHistogram(int maxval)
    {
      size = maxval + 1;
      data = new int[size];
      reset();
    }

    void reset()
    {
      for (int i = 0; i < size; i++)
        data[i] = 0;

      min = size - 1;
      max = 0;
    }

    ~MinMaxHistogram()
    {
      delete [] data;
    }

    inline void inc(int i)
    {
      int oldValue = data[i]++;

      if (oldValue == 0)
        {
          // was empty
          if (i < min) // new min?
            min = i;

          if (i > max) // new max?
            max = i;
        }
    }

    inline void dec(int i)
    {
      int newValue = --data[i];
      if (newValue == 0)
        {
          // now empty
          if (i == min) // if i was min
            {
              // find new min
              while (min < size - 1 && data[min] == 0)
                min++;
            }

          if (i == max) // if i was max
            {
              // find new max
              while (max > 0 && data[max] == 0)
                max--;
            }
        }
    }
    int getMin() const
    {
      return min;
    }
    int getMax() const
    {
      return max;
    }
  };
//------------------------------------------------------

  int** newtemp(int dx, int dy)
  {
    int** tmp = new int* [dx];

    for (int x = 0; x < dx; x++)
      tmp[x] = new int[dy];

    return tmp;
  }

  void deletetemp(int** tmp, int dx, int dy)
  {
    for (int x = 0; x < dx; x++)
      delete [] tmp[x];

    delete [] tmp;
  }

//------------------------------------------------------
#define FNAME "DilateImg"
  int DilateImgO(const Image& pn1, int sx, int sy, const Image& pn2)
  {
    // dilatation in rectangular window
    // optimized with incremental calculation of max with histogram
    int dx, dy;

    if ((pn1.maxval >= MHISTSIZE) ||
        (sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    ReturnErrorIfFailed(MatchImg(pn1, pn2, dx, dy));
    int maxval = pn1.maxval;

    if (pn2.maxval < maxval)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    MinMaxHistogram mh(maxval);
    int** tmp = newtemp(dx, dy);

    // horizontal filtering
    for (int y = 0; y < dy; y++) // all rows
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
    for (int x = 0; x < dx; x++) // all columns
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

    deletetemp(tmp, dx, dy);

    return OK;
  }

  int DilateImg(const Image& sourceImage, int sx, int sy,
                const Image& destinationImage)
  {
    // Dilatation mit rechteckigen Fenster

    Image imgs = sourceImage;
    int need_temp;
    int dx, dy;
    int x, y;
    int xx, yy;
    int val;

    if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    ReturnErrorIfFailed(MatchImg(imgs, destinationImage, dx, dy));

    if (imgs.maxval < MHISTSIZE)
      return DilateImgO(imgs, sx, sy, destinationImage);

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
          PutVal(destinationImage, x, y, 0);
        // normale Werte
        for (x = sx1; x < dx - sx1; x++)
          {
            val = 0;

            for (xx = x - sx1; xx <= x + sx1; xx++)
              val = max(val, GetVal(imgs, xx, y));

            PutVal(destinationImage, x, y, val);
          }
        // rechter Rand auf 0
        for (x = dx - sx1; x < dx; ++x)
          PutVal(destinationImage, x, y, 0);
      }

    // filter cols
    int* linebuffer = new int[dy];

    for (x = 0; x < dx; x++)
      {
        for (y = 0; y < sy1; y++)
          linebuffer[y] = 0;

        for (y = sy1; y < dy - sy1; y++)
          {
            val = 0;

            for (yy = -sy1; yy <= sy1; yy++)
              val = max(val, GetVal(destinationImage, x, y + yy));

            linebuffer[y] = val;
          }

        for (y = dy - sy1; y < dy; y++)
          linebuffer[y] = 0;

        for (y = 0; y < dy; y++)
          PutVal(destinationImage, x, y, linebuffer[y]);
      }

    if (need_temp)
      FreeImg(imgs);

    delete [] linebuffer;
    return OK;
  }

  /* Dilate/Erode with given mask (as int*) */

  int DilateImg(const Image& sourceImage,
                int nbhx, int nbhy, int* mask,
                const Image& destinationImage)
  {
    Image imgs = sourceImage;
    Image imgh;
    int needs_temp = false;
    int dx, dy;

    if (mask == NULL)
      return DilateImg(imgs, nbhx, nbhy, destinationImage);

    if ((nbhx & 1) == 0 || (nbhy & 1) == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (destinationImage == imgs)
      {
        imgh = NewImg(imgs, true);
        imgs = imgh;
        needs_temp = true;
      }

    ReturnErrorIfFailed(MatchImg(imgs, destinationImage, dx, dy));

    int nx2 = nbhx / 2;
    int ny2 = nbhy / 2;

    // use given mask
    for (int y = 0; y < dy ; y++)
      {
        int y1 = y - ny2;
        if (y1 < 0) y1 = 0;
        int y2 = y + ny2;
        if (y2 >= dy) y2 = dy - 1;

        for (int x = 0; x < dx; x++)
          {
            int x1 = x - nx2;
            if (x1 < 0) x1 = 0;
            int x2 = x + nx2;
            if (x2 >= dx) x2 = dx - 1;
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

                        if (aval > val) val = aval;
                      }
                  }
              }
            PutValUnchecked(destinationImage, x, y, val);
          }
      }

    if (needs_temp) FreeImg(imgh);

    return OK;
  }

  int DilateImg(const Image& sourceImage,
                int nbh, int* mask,
                const Image& destinationImage)
  {
    return DilateImg(sourceImage, nbh, nbh, mask, destinationImage);
  }

#undef FNAME
#define FNAME "ErodeImg"
  int ErodeImg(const Image& sourceImage,
               int nbhx, int nbhy, int* mask,
               const Image& destinationImage)
  {
    Image imgs = sourceImage;
    Image imgh;
    int needs_temp = false;
    int dx, dy;

    if (mask == NULL)
      return DilateImg(imgs, nbhx, nbhy, destinationImage);

    if ((nbhx & 1) == 0 || (nbhy & 1) == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (destinationImage == imgs)
      {
        imgh = NewImg(imgs, true);
        imgs = imgh;
        needs_temp = true;
      }

    ReturnErrorIfFailed(MatchImg(imgs, destinationImage, dx, dy));

    int nx2 = nbhx / 2;
    int ny2 = nbhy / 2;

    // use given mask
    for (int y = 0; y < dy ; y++)
      {
        int y1 = y - ny2;
        if (y1 < 0) y1 = 0;
        int y2 = y + ny2;
        if (y2 >= dy) y2 = dy - 1;

        for (int x = 0; x < dx; x++)
          {
            int x1 = x - nx2;
            if (x1 < 0) x1 = 0;
            int x2 = x + nx2;
            if (x2 >= dx) x2 = dx - 1;
            int val = imgs->maxval;
            for (int yy = y1 ; yy <= y2; yy++)
              {
                int my = yy - y + ny2;
                for (int xx = x1; xx <= x2 ; xx++)
                  {
                    int mx = xx - x + nx2;
                    if (mask[mx + nbhx * my] != 0)
                      {
                        int aval = GetValUnchecked(imgs, xx, yy);

                        if (aval < val) val = aval;
                      }
                  }
              }
            PutValUnchecked(destinationImage, x, y, val);
          }
      }

    if (needs_temp) FreeImg(imgh);

    return OK;
  }

  int ErodeImg(const Image& sourceImage,
               int nbh, int* mask,
               const Image& destinationImage)
  {
    return ErodeImg(sourceImage, nbh, nbh, mask, destinationImage);
  }

#undef FNAME

#define FNAME "DilateImg"
  int DilateImg(const Image& sourceImage, const IMatrix& mask, const Image& destinationImage)
  {
    int nx = mask.cols();
    int ny = mask.rows();
    int* imask = new int[nx * ny];

    int i = 0;
    for (int y = 0; y < ny; y++)
      for (int x = 0; x < nx; x++)
        imask[i++] = mask[y][x];
    int rc = DilateImg(sourceImage, nx, ny, imask, destinationImage);
    delete [] imask;
    return rc;
  }

  int DilateImg(const Image& sourceImage, const Image& destinationImage, const IMatrix& mask)
  {
    return DilateImg(sourceImage, mask, destinationImage);
  }
#undef FNAME

#define FNAME "ErodeImg"
  int ErodeImg(const Image& sourceImage, const IMatrix& mask, const Image& destinationImage)
  {
    int nx = mask.cols();
    int ny = mask.rows();
    int* imask = new int[nx * ny];

    int i = 0;
    for (int y = 0; y < ny; y++)
      for (int x = 0; x < nx; x++)
        imask[i++] = mask[y][x];
    int rc = ErodeImg(sourceImage, nx, ny, imask, destinationImage);
    delete [] imask;
    return rc;
  }

  int ErodeImg(const Image& sourceImage, const Image& destinationImage, const IMatrix& mask)
  {
    return ErodeImg(sourceImage, mask, destinationImage);
  }

#undef FNAME

//--------------------------------------------------
#define FNAME "ErodeImg"
  int ErodeImgO(const Image& pn1, int sx, int sy, const Image& pn2)
  {
    // erosion in rectangular window
    // optimized with incremental calculation of min with histogram
    int x, y;

    if (pn1.maxval >= MHISTSIZE)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sx1 = sx / 2;
    int sy1 = sy / 2;
    int dx, dy;

    ReturnErrorIfFailed(MatchImg(pn1, pn2, dx, dy));

    int maxval = pn1.maxval;

    if (pn2.maxval < maxval)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    MinMaxHistogram mh(maxval);

    int** tmp = newtemp(dx, dy);

    // horizontal filtering
    for (y = 0; y < dy; y++) // all rows
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
            tmp[xd][y] = mh.getMin();
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
            tmp[xd][y] = mh.getMin();
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
            tmp[xd][y] = mh.getMin();
            mh.dec(GetValUnchecked(pn1, xlpn1, ypn1));
            //            xl++;
            xlpn1++;
            xd++;
          }
      }

    // vertical filtering
    for (x = 0; x < dx; x++) // all columns
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

    deletetemp(tmp, dx, dy);

    return OK;
  }

  int ErodeImg(const Image& sourceImage, int sx, int sy,
               const Image& destinationImage)
  {
    // Dilatation mit rechteckigen Fenster

    Image imgs = sourceImage;
    int need_temp;
    int dx, dy;
    int x, y;
    int xx, yy;
    int val;
    int* buffer = NULL;

    if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    ReturnErrorIfFailed(MatchImg(imgs, destinationImage, dx, dy));

    if (imgs.maxval < MHISTSIZE)
      return ErodeImgO(imgs, sx, sy, destinationImage);

    need_temp = (destinationImage == imgs);

    if (need_temp)
      {
        imgs = NewImg(imgs, true);
      }

    // filter rows

    for (y = 0; y < dy; y++)
      {
        for (x = 0; x < sx1; ++x)
          PutVal(destinationImage, x, y, 0);
        for (x = sx1; x < dx - sx1; x++)
          {
            val = imgs.maxval;

            for (xx = -sx1; xx <= sx1; xx++)
              val = min(val, GetVal(imgs, x + xx, y));

            PutVal(destinationImage, x, y, val);
          }
        for (x = dx - sx1; x < dx; ++x)
          PutVal(destinationImage, x, y, 0);
      }

    // filter cols
    buffer = new int[dy];

    for (x = 0; x < dx; x++)
      {
        for (y = 0; y < sy1; y++)
          buffer[y] = 0;

        for (y = sy1; y < dy - sy1; y++)
          {
            val = imgs.maxval;

            for (yy = -sy1; yy <= sy1; yy++)
              val = min(val, GetVal(destinationImage, x, y + yy));

            buffer[y] = val;
          }

        for (y = dy - sy1; y < dy; y++)
          buffer[y] = 0;

        for (y = 0; y < dy; y++)
          PutVal(destinationImage, x, y, buffer[y]);
      }

    if (need_temp) FreeImg(imgs);

    delete [] buffer;
    return OK;
  }
#undef FNAME

  int ErodeImg(const Image& img1, const Image& img2, int nx, int ny)
  {
    if (ny < 0) ny = nx;
    return ErodeImg(img1, nx, ny, img2);
  }

  int DilateImg(const Image& img1, const Image& img2, int nx, int ny)
  {
    if (ny < 0) ny = nx;
    return DilateImg(img1, nx, ny, img2);
  }

#define FNAME "OpeningImg"
  int OpeningImg(const Image& img1, const Image& img2, int nx, int ny)
  {
    if (ny < 0) ny = nx;
    ReturnErrorIfFailed(ErodeImg(img1, nx, ny, img2));
    ReturnErrorIfFailed(DilateImg(img2, nx, ny, img2));
    return OK;
  }
#undef FNAME
#define FNAME "ClosingImg"
  int ClosingImg(const Image& img1, const Image& img2, int nx, int ny)
  {
    if (ny < 0) ny = nx;
    ReturnErrorIfFailed(DilateImg(img1, nx, ny, img2));
    ReturnErrorIfFailed(ErodeImg(img2, nx, ny, img2));
    return OK;
  }
#undef FNAME

#define FNAME "OpeningImg"
  int OpeningImg(const Image& img1, const Image& img2, const IMatrix& m)
  {
    ReturnErrorIfFailed(ErodeImg(img1, img2, m));
    ReturnErrorIfFailed(DilateImg(img2, img2, m));
    return OK;
  }
#undef FNAME
#define FNAME "ClosingImg"
  int ClosingImg(const Image& img1, const Image& img2, const IMatrix& m)
  {
    ReturnErrorIfFailed(DilateImg(img1, img2, m));
    ReturnErrorIfFailed(ErodeImg(img2, img2, m));
    return OK;
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

    int** tmp1 = newtemp(dx, dy);
    int** tmp2 = newtemp(dx, dy);

    MinMaxHistogram mh(maxval);

    // horizontal filtering
    for (y = 0; y < dy; y++) // all rows
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
    for (x = 0; x < dx; x++) // alle columns (min)
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

    for (x = 0; x < dx; x++) // all columns (max)
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

    deletetemp(tmp1, dx, dy);
    deletetemp(tmp2, dx, dy);
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

    int** tmp1 = newtemp(dx, dy);
    int** tmp2 = newtemp(dx, dy);
    MinMaxHistogram mh(maxval);

    // horizontal filtering
    for (y = 0; y < dy; y++) // all rows
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
    for (x = 0; x < dx; x++) // alle columns (min)
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

    for (x = 0; x < dx; x++) // all columns (max)
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

#ifdef CONTROLLED_REFRESH
    pn2->needRefresh();
    pn3->needRefresh();
#endif
    deletetemp(tmp1, dx, dy);
    deletetemp(tmp2, dx, dy);
  }

  int MinMaxImg(const Image& pn1, int sx, int sy,
                const Image& pn2, const Image& pn3)
  {
    // dilatation and erosion in rectangular window
    // optimized with incremental calculation of max/min with histogram
    int dx, dy;

    if (pn1->maxval >= MHISTSIZE)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    ReturnErrorIfFailed(MatchImg(pn1, pn2, pn3, dx, dy));

    if ((sx < 1) || ((sx & 1) != 1) ||
        (sy < 1) || ((sy & 1) != 1) ||
        (sx >= dx) || (sy >= dy))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((pn1->maxval != pn2->maxval) ||
        (pn2->maxval != pn3->maxval))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    int ptyp = pn1->ImageType();

    if ((pn2->ImageType() != ptyp) || (pn3->ImageType() != ptyp))
      ptyp = 0; // "mixed type" == undefined

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
