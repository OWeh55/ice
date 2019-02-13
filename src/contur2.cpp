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
//
//  Konturfolge und Kantendetektion
//
//
//   (C) m.schubert 9.93
//       Ortmann 10/99
//
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "contools.h"
#include "contfeat.h"

#include "conturfunctions.h"

namespace ice
{
#define FNAME "SearchGradStart"
  int SearchGradStart(const Image& imgv, const Image& imgo,
                      int pgl, int diff,
                      int ps[2], int direct)
  {
    int wxi, wyi, wxa, wya;
    int v1, v2, v3, h1, h2, h3, h, v;
    int xl, xr, yo, yu;
    int grd1, grd2;
    int dir, s;
    int x, y;
    int xf, yf;

    x = ps[0];
    y = ps[1];

    if ((direct != HORZ) && (direct != VERT))
      throw IceException(FNAME, M_WRONG_PARAM);

    if (!IsImg(imgv))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (IsImg(imgo))
      {
        RETURN_ERROR_IF_FAILED(MatchImg(imgo, imgv));
      }

    wxi = 0;
    wyi = 0;
    wxa = imgv->xsize - 1;
    wya = imgv->ysize - 1;

    if (((wxa - wxi) < 1) || ((wya - wyi) < 1))
      throw IceException(FNAME, M_WRONG_WINDOW2);

    if ((wxi - 1) == x)
      {
        x++;  /* an verkleinertes Suchfenster anpassen*/
      }

    if ((wyi - 1) == y)
      {
        y++;
      }

    if ((wxa + 1) == x)
      {
        if (direct == HORZ)
          {
            y++;
            x = wxi;
          }
        else
          {
            return NOT_FOUND;
          }
      }

    if ((wya + 1) == y)
      {
        if (direct == VERT)
          {
            x++;
            y = wyi;
          }
        else
          {
            return NOT_FOUND;
          }
      }

    if ((x < wxi) || (y < wyi) || (x > wxa) || (y > wya))
      throw IceException(FNAME, M_WRONG_STARTPOINT);

    xl = x - 1;
    xr = x + 1;
    yo = y - 1;
    yu = y + 1;
    pgl *= 6;
    pgl *= pgl;

    if (direct == HORZ)
      {
        do   /*Bild durchmustern ab Startpunkt*/
          {
            /*nächste Zeile*/

            /* check bounds */
            if (x > wxa || y > wya || xl > wxa || yo > wya || yu > wya)
              {
                break;
              }

            v1 = GetVal(imgv, xl, yo) + GetVal(imgv, x, yo);
            v3 = GetVal(imgv, xl, yu) + GetVal(imgv, x, yu);
            h1 = GetVal(imgv, xl, yo) + GetVal(imgv, xl, y) + GetVal(imgv, xl, yu);
            h2 = GetVal(imgv, x, yo) + GetVal(imgv, x, y) + GetVal(imgv, x, yu);

            do                            /*nächster Punkt*/
              {

                /* check bounds */
                if (xr > wxa || yo > wya || yu > wya || y > wya)
                  {
                    break;
                  }

                v1 += GetVal(imgv, xr, yo);
                v3 += GetVal(imgv, xr, yu);
                h3 = GetVal(imgv, xr, yo) + GetVal(imgv, xr, y) + GetVal(imgv, xr, yu);

                if (!IsImg(imgo) || (GetVal(imgo, x, y) == 0))
                  {
                    h = h3 - h1;
                    v = v3 - v1;

                    if ((grd1 = h * h + v * v) > pgl)
                      {
                        do   /*lokales maximum in zeile suchen*/
                          {
                            grd2 = grd1;
                            ps[0] = x;
                            ps[1] = y;

                            /* check bounds */
                            if (xl > wxa || yo > wya || yu > wya)
                              {
                                break;
                              }

                            v1 -= GetVal(imgv, xl, yo);
                            v3 -= GetVal(imgv, xl, yu);
                            h1 = h2;
                            h2 = h3;
                            x++;
                            xl++;
                            xr++;

                            if (x > wxa || xr > wxa || y > wya || yu > wya || yo > wya)
                              {
                                break;
                              }

                            v1 += GetVal(imgv, xr, yo);
                            v3 += GetVal(imgv, xr, yu);
                            h3 = GetVal(imgv, xr, yo) + GetVal(imgv, xr, y) + GetVal(imgv, xr, yu);
                            h = h3 - h1;
                            v = v3 - v1;
                          }
                        while ((grd1 = h * h + v * v) > grd2);

                        s = 0;

                        if (IsImg(imgo))
                          for (dir = 0;  dir < 8;  dir++)
                            {
                              Freeman(dir).move(ps[0], ps[1], xf, yf);
                              s += GetVal(imgo, xf, yf);
                            }

                        if (s == 0)
                          {
                            return OK;
                          }
                      }
                  }

                /* check bounds */
                if (xl > wxa || yo > wya || yu > wya)
                  {
                    break;
                  }

                v1 -= GetVal(imgv, xl, yo);
                v3 -= GetVal(imgv, xl, yu);
                h1 = h2;
                h2 = h3;
                x++;
                xl++;
                xr++;
              }
            while (x <= wxa);

            x = wxi;
            xl = x - 1;
            xr = x + 1;
            y += diff;
            yo += diff;
            yu += diff;
          }
        while (y <= wya);

        return NOT_FOUND;
      }
    else                                /*vertikale Suchlinien*/
      {
        do                                /*Bild durchmustern ab Startpunkt*/
          {
            h1 = GetVal(imgv, xl, yo) + GetVal(imgv, xl, y);
            h3 = GetVal(imgv, xr, yu) + GetVal(imgv, xr, y);
            v1 = GetVal(imgv, xl, yo) + GetVal(imgv, x, yo) + GetVal(imgv, xr, yo);
            v2 = GetVal(imgv, xl, y) + GetVal(imgv, x, y) + GetVal(imgv, xr, y);

            do
              {
                h1 += GetVal(imgv, xl, yu);
                h3 += GetVal(imgv, xr, yu);
                v3 = GetVal(imgv, xl, yu) + GetVal(imgv, x, yu) + GetVal(imgv, xr, yu);

                if (!IsImg(imgo) || (GetVal(imgo, x, y) == 0))
                  {
                    h = h3 - h1;
                    v = v3 - v1;

                    if ((grd1 = h * h + v * v) > pgl)
                      {
                        do
                          {
                            grd2 = grd1;
                            ps[0] = x;
                            ps[1] = y;
                            h1 -= GetVal(imgv, xl, yo);
                            h3 -= GetVal(imgv, xr, yo);
                            v1 = v2;
                            v2 = v3;
                            y++;
                            yo++;
                            yu++;

                            if (y > wya)
                              {
                                break;
                              }

                            h1 += GetVal(imgv, xl, yu);
                            h3 += GetVal(imgv, xr, yu);
                            v3 = GetVal(imgv, xl, yu) + GetVal(imgv, x, yu) + GetVal(imgv, xr, yu);
                            h = h3 - h1;
                            v = v3 - v1;
                          }
                        while ((grd1 = h * h + v * v) >= grd2);

                        s = 0;

                        if (IsImg(imgo))
                          for (dir = 0;  dir < 8;  dir++)
                            {
                              Freeman(dir).move(ps[0], ps[1], xf, yf);
                              s += GetVal(imgo, xf, yf);
                            }

                        if (s == 0)
                          {
                            return OK;
                          }
                      }
                  }

                h1 -= GetVal(imgv, xl, yo);
                h3 -= GetVal(imgv, xr, yo);
                v1 = v2;
                v2 = v3;
                y++;
                yo++;
                yu++;
              }
            while (y <= wya);

            y = wyi;
            x += diff;
            xl += diff;
            xr += diff;
            y = wyi;
            yo = y - 1;
            yu = y + 1;
          }
        while (x <= wxa);

        return NOT_FOUND;
      }
  }
#undef FNAME
  /*****************************************************/
  int Gradient(int v, int h, int* dir)
  {
    int grad;
    int val;
    grad = v * v + h * h;

    if (h == 0)
      {
        if (v < 0)
          {
            *dir = 6;
          }
        else
          {
            *dir = 2;
          }

        return grad;
      }

    val = abs(10000 * v / h);

    if (val < 4142)
      {
        *dir = 0;
      }
    else if (val < 24142)
      {
        *dir = 1;
      }
    else
      {
        *dir = 2;
      }

    if (h < 0)
      if (v < 0)
        {
          *dir += 4;
        }
      else
        {
          *dir = 4 - *dir;
        }
    else if ((v < 0) && (*dir > 0))
      {
        *dir = 8 - *dir;
      }

    return grad;
  }
  /*****************************************************/
  int ObjectGrad(const Image& imgv, const Image& imgo, int x, int y, int thr)
  {
    int h1, h3, v1, v3;
    int xl, xr, yo, yu;
    int gdir;

    xl = x - 1;
    xr = x + 1;
    yo = y - 1;
    yu = y + 1;

    if (IsImg(imgo))
      if (GetVal(imgo, x, y) > 0)
        {
          return -2;
        }

    if (xl < 0)
      {
        return -3;
      }

    if (xr >= imgv->xsize)
      {
        return -3;
      }

    if (yo < 0)
      {
        return -3;
      }

    if (yu >= imgv->ysize)
      {
        return -3;
      }

    h1 = GetVal(imgv, xl, yo) + GetVal(imgv, xl, y) + GetVal(imgv, xl, yu);
    h3 = GetVal(imgv, xr, yo) + GetVal(imgv, xr, y) + GetVal(imgv, xr, yu);
    v1 = GetVal(imgv, xr, yo) + GetVal(imgv, x, yo) + GetVal(imgv, xl, yo);
    v3 = GetVal(imgv, xr, yu) + GetVal(imgv, x, yu) + GetVal(imgv, xl, yu);

    if (Gradient(v3 - v1, h3 - h1, &gdir) < thr)
      {
        return -1;
      }

    return gdir;
  }
  /*****************************************************/
  int GradVal(const Image& imgv, const Image& imgo, int x, int y, int* dir)
  {
    int h1, h3, v1, v3;
    int xl, xr, yo, yu;
    xl = x - 1;
    xr = x + 1;
    yo = y - 1;
    yu = y + 1;

    if (IsImg(imgo))
      if (GetVal(imgo, x, y) > 0)
        {
          return -1;
        }

    if (xl < 0)
      {
        return -2;
      }

    if (xr >= imgv->xsize)
      {
        return -3;
      }

    if (yo < 0)
      {
        return -4;
      }

    if (yu >= imgv->ysize)
      {
        return -5;
      }

    h1 = GetVal(imgv, xl, yo) + GetVal(imgv, xl, y) + GetVal(imgv, xl, yu);
    h3 = GetVal(imgv, xr, yo) + GetVal(imgv, xr, y) + GetVal(imgv, xr, yu);
    v1 = GetVal(imgv, xr, yo) + GetVal(imgv, x, yo) + GetVal(imgv, xl, yo);
    v3 = GetVal(imgv, xr, yu) + GetVal(imgv, x, yu) + GetVal(imgv, xl, yu);
    return Gradient(v3 - v1, h3 - h1, dir);
  }
  /*****************************************************/
#define FNAME "CalcGradContur"
  Contur CalcGradContur(const Image& imgv, Image& imgo,
                        int pgl, int maxgap,
                        int ps[2], int lng, double& meangrad)
  {
    try
      {
        int wxi, wyi, wxa, wya;
        int i;
        int gap;
        Contur c;
        int startdir;
        int xx, yy, xs, ys;
        int xf, yf;
        int dir, aktdir, nextdir, grdir, maxdir, maxgrad;
        int val;
        xs = ps[0];
        ys = ps[1];
        nextdir = 0;
        maxdir = 0;

        if (!IsImg(imgv))
          throw IceException(FNAME, M_WRONG_IMAGE);

        if (!IsImg(imgo))
          throw IceException(FNAME, M_WRONG_IMAGE);

        MatchImg(imgv, imgo);

        wxi = 0;
        wyi = 0;
        wxa = imgv->xsize - 1;
        wya = imgv->ysize - 1;

        if (((wxa - wxi) < 1) || ((wya - wyi) < 1))
          throw IceException(FNAME, M_WRONG_WINDOW2);

        if (xs < wxi || xs > wxa || ys < wyi || ys > wya)
          throw IceException(FNAME, M_WRONG_STARTPOINT);

        if (maxgap < 1)
          {
            maxgap = 1;
          }

        pgl *= 6;
        pgl *= pgl;

        if ((val = GradVal(imgv, imgo, xs, ys, &startdir)) < pgl)
          throw IceException(FNAME, M_WRONG_STARTPOINT);

        gap = 0;
        PutVal(imgo, xs, ys, 1);
        c.setStart(xs, ys); // set startpoint of Contur
        xx = xs;
        yy = ys;   // begin of contur following
        aktdir = startdir; // save start direction
        meangrad = val;    // strength of edges

        do                 // start seraching backwards
          {
            maxgrad = 0;

            for (dir = (aktdir + 1) & 7, i = 0; i < 3; dir = (dir + 1) & 7, i++)
              {
                Freeman(dir).move(xx, yy, xf, yf);
                val = GradVal(imgv, imgo, xf, yf, &grdir);

                if (val < 0)
                  {
                    maxgrad = val;
                    break;
                  }

                if (val > maxgrad)
                  {
                    maxgrad = val;   /* maximalen Gradientenbetrag für 3 Richtungen*/
                    maxdir = dir;
                    nextdir = grdir;
                  }
              }

            if (maxgrad < 0)
              {
                break;
              }

            if (maxgrad < pgl)
              {
                gap++;  /* kurze Lücken überspringen*/
              }
            else
              {
                gap = 0;  /* Lücke zuende*/
              }

            if (gap >= maxgap)
              {
                break;  /* Lücke zu groß*/
              }

            Freeman(maxdir).move(xx, yy);
            meangrad += maxgrad;           /* Kantenstärke aktualisieren */
            PutVal(imgo, xx, yy, 1);
            c.add(maxdir);
            aktdir = nextdir;              /* nächste Suchrichtung*/
          }
        while ((lng <= 0 || c.Number() < lng) && !(xx == xs && yy == ys));

        c.invertDir();

        aktdir = startdir;            // restore start direction

        do                            // search forward
          {
            maxgrad = 0;

            for (dir = (aktdir + 5) & 7, i = 0; i < 3; dir = (dir + 1) & 7, i++)
              {
                Freeman(dir).move(xx, yy, xf, yf);
                val = GradVal(imgv, imgo, xf, yf, &grdir);

                if (val < 0)
                  {
                    maxgrad = val;
                    break;
                  }

                if (val >= maxgrad)
                  {
                    maxgrad = val;     /* maximalen Gradientenbetrag für 3 Richtungen*/
                    maxdir = dir;
                    nextdir = grdir;
                  }
              }

            if (maxgrad < 0)
              {
                break;
              }

            if (maxgrad < pgl)
              {
                gap++;  /* kurze Lücken überspringen*/
              }
            else
              {
                gap = 0;  /* Lücke zuende*/
              }

            if (gap >= maxgap)
              {
                break;  /* Lücke zu groß*/
              }

            Freeman(maxdir).move(xx, yy);  /* nächster Konturpunkt*/
            meangrad += maxgrad;           /* Kantenstärke aktualisieren*/
            PutVal(imgo, xx, yy, 1);
            c.add(maxdir);
            aktdir = nextdir;              /* nächste Suchrichtung*/
          }
        while ((lng <= 0 || c.Number() < lng) && !(xx == xs && yy == ys));

        if (c.Number() == 0)
          {
            PutVal(imgo, xs, ys, 0);
            return Contur();
          }

        for (dir = 0; dir < 8; dir++)
          {
            Freeman(dir).move(xx, yy, xf, yf);

            if ((xf == c.Start().x) && (yf == c.Start().y))
              {
                c.add(dir);
                meangrad += GradVal(imgv, imgo, xx, yy, &grdir);
                break;
              }
          }

        meangrad = sqrt(meangrad / (c.Number() + 1)) / 6;
        MarkContur(c, 0, imgo);
        return c;
      }
    RETHROW;
  }

