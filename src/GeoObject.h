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
 * Klassen zur Repräsentation geometrischer Elemente
 */
#ifndef GEO_OB_H
#define GEO_OB_H

#include "Vector.h"
#include "Point.h"

namespace ice
{

  /** abstract base class for geometric 2d objects

   */
  class GeoObject
  {
  public:
    /**
     * standard constructor: object in point (0,0)
     */
    GeoObject(): p(0, 0) {}

    /**
     * constructor with coordinates: object in point (xp,yp)
     * @param xp  x coordinate
     * @param yp  y coordinate
     */
    GeoObject(double xp, double yp): p(xp, yp) {}

    /**
     * constructor with coordinates: object in point pp
     * @param pp  position
     */
    explicit GeoObject(Point pp): p(pp) {}

    /**
     * constructor with coordinates: object in point pp
     * @param pp  position
     */
    explicit GeoObject(IPoint pp): p(pp) {}

    /**
     * constructor with coordinates: object in point v
     * @param v  position
     */
    explicit GeoObject(const Vector& v);

    /**
     * constructor with coordinates in double array
     * @param d position
     */
    explicit GeoObject(double d[]): p(d[0], d[1]) {}

    /**
     * destructor
     */
    virtual ~GeoObject() {};

    /**
     * get methode for position
     */
    virtual Point getPos() const
    {
      return p;
    }

    /**
     * set methode for position
     */
    virtual void setPos(Point pp)
    {
      p = pp;
    }

    /**
     * conversion to Vector.
     * gives an description of the GeoObject as Vector\n
     * must be reimplemented by child
     */
    virtual operator Vector() const
    {
      return Vector(p);
    }

    /**
     * area of object
     */

    virtual double getArea() const = 0;

    /**
     * Shift by distance
     *
     * @param dx  x shift
     * @param dy  y shift
     */
    virtual void Shift(double dx, double dy);
    /**
     * Shift by distance
     *
     * @param p  shift by p.x, p.y
     */
    virtual void Shift(Point p)
    {
      Shift(p.x, p.y);
    }

    // ------------- distances -----------------------------------

    /**
     * distance of point to GeoObject
     * should _not_ be reimplemented by child
     *
     * @param dx  x coordinate of point
     * @param dy  y coordinate of point
     */
    virtual double Distance(double dx, double dy) const
    {
      return distance_(Point(dx, dy));
    };

    /**
     * distance of point to GeoObject
     * should _not_ be reimplemented by child
     *
     * @param p  position of point
     */
    virtual double Distance(Point p) const
    {
      return distance_(p);
    }

    // --------- inside -------------------------------

    /**
     * is point inside GeoObject.
     * should not be reimplemented by child
     *
     * @param x  x coordinate of point
     * @param y  y coordinate of point
     */
    virtual bool Inside(double x, double y) const
    {
      return inside_(Point(x, y));
    }

    /**
     * is point inside GeoObject.
     * should not be reimplemented by child
     *
     * @param p position of point
     * should not be reimplemented by child
     */
    virtual bool Inside(Point p) const
    {
      return inside_(p);
    };

    /**
     * getInsidePoint(IPoint& p) const
     * get a point (of type IPoint) inside of object
     * returns false, of no such point exists
     * should not be reimplemented by child
     */

    virtual bool getInsidePoint(IPoint& p) const
    {
      return false;
    }

  protected:
    // ------------- distances -----------------------------------
    /**
     * "real" distance method.
     * *this* method must be reimplemented by childs
     */
    virtual double distance_(Point p) const = 0;

    // --------- Inside -------------------------------

    /** "real" inside method.
     *  *this* method must be reimplemented by childs
     */
    virtual bool inside_(Point p) const
    {
      return false;
    }

  protected:
    Point p;
  };
}
#endif
