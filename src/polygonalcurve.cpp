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

#include <limits>
#include "convex.h"
#include "lists.h"

#include "reduced_poly.h"

#include "polygonalcurve.h"

namespace ice
{
#define FNAME "PolygonalCurve::PolygonalCurve"
  PolygonalCurve::PolygonalCurve(const Matrix& im, bool close): closed(close)
  {
    if (im.cols() < 2 || im.rows() < 2)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    for (int i = 0; i < im.rows(); i++)
      pl.push_back(Point(im[i][0], im[i][1]));

    p = pl[0]; // Startpunkt = Position des GeoObject
  }
#undef FNAME

  PolygonalCurve::PolygonalCurve(const Contur& c, bool close): closed(close)
  {
    c.getPoints(pl, false);
  }

  void PolygonalCurve::setPos(Point pp)
  {
    // verschiebt die Position des Objektes
    Point dp = pp - p;
    p = pp;

    for (unsigned int i = 0; i < pl.size(); i++)
      pl[i] += dp;
  }

  /******************************************************/
#define FNAME "PolygonalCurve::MakeContur"
  Contur PolygonalCurve::MakeContur() const
  {
    Contur c;

    c.SetStart(IPoint(pl[0]));

    for (unsigned int i = 1; i < pl.size(); i++)
      {
        c.Add(IPoint(pl[i]));
      }

    // close contur ?
    if (closed)
      c.Add(IPoint(pl[0]));

    return c;
  }
#undef FNAME
  /******************************************************/
#define FNAME "PolygonalCurve::Inside"

// Hilfsfunktion für Inside
  static bool handleOneEdge(Point p, Point p1, Point p2,
                            int& lastdir, int& c)
  {
    // test auf schnitt der horizontalen Scanlinie mit der Kante p1->p2
    Point dp = p2 - p1;
    Point dp1 = p - p1;

    if (dp1.y == 0.0) // scanlinie trifft Ecke
      {
        if (dp1.x == 0.0) // Punkt ist Eckpunkt
          return true;

        if (dp.y < 0.0)
          {
            if (lastdir == -1) // weiter fallend
              if (dp1.x > 0.0) c++;
          }
        else if (dp.y > 0.0)
          {
            if (lastdir == 1) // weiter steigend
              if (dp1.x > 0.0) c++;
          }
        else
          //    if (dp.y==0.0) // Punkt auf horizontaler Kante
          {
            if ((p1.x <= p.x) && (p2.x >= p.x))
              return true;
          }
      }
    else if (p1.y < p.y && p2.y > p.y)
      {
        double z = dp.Cross(dp1);

        /* Punkt auf der Kante? */
        if (z == 0.0) return true;

        /* Kante schneidet Scanlinie _links_ vom Punkt? */
        if (z < 0) c++;
      }
    else if (p1.y > p.y && p2.y < p.y)
      {
        double z = dp.Cross(dp1);

        /* Punkt auf der Kante? */
        if (z == 0.0) return true;

        /* Kante schneidet Scanlinie _links_ vom Punkt? */
        if (z > 0) c++;
      }

    if (dp.y > 0.0) lastdir = 1;
    else if (dp.y < 0.0) lastdir = -1;

    return false;
  }

