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
 * moment based fitting of geometric primitives
 *
 *    W. Ortmann 03/2009,09/2011,10/2012
 */

#include "polygon.h"
#include "Trafo.h"
#include "moments.h"

namespace ice
{
// fitting
  Trafo AffineFit(const Moments& m1, const Moments& m2);

  Matrix FitTriangle(const Moments& m);
//  Matrix FitTriangle(const Moments &m,double & guetemass);

  Matrix FitEquilateralTriangle(const Moments& m);
  Matrix FitEquilateralTriangle(const Moments& m, double& gutemass);

  Matrix FitIsoscelesTriangle(const Moments& m);
  Matrix FitIsoscelesTriangle(const Moments& m, double& gutemass);

  Matrix FitParallelogram(const Moments& m);
  Matrix FitParallelogram(const Moments& m, double& guetemass);

  Matrix FitSquare(const Moments& m);
  Matrix FitSquare(const Moments& m, double& guetemass);

  Matrix FitRectangle(const Moments& m);
  Matrix FitRectangle(const Moments& m, double& guetemass);

  Matrix FitPolygon(const Moments& m, const Matrix& pl);
  Matrix FitPolygon(const Moments& m, const Matrix& pl, double& guetemass);

  Polygon FitPolygon(const Moments& m, const Polygon& pl);
  Polygon FitPolygon(const Moments& m, const Polygon& pl, double& guetemass);

  int FitCircle(const Moments& m, double& x0, double& y0, double& radius);
  int FitCircle(const Moments& m, double& x0, double& y0, double& radius,
                double& guetemass);

  Circle FitCircle(const Moments& m);
  Circle FitCircle(const Moments& m, double& guetemass);

  Ellipse FitEllipse(const Moments& m);

  double Orientation(const Moments& m);
}
