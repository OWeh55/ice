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
 * DistortionR2.h
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

#ifndef _DISTORTIONR2_H
#define _DISTORTIONR2_H

#include "Distortion.h"

namespace ice
{
  class DistortionR2 : public Distortion
  {
    // radial distortion depends on r^2
    // abstract
  public:
    DistortionR2(): Distortion() {}
    DistortionR2(double x0p, double y0p): Distortion(x0p, y0p) {}
    DistortionR2(double x0p, double y0p, int dimx, int dimy): Distortion(x0p / dimx, y0p / dimy) {}

    using Distortion::rectify;
    virtual int rectify(double x, double y, double& xr, double& yr) const;
    using Distortion::distort;
    virtual int distort(double x, double y, double& xd, double& yd) const;

  protected:
    virtual double fr(double r) const
    {
      return r * ffr2(r * r);
    };
    virtual double ffr2(double r2) const = 0;
    virtual double frs(double r2) const = 0;
  };
}
#endif
