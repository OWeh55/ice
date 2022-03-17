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

#include <limits>
#include "convexhull.h"
#include "polygon.h"
#include "lists.h"
using namespace std;
namespace ice
{
#define FNAME "Polygon::isValid"
  bool Polygon::isValid() const
  {
    int points = pl.size();

    if (points < 4)
      {
        return true;
      }

    LineSeg e1;

    for (int i = 0; i < points - 2; i++)
      {
        Edge(i, e1);

        for (int j = i + 2; j < points && j < i + points - 1; j++)
          {
            //   cout << i << " " << j << endl;
            LineSeg e2;
            Edge(j, e2);

            if (e1.Intersection(e2))
              {
                return false;
              }
          }
      }

    return true;
  }
#undef FNAME

  /******************************************************/
#if 0
#define FNAME "Polygon::MakeContur"
  Contur Polygon::MakeContur() const
  {
    Contur c;

    if (!isValid())
      throw IceException(FNAME, M_INVALID_STRUCT);

    c.SetStart(IPoint(pl[0]));

    for (unsigned int i = 1; i < pl.size(); i++)
      {
        c.Add(IPoint(pl[i]));
      }

    // close contur
    c.Add(IPoint(pl[0]));
    return c;
  }
#undef FNAME
#endif
  /******************************************************/
#define FNAME "Polygon::Distance"
  double Polygon::distance_(Point p) const
  {
    double mind = numeric_limits<double>::max();

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        double d = Edge(i).distance_(p);

        if (d < mind)
          {
            mind = d;
          }
      }

    return mind;
  }
#undef FNAME

#define FNAME "Polygon::Distance"
  double Polygon::Distance(const Polygon& p, int mode, int pmode) const
  {
    double dist1 = DistanceDir(p, pmode);
    double dist2 = p.DistanceDir(*this, pmode);

    switch (mode)
      {
      case DPP_MIN:

        if (dist1 < dist2)
          {
            return dist1;
          }
        else
          {
            return dist2;
          }

      case DPP_MAX:

        if (dist1 > dist2)
          {
            return dist1;
          }
        else
          {
            return dist2;
          }

      case DPP_MEAN:
        return 0.5 * (dist1 + dist2);
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }
  }

  double Polygon::DistanceDir(const Polygon& p, int pmode) const
  {
    double dist = Distance(p.pl[0]);

    for (unsigned int i = 1; i < p.pl.size(); i++)
      {
        double pdist = Distance(p.pl[i]);

        switch (pmode)
          {
          case DPP_MIN:

            if (pdist < dist)
              {
                dist = pdist;
              }

            break;
          case DPP_MAX:

            if (pdist > dist)
              {
                dist = pdist;
              }

            break;
          case DPP_MEAN:
            dist += pdist;
            break;
          default:
            throw IceException(FNAME, M_WRONG_MODE);
          }
      }

    if (pmode == DPP_MEAN)
      {
        dist /= p.pl.size();
      }

    return dist;
  }
#undef FNAME

#define FNAME "Reduced"
  Polygon Polygon::Reduced(int n, int mode) const
  {
    Polygon res;
    PolygonalCurve::Reduced(n, res, mode);
    return res;
  }

  Polygon Polygon::ReducedToPrecision(double prec, int mode) const
  {
    Polygon res;
    PolygonalCurve::ReducedToPrecision(prec, res, mode);
    return res;
  }
#undef FNAME

#define FNAME "Polygon::ConvexHull"
  Polygon Polygon::ConvexHull() const
  {
    Polygon res;
    ConvexHull(res);
    return res;
  }

  void Polygon::ConvexHull(Polygon& p) const
  {
    std::vector<Point> npl;
    ice::ConvexHull(pl, npl);

    p.Reset(npl[0]);

    for (unsigned int i = 1; i < npl.size(); i++)
      {
        p.add(npl[i]);
      }
  }

#undef FNAME
}
