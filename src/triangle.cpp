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

#include <algorithm>

#include "triangle.h"

using namespace std;

namespace ice
{
  bool Triangle::inside_(Point pp) const
  {
    if (!nonDegenerate)
      {
        return false;
      }
    // Compute vectors
    Point v0 = p2 - p1;
    Point v1 = p3 - p1;
    Point vp = pp - p1;

    // Compute dot products
    double dot00 = v0 * v0;
    double dot01 = v0 * v1;
    double dot0p = v0 * vp;
    double dot11 = v1 * v1;
    double dot1p = v1 * vp;

    // Compute barycentric coordinates
    double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot0p - dot01 * dot1p) * invDenom;
    double v = (dot00 * dot1p - dot01 * dot0p) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
  }

  double Triangle::distance_(Point pp) const
  {
    double d = LineSeg(p1, p2).Distance(pp);
    double dh = LineSeg(p2, p3).Distance(pp);
    if (dh < d)
      {
        d = dh;
      }
    dh = LineSeg(p3, p1).Distance(pp);
    if (dh < d)
      {
        d = dh;
      }
    return d;
  }

  double Triangle::getArea() const
  {
    double area2 = 0.0;
    area2 += (p1.x - p2.x) * (p1.y + p2.y);
    area2 += (p2.x - p3.x) * (p2.y + p3.y);
    area2 += (p3.x - p1.x) * (p3.y + p1.y);
    return fabs(area2 * 0.5);
  }

  bool Triangle::isInsideCC(Point point) const
  {
    // returns if point is in the triangle's circumcircle.
    // a point exactly on the circle is also considered to be in the circle.

    double dist2 = (point - CCCenter).r2();
    return dist2 <= CCR2;
  }

  bool Triangle::isCCW() const
  {
    Point v1 = p2 - p1;
    Point v2 = p3 - p1;
    return v1.Cross(v2) > 0;
  }

  void Triangle::makeCCW()
  {
    if (!isCCW())
      {
        swap(p2, p3);
      }
  }

  static void perpendicularBisector(Point p1, Point p2, Point& m, Point& s)
  {
    m = (p1 + p2) * 0.5; // mittelpunkt
    Point h12 = p2 - p1;
    s = Point(-h12.y, h12.x); // senkrechte richtung
  }

  void Triangle::initCC()
  {
    Point m12, v12;
    perpendicularBisector(p1, p2, m12, v12);

    Point m13, v13;
    perpendicularBisector(p1, p3, m13, v13);

    double a1 = v12.x;
    double b1 = -v13.x;
    double i1 = m13.x - m12.x;
    double a2 = v12.y;
    double b2 = -v13.y;
    double i2 = m13.y - m12.y;

    double k1, k2;
    nonDegenerate = Solve2(a1, b1, i1, a2, b2, i2, k1, k2);
    if (nonDegenerate)
      {
        CCCenter = k1 * v12 + m12;

        CCR2 = (CCCenter - p1).r2();
        CCR = sqrt(CCR2);
      }
  }

  static bool intrange(double v1, double v2, int& i1, int& i2)
  {
    if (v1 > v2)
      {
        std::swap(v1, v2);
      }
    i1 = (int) ceil(v1);
    i2 = (int) floor(v2);
    return i2 >= i1;
  }

  void Triangle2Region(const Triangle& t, Region& r)
  {
    Point p1 = t.P1();
    Point p2 = t.P2();
    Point p3 = t.P3();

    // sort ascending y
    if (p1.y > p2.y)
      {
        swap(p1, p2);
      }
    if (p2.y > p3.y)
      {
        swap(p2, p3);
      }
    if (p1.y > p2.y)
      {
        swap(p1, p2);
      }

    // lines as functions x(y) = ay + b
    double a12 = p1.y == p2.y ? 0 : (p2.x - p1.x) / (p2.y - p1.y);
    double b12 = p1.x - a12 * p1.y;

    double a13 = p1.y == p3.y ? 0 : (p3.x - p1.x) / (p3.y - p1.y);
    double b13 = p1.x - a13 * p1.y;

    double a23 = p2.y == p3.y ? 0 : (p3.x - p2.x) / (p3.y - p2.y);
    double b23 = p2.x - a23 * p2.y;

    int y1, y2;
    if (intrange(p1.y, p2.y, y1, y2))
      {
        for (int y = y1; y <= y2; y++)
          {
            double x1 = a12 * y + b12;
            double x2 = a13 * y + b13;
            int ix1, ix2;
            if (intrange(x1, x2, ix1, ix2))
              {
                r.add(ix1, y, ix2, y);
              }
          }
      }

    if (intrange(p2.y, p3.y, y1, y2))
      {
        for (int y = y1; y <= y2; y++)
          {
            double x1 = a23 * y + b23;
            double x2 = a13 * y + b13;
            int ix1, ix2;
            if (intrange(x1, x2, ix1, ix2))
              {
                r.add(ix1, y, ix2, y);
              }
          }
      }
  }
}
