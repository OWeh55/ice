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
#ifndef _DISTANCE_H
#define _DISTANCE_H

/* base functions for distances and intersection of geometric primitives */

#include <stdlib.h>

#include "base.h"
#include "Vector.h"
#include "Point.h"

namespace ice
{
  double DistancePointLinesegment(double x, double y, // point coordinates
                                  double x1, double y1, // linesegment start
                                  double x2, double y2); // .. end

  bool Intersect(double a[2], double b[2], double c[2], double d[2], double* l);
  bool Intersect(const Vector& a, const Vector& b,
                 const Vector& c, const Vector& d, Vector& s);
  bool Intersect(const Point& a, const Point& b,
                 const Point& c, const Point& d,
                 Point& s);

  inline double Distance(double x1, double y1, double x2, double y2)
  {
    return sqrt(Sqr(x2 - x1) + Sqr(y2 - y1));
  }

  inline double Distance(double x, double y)
  {
    return sqrt(x * x + y * y);
  }

  template<typename T>
  double Distance(pointT<T> p1, pointT<T> p2)
  {
    return sqrt(Sqr(p2.x - p1.x) + Sqr(p2.y - p1.y));
  }

  inline double Distance2(double x1, double y1, double x2, double y2)
  {
    return sqrt(Sqr(x2 - x1) + Sqr(y2 - y1));
  }

  template<typename T>
  inline double Distance2(pointT<T> p1, pointT<T> p2)
  {
    return Sqr(p2.x - p1.x) + Sqr(p2.y - p1.y);
  }

  double Distance(const Vector& p1, const Vector& p2);
}
#endif
