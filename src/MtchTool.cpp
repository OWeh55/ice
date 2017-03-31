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
// 1998 - Torsten Baumbach

#include <float.h>
#include <limits.h>

#include "contlist.h"
#include "macro.h"

#include "MtchTool.h"
#include "DPList.h"
#include "hist.h"

namespace ice
{
  /*****************************************************************************/
  /*****************************************************************************/

  double Gauss2D(double x, double y, double xm, double ym,
                 double s1, double s2, double s12)
  {

    double f = (1 - s12 * s12);
    double xx = x - xm;
    double yy = y - ym;

    return 1 / ((M_PI + M_PI) * s1 * s2 * sqrt(f)) *
           exp(-1 / (f + f) * ((xx * xx) / (s1 * s1) - 2 * s12 * xx * yy / (s1 * s2) + (yy * yy) / (s2 * s2)));
  }

  /*****************************************************************************/
  /*****************************************************************************/

  void Get2DGaussParamsD(ImageD imgd, Image imgo,
                         double& x0, double& y0,
                         double& sx, double& sy, double& sxy,
                         double& a, double& b)
  {

    double g, volume = 0, gmax = -1, gmin = DBL_MAX;

    for (int y = 0; y < imgo.ysize; y++)
      for (int x = 0; x < imgo.xsize; x++)
        {

          if (GetVal(imgo, x, y))
            {
              g = GetValD(imgd, x, y);

              if (gmax < g)
                {
                  gmax = g;
                }

              if (gmin > g)
                {
                  gmin = g;
                }
            }
        }

    for (int y = 0; y < imgo.ysize; y++)
      for (int x = 0; x < imgo.xsize; x++)
        {
          if (GetVal(imgo, x, y))
            {
              volume += (GetValD(imgd, x, y) - gmin);
            }
        }

    if (x0 == -1 || y0 == -1)   // Mittelwert bestimmen
      {
        x0 = 0;
        y0 = 0;

        for (int y = 0; y < imgo.ysize; y++)
          for (int x = 0; x < imgo.xsize; x++)
            {
              if (GetVal(imgo, x, y))
                {
                  x0 += (g = (GetValD(imgd, x, y) - gmin)) * x;
                  y0 += g * y;
                }
            }
        if (volume != 0)
          {
            x0 /= volume;
            y0 /= volume;
          }
        else
          {
            x0 = imgo->xsize / 2;
            y0 = imgo->ysize / 2;
          }
      }

    sx = sy = sxy = 0;

    for (int y = 0; y < imgo.ysize; y++)
      for (int x = 0; x < imgo.xsize; x++)
        {
          if (GetVal(imgo, x, y))
            {
              sx += Sqr(x - x0) * (g = GetValD(imgd, x, y) - gmin);
              sy += Sqr(y - y0) * g;
              sxy += (x - x0) * (y - y0) * g;
            }
        }

    if (volume != 0)
      {
        sx /= volume;
        sx = sqrt(sx);
        sy /= volume;
        sy = sqrt(sy);
        sxy /= volume * sx * sy;
      }
    else
      {
        sx = DBL_MAX;
        sy = DBL_MAX;
        sxy = 0;
      }

    double max = -DBL_MAX, min = DBL_MAX;

    for (int y = 0; y < imgo.ysize; y++)
      for (int x = 0; x < imgo.xsize; x++)
        {
          if (GetVal(imgo, x, y))
            {
              double f = Gauss2D(x, y, x0, y0, sx, sy, sxy);
              if (max < f)
                {
                  max = f;
                }
              if (min > f)
                {
                  min = f;
                }
            }
        }

    if (max > min)
      {
        a = (gmax - gmin) / (max - min);
      }
    else
      {
        a = 1;
      }

    b = -min * a;
  }

  /****************************************************************************/
  /****************************************************************************/
#if 0
#define FNAME "AmplitudeImgD"

