/*
 * - C++ - Library for image processing
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
 * Distortion.h
 * datatype for radial distortion
 * Let r be the radius in undistorted image
 * Let r' be the radius in distorted image
 *
 * Distortion - abstract base class
 *
 * DistortionR - abstract
 *       r´ = r * fr(r)
 *
 * DistortionR2 - abstract
 *       r´ = r * fr(r^2)
 *
 * Distortion0
 *       r' = r * (1 + d2 * r^2)
 *
 * Distortion1
 *       r' = r * (1 + d2 * r^2 + d4 * r^4)
 *
 * Distortion2
 *       r' = r * (1 + d2 * r^2 + d3 * r^3 + d4 * r^4)
 *
 * Distortion3
 *       r' = r * (1 + d2 * r^2 + d4 * r^4 + d6 * r^6)
 *
 * calibration and correction of distortion in images
 *
 * Mike Leuschner 07/2004
 * Wolfgang Ortmann 06/05, 12/08, 11/12
 */

#ifndef _DISTORTION3_H
#define _DISTORTION3_H

#include <string>
#include <iostream>
#include <vector>

#include "base.h"
#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include "Trafo.h"
#include "DistortionR2.h"

namespace ice
{
  class Distortion3: public DistortionR2
  {
    // radial distortion
    // r' = r * (1 + d2 * r^2 + d4 * r^4 + d6 * r^6)
  public:

    Distortion3();
    Distortion3(double x0, double y0, double d2 = 0.0, double d4 = 0.0, double d6 = 0.0);
    Distortion3(double x0p, double y0p, double d2p, double d4p, double d6p,
                int dimx, int dimy);

    Distortion3(const Matrix& mark, const Matrix& orig,
                Trafo& tr, const ice::Vector& ImageCenter);
    Distortion3(const Matrix& mark, const Matrix& orig, const ice::Vector& ImageCenter);
    Distortion3(const Matrix& mark, const Matrix& orig, Trafo& tr);
    Distortion3(const Matrix& mark, const Matrix& orig);

    virtual ~Distortion3() {};

    //      virtual double fr(double r) const;
    virtual double ffr2(double r) const;
    virtual double frs(double r) const;

    virtual double D2() const
    {
      return d2;
    }
    virtual double D4() const
    {
      return d4;
    }
    virtual double D6() const
    {
      return d6;
    }

    virtual Vector MakeVector() const;
    virtual void set(const Vector& v);

    virtual std::string toString() const;
    virtual void set(const std::string& parastring);

    virtual Distortion3& operator =(const Distortion3& d);

    //    virtual int Distort(double x,double y,double &xd,double &yd) const;

  protected:

    double d2, d4, d6;
  };
}
#endif
