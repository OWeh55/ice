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
#define FNAME "clearImgD"
  void clearImgD(ImageD p)
  {
    setImgD(p, 0);
  }
#undef FNAME

  /********************************************************************/
  /* Bild definiert setzten                                     */
  /********************************************************************/

#define FNAME "setImgD"
  void setImgD(ImageD p, double val)
  {
    if (!p.isValid())
      throw IceException(FNAME, M_WRONG_IMAGED);
    WindowWalker w(p);
    for (w.init(); !w.ready(); w.next())
      {
        p.setPixel(w, val);
      }
  }
#undef FNAME

#define FNAME "SmearImgD"
  void smearImgD(ImageD pn1, ImageD pn2, int sx, int sy)
  {
    int xSize, ySize;
    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, xSize, ySize));

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    ImageD tmp = NewImgD(xSize - sx + 1, ySize);

    // horizontale Filterung
    for (int y = 0; y < ySize; y++)   // alle zeilen
      {
        int y1 = y;
        int x1 = 0, x2 = 0;
        double gsum = 0;

        while (x2 < sx - 1)
          {
            gsum += GetValD(pn1, x2, y1);
            x2++;
          }

        while (x2 < xSize)
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
      for (int y = 0; y < ySize; y++)
        {
          PutValD(pn2, x, y, 0);
        }

    for (int x = 0; x < xSize - sx + 1; x++)   // alle spalten
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

        while (y2 < ySize)
          {
            gsum += GetValD(tmp, x, y2);
            PutValD(pn2, x + sx1, yt, gsum);
            gsum -= GetValD(tmp, x, y1);
            y1++;
            y2++;
            yt++;
          }

        while (yt < ySize)
          {
            PutValD(pn2, x + sx1, yt, 0);
            yt++;
          }
      }

    for (int x = xSize - sx1; x < xSize; x++)
      for (int y = 0; y < ySize; y++)
        {
          PutValD(pn2, x, y, 0);
        }
  }

  void smearImgD(const ImageD src, ImageD dest, int nx)
  {
    return smearImgD(src, dest, nx, nx);
  }

#undef FNAME

  /*********************************************************************/
  /* Bildaddition                  */
  /*********************************************************************/
#define FNAME "addImgD"
  void addImgD(ImageD pn1, ImageD pn2, ImageD pn3)
  {
    if ((!IsImgD(pn1)) || (!IsImgD(pn2)) || (!IsImgD(pn3)))
      throw IceException(FNAME, M_WRONG_IMAGED);

    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, pn3));

    WindowWalker w(pn1);
    for (w.init(); !w.ready(); w.next())
      {
        pn3.setPixel(w, pn1.getPixel(w) + pn2.getPixel(w));
      }
  }
#undef FNAME

  /*********************************************************************/
  /* BildMultiplikation                */
  /*********************************************************************/
#define FNAME "MulImgD"
  void mulImgD(ImageD pn1, ImageD pn2, ImageD pn3)
  {
    if ((!IsImgD(pn1)) || (!IsImgD(pn2)) || (!IsImgD(pn3)))
      throw IceException(FNAME, M_WRONG_IMAGED);

    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2, pn3));

    WindowWalker w(pn1);
    for (w.init(); !w.ready(); w.next())
      {
        pn3.setPixel(w, pn1.getPixel(w) * pn2.getPixel(w));
      }
  }
#undef FNAME

  /*********************************************************************/
  /* Bild transportieren                   */
  /*********************************************************************/
#define FNAME "MoveImgD"
  int MoveImgD(ImageD pn1, ImageD pn2)
  {
    if ((!IsImgD(pn1)) || (!IsImgD(pn2)))
      throw IceException(FNAME, M_WRONG_IMAGED);

    RETURN_ERROR_IF_FAILED(MatchImgD(pn1, pn2));

    WindowWalker w(pn1);
    for (w.init(); !w.ready(); w.next())
      {
        pn2.setPixel(w, pn1.getPixel(w));
      }
    return OK;
  }
