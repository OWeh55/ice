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
/*************************************************************/
/* Berechnung und Transformation von Momenten                */
/* Berechnung von Invarianten                                */
/*                                                           */
/*          (C) W.Ortmann 09/05, 02/09, 09/12                */
/*************************************************************/
#ifndef _MOMENTS_H
#define _MOMENTS_H

#include <vector>

#include "Point.h"
#include "Contur.h"
#include "geo.h"
#include "circle.h"
#include "ellipse.h"

#include "momente.h"

namespace ice
{
  class Region;

  class Moments
  {
  public:
    Moments()
    {
      init();
    }

    Moments(double mp[15]);

    explicit Moments(const Region& r);
    explicit Moments(const Contur& c);
    explicit Moments(const Matrix& p);
    explicit Moments(const std::vector<Point>& p);
    explicit Moments(PointList p);
    explicit Moments(PointList p, int a1, int a2);

    void Init()
    {
      init();
    }

    // pointer to "old" moments (for compatibility)
    double* oldMoments()
    {
      return mom;
    }

    const double* oldMoments() const
    {
      return mom;
    }

    // modify moments by adding a point
    void add(Point p, double val);

    // modify moments by adding new regions
    void add(const Region& r);
    void add(const Contur& c);

    // polygon
    void add(const Matrix& p);
    void add(const std::vector<Point>& p);

    void add(PointList p);
    void add(PointList p, int a1, int a2);

    void add(int y, int x1, int x2);     // add horizontal line segment
    void add(const Moments& m);
    void add(double m[15]);

    Point Center() const;
    double& operator()(int z, int s);   // moments
    const double& operator()(int z, int s) const;

    void getCentroid(double& xm, double& ym) const
    {
      xm = mom[i10] / mom[i00];
      ym = mom[i01] / mom[i00];
    }
    Moments CentralMoments() const;

    // differences
    double Diff(const Moments& m) const;

    // Calculate transformed moments
    Moments Translate(double dx, double dy) const; // translation
    Moments Translate(Point p) const
    {
      return Translate(p.x, p.y);
    }

    Moments Scale(double a) const; // isotrope scaling
    Moments Scale(double a, double b) const; // anisotrope scaling
    Moments XShear(double a) const; // x shearing
    Moments YShear(double b) const; // y shearing
    Moments Rotate(double fi) const;// rotation (around origin)
    Moments AffineTransform(const Trafo& tr) const; // affine transform

    // normalization
    Moments NormalizeSign() const;

    Moments NormalizeTranslation() const;
    Moments NormalizeTranslation(double& x, double& y) const;

    Moments NormalizeXShearing() const;
    Moments NormalizeXShearing(double& beta) const;

    Moments NormalizeYShearing() const;
    Moments NormalizeYShearing(double& beta) const;

    enum scalemode { isotropic, anisotropic };
    Moments NormalizeScaling(scalemode mode = anisotropic) const;
    Moments NormalizeScaling(double& alpha) const;
    Moments NormalizeScaling(double& alpha, double& beta) const;

    Moments NormalizeRotation() const;
    Moments NormalizeRotation(double& phi) const;
    Moments NormalizeRotation(double& c, double& s) const;

    // complex normalization
    enum nmode {standard, polynom, iteration};
    Moments Normalize(Trafo& tr, nmode mode = standard) const;
    Moments Normalize(nmode mode = standard) const;

    // invariants from moments
    Vector AffineAlgebraicInvariants() const;
    Vector HuInvariants() const;
    Vector AffineHuInvariants(bool trans = true) const;

  private:
    double mom[15]; // moments in double array
    // compatible with old moment functions
    void init()
    {
      for (int i = 0; i < 15; i++)
        {
          mom[i] = 0.0;
        }
    }
  };
}
#endif
