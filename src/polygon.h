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
    virtual double distance_(Point p) const override;
  };
}
#endif
