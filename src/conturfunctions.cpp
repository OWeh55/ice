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
#include "freeman.h"
#include "macro.h"
#include "numbase.h"
#include "contools.h"
#include "contfeat.h"

#include "conturfunctions.h"

namespace ice
{
#define FNAME "SearchStart"
//===================================================
  inline object_rc checkobjectstart(const Image& imgv, const Image& imgo,
                                    OFunc object, int pgl,
                                    int x, int y)
  {
    if (IsImg(imgo))
      {
        if (GetValUnchecked(imgo, x, y) != 0)
          {
            return isunknown;
          }
      }

    return object(imgv, x, y, pgl);
  }

  inline object_rc checkobjectstartchecked(const Image& imgv, const Image& imgo,
      OFunc object, int pgl,
      int x, int y)
  {
    if ((x < 0) || (x >= imgv.xsize) || (y < 0) || (y >= imgv.ysize))
      {
        return isunknown;
      }

    return checkobjectstart(imgv, imgo, object, pgl, x, y);
  }

  inline object_rc checkobjectconturchecked(const Image& imgv, const Image& imgo,
      OFunc object, int pgl,
      int x, int y,
      object_rc marked, object_rc outside)
  {
    if ((x < 0) || (x >= imgv.xsize) || (y < 0) || (y >= imgv.ysize))
      {
        return outside;
      }

    if (IsImg(imgo))
      {
        if (GetValUnchecked(imgo, x, y) != 0)
          {
            return marked;
          }
      }

    return object(imgv, x, y, pgl);
  }

  int SearchStart(const Image& imgv, const Image& imgo,
                  OFunc object,
                  int pgl, int diff, IPoint& ps,
                  int direct)
  {
    int sp[2];
    sp[0] = ps.x;
    sp[1] = ps.y;
    int ret = SearchStart(imgv, imgo, object, pgl, diff, sp, direct);
    ps = IPoint(sp[0], sp[1]);
    return ret;
  }

