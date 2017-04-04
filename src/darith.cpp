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
/*********************************************************************/
/*  Bildarithmetik für Double-Bilder                                 */
/*********************************************************************/
#include <float.h>
#include <limits.h>

#include "IceException.h"
#include "defs.h"
#include "macro.h"
#include "WindowWalker.h"
#include "based.h"
#include "numbase.h"
#include "darith.h"
#include "analygeo.h"
#include "trans.h"

namespace ice
{
  /********************************************************************/
  /* Bild loeschen                            */
  /********************************************************************/
#define FNAME "ClearImgD"
  int ClearImgD(ImageD p)
  {
    RETURN_ERROR_IF_FAILED(SetImgD(p, 0));
    return OK;
  }
#undef FNAME

  /********************************************************************/
  /* Bild definiert setzten                                     */
  /********************************************************************/

#define FNAME "SetImgD"
  int SetImgD(ImageD p, double val)
  {
    if (!p.isValid())
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_PARAM);
      }
    for (int y = 0; y < p.ysize; ++y)
      for (int x = 0; x < p.xsize; ++x)
        {
          PutValD(p, x, y, val);
        }
    return OK;
  }
#undef FNAME

#define FNAME "SmearImgD"

  int SmearImgD(ImageD pn1, ImageD pn2, int sx, int sy)
  {

    int dx, dy;
    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, dx, dy));

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    ImageD tmp = NewImgD(dx - sx + 1, dy);

    // horizontale Filterung
    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        int y1 = y;
        int x1 = 0, x2 = 0;
        double gsum = 0;

        while (x2 < sx - 1)
          {
            gsum += GetValD(pn1, x2, y1);
            x2++;
          }

        while (x2 < dx)
          {
            gsum += GetValD(pn1, x2, y1);
            PutValD(tmp, x1, y, gsum);
            gsum -= GetValD(pn1, x1, y1);
            x1++;
            x2++;
          }
      }

    // vertikale Filterung
    for (int x = 0; x < sx1; x++)
      for (int y = 0; y < dy; y++)
        {
          PutValD(pn2, x, y, 0);
        }

    for (int x = 0; x < dx - sx + 1; x++)   // alle spalten
      {
        int y1 = 0, y2 = 0;
        int yt = 0;
        double gsum = 0;

        while (y2 < sy - 1)
          {
            gsum += GetValD(tmp, x, y2);
            y2++;
          }

        while (yt < sy1)
          {
            PutValD(pn2, x + sx1, yt, 0);
            yt++;
          }

        while (y2 < dy)
          {
            gsum += GetValD(tmp, x, y2);
            PutValD(pn2, x + sx1, yt, gsum);
            gsum -= GetValD(tmp, x, y1);
            y1++;
            y2++;
            yt++;
          }

        while (yt < dy)
          {
            PutValD(pn2, x + sx1, yt, 0);
            yt++;
          }
      }

    for (int x = dx - sx1; x < dx; x++)
      for (int y = 0; y < dy; y++)
        {
          PutValD(pn2, x, y, 0);
        }

    //    FreeImgD(tmp);
    return OK;
  }

  int SmearImgD(const ImageD src, ImageD dest, int nx)
  {
    return SmearImgD(src, dest, nx, nx);
  }

#undef FNAME

  /*********************************************************************/
  /* Bildaddition                  */
  /*********************************************************************/
#define FNAME "AddImgD"
  int AddImgD(ImageD pn1, ImageD pn2, ImageD pn3)
  {
    int i, j, hx, hy;

    if ((!IsImgD(pn1)) || (!IsImgD(pn2)) || (!IsImgD(pn3)))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, pn3, hx, hy));

    for (i = 0; i < hx; i++)
      for (j = 0; j < hy; j++)
        {
          PutValD(pn3, i, j, GetValD(pn1, i, j) + GetValD(pn2, i, j));
        }

    return OK;
  }
#undef FNAME

  /*********************************************************************/
  /* BildMultiplikation                */
  /*********************************************************************/
#define FNAME "MulImgD"
  int MulImgD(ImageD pn1, ImageD pn2, ImageD pn3)
  {
    int i, j, hx, hy;

    if ((!IsImgD(pn1)) || (!IsImgD(pn2)) || (!IsImgD(pn3)))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, pn3, hx, hy));

    for (i = 0; i < hx; i++)
      for (j = 0; j < hy; j++)
        {
          PutValD(pn3, i, j, GetValD(pn1, i, j)*GetValD(pn2, i, j));
        }

    return OK;
  }
#undef FNAME

  /*********************************************************************/
  /* Bild transportieren                   */
  /*********************************************************************/
#define FNAME "MoveImgD"
  int MoveImgD(ImageD pn1, ImageD pn2)
  {
    int i, j, hx, hy;

    if ((!IsImgD(pn1)) || (!IsImgD(pn2)))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, hx, hy));

    for (i = 0; i < hx; i++)
      for (j = 0; j < hy; j++)
        {
          PutValD(pn2, i, j, GetValD(pn1, i, j));
        }

    return OK;
  }
