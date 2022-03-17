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

#include "numbase.h"
#include "analygeo.h"
#include "distance.h"

namespace ice
{
  double DistancePointLinesegment(double x, double y, // point coordinates
                                  double x1, double y1, // linesegment start
                                  double x2, double y2) // .. end
  {
    double xn = x1;
    double yn = y1;

    double dx1 = x2 - x1;
    double dy1 = y2 - y1;
    double dx2 =  x - x1;
    double dy2 =  y - y1;

    double len2 = dx1 * dx1 + dy1 * dy1;

    // Geradensegment nicht entartet?
    if (len2 > EPSILON)
      {
        double my = (dx1 * dx2 + dy1 * dy2) / len2;

        /* Fusspunkt hinter Endpunkt? */
        if (my >= 1)
          {
            xn = x2;
            yn = y2;
          }
        else

          /* Fusspunkt nach Anfangspunkt? */
          if (my > 0)
            {
              // Fusspunkt zwischen Anfangs- und Endpunkt
              xn = x1 + my * dx1;
              yn = y1 + my * dy1;
            }
      }

    return Distance(x, y, xn, yn);
  }
  /*************************************************************************/
#define FNAME "Intersect"
  bool Intersect(double a[2], double b[2], double c[2], double d[2], double* l)
  {
    double l1, l2;
    double x21, x31, x43, y21, y31, y43;
    x21 = b[0] - a[0];
    y21 = b[1] - a[1];
    x31 = c[0] - a[0];
    y31 = c[1] - a[1];
    x43 = d[0] - c[0];
    y43 = d[1] - c[1];

    l1 = (x21 * y43 - x43 * y21);

    if (fabs(l1) < 1e-10)
      {
        return false;
      }

    *l = (x31 * y43 - x43 * y31) / l1;

    if ((*l <= 0.00001) || (*l >= 0.9999))
      {
        return false;
      }

    l2 = -1;

    if (fabs(y43) > 1e-10)
      {
        l2 = (*l * y21 - y31) / y43;
      }
    else if (fabs(x43) > 1e-10)
      {
        l2 = (*l * x21 - x31) / x43;
      }

    if (l2 <= 0.00001 || l2 >= 0.9999)
      {
        return false;
      }

    return true;
  }

  bool Intersect(const Point& a, const Point& b,
                 const Point& c, const Point& d,
                 Point& res)
  {
    bool result = true;

    double x21 = b.x - a.x;
    double y21 = b.y - a.y;
    double x31 = c.x - a.x;
    double y31 = c.y - a.y;
    double x43 = d.x - c.x;
    double y43 = d.y - c.y;

    double l = x21 * y43 - x43 * y21;

    if (l == 0.0)
      {
        // Parallele Geraden
        return false;
      }

    double l1 = (x31 * y43 - x43 * y31) / l;

    // Geradenschnittpunkt ausserhalb strecke 1
    if ((l1 < 0.0) || (l1 > 1.0))
      {
        result = false;
      }

    double l2 = (x31 * y21 - x21 * y31) / l;

    // Geradenschnittpunkt ausserhalb strecke 2
    if (l2 < 0.0 || l2 > 1.0)
      {
        result = false;
      }

    // Schnittpunkt immer berechnen, auch wenn ausserhalb der Strecken
    res = l1 * (b - a) + a;
    return result;
  }

  bool Intersect(const Vector& a, const Vector& b,
                 const Vector& c, const Vector& d, Vector& s)
  {
    Point ap(a[0], a[1]);
    Point bp(b[0], b[1]);
    Point cp(c[0], c[1]);
    Point dp(d[0], d[1]);
    Point sp;
    bool rc = Intersect(ap, bp, cp, dp, sp);
    s = Vector(sp.x, sp.y);
    return rc;
  }
#undef FNAME
}
