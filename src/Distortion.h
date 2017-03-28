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
 * datatype for radial distortions
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

#ifndef _DISTORTION_H
#define _DISTORTION_H

#include <string>
#include <vector>

#include "base.h"
#include "Point.h"
#include "Vector.h"
#include "Matrix.h"
#include "Trafo.h"

namespace ice
{

  class Distortion
  {
    // abstract base class
  public:
    Distortion(): x0(0), y0(0) {}
    Distortion(double x0p, double y0p): x0(x0p), y0(y0p) {}
    Distortion(double x0p, double y0p, int dimx, int dimy): x0(x0p * dimx), y0(y0p * dimy) {}
    virtual ~Distortion() {};

    // basic distortion and rectifying
    virtual int Rect(double x, double y, double& xr, double& yr) const = 0;
    virtual int Distort(double x, double y, double& xd, double& yd) const = 0;

    // overloaded methods with different parameters
    virtual int Rect(double& xr, double& yr) const;
    virtual Point Rect(const Point& p) const;
    virtual Vector Rect(const Vector& v) const;

    virtual int Distort(double& x, double& y) const;
    virtual Vector Distort(const Vector& v) const;
    virtual Point Distort(const Point& v) const;
    virtual int Distort(std::vector<Point>& pl) const;

    virtual Image RectImg(const Image& source, int mode = DEFAULT) const;
    virtual int RectImg(const Image& source, const Image& dest,
                        int mode = DEFAULT) const;

    virtual Vector MakeVector() const = 0;
    virtual void Set(const Vector& v) = 0;

    virtual double X0() const
    {
      return x0;
    }
    virtual double Y0() const
    {
      return y0;
    }

    virtual int Calc(const std::vector<Point>& marker, const std::vector<Point>& orig,
                     Trafo& tr, Point& center);

    virtual int Calc(const std::vector<Point>& marker, const std::vector<Point>& orig, Trafo& tr);
    virtual int Calc(const std::vector<Point>& marker, const std::vector<Point>& orig);

    virtual int Calc(const std::vector<std::vector<Point> >& marker,
                     const std::vector<std::vector<Point> >& orig);

    virtual int Calc(const Matrix& marker, const Matrix& orig, Trafo& tr, double xm, double ym);
    virtual int Calc(const Matrix& marker, const Matrix& orig, Trafo& tr);
    virtual int Calc(const Matrix& marker, const Matrix& orig, double xm, double ym);
    virtual int Calc(const Matrix& marker, const Matrix& orig);

    virtual std::string toString() const = 0;
    virtual void Set(const std::string& parastring) = 0;

  protected:
    double x0, y0;
    static bool ReadPara(std::istream& is, const std::string& name, double& val);
  };
}
#endif