#undef FNAME

#define FNAME "findMax"
  double findMax(const ImageD& img, int& PosX, int& PosY)
  {
    if (! IsImgD(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
      }

    int xSize = img.xsize;
    int ySize = img.ysize;

    double max = img.getPixelUnchecked(0, 0);

    for (int y = 0; y < ySize; y++)
      for (int x = 0; x < xSize; x++)
        {
          double g = img.getPixelUnchecked(x, y);
          if (g > max)
            {
              max = g;
              PosX = x;
              PosY = y;
            }
        }
    return max;
  }

  IPoint findMax(const ImageD& imgD, const Image& mark)
  {
    IPoint res(-1, -1); // point outside == not found
    if (! IsImgD(imgD) || ! IsImg(mark))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
      }

    int xSize = imgD.xsize;
    int ySize = imgD.ysize;

    if (xSize != mark.xsize || ySize != mark.ysize)
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);

      }

    double max = -DBL_MAX;

    WindowWalker w(imgD);
    for (w.init(); !w.ready(); w.next())
      {
        if (mark.getPixel(w) == 0)
          {
            double g = imgD.getPixelUnchecked(w);
            if (g > max)
              {
                max = g;
                res = w;
              }
          }
      }
    return res;
  }

  double findMax(const ImageD& img, IPoint& p)
  {
    return findMax(img, p.x, p.y);
  }

  double findMax(const ImageD& img)
  {
    int x, y;
    return findMax(img, x, y);
  }
#undef FNAME

#define FNAME "findMin"
  double findMin(const ImageD& img, int& PosX, int& PosY)
  {
    if (! IsImgD(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
      }

    int xSize = img.xsize;
    int ySize = img.ysize;

    double min = img.getPixelUnchecked(0, 0);

    for (int y = 0; y < ySize; y++)
      for (int x = 0; x < xSize; x++)
        {
          double g = img.getPixelUnchecked(x, y);
          if (g < min)
            {
              min = g;
              PosX = x;
              PosY = y;
            }
        }

    return min;
  }

  double findMin(const ImageD& img, IPoint& p)
  {
    return findMin(img, p.x, p.y);
  }

  double findMin(const ImageD& img)
  {
    int x, y;
    return findMin(img, x, y);
  }
#undef FNAME

#define FNAME "LogPolarImgD"
  int LogPolarImgD(ImageD imgs, ImageD imgd, double r1, double r2, int sym)
  {
    int hx, hy;
    double xs, ys;
    int dsx, dsy;
    int x0, y0;
    double r, fi, p[2];
    double rfac;

    if ((!IsImgD(imgs)) || (!IsImgD(imgd)))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    hx = imgs.xsize;
    hy = imgs.ysize;

    x0 = hx / 2;
    y0 = hy / 2;

    dsx = imgd.xsize;
    dsy = imgd.ysize;

    if (r2 == 0.0)
      {
        r2 = Min(hx, hy) / 2.0;
      }

    rfac = (log(r2) - log(r1)) / dsy;

    for (int y = 0; y < imgd.ysize; ++y)
      for (int x = 0; x < imgd.xsize; ++x)
        {
          fi = (x) * M_PI * 2 / sym / dsx;
          r = exp((y) * rfac) * r1;
          ConvPolarCartes(r, fi, p);
          xs = p[0] + x0;
          ys = p[1] + y0;

          if (imgs.inside(xs, ys))
            {
              PutValD(imgd, x, y, GetInterpolValD(imgs, xs, ys));
            }
          else
            {
              PutValD(imgd, x, y, 0.0);
            }
        }
    return OK;
  }
#undef FNAME
#define FNAME "LogPolarImg"
  int LogPolarImg(Image imgs, Image imgd,
                  double r1, double r2, int sym)
  {
    Image himgd;
    int hx, hy;
    double xs, ys;
    int dsx, dsy;
    int x0, y0;
    double r, fi, p[2];
    double rfac;

    if (!IsImg(imgs))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    hx = imgs.xsize;
    hy = imgs.ysize;

    x0 = (hx) / 2;
    y0 = (hy) / 2;

    if (!IsImg(imgd))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }

    himgd = imgd;

    dsx = himgd.xsize;
    dsy = himgd.ysize;

    if (r2 == 0.0)
      {
        r2 = Min(hx, hy) / 2.0;
      }

    rfac = (log(r2) - log(r1)) / dsy;

    for (int y = 0; y < himgd.ysize; ++y)
      for (int x = 0; x < himgd.xsize; ++x)
        {
          fi = x * M_PI * 2 / sym / dsx;
          r = exp(y * rfac) * r1;
          ConvPolarCartes(r, fi, p);
          xs = p[0] + x0;
          ys = p[1] + y0;

          if (
            (xs >= 0) && (xs < imgs->xsize) &&
            (ys >= 0) && (ys < imgs->ysize)
          )
            {
              PutVal(himgd, x, y, (int)GetInterpolVal(imgs, xs, ys));
            }
          else
            {
              PutVal(himgd, x, y, 0);
            }
        }
    return OK;
  }
