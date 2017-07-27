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
// conv.h
// convolution and deconvolution based on fourier transform

#ifndef CONVOLUTION_FFT_H
#define CONVOLUTION_FFT_H
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
  void Convolution(const Vector& s1, const Vector& s2,
                   Vector& d);
  void Convolution(const std::vector<double>& s1, const std::vector<double>& s2,
                   std::vector<double>& d);

  // s1 * d = s2
  void InvConvolution(const Vector& s1, const Vector& s2, Vector& d,
                      double noise = 0.0001);
  void InvConvolution(const std::vector<double>& s1, const std::vector<double>& s2, std::vector<double>& d,
                      double noise = 0.0001);

  /* ==== two dimensional ===
   */

  // id = is1 * is2
  void Convolution(const ImageD& is1, const ImageD& is2,
                   ImageD& id, int mode = MD_USE_BIAS);

  void Convolution(const Image& is1, const Image& is2,
                   Image& id, double factor = 0.0, int mode = MD_USE_BIAS);

  void Convolution(const Image& is1, const Image& is2,
                   ImageD& id, int mode = MD_USE_BIAS);

  // is1 * id = is2
  void InvConvolution(const ImageD& is1, const ImageD& is2,
                      ImageD& id,
                      double noise = 0.000001, int mode = MD_USE_BIAS);

  void InvConvolution(const Image& is1, const Image& is2, Image& id,
                      double factor = 0.0, double noise = 0.000001,
                      int mode = MD_USE_BIAS);

  void InvConvolution(const Image& is1, const Image& is2,
                      ImageD& id, double noise = 0.000001,
                      int mode = MD_USE_BIAS);
}
#endif
