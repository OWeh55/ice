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
 * The class polygon
 */

#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

#include "Point.h"
#include "Matrix.h"
#include "Contur.h"
#include "polygonalcurve.h"
#include "lineseg.h"
#include "polygonim.h"

namespace ice
{
  class Polygon: public PolygonalCurve
  {
  public:
    Polygon(): PolygonalCurve(true) {}
    Polygon(Point p): PolygonalCurve(p, true) {}

    explicit Polygon(const Matrix& im): PolygonalCurve(im, true) {};
    explicit Polygon(const std::vector<Point>& im): PolygonalCurve(im, true) {};
    //    explicit Polygon(const Contur &c);

    using GeoObject::Distance;
    double Distance(const Polygon& p, int mode = DPP_MIN, int pmode = DPP_MIN) const;
    double DistanceDir(const Polygon& p, int pmode = DPP_MIN) const;

    //    virtual Contur MakeContur() const;
    virtual bool isValid() const;

    using PolygonalCurve::Reduced;
    Polygon Reduced(int nr, int mode = 1) const;

    using PolygonalCurve::ReducedToPrecision;
    Polygon ReducedToPrecision(double prec, int mode = 1) const;

    Polygon ConvexHull() const;
    void ConvexHull(Polygon& p) const;

  protected:
    virtual double distance_(Point p) const;
  };
}
#endif