#undef FNAME
#define FNAME "LogPolarC"
  int LogPolarC(ImageD imgs, ImageD imgd,
                double x, double y, double& r, double& fi,
                double r1, double r2, int sym)
  {
    int dsx, dsy;
    double rfac;

    if (! imgs.isValid())
      {
        // if no source is given then r1 and r2 must be given
        if (r2 == 0)
          {
            throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
          }
      }
    else
      {
        if (!IsImgD(imgs))
          {
            throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
          }
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    if (!IsImgD(imgd))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    if (r2 == 0)
      {
        r2 = Min(imgs.xsize, imgs.ysize) / 2.0;
      }

    dsx = imgd.xsize;
    dsy = imgd.ysize;

    rfac = (log(r2) - log(r1)) / dsy;

    fi = (x) * M_PI * 2 / sym / dsx;
    r = exp((y) * rfac) * r1;

    return OK;
  }
#undef FNAME
//
#define FNAME "PolarImgD"
  ImageD PolarImgD(ImageD imgs, ImageD imgd, double r1, double r2, int sym)
  {
    ImageD himgd;
    int hx, hy;
    double xs, ys;
    int dsx, dsy;
    int x0, y0;
    double r, fi, p[2];
    double rfac;

    if (!IsImgD(imgs))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    hx = imgs.xsize;
    hy = imgs.ysize;

    x0 = hx / 2;
    y0 = hy / 2;

    if (! imgd.isValid())
      {
        /* Neues Bild anlegen */
        himgd = NewImgD(hx, hy, imgs.minval, imgs.maxval);
      }
    else
      {
        himgd = imgd;
      }

    dsx = himgd.xsize;
    dsy = himgd.ysize;

    if (r2 == 0.0)
      {
        r2 = Min(hx, hy) / 2.0;
      }

    rfac = (r2 - r1) / dsy;

    for (int y = 0; y < himgd.ysize; ++y)
      for (int x = 0; x < himgd.xsize; ++x)
        {
          fi = (x) * M_PI * 2 / sym / dsx;
          r = (y) * rfac + r1;
          ConvPolarCartes(r, fi, p);
          xs = p[0] + x0;
          ys = p[1] + y0;

          if (
            (xs >= 0) && (xs < imgs.xsize) &&
            (ys >= 0) && (ys < imgs.ysize)
          )
            {
              PutValD(himgd, x, y, GetInterpolValD(imgs, xs, ys));
            }
          else
            {
              PutValD(himgd, x, y, 0.0);
            }
        }
    return himgd;
  }
#undef FNAME
#define FNAME "PolarImg"
  int PolarImg(Image imgs, Image imgd,
               double r1, double r2, int sym)
  {
    Image himgd;
    int hx, hy;
    double xs, ys;
    int dsx, dsy;
    int x0, y0;
    double r, fi, p[2];
    double rfac;

    if (!IsImg(imgs))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    hx = imgs->xsize;
    hy = imgs->ysize;

    x0 = hx / 2;
    y0 = hy / 2;

    if (!IsImg(imgd))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }

    himgd = imgd;

    dsx = himgd->xsize;
    dsy = himgd->ysize;

    if (r2 == 0.0)
      {
        r2 = Min(hx, hy) / 2.0;
      }

    rfac = (r2 - r1) / dsy;

    for (int y = 0; y < himgd.ysize; y++)
      for (int x = 0; x < himgd.xsize; x++)
        {
          fi = (x) * M_PI * 2 / sym / dsx;
          r = (y) * rfac + r1;
          ConvPolarCartes(r, fi, p);
          xs = p[0] + x0;
          ys = p[1] + y0;

          if (
            (xs >= 0) && (xs < imgs->xsize) &&
            (ys >= 0) && (ys < imgs->ysize)
          )
            {
              PutVal(himgd, x, y, (int)GetInterpolVal(imgs, xs, ys));
            }
          else
            {
              PutVal(himgd, x, y, 0);
            }
        }
    return OK;
  }
#undef FNAME
#define FNAME "PolarC"
  int PolarC(ImageD imgs, ImageD imgd,
             double x, double y, double& r, double& fi,
             double r1, double r2, int sym)
  {
    int dsx, dsy;
    double rfac;

    if (!imgs.isValid())
      {
        // if no source is given then r1 and r2 must be given
        if (r2 == 0)
          {
            throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
          }
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    if (!IsImgD(imgd))
      {
        throw IceException(FNAME, M_WRONG_IMAGED, WRONG_POINTER);
      }

    if (r2 == 0)
      {
        r2 = Min(imgs.xsize, imgs.ysize) / 2.0;
      }

    dsx = imgd.xsize;
    dsy = imgd.ysize;

    rfac = (r2 - r1) / dsy;

    fi = x * M_PI * 2 / sym / dsx;
    r = y * rfac + r1;
    return OK;
  }
#undef FNAME
}