  Contur CalcGradContur(const Image& imgv, Image& imgo,
                        int pgl, int maxgap,
                        int ps[2], int lng)
  {
    double meangrad;
    return CalcGradContur(imgv, imgo, pgl, maxgap, ps, lng, meangrad);
  }
#undef FNAME
  /*************************************************************/
  int GetThreshold(const Image& img, int s[2])
  {
    int x, y;
    int dir, thr;
    int start, val, min, max, vo, vu, vl, vr;
    int x1, y1, x2, y2;
    x = s[0];
    y = s[1];
    min = max = start = GetVal(img, x, y);
    vo = vu = vl = vr = 0;
    x--;
    y--;
    val = GetVal(img, x, y);
    vo += val;
    vl += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    x++;
    val = GetVal(img, x, y);
    vo += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    x++;
    val = GetVal(img, x, y);
    vo += val;
    vr += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    y++;
    val = GetVal(img, x, y);
    vr += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    y++;
    val = GetVal(img, x, y);
    vr += val;
    vu += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    x--;
    val = GetVal(img, x, y);
    vu += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    x--;
    val = GetVal(img, x, y);
    vu += val;
    vl += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    y--;
    val = GetVal(img, x, y);
    vl += val;

    if (val < min)
      {
        min = val;
      }

    if (val > max)
      {
        max = val;
      }

    Gradient(vu - vo, vr - vl, &dir);
    Freeman(dir).move(s[0], s[1], x1, y1);
    Freeman(dir + 4).move(s[0], s[1], x2, y2);
    thr = (GetVal(img, x1, y1) + GetVal(img, x2, y2)) / 2;

    if (thr > start)
      {
        return start;
      }
    else
      {
        return thr;
      }
  }
  /************************************************/
  int grd(const Image& img, int x, int y)
  {
    int v1 = 0, v2 = 0, h1 = 0, h2 = 0;
    int g;
    x--;
    g = GetVal(img, x, y);
    v1 += g;
    y--;
    g = GetVal(img, x, y);
    v1 += g;
    h1 += g;
    x++;
    g = GetVal(img, x, y);
    h1 += g;
    x++;
    g = GetVal(img, x, y);
    h1 += g;
    v2 += g;
    y++;
    g = GetVal(img, x, y);
    v2 += g;
    y++;
    g = GetVal(img, x, y);
    v2 += g;
    h2 += g;
    x--;
    g = GetVal(img, x, y);
    h2 += g;
    x--;
    g = GetVal(img, x, y);
    h2 += g;
    v1 += g;
    return ((int)(Sqr(v2 - v1) + Sqr(h2 - h1)));
  }
  /************************************************/
#define FNAME "SearchCStart"
  int SearchCStart(const Image& imgv, const Image& imgo, Contur c, int& index, int pgl, int s[2])
  {
    int j, x, y, g;
    int xf, yf;
    int g1, h, dir;
    pgl = (int)Sqr(pgl * 6);

    if (index >= c.Number())
      {
        return NOT_FOUND;
      }

    RETURN_ERROR_IF_FAILED(c.getPoint(index, x, y));

    g = GradVal(imgv, imgo, x, y, &dir);

    do
      {
        if ((g >= pgl) && (((dir - c.DirCode(index)) & 7) < 3))   /*Schwelle überschritten*/
          {
            do                          /*lokales Maximum suchen*/
              {
                s[0] = x;
                s[1] = y;
                g1 = g;
                index++;
                c.getPoint(index, x, y);
                g = GradVal(imgv, imgo, x, y, &dir);
              }
            while ((index < c.Number()) && (g > g1));

            if ((index == c.Number()) && (g > g1) && c.isClosed())
              {
                s[0] = x;
                s[1] = y;          /*testen, ob Startpunkt lok. Maximum*/
                g1 = g;
                c.getPoint(1, x, y);
                g = GradVal(imgv, imgo, x, y, &dir);

                if (g > g1)
                  {
                    return NOT_FOUND;
                  }
              }

            h = 0;

            if (IsImg(imgo))
              {
                for (j = 0; j < 8; j++)
                  {
                    Freeman(j).move(s[0], s[1], xf, yf);
                    h += GetVal(imgo, xf, yf);
                  }

                h += GetVal(imgo, s[0], s[1]);
              }

            if (h == 0)
              {
                return OK;
              }
          }

        index++;                            /*nächsten Konturpunkt*/
        c.getPoint(index, x, y);
        g = GradVal(imgv, imgo, x, y, &dir);
      }
    while (index < c.Number());

    return NOT_FOUND;
  }
#undef FNAME
  /************************************************/
#define FNAME "SearchCircStart"
  int SearchCircStart(const Image& imgv, const Image& imgo, int p[2], int r, int pgl, int s[2])
  {
    int i, j, x, y, g;
    int xf, yf;
    int g1, h, dir;
    double par[3];
    int st[2];
    Contur c;
    int found = NOT_FOUND;
    pgl = (int)Sqr(pgl * 6);
    par[0] = p[0];
    par[1] = p[1];
    par[2] = r;
    RETURN_ERROR_IF_FAILED(c = CircleContur(par));
    i = 0;
    c.getPoint(i, x, y);
    g = GradVal(imgv, imgo, x, y, &dir);

    do
      {
        if ((g >= pgl) && (((dir - c.DirCode(i)) & 7) < 3))   /*Schwelle überschritten*/
          {
            do                          /*lokales Maximum suchen*/
              {
                st[0] = x;
                st[1] = y;
                g1 = g;
                i++;
                c.getPoint(i, x, y);     /*nächsten Punkt testen*/
                g = GradVal(imgv, imgo, x, y, &dir);
              }
            while ((i < c.Number()) && (g > g1));

            if ((i == c.Number()) && (g > g1) && c.isClosed())
              {
                st[0] = x;
                st[1] = y;      /*testen, ob Startpunkt lok. Maximum*/
                g1 = g;
                c.getPoint(1, x, y);
                g = GradVal(imgv, imgo, x, y, &dir);

                if (g <= g1)
                  {
                    for (j = 0, h = 0; j < 8; j++)
                      {
                        Freeman(h).move(st[0], st[1], xf, yf);
                        h += GetVal(imgo, xf, yf);
                      }

                    h += GetVal(imgo, st[0], st[1]);

                    if (h == 0)
                      {
                        s[0] = st[0];
                        s[1] = st[1];
                        return OK;
                      }
                  }
              }

            for (j = 0, h = 0; j < 8; j++)
              {
                Freeman(h).move(st[0], st[1], xf, yf);
                h += GetVal(imgo, xf, yf);
              }

            h += GetVal(imgo, st[0], st[1]);

            if (h == 0)
              {
                s[0] = st[0];
                s[1] = st[1];
                pgl = g1;
                found = OK;
              }
          }

        i++;
        c.getPoint(i, x, y);          /*nächsten Konturpunkt*/
        g = GradVal(imgv, imgo, x, y, &dir);
      }
    while (i < c.Number());

    return found;
  }
#undef FNAME
  /*****************************************************/
#define FNAME "GetContur"
  Contur GetContur(int ps[2], int(*cls)(int, int, void*), void* par, int lng)
  {
    int cl;
    Contur c;
    int cr;
    int xx, yy, xs, ys;
    int xf, yf;

    xs = ps[0];
    ys = ps[1];

    if (cls == NULL)
      throw IceException(FNAME, M_WRONG_PARAM);

    // Test vom Startpunkt alle acht Richtungen nach links beginnend
    Freeman dir(4);
    cr = 0;
    dir.move(xs, ys, xf, yf);

    while ((cr < 4) && (cls(xf, yf, par) >= 0))
      {
        dir = dir + 2;
        cr++;         /*ersten Untergrundpkt. suchen*/
        dir.move(xs, ys, xf, yf);
      }

    if (cr == 4)                    /* Startpunkt liegt im Objekt */
      throw IceException(FNAME, M_WRONG_STARTPOINT3);

    cr = 0;                        /* Zähler untersuchte Richtungen */
    dir.move(xs, ys, xf, yf);

    while ((cr < 8) && (cls(xf, yf, par) < 0))
      {
        dir = dir + 1;
        cr++;
        dir.move(xs, ys, xf, yf);
      }

    if (cr == 8)                    /* Startpunkt liegt im Untergrund */
      throw IceException(FNAME, M_WRONG_STARTPOINT2);

    c.setStart(xs, ys);

    xx = xs;
    yy = ys;                   /* Beginn der Konturfolge */

    Freeman startdir = dir;            /* Startrichtung merken für Endetest*/

    do
      {
        dir.move(xx, yy);
        c.add(dir);
        dir = dir + 6;                   /* neue Richtung bestimmen */
        dir.move(xx, yy, xf, yf);

        while ((cl = cls(xf, yf, par)) < 0)
          {
            dir = dir + 1;
            dir.move(xx, yy, xf, yf);

            if (cl == -4)
              {
                break;  /* Abbruch, Kontur offen lassen */
              }
          }
      }
    while ((lng <= 0 || c.Number() < lng) && !(xx == xs && yy == ys && dir == startdir) && cl != -4);

    return c;
  }
  /**********************************************************/
}
