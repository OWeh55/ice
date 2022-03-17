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
 * DistortionR.h
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

#ifndef _DISTORTORTIONR_H
#define _DISTORTORTIONR_H

#include "Distortion.h"

namespace ice
{
  class DistortionR : public Distortion
  {
    // Model for radial distortion r'=fr(r)
    // abstract
  public:
    DistortionR(): Distortion() {}
    DistortionR(double x0p, double y0p): Distortion(x0p, y0p) {}
    DistortionR(double x0p, double y0p, int dimx, int dimy): Distortion(x0p, y0p, dimx, dimy) {}

    using Distortion::rectify;
    virtual int rectify(double x, double y, double& xr, double& yr) const override;
    using Distortion::distort;
    virtual int distort(double x, double y, double& xd, double& yd) const override;

  protected:
    virtual double fr(double r) const
    {
      return r * ffr(r);
    };
    virtual double ffr(double r) const = 0;
    virtual double frs(double r) const = 0;
  };
}
#endif
