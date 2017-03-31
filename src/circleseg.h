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
/*
  Klassen zur Repräsentation geometrischer Elemente
*/
#ifndef CIRCLESEG_H
#define CIRCLESEG_H

#include "Vector.h"
#include "base.h"
#include "numbase.h"
#include "geo_ob.h"
#include "distance.h"
#include "Point.h"
#include "circle.h"

namespace ice
{
  /*--------------- Kreis-Segment -------------------*/
  class CircleSeg : public Circle
  {
  private:
    void normalize();
    bool phi_inside(double phi) const
    {
      if (phi < phi1)
        {
          phi += 2 * M_PI;
        }

      return (phi <= phi2);
    }

  public:
    CircleSeg(): Circle(), phi1(0), phi2(0) {}
    CircleSeg(double xp, double yp,
              double rp,
              double phi1p, double phi2p)
      : Circle(xp, yp, rp), phi1(phi1p), phi2(phi2p)
    {
      normalize();
    }

    CircleSeg(const Vector& v);

    CircleSeg(double p[]): Circle(p), phi1(p[3]), phi2(p[4])
    {
      normalize();
    }

    CircleSeg(Point p, double rp, double phi1p, double phi2p)
      : Circle(p, rp), phi1(phi1p), phi2(phi2p)
    {
      normalize();
    }

    virtual double getPhi1() const
    {
      return phi1;
    }
    virtual void setPhi1(double phi1);

    virtual double getPhi2() const
    {
      return phi2;
    }
    virtual void setPhi2(double phi2);

    virtual bool getInsidePoint(IPoint& pp) const
    {
      IPoint ip(p);
      double mfi = (phi1 + phi2) / 2.0;
      Point drad(cos(mfi)*r * 0.7, sin(mfi)*r * 0.7);
      pp = ip + IPoint(drad);
      return inside_(pp);
    }

  protected:
    double phi1, phi2;

    virtual double distance_(Point p) const;

    virtual bool inside_(Point p) const;
  };
}
#endif