  ImageD AmplitudeImgD(ImageD re, ImageD im, ImageD dest, int smear)
  {
    if (re == NULL || im == NULL ||
        re->xsize != im->xsize || re->ysize != im->ysize)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (dest == NULL)
      {
        dest = NewImgD(re->xsize, re->ysize, -DBL_MAX, DBL_MAX);

        if (dest == NULL)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return NULL;
          }
      }
    else if (dest->xsize != re->xsize || dest->ysize != re->ysize)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }

    if (smear)
      {
        int x, y;
        wloop(re, x, y)
        {
          double g1 = sqrt(Sqr(GetValD(re, (x - 1 + re->xsize) % re->xsize, (y - 1 + re->ysize) % re->ysize)) + Sqr(GetValD(im, (x - 1 + re->xsize) % re->xsize, (y - 1 + re->ysize) % re->ysize)));
          double g2 = sqrt(Sqr(GetValD(re, (x - 1 + re->xsize) % re->xsize, y)) +  Sqr(GetValD(im, (x - 1 + re->xsize) % re->xsize, y)));
          double g3 = sqrt(Sqr(GetValD(re, (x - 1 + re->xsize) % re->xsize, (y + 1 + re->ysize) % re->ysize)) + Sqr(GetValD(im, (x - 1 + re->xsize) % re->xsize, (y + 1 + re->ysize) % re->ysize)));
          double g4 = sqrt(Sqr(GetValD(re, (x + 1 + re->xsize) % re->xsize, (y - 1 + re->ysize) % re->ysize)) + Sqr(GetValD(im, (x + 1 + re->xsize) % re->xsize, (y - 1 + re->ysize) % re->ysize)));
          double g5 = sqrt(Sqr(GetValD(re, (x + 1 + re->xsize) % re->xsize, y)) +  Sqr(GetValD(im, (x + 1 + re->xsize) % re->xsize, y)));
          double g6 = sqrt(Sqr(GetValD(re, (x + 1 + re->xsize) % re->xsize, (y + 1 + re->ysize) % re->ysize)) + Sqr(GetValD(im, (x + 1 + re->xsize) % re->xsize, (y + 1 + re->ysize) % re->ysize)));
          double g7 = sqrt(Sqr(GetValD(re, x, (y - 1 + re->ysize) % re->ysize)) +  Sqr(GetValD(im, x, (y - 1 + re->ysize) % re->ysize)));
          double g8 = sqrt(Sqr(GetValD(re, x, y)) +    Sqr(GetValD(im, x, y)));
          double g9 = sqrt(Sqr(GetValD(re, x, (y + 1 + re->ysize) % re->ysize)) +  Sqr(GetValD(im, x, (y + 1 + re->ysize) % re->ysize)));
          PutValD(dest, x, y, (g1 + g2 + g3 + g4 + g5 + g6 + g7 + g8 + g9 + g9) / 10);
        }
      }
    else
      {
        int x, y;
        wloop(re, x, y) PutValD(dest, x, y, sqrt(Sqr(GetValD(re, x, y)) + Sqr(GetValD(im, x, y))));
      }

    return dest;
  }
#undef FNAME
#endif
  /************************************************************************/
  /************************************************************************/
#define FNAME "CutFrameImg"

  int CutFrameImg(Image img, int dx, int dy, int mode, Image dest)
  {

    if (
      (!IsImg(img)) || (!IsImg(dest)) ||
      dy >= img->xsize / 2 || dy >= img->ysize / 2 ||
      dx < 0 || dy < 0
    )
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (mode == CF_CUT)
      {

        int x, y;
        wloop(dest, x, y)
        PutVal(dest, x, y, GetVal(img, x + dx, y + dy));
      }
    else if (mode == CF_SET)
      {
        int x, y;
        wloop(dest, x, y)
        PutVal(dest, x, y, GetVal(img, x - dest->xsize / 2 + img->xsize / 2, y - dest->ysize / 2 + img->ysize / 2));
      }
    else
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return OK;
  }
#undef FNAME

//
//

#define FNAME "EntropyImg"

  double EntropyImg(Image img)
  {
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    double entropy = 0.0;
    double p;
    double l2 = log(2.0);
    int i;

    Hist h = HistImg(img);

    for (i = 1; i <= h.classes(); i++)
      {
        p = h.Rel(i);

        if (p > 0)
          {
            entropy -= p * log(p) / l2;
          }
      }

    return entropy;
  }

#undef FNAME

//
//

