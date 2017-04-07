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
 * Distortion0.h
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

#ifndef _DISTORTION0_H
#define _DISTORTION0_H

#include <string>
#include <iostream>
#include <vector>

#include "Vector.h"
#include "Matrix.h"
#include "Trafo.h"

#include "DistortionR2.h"

namespace ice
{
  class Distortion0: public DistortionR2
  {
    // radial distortion
    // r' = r * (1 + d2*r^2)

  public:
    // standard constructor
    Distortion0();
    // constructor with known parameters of distortion
    Distortion0(double x0, double y0, double d2 = 0.0);
    Distortion0(double x0, double y0, double d2, int dimx, int dimy);

    // constructors, which calculate distortion from references
    Distortion0(const Matrix& mark, const Matrix& orig, Trafo& tr, const Vector& ImageCenter);
    Distortion0(const Matrix& mark, const Matrix& orig, const Vector& ImageCenter);
    Distortion0(const Matrix& mark, const Matrix& orig, Trafo& tr);
    Distortion0(const Matrix& mark, const Matrix& orig);

    virtual ~Distortion0() {};

    virtual double ffr2(double r2) const;
    virtual double frs(double r) const;

    double D2() const
    {
      return d2;
    }

    virtual Vector makeVector() const;
    virtual void set(const Vector& v);

    virtual std::string toString() const;
    virtual void set(const std::string& parastring);

    virtual Distortion0& operator =(const Distortion0& d);

  protected:

    double d2;
  };
}
#endif
