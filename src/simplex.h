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
 * simplex.h  Fitting mittels Simplexmethode
 *
 * H. Suesse , Dezember 2000
 * W. Ortmann , 3/2012
 */

#ifndef _SIMPLEX_H_
#define _SIMPLEX_H_

#define DEFAULT_LIMIT 1000000.0
#define MAXMATCHPOINTSLINOPT 10000

#include <vector>

#include "lineseg.h"
#include "pointlist.h"
#include "Trafo.h"

namespace ice
{
  int MatchPointlistsLinOpt(PointList pl1, PointList pl2,
                            double tr[][3],
                            int mode = TRM_AFFINE, double limit = DEFAULT_LIMIT);

  Trafo MatchPointlistsLinOpt(const Matrix& p1, const Matrix& p2,
                              int mode = TRM_AFFINE);

  Trafo MatchPointlistsLinOpt(const Matrix& p1, const Matrix& p2,
                              int mode, const Vector& weights,
                              double limit = DEFAULT_LIMIT);

  Trafo MatchPointlistsLinOpt(const std::vector<Point>& p1,
                              const std::vector<Point>& p2,
                              int mode = TRM_AFFINE);

  Trafo MatchPointlistsLinOpt(const std::vector<Point>& p1,
                              const std::vector<Point>& p2,
                              int mode, const std::vector<double>& weights,
                              double limit = DEFAULT_LIMIT);


  int FitLineLinOpt(PointList pl, int a1, int a2, int step,
                    double par[2], double limit = DEFAULT_LIMIT);

  LineSeg FitLineLinOpt(const std::vector<Point>& pl,
                        int step = 1, double limit = DEFAULT_LIMIT);

  int FitLineLinOpt(const Matrix& m, double& p, double& phi,
                    double limit = DEFAULT_LIMIT);

  int FitCircleLinOpt(PointList pl, int a1, int a2, int step,
                      double par[3], double limit = DEFAULT_LIMIT);

  int FitEllipseLinOpt(PointList pl, int a1, int a2, int step,
                       double par[5], double limit = DEFAULT_LIMIT);

  int LinearOptimization(int m, int n, double** matrix,
                         double* r_side, double* cost_function,
                         double* solution, double& costs);
}
#endif
