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
/********************************************************************
  header zu "feature.c"
  Merkmalsgewinnung
  Suesse   7/93/ Schubert 4/96
********************************************************************/
#ifndef _FEATURE_H
#define _FEATURE_H

#include "Contur.h"
#include "base.h"
#include "based.h"

namespace ice
{
  int InvarFivePoint2d(double p[5][2], double* i1, double* i2);
  int InvarFivePoint3d(double p[5][3], double* i1, double* i2);
  ImageD MomentImg(Image imgs, int p, int q, int neigbh, ImageD imgd);
}
#endif
