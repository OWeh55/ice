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
#include <math.h>

#include "Vector.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "defs.h"
#include "numbase.h"
#include "base.h"
#include "lineseg.h"
#include "drawline.h"

namespace ice
{
#define FNAME "LineSeg::LineSeg"
  LineSeg::LineSeg(double pp, double phi) :
    // dem Ursprung nächster Punkt als Startpunkt
    GeoObject(pp * cos(phi), pp * sin(phi)),
    // Punkt mit Abstand 1 als zweiter Punkt
    dp(-sin(phi), cos(phi)),
    type(line)
  {
  }

  LineSeg::LineSeg(const Vector& v, int typep) : GeoObject(v), type(typep)
  {
    if (v.Size() < 4)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    dp.x = v[2] - p.x;
    dp.y = v[3] - p.y;
  }
#undef FNAME

#define FNAME "LineSeg::setType"
  void LineSeg::setType(int typep)
  {
    if ((typep != line) && (typep != ray) && (typep != segment) && (typep != inv_ray))
      throw IceException(FNAME, M_WRONG_MODE);

    type = typep;
  }
#undef FNAME

  void LineSeg::CalcHesse(double& pp, double& phi) const
  {
    double d1 = -dp.y;
    double d2 = dp.x;

    if (d1 == 0.0 && d2 == 0.0)
      {
        // Punkte sind identisch, Gerade ist mehrdeutig
        // Gerade durch Ursprung wählen
        d1 = p.y;
        d2 = p.x;

        if (d1 == 0.0 && d2 == 0.0)
          // beide punkte im Ursprung !!
          {
            // beliebige Belegung
            d1 = 1.0;
            d2 = 0.0;
          }
      }

    double hs = sqrt(d1 * d1 + d2 * d2);

    pp = (-p.x * d1 - p.y * d2) / hs;

    if (pp >= 0)
      {
        d1 = -d1;
        d2 = -d2;
      }
    else
      {
        pp = -pp;
      }

    phi = atan2(d2, d1);
    return;
  }

  double LineSeg::OrientationAngle() const
  {
    double dy = dp.y;
    double dx = dp.x;

    if (dx == 0.0 && dy == 0.0)
      {
        // Punkte sind identisch, Gerade ist mehrdeutig
        // Gerade durch Ursprung wählen
        dy = p.y;
        dx = p.x;

        if (dx == 0.0 && dy == 0.0)
          // beide punkte im Ursprung !!
          {
            // beliebige Belegung
            dx = 1.0;
            dy = 0.0;
          }
      }

    return atan2(dy, dx);
  }

  double LineSeg::Angle(const LineSeg& sec) const
  {
    double alpha1 = OrientationAngle();
    double alpha2 = sec.OrientationAngle();

    while (alpha2 < alpha1)
      {
        alpha2 += 2 * M_PI;
      }

    return alpha2 - alpha1;
  }

#define FNAME "LineSeg::ClosestPoint"
  Point LineSeg::ClosestPoint(Point pp, double& my) const
  {
    double dx2 = pp.x - p.x;
    double dy2 = pp.y - p.y;

    double len2 = dp.x * dp.x + dp.y * dp.y;

    /* Geradensegment entartet? */
    if (len2 < 1e-15)
      {
        return p;
      }
    else
      {
        my = (dp.x * dx2 + dp.y * dy2) / len2 ;
        my = limitedMy(my); // my je nach typ beschränken
        return RelPoint(my);
      }
  }

  Point LineSeg::ClosestPoint(Point pp) const
  {
    double my;
    return ClosestPoint(pp, my);
  }
#undef FNAME
#define FNAME "LineSeg::Intersection"

  bool LineSeg::Intersection(const LineSeg& second) const
  {
    Point p;
    return Intersection(second, p);
  }

  bool LineSeg::Intersection(const LineSeg& second, Point& res) const
  {
    double my1, my2;
    return Intersection(second, res, my1, my2);
  }

  bool LineSeg::Intersection(const LineSeg& second, Point& pp, double& my1, double& my2) const
  {
    if (! Solve2(dp.x, -second.dp.x, second.p.x - p.x,
                 dp.y, -second.dp.y, second.p.y - p.y,
                 my1, my2))
      {
        return false;  // no intersection
      }

    pp = RelPoint(my1);

    // check if inside (of line segment or ray)
    if (type & begin_l)
      {
        if (my1 < 0.0)
          {
            return false;
          }
      }

    if (type & end_l)
      {
        if (my1 > 1.0)
          {
            return false;
          }
      }

    if (second.type & begin_l)
      {
        if (my2 < 0.0)
          {
            return false;
          }
      }

    if (second.type & end_l)
      {
        if (my2 > 1.0)
          {
            return false;
          }
      }

    return true;
  }
#undef FNAME

  Point LineSeg::Normal() const
  {
    return Point(dp.y, -dp.x).normalized();
  }

}
