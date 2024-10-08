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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "defs.h"
#include "macro.h"

#include "base.h"
#include "visual/visual.h"
#include "lineseg.h"
#include "fit.h"
#include "drawline.h"
#include "polygon.h"
#include "visual/polygon_vis.h"
using namespace std;
namespace ice
{
  /******************************************************/
#define FNAME "SelLine"
  int SelLine(const Image& img, IPoint& p1, IPoint& p2)
  {
    try
      {
        Visual v = getVisual(img);
        return v->SelectLine(p1, p2);
      }
    RETHROW;
  }

  int SelLineFromStart(const Image& img, const IPoint& p1, IPoint& p2)
  {
    try
      {
        Visual v = getVisual(img);
        v->SelectLineFromStart(p1, p2);
        return OK;
      }
    RETHROW;
  }

  LineSeg SelLine(const Image& img)
  {
    IPoint p1, p2;
    SelLine(img, p1, p2);
    return LineSeg(p1, p2);
  }
#undef FNAME
  /******************************************************/
#define FNAME "SelPolygon"
#if 0
  PointList SelPolygon(const Image& img)
  {
    int i, j, * p, * pp1, * pp2, * p1, * p2;
    PointList pl;
    int maxp = 100;
    p = (int*)malloc(maxp * 2 * sizeof(int));
    if (p == NULL)
      throw IceException(FNAME, M_NO_MEM);
    /* let pp1 point to p */
    pp1 = p;

    /* let pp1 (and therefore p) point to the first point */
    if (SelPoint(0, img, pp1) < 0)
      {
        free(p);
        return (NULL);
      }

    /* let pp2 point to the adr of the following point of pp1 */
    pp2 = pp1 + 2;
    i = 1; // pointlist counter

    /* call loop using first point (pp1) as starting point and
     * get the second point into pp2
     */
    while (SelLine(img, pp1, pp2) >= 0)
      {
        /* now draw all lines beginning from p ending at pp2 that was
         * given on in SelLine() above
         */
        for (p1 = p, p2 = p + 2; p2 <= pp2; p1 += 2, p2 += 2)
          {
            Line(p1[0], p1[1], p2[0], p2[1], 1, 0, img);
          }
        i++;

        /* now let pp1 be the latest point and have pp2 point to the next point's adress */
        pp1 = pp2;
        pp2 = pp1 + 2;

        /* reallocate memory for the pointlist if maximum is exceeded*/
        if (i > maxp)
          {
            maxp += 100;
            p = (int*)realloc(p, maxp * 2 * sizeof(int));
            if (p == NULL)
              throw IceException(FNAME, M_NO_MEM);
            pp2 = p + i * 2;
            pp1 = pp2 - 2;
          }
      } // while SelLine

    /* dont let it run to pp2, because SelLine() returns nothing right */
    for (p1 = p, p2 = p + 2; p2 <= pp1; p1 += 2, p2 += 2)
      {
        Line(p1[0], p1[1], p2[0], p2[1], 0, 0, img);
      }

    pl = NewPointList(i);
    pp1 = p;

    for (j = 0; j < i; j++)
      {
        PutPoint(pl, j, *(pp1), *(pp1 + 1), 1);
        pp1 += 2;
      }
    free(p);
    return (pl);
  }
#endif

  Polygon SelPolygon(const Image& img)
  {
    Polygon pl;
    IPoint p, p2;
    vector<IPoint> pp;
    int rc;

    /* let pp1 (and therefore p) point to the first point */
    p = SelPoint(img, rc);

    if (rc < 0)   // Abbruch
      {
        return pl;
      }

    pp.push_back(p);

    /* call loop using first point (pp1) as starting point and
     * get the second point into pp2
     */
    while (SelLineFromStart(img, p, p2) >= 0)
      {
        /* now draw all lines beginning from p ending at pp2 that was
         * given on in SelLine() above
         */
        pp.push_back(p2);
        for (unsigned int i = 1; i < pp.size(); i++)
          {
            Line(pp[i - 1], pp[i], 1, img);
          }

        p = p2;
      } // while SelLine

    pl.Reset(pp[0]);

    for (unsigned int i = 1; i < pp.size(); i++)
      {
        pl.add(pp[i]);
      }

    return pl;
  }
#undef FNAME
}
