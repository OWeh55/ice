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
#ifndef ICE_TRIANGLE_H
#define ICE_TRIANGLE_H

#include "numbase.h"
#include "GeoObject.h"
#include "region.h"
#include "lineseg.h"

namespace ice
{
  class Triangle: public GeoObject
  {
  public:
    /** standard constructor
     */
    Triangle(): GeoObject(-1, 0), p1(-1, 0), p2(0, 1), p3(1, 0)
    {
      initCC();
    }
    /** constructor with given Points.
    \param pp1 Corner of triangle
    \param pp2 Corner of triangle
    \param pp3 Corner of triangle
    */
    Triangle(Point pp1, Point pp2, Point pp3):
      GeoObject(pp1), p1(pp1), p2(pp2), p3(pp3)
    {
      initCC();
    }

    virtual bool isNonDegenerate()
    {
      return nonDegenerate;
    }

    /** get Corner
     */
#define FNAME "Triangle::getCorner"
    virtual Point getCorner(int i) const
    {
      switch (i)
        {
        case 0:
          return p1;
        case 1:
          return p2;
        case 2:
          return p3;
        default:
          throw IceException(FNAME, M_WRONG_INDEX);
        }
    }
#undef FNAME

    const Point& P1() const
    {
      return p1;
    }
    const Point& P2() const
    {
      return p2;
    }
    const Point& P3() const
    {
      return p3;
    }

    virtual Point getCCCenter() const
    {
      return CCCenter;
    }

    virtual const double& getCCRadius() const
    {
      return CCR;
    }

    virtual double getArea() const override;

    virtual bool isInsideCC(Point point) const;

    virtual bool isCCW() const;

    virtual void makeCCW();

    virtual bool getInsidePoint(IPoint& pp) const override
    {
      IPoint ip((p1 + p2 + p3) / 3);
      pp = ip;
      return inside_(pp);
    }

  protected:

    virtual bool inside_(Point pp) const override;
    virtual double distance_(Point pp) const override;

  private:
    Point p1, p2, p3;

    Point CCCenter;   // center of circumcircle
    double CCR;       // radius of circumcircle
    double CCR2;      // radius of circumcircle, squared
    bool nonDegenerate;

    void initCC();
  };

  void Triangle2Region(const Triangle& t, Region& r);
}
#endif