#undef FNAME

#define FNAME "findMax"
  double findMax(const ImageD& img, int& PosX, int& PosY)
  {
    if (! IsImgD(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

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
      throw IceException(FNAME, M_WRONG_IMAGE);

    int xSize = imgD.xsize;
    int ySize = imgD.ysize;

    if (xSize != mark.xsize || ySize != mark.ysize)
      {
        throw IceException(FNAME, M_WRONG_IMAGE);
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
      throw IceException(FNAME, M_WRONG_IMAGE);

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
      throw IceException(FNAME, M_WRONG_IMAGED);

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    hx = imgs.xsize;
    hy = imgs.ysize;

    x0 = hx / 2;
    y0 = hy / 2;

    dsx = imgd.xsize;
    dsy = imgd.ysize;

    if (r2 == 0.0)
      {
        r2 = std::min(hx, hy) / 2.0;
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
      throw IceException(FNAME, M_WRONG_IMAGED);

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    hx = imgs.xsize;
    hy = imgs.ysize;

    x0 = (hx) / 2;
    y0 = (hy) / 2;

    if (!IsImg(imgd))
      throw IceException(FNAME, M_WRONG_IMAGE);

    himgd = imgd;

    dsx = himgd.xsize;
    dsy = himgd.ysize;

    if (r2 == 0.0)
      {
        r2 = std::min(hx, hy) / 2.0;
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
          throw IceException(FNAME, M_WRONG_PARAMETER);
      }
    else
      {
        if (!IsImgD(imgs))
          throw IceException(FNAME, M_WRONG_IMAGED);
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (!IsImgD(imgd))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if (r2 == 0)
      {
        r2 = std::min(imgs.xsize, imgs.ysize) / 2.0;
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
    int dsx, dsy;
    int x0, y0;
    double r, fi, p[2];
    double rfac;

    if (!IsImgD(imgs))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    double hx = imgs.xsize;
    double hy = imgs.ysize;

    x0 = hx / 2;
    y0 = hy / 2;

    ImageD himgd;
    if (! imgd.isValid())
      {
        /* Neues Bild anlegen */
        himgd.create(imgs);
      }
    else
      {
        himgd = imgd;
      }

    dsx = himgd.xsize;
    dsy = himgd.ysize;

    if (r2 == 0.0)
      {
        r2 = std::min(hx, hy) / 2.0;
      }

    rfac = (r2 - r1) / dsy;

    for (int y = 0; y < himgd.ysize; ++y)
      for (int x = 0; x < himgd.xsize; ++x)
        {
          fi = (x) * M_PI * 2 / sym / dsx;
          r = (y) * rfac + r1;
          ConvPolarCartes(r, fi, p);
          double xs = p[0] + x0;
          double ys = p[1] + y0;

          if (
            (xs >= 0) && (xs < imgs.xsize) &&
            (ys >= 0) && (ys < imgs.ysize)
          )
            {
              himgd.setPixel(x, y, imgs.getPixelInterpol(xs, ys));
            }
          else
            {
              himgd.setPixel(x, y, 0.0);
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
      throw IceException(FNAME, M_WRONG_IMAGED);

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    hx = imgs->xsize;
    hy = imgs->ysize;

    x0 = hx / 2;
    y0 = hy / 2;

    if (!IsImg(imgd))
      throw IceException(FNAME, M_WRONG_IMAGE);

    himgd = imgd;

    dsx = himgd->xsize;
    dsy = himgd->ysize;

    if (r2 == 0.0)
      {
        r2 = std::min(hx, hy) / 2.0;
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
          throw IceException(FNAME, M_WRONG_PARAMETER);
      }

    if ((sym < 1) || (r1 <= 0.0) || (r2 < 0.0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (!IsImgD(imgd))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if (r2 == 0)
      {
        r2 = std::min(imgs.xsize, imgs.ysize) / 2.0;
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
