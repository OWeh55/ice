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
/*************************************************************************
 *  Feinanpassung von Linien an Grauwert- bzw. Gradientenprofil          *
 *                                                                       *
 *  (C) m.schubert 11'94                                                 *
 *************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "analygeo.h"
#include "pointlist.h"
#include "fit_pointlist.h"
#include "Contur.h"
#include "contfeat.h"
#include "contools.h"

namespace ice
{
  /*************************************************************************/
#define FNAME "FitGrayLine"
  double FitGrayLine(Image img, double lp[2][2], int dist, double* p, double* phi)
  {
    int cp[4][2];
    Contur c;
    PointList pl;
    int i, cnt;
    double cc, ss;
    double par[2], mean = 0;
    double md;
    int ma;
    int thr;
    /*if((lp[0][0]<img->wxi+dist)||(lp[0][0]>img->wxa-dist)) return(-1);
      if((lp[0][1]<img->wyi+dist)||(lp[0][1]>img->wya-dist)) return(-1);
      if((lp[1][0]<img->wxi+dist)||(lp[1][0]>img->wxa-dist)) return(-1);
      if((lp[1][1]<img->wyi+dist)||(lp[1][1]>img->wya-dist)) return(-1);*/

    ConvPointHesse(lp[0], lp[1], p, phi);

    cc = cos(*phi);
    ss = sin(*phi);
    cp[0][0] = (int)(lp[0][0] + cc * dist + 0.5);
    cp[0][1] = (int)(lp[0][1] + ss * dist + 0.5);
    cp[1][0] = (int)(lp[0][0] - cc * dist + 0.5);
    cp[1][1] = (int)(lp[0][1] - ss * dist + 0.5);
    cp[2][0] = (int)(lp[1][0] - cc * dist + 0.5);
    cp[2][1] = (int)(lp[1][1] - ss * dist + 0.5);
    cp[3][0] = (int)(lp[1][0] + cc * dist + 0.5);
    cp[3][1] = (int)(lp[1][1] + ss * dist + 0.5);
    c = LineContur(cp[0], cp[1]);
    c.Add(cp[2][0], cp[2][1]);
    c.Add(cp[3][0], cp[3][1]);
    c.Add(cp[0][0], cp[0][1]);

    pl = RegionPointList(c);

    for (i = 0; i < pl->lng; i++)
      {
        pl->wptr[i] = GetVal(img, (int)pl->xptr[i], (int)pl->yptr[i]);
        mean += pl->wptr[i];
      }

    if (mean == 0)
      {
        FreePointList(pl);
        throw IceException(FNAME, M_WRONG_POINTLIST);
      }

    thr = (int)(mean / (double)pl->lng + 0.5);
    mean = 0;
    cnt = 0;

    for (i = 0; i < pl->lng; i++)
      {
        if (pl->wptr[i] < thr)
          {
            pl->wptr[i] = 0;
          }
        else
          {
            mean += pl->wptr[i];
            cnt++;
          }
      }

    try
      {
        FitLine(pl, 0, pl->lng - 1, 3, par, &md, &ma);
      }
    catch (IceException& ex)
      {
        FreePointList(pl);
        throw IceException(ex, FNAME);
      }

    *p = par[0];
    *phi = par[1];
    FreePointList(pl);
    return (mean / cnt);
  }
#undef FNAME
  /*************************************************************************/
#define FNAME "FitGradLine"
  double FitGradLine(Image img, double lp[2][2], int dist, double* p, double* phi)
  {
    int cp[4][2];
    Contur c;
    PointList pl;
    int x1, x2, y1, y2, g;
    int i, x, y;
    double cc, ss;
    double par[2], mean = 0, gew;
    double md;
    int ma, cnt;
    double thr;

    if (!
        (Inside(img, (int)lp[0][0], (int)lp[0][1]) &&
         Inside(img, (int)lp[1][0], (int)lp[1][1]))
       )
      {
        return -1;
      }

    ConvPointHesse(lp[0], lp[1], p, phi);

    cc = cos(*phi);
    ss = sin(*phi);

    cp[0][0] = (int)(lp[0][0] + cc * dist + 0.5);
    cp[0][1] = (int)(lp[0][1] + ss * dist + 0.5);
    cp[1][0] = (int)(lp[0][0] - cc * dist + 0.5);
    cp[1][1] = (int)(lp[0][1] - ss * dist + 0.5);
    cp[2][0] = (int)(lp[1][0] - cc * dist + 0.5);
    cp[2][1] = (int)(lp[1][1] - ss * dist + 0.5);
    cp[3][0] = (int)(lp[1][0] + cc * dist + 0.5);
    cp[3][1] = (int)(lp[1][1] + ss * dist + 0.5);

    c = LineContur(cp[0], cp[1]);
    c.Add(cp[2][0], cp[2][1]);
    c.Add(cp[3][0], cp[3][1]);
    c.Add(cp[0][0], cp[0][1]);
    pl = RegionPointList(c);
    mean = 0;

    for (i = 0; i < pl->lng; i++)
      {
        x = (int)(pl->xptr[i] - 1);
        y = (int)(pl->yptr[i] - 1);
        g = GetVal(img, x, y);
        x1 = g;
        y1 = g;
        x++;
        y1 += GetVal(img, x, y);
        x++;
        g = GetVal(img, x, y);
        y1 += g;
        x2 = g;
        y++;
        x2 += GetVal(img, x, y);
        y++;
        g = GetVal(img, x, y);
        x2 += g;
        y2 = g;
        x--;
        y2 += GetVal(img, x, y);
        x--;
        g = GetVal(img, x, y);
        y2 += g;
        x1 += g;
        y--;
        x1 += GetVal(img, x, y);
        gew = fabs((x2 - x1) * ss - (y2 - y1) * cc);
        pl->wptr[i] = gew;
        mean += gew;
      }

    thr = mean / pl->lng;
    mean = 0;
    cnt = 0;

    for (i = 0; i < pl->lng; i++)
      {
        if (pl->wptr[i] < thr)
          {
            pl->wptr[i] = 0;
          }
        else
          {
            mean += pl->wptr[i];
            cnt++;
          }
      }

    try
      {
        FitLine(pl, 0, pl->lng - 1, 0, par, &md, &ma);
      }
    catch (IceException& ex)
      {
        FreePointList(pl);
        throw IceException(ex, FNAME);
      }
    *p = par[0];
    *phi = par[1];
    FreePointList(pl);
    return mean / cnt;
  }
#undef FNAME
}
