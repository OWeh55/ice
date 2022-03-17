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
#include <queue>

#include "macro.h"
#include "arith.h"
#include "contools.h"
#include "dijkstra.h"

using namespace std;

namespace ice
{

// Minimaler Wertbereich für Markierungsbild.
// 0 = unbehandelt, 1= ziel, 2-9 = Richtungskode + 2 für minimalen pfad

  constexpr int minrange = 10;

  struct PointX: public IPoint
  {
    // Point with value and direction
    double val;
    Freeman dir;
    PointX(int x, int y, double v, int d): IPoint(x, y), val(v), dir(d) {}
    PointX(IPoint s, double v, int d): IPoint(s), val(v), dir(d) {}
    bool operator<(const PointX& second) const
    {
      // invers zu Operator < double, da priority_queue "falsch" herum arbeitet
      return val > second.val;
    }
  };

  // cost function
  double cf(const Image& img, const PointX& p)
  {
    return GetVal(img, p) + 1;
  }

#define FNAME "Dijkstra"
  Contur Dijkstra(const Image& img, IPoint start, Image& marker)
  {
    try
      {
        Contur res;
        priority_queue<PointX> heap;

        int dx, dy;

        MatchImg(img, marker, dx, dy);

        if (marker.maxval < minrange)
          throw IceException(FNAME, M_LOWRANGE);

        if (!Inside(img, start))
          throw IceException(FNAME, M_OUTSIDE);

        // prepare marker image:
        // 0           => 0 - unhandled
        // all markers => 1 - destination
        //
        binImg(marker, marker, 1, 1);

        if (marker.getPixel(start) != 0)
          throw IceException(FNAME, M_WRONG_STARTPOINT);

        PointX ap(start, 0, 0);
        heap.push(ap);

        bool ready = false;

        while (!ready && heap.size() > 0)
          {
            ap = heap.top();
            heap.pop();
            int state = marker.getPixel(ap);

            if (state == 1)   // reached destination
              {
                ready = true;
              }

            if (state < 2)   // point unhandled
              {
                marker.setPixel(ap, ap.dir.Int() + 2);

                if (!ready)
                  {
                    for (int dir = 0; dir < 8 && !ready; dir++)
                      {
                        PointX np(ap);
                        Freeman(dir).move(np);

                        if (Inside(marker, np))
                          {
                            int mrk = marker.getPixel(np);

                            if (mrk < 2)   // unbehandelt
                              {
                                if (dir & 1)
                                  {
                                    np.val += cf(img, np) * 1.414;
                                  }
                                else
                                  {
                                    np.val += cf(img, np);
                                  }

                                np.dir = dir;
                                heap.push(np);
                              }
                          }
                      }
                  }
              }
          }

        res.setStart(ap.x, ap.y);

        while (!(ap == start))
          {
            res.add(ap.x, ap.y);
            int dir = (marker.getPixel(ap) - 2 + 4) % 8;
            Freeman(dir).move(ap);
          }

        res.add(start);
        res.invertDir();
        return res;
      }
    RETHROW;
  }

  Contur Dijkstra(const Image& img, IPoint start, IPoint end)
  {
    Image mark;
    mark.create(img.xsize, img.ysize, minrange);
    mark.set(0);
    mark.setPixel(end, 1);
    return Dijkstra(img, start, mark);
  }
#undef FNAME
}
