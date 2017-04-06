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
#include <limits.h>
#include <string.h>
#include <vector>

#include "IceException.h"
#include "lists.h"
#include "macro.h"

#include "convex.h"
#include "convexhull.h"

namespace ice
{
  inline double TriangleDet(double x1, double y1,
                            double x2, double y2,
                            double x3, double y3)
  {
    //    return x1*y2+y1*x3+x2*y3-x1*y3-y1*x2-y2*x3;
    return (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
  }

#define FNAME "ConvexHull"

  PointList ConvexHull(PointList pl)
  {
    if (pl == NULL || pl->lng == 0)
      throw IceException(FNAME, M_EMPTY_POINTLIST);

    if (pl->lng <= 3)
      {
        PointList pl2 = NewPointList(3);

        for (int i = 0; i < pl->lng; i++)
          {
            PutPoint(pl2, 0, pl->xptr[i], pl->yptr[i], pl->wptr[i]);
          }

        return pl2;
      }

    int xi, yi, xa, ya, nr;

    xi = yi = INT_MAX;
    xa = ya = 0;

    for (nr = 0; nr < pl->lng; nr++)
      {
        if (xi > pl->xptr[nr])
          {
            xi = (int)(pl->xptr[nr]);
          }

        if (yi > pl->yptr[nr])
          {
            yi = (int)(pl->yptr[nr]);
          }

        if (xa < pl->xptr[nr])
          {
            xa = (int)(pl->xptr[nr]);
          }

        if (ya < pl->yptr[nr])
          {
            ya = (int)(pl->yptr[nr]);
          }
      }

    int xsize = xa - xi + 1;

    int* ymin = new int[xsize];

    if (ymin == NULL)
      throw IceException(FNAME, M_NO_MEM);

    int* ymax = new int[xsize];

    if (ymax == NULL)
      {
        delete ymin;
        throw IceException(FNAME, M_NO_MEM);
      }

    int* imin = new int[xsize];

    if (imin == NULL)
      {
        delete ymin;
        delete ymax;
        throw IceException(FNAME, M_NO_MEM);
      }

    int* imax = new int[xsize];

    if (imax == NULL)
      {
        delete ymin;
        delete ymax;
        delete imin;
        throw IceException(FNAME, M_NO_MEM);
      }

    int* used = new int[xsize];

    if (used == NULL)
      {
        delete ymin;
        delete ymax;
        delete imin;
        delete imax;
        throw IceException(FNAME, M_NO_MEM);
      }

    for (nr = 0; nr < xsize; nr++)
      {
        ymin[nr] = INT_MAX;
        ymax[nr] = -1;
        used[nr] = 0;
      }

    int usedcnt = 0;

    for (nr = 0; nr < pl->lng; nr++)
      {
        int index = (int)(pl->xptr[nr] - xi);

        if (ymin[index] == INT_MAX)
          {
            usedcnt++;
          }

        if (ymin[index] > pl->yptr[nr])
          {
            ymin[index] = RoundInt(pl->yptr[nr]);
            imin[index] = nr;
          }

        if (ymax[index] < pl->yptr[nr])
          {
            ymax[index] = RoundInt(pl->yptr[nr]);
            imax[index] = nr;
          }
      }

    if (usedcnt == 2)
      {
        PointList pl2 = NewPointList(4);
        PutPoint(pl2, 0, pl->xptr[imin[0]], pl->yptr[imin[0]], pl->wptr[imin[0]]);
        PutPoint(pl2, 1, pl->xptr[imax[0]], pl->yptr[imax[0]], pl->wptr[imax[0]]);
        PutPoint(pl2, 2, pl->xptr[imax[xsize - 1]], pl->yptr[imax[xsize - 1]], pl->wptr[imax[xsize - 1]]);
        PutPoint(pl2, 3, pl->xptr[imin[xsize - 1]], pl->yptr[imin[xsize - 1]], pl->wptr[imin[xsize - 1]]);
        delete ymin;
        delete ymax;
        delete imin;
        delete imax;
        delete used;
        return pl2;
      }

    int* pl2 = new int[usedcnt + usedcnt];

    if (pl2 == NULL)
      {
        delete ymin;
        delete ymax;
        delete imin;
        delete imax;
        delete used;
        throw IceException(FNAME, M_NO_MEM);
      }

    int current = -1;
    double det;
    nr = 0;

    while (ymin[++current] == INT_MAX);

    pl2[nr++] = current;
    used[current] = 1;

    while (ymin[++current] == INT_MAX);

    pl2[nr++] = current;
    used[current] = 1;

    while (ymin[++current] == INT_MAX);

    while (1)
      {
        pl2[nr] = current;
        used[current] = 1;

        if (nr > 1)
          {
            det = TriangleDet(pl->xptr[imin[pl2[nr - 2]]], pl->yptr[imin[pl2[nr - 2]]],
                              pl->xptr[imin[pl2[nr - 1]]], pl->yptr[imin[pl2[nr - 1]]],
                              pl->xptr[imin[pl2[nr]]], pl->yptr[imin[pl2[nr]]]);

            if (det < 0)
              {
                nr--;
                used[pl2[nr]] = 0;
              }
            else
              {
                nr++;

                if (current == xsize - 1)
                  {
                    break;
                  }

                while (ymin[++current] == INT_MAX);
              }
          }
        else
          {
            nr++;

            if (current == xsize - 1)
              {
                break;
              }

            while (ymin[++current] == INT_MAX);
          }
      }

    int nr2 = nr - 1;

    if (imin[current] == imax[current] || ymin[current] == INT_MAX)
      {
        while (ymin[--current] == INT_MAX);
      }

    pl2[nr++] = current;

    while (ymin[--current] == INT_MAX);

    while (1)
      {
        pl2[nr] = current;

        if (nr > nr2 + 1)
          {
            det = TriangleDet(pl->xptr[imax[pl2[nr - 2]]], pl->yptr[imax[pl2[nr - 2]]],
                              pl->xptr[imax[pl2[nr - 1]]], pl->yptr[imax[pl2[nr - 1]]],
                              pl->xptr[imax[pl2[nr]]], pl->yptr[imax[pl2[nr]]]);

            if (det < 0)
              {
                nr--;
              }
            else
              {
                nr++;

                if (current == 0)
                  {
                    break;
                  }

                do
                  {
                    while (ymin[--current] == INT_MAX);
                  }
                while (ymin[current] == ymax[current] && used[current] && current > 0);
              }
          }
        else
          {
            nr++;

            if (current == 0)
              {
                break;
              }

            do
              {
                while (ymin[--current] == INT_MAX);
              }
            while (ymin[current] == ymax[current] && used[current] && current > 0);
          }
      }

    PointList pl3;
    pl3 = NewPointList(nr);

    if (pl3 == NULL)
      {
        delete ymin;
        delete ymax;
        delete imin;
        delete imax;
        delete used;
        delete pl2;
        throw IceException(FNAME, M_NO_MEM);
      }

    for (current = 0; current <= nr2; current++)
      {
        PutPoint(pl3, current, pl->xptr[imin[pl2[current]]], pl->yptr[imin[pl2[current]]], pl->wptr[imin[pl2[current]]]);
      }

    for (current = nr2 + 1; current < nr; current++)
      {
        PutPoint(pl3, current, pl->xptr[imax[pl2[current]]], pl->yptr[imax[pl2[current]]], pl->wptr[imax[pl2[current]]]);
      }

    delete ymin;
    delete ymax;
    delete imin;
    delete imax;
    delete used;
    delete pl2;

    return pl3;
  }

  Matrix ConvexHull(const Matrix& pl)
  {
    try
      {
        std::vector<Point> vpl(pl.rows());
        for (int i = 0; i < pl.rows(); i++)
          {
            vpl[i].x = pl[i][0];
            vpl[i].y = pl[i][1];
          }

        Matrix res(0, 2);
        std::vector<Point> rpl;
        ConvexHull<Point>(vpl, rpl);

        for (unsigned int i = 0; i < rpl.size(); i++)
          {
            res.Append(Vector(rpl[i].x, rpl[i].y));
          }

        return res;
      }
    RETHROW;
  }

  Contur ConvexHull(const Contur& c)
  {
    try
      {
        Contur res;
        if (!c.isValid())
          throw IceException(FNAME, M_INVALID_CONTUR);

        if (c.Number() < 3)
          {
            return c;
          }

        bool hole = c.isHole();

        std::vector<IPoint> pl;
        c.getPoints(pl);

        ConvexHull(pl, pl);

        res.setStart(pl[0]);
        for (unsigned int i = 1; i < pl.size(); i++)
          {
            res.add(pl[i]);
          }
        res.add(pl[0]); // close contur

        if (hole)
          {
            res.invertDir();
          }
        return res;
      }
    RETHROW;
  }

#undef FNAME

  int LinesIntersect(double x11, double y11, double x12, double y12,
                     double x21, double y21, double x22, double y22)
  {

    double xm1, ym1, xm2, ym2;
    double det1, det2;

    xm1 = (x11 + x12 + x21) / 3;
    ym1 = (y11 + y12 + y21) / 3;
    xm2 = (x11 + x12 + x22) / 3;
    ym2 = (y11 + y12 + y22) / 3;

    det1 = TriangleDet(x11, y11, x12, y12, xm1, ym1);
    det2 = TriangleDet(x11, y11, x12, y12, xm2, ym2);

    if (det1 * det2 > 0)
      {
        return false;
      }

    xm1 = (x21 + x22 + x11) / 3;
    ym1 = (y21 + y22 + y11) / 3;
    xm2 = (x21 + x22 + x12) / 3;
    ym2 = (y21 + y22 + y12) / 3;

    det1 = TriangleDet(x21, y21, x22, y22, xm1, ym1);
    det2 = TriangleDet(x21, y21, x22, y22, xm2, ym2);

    return det1 * det2 <= 0;
  }

// #define POLY_ERROR   -1
// #define POLY_DISJUNKT  0
// #define POLY_INTERSECT 1
// #define POLY_1_IN_2    2
// #define POLY_2_IN_1    3

#define FNAME "ConvexPolygonsIntersect"

  int ConvexPolygonsIntersect(PointList pl1, PointList pl2)
  {

    if (pl1 == NULL || pl1->lng < 3 || pl2 == NULL || pl2->lng < 3)
      throw IceException(FNAME, M_WRONG_PARAM);

    int i, j;

    for (i = 0; i < pl1->lng; i++)
      {
        for (j = 0; j < pl2->lng; j++)
          {
            if (LinesIntersect(pl1->xptr[i], pl1->yptr[i],
                               pl1->xptr[(i + 1) % (pl1->lng)], pl1->yptr[(i + 1) % (pl1->lng)],
                               pl2->xptr[j], pl2->yptr[j],
                               pl2->xptr[(j + 1) % (pl2->lng)], pl2->yptr[(j + 1) % (pl2->lng)]))
              {
                return POLY_INTERSECT;
              }
          }
      }

    // Testen, ob ineinander liegend

    int sign1 = 0, sign2 = 0;

    for (i = 0; i < pl2->lng; i++)
      {
        double det = TriangleDet(pl1->xptr[0], pl1->yptr[0],
                                 pl2->xptr[i], pl2->yptr[i],
                                 pl2->xptr[(i + 1) % (pl2->lng)], pl2->yptr[(i + 1) % (pl2->lng)]);

        if (det > 0)
          {
            sign1 = 1;

            if (sign2 == 1)
              {
                break;
              }
          }
        else if (det < 0)
          {
            sign2 = 1;

            if (sign1 == 1)
              {
                break;
              }
          }
      }

    if (sign1 == 0 || sign2 == 0)
      {
        return POLY_1_IN_2;
      }

    sign1 = sign2 = 0;

    for (i = 0; i < pl1->lng; i++)
      {
        double det = TriangleDet(pl2->xptr[0], pl2->yptr[0],
                                 pl1->xptr[i], pl1->yptr[i],
                                 pl1->xptr[(i + 1) % (pl1->lng)], pl1->yptr[(i + 1) % (pl1->lng)]);

        if (det > 0)
          {
            sign1 = 1;

            if (sign2 == 1)
              {
                break;
              }
          }
        else if (det < 0)
          {
            sign2 = 1;

            if (sign1 == 1)
              {
                break;
              }
          }
      }

    if (sign1 == 0 || sign2 == 0)
      {
        return POLY_2_IN_1;
      }

    return POLY_DISJUNKT;
  }

#undef FNAME
}
