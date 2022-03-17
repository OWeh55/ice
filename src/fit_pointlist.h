#ifndef FIT_POINTLIST_H
#define FIT_POINTLIST_H
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

// old fitting functions with pointlists
// avoid using these functions
// use functions in fit.h
#include "pointlist.h"

namespace ice
{
  int FitLine(PointList pl, int ad1, int ad2, int step,
              double* par, double* mdist, int* madr);
  int FitCircle(PointList pl, int ad1, int ad2, int step,
                double* par, double* mdist, int* madr);

  int FitEllipse(PointList pl, int ad1, int ad2, int step,
                 double* par, double* mdist, int* madr, int mode = 1);

  /******************************************************************/

  Segment FitLineSegment(PointList pl, int ad1, int ad2, int step,
                         double* mdist, int* madr);
  Segment FitCircleSegment(PointList pl, int ad1, int ad2, int step,
                           double* mdist, int* madr);
  Segment FitEllipseSegment(PointList pl, int ad1, int ad2, int step,
                            double* mdist, int* madr);

}
#endif