  bool PolygonalCurve::inside_(Point p) const
  {
    if (!closed)
      return false;

    int nPoints = pl.size();
    int lastdir = 0;

    for (int i = nPoints - 1; i >= 0 && lastdir == 0; i--)
      {
        unsigned int i1 = (i + 1) % nPoints;
        if (pl[i].y < pl[i1].y) lastdir = 1;
        else if (pl[i].y > pl[i1].y) lastdir = -1;
      }

    int c = 0;

    for (int i = 0; i < nPoints; i++)
      {
        int i1 = (i + 1) % nPoints;
        if (handleOneEdge(p, pl[i], pl[i1],
                          lastdir, c)) return false;
      }

    return (c & 1) == 1;
  }

#undef FNAME

#define FNAME "PolygonalCurve::Distance"
  double PolygonalCurve::distance_(Point p) const
  {
    double mind = std::numeric_limits<double>::max();

    unsigned int last = pl.size() - 1;

    if (closed)
      last = pl.size();

    for (unsigned int i = 0; i < last; i++)
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

#define FNAME "PolygonalCurve::Distance"
  double PolygonalCurve::Length() const
  {
    double len = 0.0;

    for (unsigned int i = 1; i < pl.size(); i++)
      len += ice::Distance(pl[i], pl[i - 1]);

    if (closed)
      len += ice::Distance(pl[0], pl[pl.size() - 1]);

    return len;
  }
#undef FNAME

#define FNAME "PolygonalCurve::Reduced"
  PolygonalCurve PolygonalCurve::Reduced(int nRemainingPoints, int mode) const
  {
    PolygonalCurve res;
    Reduced(nRemainingPoints, res, mode);
    return res;
  }

  void PolygonalCurve::Reduced(int nRemainingPoints, PolygonalCurve& result, int mode) const
  {
    int nAllPoints  = pl.size();

    if (nRemainingPoints < 3 || nRemainingPoints > nAllPoints)
      {
        // reducing to less then 3 point does not make sense
        // "reducing" to more then original points does not make sense
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

    // already reduced enough
    if (nAllPoints == nRemainingPoints)
      {
        result = *this;
        return;
      }

    switch (mode)
      {
      case 2:
      {
        reduced_poly* rpl;
        if (closed)
          rpl = new reduced_poly_cyclic(pl, reduced_poly_cyclic::two);
        else
          rpl = new reduced_poly(pl, reduced_poly::two);

        while (rpl->size() < nRemainingPoints)
          rpl->split();

        while (rpl->size() < (nRemainingPoints + nAllPoints) / 2)
          rpl->split();

        while (rpl->size() > nRemainingPoints)
          rpl->merge();

        rpl->getPolygonalCurve(result);
        delete rpl;
      }
      break;
      case 1:
      {
        reduced_poly* rpl;
        if (closed)
          rpl = new reduced_poly_cyclic(pl, reduced_poly_cyclic::all);
        else
          rpl = new reduced_poly(pl, reduced_poly::all);

        while (rpl->size() > nRemainingPoints) // reduce to wished number of edges
          {
            rpl->merge();
          }

        rpl->getPolygonalCurve(result);
        delete rpl;
      }
      break;
      case 3:
      {
        reduced_poly* rpl;
        if (closed)
          rpl = new reduced_poly_cyclic(pl, reduced_poly_cyclic::all);
        else
          rpl = new reduced_poly(pl, reduced_poly::all);

        rpl->merge();

        while (rpl->size() > nRemainingPoints) // reduce to wished number of edges
          {
            rpl->split();
            rpl->merge();
            rpl->merge();
          }

        rpl->getPolygonalCurve(result);
      }
      break;
      default:
      {
        // undefined mode
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

      }
  }

  PolygonalCurve PolygonalCurve::ReducedToPrecision(double prec, int mode) const
  {
    PolygonalCurve res;
    ReducedToPrecision(prec, res, mode);
    return res;
  }

  void PolygonalCurve::ReducedToPrecision(double prec, PolygonalCurve& result, int mode) const
  {
    if (prec < 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

    switch (mode)
      {
      case 1:
      {
        reduced_poly* rpl;
        if (closed)
          rpl = new reduced_poly_cyclic(pl, reduced_poly_cyclic::all);
        else
          rpl = new reduced_poly(pl, reduced_poly::all);

        while (rpl->size() > 3 && rpl->precision() < prec) // reduce to given precision
          {
            rpl->merge();
            // std::cout << rpl->size() << ": " << rpl->precision() << std::endl;
          }

        if (rpl->precision() > prec)
          rpl->split(); // undo last step
        //  std::cout << rpl.size() << ": " << rpl.precision() << std::endl;
        rpl->getPolygonalCurve(result);
        delete rpl;
      }
      break;
      case 2:
      {
        reduced_poly* rpl;
        if (closed)
          rpl = new reduced_poly_cyclic(pl, reduced_poly_cyclic::two);
        else
          rpl = new reduced_poly(pl, reduced_poly::two);

        rpl->split(); // we need at least 3 corners

        while (rpl->precision() > prec)
          {
            rpl->split();
            //      std::cout << rpl.size() << ": " << rpl.precision() << std::endl;
          }

        rpl->getPolygonalCurve(result);
        delete rpl;
      }
      break;
      case 3:
      {
        reduced_poly* rpl;
        if (closed)
          rpl = new reduced_poly_cyclic(pl, reduced_poly_cyclic::all);
        else
          rpl = new reduced_poly(pl, reduced_poly::all);

        rpl->merge();
        while (rpl->size() > 3 && rpl->precision() < prec) // reduce to given precision
          {
            rpl->split();
            rpl->merge();
            rpl->merge();
            //      std::cout << rpl.size() << ": " << rpl.precision() << std::endl;
          }

        if (rpl->precision() > prec)
          rpl->split(); // undo last step

        rpl->getPolygonalCurve(result);
        delete rpl;
      }
      break;
      default:
      {
        // undefined mode
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

      }
    return;
  }
#undef FNAME

#define FNAME "PolygonalCurve::getClosestCorner"
  int PolygonalCurve::getClosestCorner(Point p) const
  {
    int mini = 0;
    double mdist = ice::Distance(pl[0], p);

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        double da = ice::Distance(pl[i], p);

        if (da < mdist)
          {
            mdist = da;
            mini = i;
          }
      }

    return mini;
  }
#undef FNAME
#define FNAME "PolygonalCurve::getClosestEdge"
  int PolygonalCurve::getClosestEdge(Point p) const
  {
    int mini = 0;
    double mdist = ice::Distance(pl[0], p);

    unsigned int last = pl.size() - 1;

    if (closed)
      last = pl.size();

    for (unsigned int i = 0; i < last; i++)
      {
        double da = Edge(i).Distance(p);

        if (da < mdist)
          {
            mdist = da;
            mini = i;
          }
      }

    return mini;
  }
#undef FNAME
}
