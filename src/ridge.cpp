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
/*****************************************************
 *  Linienverfolgung                                 *
 *                                                   *
 *                                                   *
 *                                                   *
 *  (C) m.schubert 9.95                              *
 *****************************************************/

#include <stdlib.h>
#include <stddef.h>  /* notwendig fuer Unix !!! */
#include <math.h>

#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "freeman.h"
#include "contools.h"
#include "contfeat.h"
#include "conturfunctions.h"

using namespace std;

namespace ice
{
#define BOFF 2

  /*****************************************************/
  int r_NextDir(Image img, int x, int y, int dir, int& val)
  {
    int v, s;
    int smax, smin;
    int dirn = 0, dir1, dir2;
    int i, j;
    int xx, yy, xf, yf;
    smax = -1;
    smin = img.maxval * 3;

    for (dir1 = (dir + 7) & 7, i = 0; i < 3; i++, dir1 = (dir1 + 1) & 7)
      {
        Freeman(dir1).move(x, y, xx, yy);
        v = GetVal(img, xx, yy);

        for (dir2 = (dir1 + 7) & 7, j = 0; j < 3; dir2 = (dir2 + 1) & 7, j++)
          {
            Freeman(dir2).move(xx, yy, xf, yf);
            s = v + GetVal(img, xf, yf);

            if (s < smin)
              {
                smin = s;
              }

            if (s > smax)
              {
                smax = s;
                dirn = dir1;
              }
          }
      }

    val = (smax - smin) / 2;
    return dirn;
  }
  /*****************************************************/
#define FNAME "SearchRidgeStart"
  int SearchRidgeStart(Image imgv, Image imgo,
                       int pgl, int diff, int ps[2], int dir)
  {
    int wxi, wyi, wxa, wya;
    int v1, v2, v3, v4, v5;
    int x, y, s, val;
    int xf, yf;
    x = ps[0];
    y = ps[1];
    pgl *= 6;

    if ((dir != HORZ) && (dir != VERT))
      throw IceException(FNAME, M_WRONG_PARAM);

    if (!IsImg(imgv))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!IsImg(imgo))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!(Inside(imgv, x, y)))
      throw IceException(FNAME, M_WRONG_STARTPOINT);

    wxi = BOFF;
    wxa = min(imgv.xsize, imgo.xsize) - 1 - BOFF;
    wyi = BOFF;
    wya = min(imgv.ysize, imgo.ysize) - 1 - BOFF;

    if (((wxa - wxi) < 1) || ((wya - wyi) < BOFF))
      throw IceException(FNAME, M_WRONG_WINDOW2);

    /* an verkleinertes Suchfenster anpassen*/
    if (x < wxi)
      {
        x = wxi;
      }

    if (y < wyi)
      {
        y = wyi;
      }

    if (x > wxa)
      {
        if (dir == HORZ)
          {
            y++;
            x = wxi;
          }
        else
          {
            return NOT_FOUND;
          }
      }

    if (y > wya)
      {
        if (dir == VERT)
          {
            x++;
            y = wyi;

            if (x > wxa)
              {
                return (NOT_FOUND);
              }
          }
        else
          {
            return (NOT_FOUND);
          }
      }

    if (dir == HORZ)
      {
        do                              /*Bild durchmustern ab Startpunkt*/
          {
            /*naechste Zeile*/
            v1 = GetVal(imgv, x - 2, y);
            v2 = GetVal(imgv, x - 1, y);
            v3 = GetVal(imgv, x  , y);
            v4 = GetVal(imgv, x + 1, y);

            do                            /*naechster Punkt*/
              {
                v5 = GetVal(imgv, x + 2, y);
                val = v3 * 6 - 2 * (v2 + v4) - v1 - v5;

                if (val > pgl)
                  {
                    if (GetVal(imgo, x, y) == 0)
                      {
                        ps[0] = x;
                        ps[1] = y;
                        s = 0;

                        for (dir = 0; dir < 8; dir++)
                          {
                            Freeman(dir).move(x, y, xf, yf);
                            s += GetVal(imgo, xf, yf);
                          }

                        if (s == 0)
                          {
                            return (OK);
                          }
                      }
                  }

                v1 = v2;
                v2 = v3;
                v3 = v4;
                v4 = v5;
                x++;
              }
            while (x <= wxa);

            x = wxi;
            y += diff;
          }
        while (y <= wya);

        return (NOT_FOUND);
      }
    else                                /*vertikale Suchlinien*/
      {
        do                                /*Bild durchmustern ab Startpunkt*/
          {
            v1 = GetVal(imgv, x, y - 2);
            v2 = GetVal(imgv, x, y - 1);
            v3 = GetVal(imgv, x, y);
            v4 = GetVal(imgv, x, y + 1);

            do
              {
                v5 = GetVal(imgv, x, y + 2);
                val = v3 * 6 - 2 * (v2 + v4) - v1 - v5;

                if (val > pgl)
                  {
                    if (GetVal(imgo, x, y) == 0)
                      {
                        ps[0] = x;
                        ps[1] = y;
                        s = 0;

                        for (dir = 0; dir < 8; dir++)
                          {
                            Freeman(dir).move(x, y, xf, yf);
                            s += GetVal(imgo, xf, yf);
                          }

                        if (s == 0)
                          {
                            return (OK);
                          }
                      }
                  }

                v1 = v2;
                v2 = v3;
                v3 = v4;
                v4 = v5;
                y++;
              }
            while (y <= wya);

            y = wyi;
            x += diff;
          }
        while (x <= wxa);

        return NOT_FOUND;
      }
  }