  int SearchStart(const Image& imgv, const Image& imgo,
                  OFunc object,
                  int pgl, int diff, int ps[2],
                  int direct)
  {
    int wxi, wyi, wxa, wya;
    int x, y;
    object_rc astat, nstat;// "alter" und neuer Status
    int xa, ya;

    x = ps[0];
    y = ps[1];

    if ((direct != HORZ) && (direct != VERT))
      throw IceException(FNAME, M_WRONG_PARAM);

    if (!IsImg(imgv))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (IsImg(imgo))
      {
        RETURN_ERROR_IF_FAILED(MatchImg(imgv, imgo));
      }

    wxi = 0;
    wyi = 0;
    wxa = imgv->xsize - 1;
    wya = imgv->ysize - 1;

    if (object == NULL)
      {
        object = ObjectThr;
      }

    if (((wxa - wxi) < 1) || ((wya - wyi) < 1))
      throw IceException(FNAME, M_WRONG_WINDOW2);

    if ((x < wxi) || (y < wyi) || (x > wxa) || (y > wya))
      {
        // cout << "nicht innerhalb:" << x << "," << y << " " << wxi << " " << wyi << " " << wxa << " " << wya << endl;
        throw IceException(FNAME, M_WRONG_STARTPOINT);
      }

    if (direct == HORZ)
      {
        do                                 /*Bild durchmustern ab Startpunkt*/
          {
            // Start der Suche oder neue Zeile
            astat = checkobjectstart(imgv, imgo, object, pgl, x, y);
            xa = x;
            ya = y;

            do
              {
                nstat = checkobjectstart(imgv, imgo, object, pgl, x, y);

                if (astat != nstat)
                  {
                    switch (astat)
                      {
                      case isunderground:

                        if (nstat == isobject)
                          {
                            ps[0] = x;
                            ps[1] = y;
                            return OK;
                          }

                        break;
                      case isunknown:

                        if (nstat == isobject)
                          {
                            if (
                              (checkobjectstartchecked(imgv, imgo, object, pgl, x, y - 1)
                               == isunderground) ||
                              (checkobjectstartchecked(imgv, imgo, object, pgl, x, y + 1)
                               == isunderground)
                            )
                              {
                                ps[0] = x;
                                ps[1] = y;
                                return OK;
                              }
                          }

                        break;
                      case isobject:

                        switch (nstat)
                          {
                          case isunderground:
                          {
                            ps[0] = xa;
                            ps[1] = ya;
                            return OK;
                          }
                          break;
                          case isunknown:

                            if (
                              (checkobjectstartchecked(imgv, imgo, object, pgl, xa, ya - 1)
                               == isunderground) ||
                              (checkobjectstartchecked(imgv, imgo, object, pgl, xa, ya + 1)
                               == isunderground)
                            )
                              {
                                ps[0] = xa;
                                ps[1] = ya;
                                return OK;
                              }

                            break;
                          case isobject: /* nothing */
                            break;
                          }
                      }
                  }

                astat = nstat;
                xa = x;
                ya = y;
                x++;
              }
            while (x <= wxa);

            x = wxi;
            y += diff;
          }
        while (y <= wya);

        return NOT_FOUND;
      }
    else
      {
        // Spaltenweise suchen
        do                                 /*Bild durchmustern ab Startpunkt*/
          {
            astat = checkobjectstart(imgv, imgo, object, pgl, x, y);
            xa = x;
            ya = y;

            do
              {
                nstat = checkobjectstart(imgv, imgo, object, pgl, x, y);

                if (nstat != astat)
                  {
                    switch (astat)
                      {
                      case isunderground:

                        if (nstat == isobject)
                          {
                            ps[0] = x;
                            ps[1] = y;
                            return OK;
                          }

                        break;
                      case isunknown:

                        if (nstat == isobject)
                          {
                            if (
                              (checkobjectstartchecked(imgv, imgo, object, pgl, x + 1, y)
                               == isunderground) ||
                              (checkobjectstartchecked(imgv, imgo, object, pgl, x - 1, y)
                               == isunderground)
                            )
                              {
                                ps[0] = x;
                                ps[1] = y;
                                return OK;
                              }
                          }

                        break;
                      case isobject:

                        switch (nstat)
                          {
                          case isunderground:
                          {
                            ps[0] = xa;
                            ps[1] = ya;
                            return OK;
                          }
                          break;
                          case isunknown:

                            if (
                              (checkobjectstartchecked(imgv, imgo, object, pgl, xa - 1, ya)
                               == isunderground) ||
                              (checkobjectstartchecked(imgv, imgo, object, pgl, xa + 1, ya)
                               == isunderground)
                            )
                              {
                                ps[0] = xa;
                                ps[1] = ya;
                                return OK;
                              }

                            break;
                          case isobject: /* nothing */
                            break;
                          }
                      }
                  }

                xa = x;
                ya = y;
                astat = nstat;
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
  /**********************************************************/
#define FNAME "CalcContur"
  Contur CalcContur(const Image& imgv, const Image& imgo,
                    OFunc object,
                    int pgl, int ps[2],
                    int lng,              // parameter is ignored
                    object_rc marked,
                    object_rc outside)
  {
    try
      {
        Contur c, c1;
        int startdir, cr;
        int xs, ys;
        int xf, yf;
        int x1, y1;
        //    int x2, y2;
        int x3, y3, x4, y4;
        int dir;
        int dir1, dir2, dir6;
        bool stop;
        bool second = false;
        object_rc obj;

        xs = ps[0];
        ys = ps[1];

        if (object == NULL)
          {
            object = ObjectThr;
          }

        if (!IsImg(imgv))
          throw IceException(FNAME, M_WRONG_IMAGE);

        if (IsImg(imgo))
          imgv.checkSizes(imgo);

        if (xs < 0 || xs >= imgv->xsize || ys < 0 || ys >= imgv->ysize)
          throw IceException(FNAME, M_WRONG_STARTPOINT);

        if (checkobjectstartchecked(imgv, imgo, object, pgl, xs, ys) != isobject)
          throw IceException(FNAME, M_WRONG_STARTPOINT);

        dir = 4;
        cr = 0;

        // Suche Untergrundpunkt in Nachbarschaft (links beginnend)
        // markierte Punkte und Punkte ausserhalb sind hier nicht untergrund

        Freeman(dir).move(xs, ys, xf, yf);

        while ((cr < 4) &&
               (checkobjectstartchecked(imgv, imgo, object, pgl, xf, yf) != isunderground))
          {
            dir = (dir + 2) & 7;
            cr++;
            Freeman(dir).move(xs, ys, xf, yf);
          }

        if (cr == 4)                    // Startpunkt liegt im Objekt
          throw IceException(FNAME, M_WRONG_STARTPOINT3);

        startdir = dir;

        do
          {
            stop = false;
            x1 = xs;
            y1 = ys;
            // x2 = xf;
            // y2 = yf;
            dir = startdir;

            if (!second)
              {
                dir2 = 2;
                dir1 = 1;
                dir6 = 6;
              }
            else
              {
                c1 = c; // store found (partial) contur
                c.reset(); // re-initialize search contur
                dir2 = 6; // search backward
                dir1 = 7;
                dir6 = 2;
              }

            // Suche
            c.setStart(x1, y1);

            do
              {
                Freeman(dir + dir2).move(x1, y1, x3, y3);
                obj = checkobjectconturchecked(imgv, imgo, object, pgl,
                                               x3, y3,
                                               marked, outside);

                if (obj == isunderground)
                  {
                    // ?? ..
                    // ?? ..
                    //    --
                    // ..|XX
                    // ..|XX
                    // rechts abbiegen
                    // x2 = x3;
                    // y2 = y3;
                    dir = (dir + dir2) & 7;
                  }
                else
                  {
                    if (obj == isunknown)
                      {
                        // ?? uu
                        // ?? uu
                        //
                        // ..|XX
                        // ..|XX
                        Freeman(dir + dir1).move(x1, y1, x4, y4);
                        obj = checkobjectconturchecked(imgv, imgo, object, pgl,
                                                       x4, y4,
                                                       marked, outside);

                        if (obj == isobject)
                          {
                            // links abbiegen
                            // XX uu
                            // XX uu
                            // --
                            // ..|XX
                            // ..|XX
                            // links abbiegen
                            c.add((dir + dir1) & 7);
                            x1 = x4;
                            y1 = y4;
                            dir = (dir + dir6) & 7;
                          }
                        else
                          {

                            // .. uu
                            // .. uu
                            //
                            // ..|XX
                            // ..|XX
                            // stop

                            // ODER

                            // uu uu
                            // uu uu
                            //
                            // ..|XX
                            // ..|XX
                            // stop
                            stop = true;
                          }
                      }
                    else     /* obj==isobject */
                      {
                        // ?? XX
                        // ?? XX
                        //
                        // ..|XX
                        // ..|XX
                        Freeman(dir + dir1).move(x1, y1, x4, y4);
                        obj = checkobjectconturchecked(imgv, imgo, object, pgl,
                                                       x4, y4,
                                                       marked, outside);

                        if (obj == isobject)
                          {
                            // XX XX
                            // XX XX
                            // --
                            // ..|XX
                            // ..|XX
                            // links abbiegen
                            c.add((dir + dir1) & 7);
                            x1 = x4;
                            y1 = y4;
                            dir = (dir + dir6) & 7;
                          }
                        else
                          {
                            if (obj == isunderground)
                              {
                                // ..|XX
                                // ..|XX
                                //   |
                                // ..|XX
                                // ..|XX
                                // geradeaus
                                x1 = x3;
                                y1 = y3;
                                //x2 = x4;
                                //y2 = y4;
                                c.add((dir + dir2) & 7);
                              }
                            else     /* obj == isunknown */
                              {
                                // uu XX
                                // uu XX
                                //
                                // ..|XX
                                // ..|XX
                                // Halt
                                stop = true;
                              }
                          }
                      }
                  }
              }

            while (((x1 != xs) || (y1 != ys) || (dir != startdir)) &&
                   (!stop));

            second = !second;
          }
        while (second && stop);

        if (!second)   // zwei durchläufe waren nötig
          {
            c.invertDir();
            c.add(c1);
          }

        return c;
      }
    RETHROW;
  }

  Contur CalcContur(const Image& imgv, const Image& imgo,
                    OFunc object,
                    int pgl, const IPoint& p,
                    int lng,
                    object_rc marked,
                    object_rc outside)
  {
    int ps[2];
    ps[0] = p.x;
    ps[1] = p.y;

    return CalcContur(imgv, imgo, object, pgl, ps, lng, marked, outside);
  }

#undef FNAME
#define FNAME "LimitingPolygon"
  int LimitingPolygon(const Contur& c, Polygon& poly)
  {
    std::vector<IPoint> opl;
    std::vector<Freeman> fc;
    RETURN_ERROR_IF_FAILED(c.getPairs(opl, fc));

    IPoint p2 = opl[0];
    Freeman(fc[0].Int() - 1).move(p2);
    Point ap(0.5 * (opl[0].x + p2.x), 0.5 * (opl[0].y + p2.y));
    poly.Reset(ap);
    for (unsigned int i = 1; i < opl.size(); i++)
      {
        p2 = opl[i];
        Freeman(fc[i].Int() - 1).move(p2);
        ap = Point(0.5 * (opl[i].x + p2.x), 0.5 * (opl[i].y + p2.y));
        //  std::cout << << ap << std::endl;
        poly.add(ap);
      }
    return OK;
  }
#undef FNAME
  /**********************************************************/
}