#define FNAME "EntropyImgD"

  double EntropyImgD(ImageD img, double& maxentro)
  {
    if (!img.isValid())
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return -1;
      }

    double entropy = 0.0;
    int i, j, g;

    double hist[256];
    memset(hist, 0, 256 * sizeof(double));
    maxentro = 0;

    for (j = 0; j < img.ysize; ++j)
      for (i = 0; i < img.xsize; ++i)
        {
          g = Min(Max((int)(GetValD(img, i, j) * 255), 0), 255);

          if (hist[g] == 0)
            {
              maxentro++;
            }

          hist[g]++;
        }

    double anz = (img.xsize) * (img.ysize);
    double l2 = log(2.0);

    for (i = 0; i < 256; i++)
      {
        hist[i] = (double)hist[i] / anz;

        if (hist[i] > 0)
          {
            entropy -= (double)hist[i] * log((double)hist[i]) / l2;
          }
      }

    maxentro = (log(Max(maxentro - 1.0, 1.0)) / log(2.0));

    return entropy;
  }

#undef FNAME

  /********************************************************************************/
  /********************************************************************************/

#define FNAME "BinObj2ConturList"

  static const int dirx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
  static const int diry[8] = {0, 1, 1, 1, 0, -1, -1, -1};

  ConturList BinObj2ConturList(Image img, int sx, int sy)
  {

    ConturList clist;

    if (!IsImg(img) || sx < 0 || sy < 0 || sx >= img->xsize || sy >= img->ysize)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return ConturList();
      }

    int weiter = true;

    Image mark = NewImg(img->xsize, img->ysize, 1);

    if (!IsImg(mark))
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return ConturList();
      }

    ClearImg(mark);

    do
      {

        int x, y, xs, ys, d, x0 = -1, y0 = -1, dir0 = 0, dir = 0, flag;
        wloop(img, x, y)
        {
          if (GetVal(img, x, y) && !GetVal(mark, x, y) &&
              (!img.getPixelClipped(x - 1, y) ||
               !img.getPixelClipped(x  , y - 1) ||
               !img.getPixelClipped(x  , y + 1) ||
               !img.getPixelClipped(x + 1, y)))
            {
              x0 = x;
              y0 = y;
              x = y = 10000;
            }
        }

        if (x0 == -1)
          {
            break;  // Kein Startpunkt mehr gefunden
          }

        // Startrichtung suchen
        for (d = 0; d < 8; d += 2)
          {
            if (!img.getPixelClipped(x0 + dirx[d], y0 + diry[d]))
              {
                dir = d;
                break;
              }
          }

        DPointList pl = NewDPointList();

        if (pl == NULL)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return ConturList();
          }

        x = x0;
        y = y0;

        do
          {

            xs = x;
            ys = y;
            PutVal(mark, xs, ys, 1);

            DAddPoint(pl, xs, ys, dir);

            flag = false;

            for (d = dir; d < dir + 8; d++)
              {
                x = xs + dirx[d % 8];
                y = ys + diry[d % 8];

                if (img.getPixelClipped(x, y))
                  {
                    if (!img.getPixelClipped(x - 1, y) || !img.getPixelClipped(x, y - 1) ||
                        !img.getPixelClipped(x + 1, y) || !img.getPixelClipped(x, y + 1) ||
                        !img.getPixelClipped(x - 1, y - 1) || !img.getPixelClipped(x + 1, y - 1) ||
                        !img.getPixelClipped(x + 1, y + 1) || !img.getPixelClipped(x - 1, y + 1))
                      {
                        flag = true;
                        break;
                      }
                  }
              }

            if (!flag)
              {
                break;
              }

            dir = d % 8;

            if (pl->lng == 1)
              {
                dir0 = dir;
              }
            else if (xs == x0 && ys == y0 && dir == dir0)
              {
                break;
              }

            dir = (dir + 6) % 8;

          }
        while (1);

        PointList plnrm = NewPointList(pl->lng);

        if (plnrm == NULL)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            FreePointList(pl);
            return ConturList();
          }

        for (int pln = 0; pln < pl->lng; pln++)
          {
            PutPoint(plnrm, (pl->lng - 1 - pln), pl->xptr[pln], pl->yptr[pln], 1);
          }

        FreePointList(pl);

        Contur c = PolygonContur(plnrm);
        FreePointList(plnrm);
        clist.Add(c);

      }
    while (weiter);

    FreeImg(mark);
    return clist;
  }
}
#undef FNAME
