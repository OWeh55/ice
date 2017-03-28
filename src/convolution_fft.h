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
// Faltung und "Ent-"Faltung auf der Basis der FT

#ifndef CONVOLUTION_FFT_H
#define CONVOLUTION_FFT_H

#include <cmath>

#include "defs.h"
#include "based.h"
#include "fourier.h"

namespace ice
{
#define MD_BIAS 1
#define MD_IGNORE_BIAS 1
#define MD_USE_BIAS 0

  // d = s1 * s2
  int Convolution(const Vector& s1, const Vector& s2,
                  Vector& d);
  // s1 * d = s2
  int InvConvolution(const Vector& s1, const Vector& s2,
                     double noise, Vector& d);

  // id = is1 * is2
  ImageD ConvolutionImgD(ImageD is1, ImageD is2,
                         ImageD id, int mode = MD_USE_BIAS);

  int ConvolutionImg(const Image& is1, const Image& is2,
                     Image& id, double factor = 0.0, int mode = MD_USE_BIAS);

  int ConvolutionImg(const Image& is1, const Image& is2,
                     ImageD id, int mode = MD_USE_BIAS);

  // is1 * id = is2
  ImageD InvConvolutionImgD(ImageD is1, ImageD is2,
                            ImageD id,
                            double noise = 0.000001, int mode = MD_USE_BIAS);

  int InvConvolutionImg(const Image& is1, const Image& is2,
                        Image& id,
                        double factor = 0.0, double noise = 0.000001,
                        int mode = MD_USE_BIAS);

  int InvConvolutionImg(const Image& is1, const Image& is2,
                        ImageD id, double noise = 0.000001,
                        int mode = MD_USE_BIAS);
}
#endif
