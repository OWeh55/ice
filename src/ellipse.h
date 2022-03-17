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
#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "Vector.h"
#include "base.h"
#include "circle.h"

namespace ice
{
  /*--------------- Ellipse -------------------*/
  class Ellipse : public Circle
  {
  public:
    Ellipse();
    Ellipse(double xp, double yp, double rp, double r2p, double phi);
    Ellipse(Point p, double rp, double r2p, double phi);

    explicit Ellipse(const Vector& v);
    explicit Ellipse(double d[]);

    double getR2() const
    {
      return r2;
    }
    int setR2(double vr);

    double getPhi() const
    {
      return phi;
    }
    void setPhi(double vr);

  protected:
    double r2;
    double phi;

    // interne Groessen
    double cc, ss, aa, bb;

    //    virtual int draw(Image img,int val,int fval=-1) const;
    virtual bool inside_(Point pp) const override;
    virtual double distance_(Point pp) const override;

  private:
    void normalize();
  };

  /*---- Ellipsen-Segment ------------------------------*/

  class EllipseSeg : public Ellipse
  {

  public:
    EllipseSeg();
    EllipseSeg(const EllipseSeg& e);

    EllipseSeg(double xp, double yp,
               double rp, double r2p, double phi,
               double phi1, double phi2);

    EllipseSeg(Point p,
               double rp, double r2p, double phi,
               double phi1, double phi2);

    explicit EllipseSeg(const Vector& v);
    explicit EllipseSeg(double d[]);

    double getPhi2() const
    {
      return phi2;
    }
    double getPhi1() const
    {
      return phi1;
    }

    void setPhi1(double val);
    void setPhi2(double val);

  protected:
    //    virtual int draw(Image img,int val,int fval=-1) const;
    virtual bool inside_(Point pp) const override;

    double phi1; // segment limits relative to ellipse
    double phi2;

    double phi1a; // segment limits relative to x axis
    double phi2a;

  private:
    void normalize();
  };
}
#endif
