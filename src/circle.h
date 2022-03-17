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
/*
  Klassen zur Repräsentation geometrischer Elemente
*/
#ifndef CIRCLE_H
#define CIRCLE_H

#include <cmath>

#include "Vector.h"
#include "base.h"
#include "numbase.h"
#include "geo_ob.h"
#include "distance.h"
#include "Point.h"

namespace ice
{
  /** representation of the geometry of circles
   *
   */
  class Circle : public GeoObject
  {

  public:
    /** standard constructor
     */
    Circle(): GeoObject(), r(0) {}
    /** constructor with given coordinates and radius.
      \param xp x coordinate of center
      \param yp y coordinate of center
      \param rp radius
    */
    Circle(double xp, double yp, double rp): GeoObject(xp, yp), r(rp) {}
    /** constructor with given center and radius
      \param p - center as Point
      \param rp - radius
    */
    Circle(Point p, double rp): GeoObject(p), r(rp) {}

    /** constructor with parameters given as Vector.
      Vector must contain x, y, and r in this order
    */
    explicit Circle(const Vector& v);

    /** constructor with parameters given as C array.
      C array must contain x, y, and r in this order
    */
    explicit Circle(double d[]);

    /** get radius
     */
    double getR() const
    {
      return r;
    }

    /** set radius
      \param r radius
    */
    int setR(double r);

    double getArea() const override
    {
      return M_PI * r * r;
    }

    virtual bool getInsidePoint(IPoint& pp) const override
    {
      IPoint ip(p);
      pp = ip;
      return inside_(pp);
    }

  protected:

    virtual bool inside_(Point pp) const  override
    {
      return ice::Distance(p, pp) < r;
    }

    virtual double distance_(Point pp) const  override
    {
      return fabs(ice::Distance(p, pp) - r);
    }

    /**
      radius of the cycle
    */
    double r;
  };

}
#endif
