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
//
// convolution and deconvolution based on fourier transform

#ifndef CONVOLUTION_FOURIER_H
#define CONVOLUTION_FOURIER_H
#include <vector>
#include <cmath>

#include "defs.h"
#include "based.h"
#include "fourier.h"

namespace ice
{
#define MD_BIAS 1
#define MD_IGNORE_BIAS 1
#define MD_USE_BIAS 0

  /* ==== one dimensional ===
   */
  // d = s1 * s2
  void calcConvolution(const Vector& s1, const Vector& s2,
                       Vector& d);
  void calcConvolution(const std::vector<double>& s1, const std::vector<double>& s2,
                       std::vector<double>& d);

  // s1 * d = s2
  void calcInvConvolution(const Vector& s1, const Vector& s2, Vector& d,
                          double noise = 0.0001);
  void calcInvConvolution(const std::vector<double>& s1, const std::vector<double>& s2, std::vector<double>& d,
                          double noise = 0.0001);

  /* ==== two dimensional ===
   */

  // id = is1 * is2
  void calcConvolution(const ImageD& is1, const ImageD& is2,
                       ImageD& id, int mode = MD_USE_BIAS);

  void calcConvolution(const Image& is1, const Image& is2,
                       Image& id, double factor = 0.0, int mode = MD_USE_BIAS);

  void calcConvolution(const Image& is1, const Image& is2,
                       ImageD& id, int mode = MD_USE_BIAS);

  // is1 * id = is2
  void calcInvConvolution(const ImageD& is1, const ImageD& is2,
                          ImageD& id,
                          double noise = 0.000001, int mode = MD_USE_BIAS);

  void calcInvConvolution(const Image& is1, const Image& is2, Image& id,
                          double factor = 0.0, double noise = 0.000001,
                          int mode = MD_USE_BIAS);

  void calcInvConvolution(const Image& is1, const Image& is2,
                          ImageD& id, double noise = 0.000001,
                          int mode = MD_USE_BIAS);
}
#endif