#undef FNAME
  /*****************************************************/
#define FNAME "CalcRidgeContur"
  Contur CalcRidgeContur(Image imgv, Image imgo,
                         int pgl, int ps[2], int lng, double& meangrad)
  {
    int wxi, wyi, wxa, wya;
    int i;
    Contur c;
    int dir, aktdir, startdir = 0;
    int xx, yy, xn, yn, xs, ys;
    int xf, yf;
    int val, maxval, meanval;
    pgl *= 2;
    xs = ps[0];
    ys = ps[1];

    if (!IsImg(imgv))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!IsImg(imgo))
      throw IceException(FNAME, M_WRONG_IMAGE);

    wxi = BOFF;
    wxa = min(imgv->xsize, imgo->xsize) - 1 - BOFF;
    wyi = BOFF;
    wya = min(imgv->ysize, imgo->ysize) - 1 - BOFF;

    if (((wxa - wxi) < 1) || ((wya - wyi) < BOFF))
      throw IceException(FNAME, M_WRONG_WINDOW2);

    if (xs < wxi || xs > wxa || ys < wyi || ys > wya)
      throw IceException(FNAME, M_WRONG_STARTPOINT);

    /*beste Startrichtung suchen*/
    maxval = 0;

    for (i = 0; i < 8; i++)
      {
        /*markierte Nachbarpunkte -> kein Startpunkt*/
        Freeman(i).move(xs, ys, xf, yf);

        if (GetVal(imgo, xf, yf) != 0)
          throw IceException(FNAME, M_WRONG_STARTPOINT);

        dir = r_NextDir(imgv, xs, ys, i, val);

        if (val > maxval)
          {
            startdir = dir;
            maxval = val;
          }
      }

    if (maxval < pgl)
      {
        /*Message(FNAME,M_WRONG_STARTPOINT,WRONG_STARTPOINT);*/
        return c;
      }

    c.setStart(xs, ys);
    /* Beginn der Konturfolge */
    PutVal(imgo, xs, ys, 1);
    c.add(startdir);

    Freeman(startdir).move(xs, ys, xx, yy);

    aktdir = startdir;              /* Startrichtung merken*/

    meanval = val;                  /* Kantenstaerke */

    do                              /* zuerst rueckwaerts suchen*/
      {
        aktdir = r_NextDir(imgv, xx, yy, aktdir, val);

        if (val < pgl)
          {
            break;
          }

        Freeman(aktdir).move(xx, yy, xn, yn);

        if (GetVal(imgo, xn, yn) != 0)
          {
            break;
          }

        if (xn < wxi || xn > wxa || yn < wyi || yn > wya)
          {
            break;
          }

        xx = xn;
        yy = yn;
        c.add(aktdir);
        PutVal(imgo, xx, yy, 1);
        meanval += val;
      }
    while ((lng <= 0 || c.Number() < lng) && !(xx == xs && yy == ys));

    /* Durchlaufrichtung der Kontur umdrehen*/
    c.invertDir();
    /*jetzt in der anderen Richtung suchen*/
    aktdir = c.DirCode(c.Number() - 1);

    do
      {
        aktdir = r_NextDir(imgv, xx, yy, aktdir, val);

        if (val < pgl)
          {
            break;
          }

        Freeman(aktdir).move(xx, yy, xn, yn);

        if (GetVal(imgo, xn, yn) != 0)
          {
            break;
          }

        if (xn < wxi || xn > wxa || yn < wyi || yn > wya)
          {
            break;
          }

        xx = xn;
        yy = yn;
        c.add(aktdir);

        PutVal(imgo, xx, yy, 1);
        meanval += val;
      }
    while ((lng <= 0 || c.Number() < lng) && !(xx == xs && yy == ys));

    /*Linienverfolgung beendet*/
    if (c.Number() == 0)
      {
        PutVal(imgo, xs, ys, 0);
        return c;
      }

    /*Kontur ggf. schliessen*/
    for (dir = 0; dir < 8; dir++)
      {
        Freeman(dir).move(xx, yy, xf, yf);

        if ((xf == c.StartX()) && (yf == c.StartY()))
          {
            c.add(dir);
            break;
          }
      }

    meangrad = sqrt((double)meanval / (c.Number() + 1));

    MarkContur(c, 0, imgo);
    return c;
  }

  Contur CalcRidgeContur(Image imgv, Image imgo,
                         int pgl, int ps[2], int lng)
  {
    double meangrad;
    return CalcRidgeContur(imgv, imgo, pgl, ps, lng, meangrad);
  }

#undef FNAME
}
