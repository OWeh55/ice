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
/* Phasenkorellation */
// ttbb (c) 1998
// ortmann 4/2017
#ifndef _PHASEN_KOR_H_
#define _PHASEN_KOR_H_

#include "defs.h"
#include "base.h"
#include "based.h"

namespace ice
{
  void PhaseCorrelationImg(const ImageD& img1, const ImageD& img2, const ImageD& img);
  void PhaseCorrelationImg(const Image& img1, const Image& img2, const Image& img);
}
#endif
