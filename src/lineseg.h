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
#ifndef LINESEG_H
#define LINESEG_H

#include <algorithm>

#include "Vector.h"
#include "base.h"
#include "GeoObject.h"
#include "Point.h"
#include "distance.h"

namespace ice
{
  /*--------------- Gerade / Strecke -------------------*/
  class LineSeg: public GeoObject
  {
  public:
    // Bitmasken:
    static const int begin_l = 1 << 0;  // anfang begrenzt
    static const int end_l   = 1 << 1;  // ende begrenzt
    // lineseg-typen
    static const int line    = 0;               // Gerade - unlimitiert
    static const int ray     = begin_l;         // Strahl - anfang limitiert
    static const int inv_ray = end_l;           // inverser Strahl - ende limitiert (nur der Vollständigkeit wegen)
    static const int segment = begin_l | end_l; // Strecke

  public:
    /** standard constructor.
      line segment from 0,0 to 0,0
     */
    LineSeg() : GeoObject(), dp(0, 0), type(segment) {}

    /** copy constructor.
     */
    LineSeg(const LineSeg& ls): GeoObject(ls), dp(ls.dp), type(ls.type) {}

    /** constructor for line segment from xp,yp to x2p,y2p
     */
    LineSeg(double xp, double yp, double x2p, double y2p, int typep = segment):
      GeoObject(xp, yp), dp(x2p - xp, y2p - yp), type(typep)
    {
      if (dp == IPoint(0, 0))
        {
          dp.x = 1e-6;
        }
    }

    /** constructor for line segment from xp,yp to x2p,y2p
     */
    LineSeg(Point p1, Point p2, int typep = segment):
      GeoObject(p1), dp(p2 - p1), type(typep)
    {
      if (p1 == p2)
        {
          dp.x = 1e-6;
        }
    }

    /** constructor for ray from p in direction fi
     */
    LineSeg(double p, double fi);

    /** constructor from parameter Vector
     */
    explicit LineSeg(const Vector& v, int typep = segment);
    // explicit LineSeg(double d[],int typep=segment);

    /** get starting point
     */
    virtual Point getP1() const
    {
      return p;
    }
    /** set starting point
     */
    virtual void setP1(Point pp)
    {
      Point p2 = p + dp;
      p = pp;
      dp = p2 - p;
    }

    /** get final point
     */
    virtual Point getP2() const
    {
      return p + dp;
    }

    /** get final point
     */
    virtual void setP2(Point pp)
    {
      dp = pp - p;
    }

    // Parameter der Hesseschen Form
    virtual void CalcHesse(double& p, double& phi) const;
    virtual double P() const
    {
      double p, phi;
      CalcHesse(p, phi);
      return p;
    }
    virtual double Phi() const
    {
      double p, phi;
      CalcHesse(p, phi);
      return phi;
    }

    // alternative name
    virtual double Length() const
    {
      return dp.r();
    }

    virtual double Length2() const
    {
      return dp.r2();
    }

    // Winkel der (orientierten) Gerade zu X-Achse
    virtual double OrientationAngle() const;

    // Winkel zwischen zwei Geraden
    virtual double Angle(const LineSeg& sec) const;

    // Abfrage/Setzen des Types
    virtual int Type() const
    {
      return type;
    }
    virtual void setType(int typep);

    virtual bool inside(Point p) const
    {
      return false;
    }

    virtual double distance_(Point p) const override
    {
      return ice::Distance(p, ClosestPoint(p));
    }

    virtual double getArea() const override
    {
      return 0;
    }

    double relCross(Point pp)
    {
      double dxp = pp.x - p.x;
      double dyp = pp.y - p.y;
      return dyp * dp.x - dxp * dp.y;
    }

    virtual bool RightOf(Point pp) const
    {
      double dxp = pp.x - p.x;
      double dyp = pp.y - p.y;
      return (dyp * dp.x - dxp * dp.y) > 0;
    }

    virtual bool LeftOf(Point pp) const
    {
      double dxp = pp.x - p.x;
      double dyp = pp.y - p.y;
      return (dyp * dp.x - dxp * dp.y) < 0;
    }

    virtual Point RelPoint(double my) const
    {
      return p + dp * my;
    }

    virtual double limitedMy(double my) const
    {
      // begrenzt das my der Parameterdarstellung auf einen dem Typ
      // entsprechenden Bereich
      if ((my < 0.0) && (type & begin_l))
        {
          my = 0.0;
        }

      if ((my > 1.0) && (type & end_l))
        {
          my = 1.0;
        }

      return my;
    }

    virtual bool limitMys(double& my1, double& my2) const
    {
      // tested ob der Bereich my1..my2 Teile von *this umfasst
      // wenn ja: begrenzt my1..my2 auf den Bereich von *this
      if (my1 > my2)
        std::swap(my1, my2);

      if (type & begin_l)
        {
          if (my2 < 0.0)
            return false;

          if (my1 < 0.0)
            my1 = 0.0;
        }

      if (type & end_l)
        {
          if (my1 > 1.0)
            return false;

          if (my2 > 1.0)
            my2 = 1.0;
        }

      return true;
    }

    virtual Point ClosestPoint(Point p) const;
    virtual Point ClosestPoint(Point p, double& my) const;

    virtual bool Intersection(const LineSeg& second) const;
    virtual bool Intersection(const LineSeg& second, Point& p) const;
    virtual bool Intersection(const LineSeg& second, Point& p, double& my1, double& my2) const;

    virtual Point Normal() const;

  protected:
    Point dp; // vector from start (GeoObject p) to end
    int type;
  };
}
#endif
