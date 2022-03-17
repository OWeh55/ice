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
#ifndef MATCHIMG_H
#define MATCHIMG_H

#include "base.h"
#include "Trafo.h"

namespace ice
{
  void Windowing(const ImageD& source, ImageD& dest,
                 int refValue = -1);

  void Windowing(const Image& source, ImageD& dest,
                 int refValue = -1);

  void Windowing(const Image& source, Image& dest,
                 int refValue = -1);

  double detectShift(const Image& img1, const Image& img2,
                     double& dx, double& dy, double beta = 0.1);

#define DT_NEARLY_SHIFT 1
#define DT_REFINE 16
  bool detectTrafo(const Image& img1, const Image& img2,
                   Trafo& tr,
                   double beta = 0.1, int iter = 5, int mode = DT_NEARLY_SHIFT);

  bool detectTrafo(const Image& img1, const Image& img2, const Image& himg,
                   Trafo& tr,
                   double beta = 0.1, int iter = 5, int mode = DT_NEARLY_SHIFT);
}
#endif
