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
#ifndef _MATCHTOOLS_H_
#define _MATCHTOOLS_H_

#include <string.h>

#include "defs.h"
#include "base.h"
#include "based.h"
#include "IceException.h"
#include "Contur.h"
#include "contlist.h"
#include "contools.h"
#include "numbase.h"
#include "arith.h"
#include "polygonim.h"
#include "fit.h"

namespace ice
{
  double Gauss2D(double x, double y,
                 double xm, double ym, double s1, double s2, double s12);

  //

  void getGaussParameters(const ImageD& imgd, const Image& imgo,
                          double& x0, double& y0,
                          double& sx, double& sy, double& sxy,
                          double& a, double& b);

//
  double EntropyImg(const Image& img);
}
#endif
