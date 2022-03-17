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
 * Author: Alexander Lärz, 2005
 */

#ifndef QFT_H
#define QFT_H

#include "quatmatrix.h"
#include "fourier.h"
#include "base.h"

#define POWER 1
#define NORM 2
#define LOG 3
#define CENTER 4
#define NOCENTER 5

namespace ice
{
  int QFourier(QuatMatrix& input, QuatMatrix& output, int option = NORMAL);
  int PowerSpektrumQFT(QuatMatrix& input, Image& output, int type = POWER, int mode = CENTER);
  int EigenwinkelSpektrumQFT(QuatMatrix& input, Image& r, Image& g, Image& b, int mode = CENTER);
  int EigenachsenSpektrumQFT(QuatMatrix& input, Image& r, Image& g, Image& b, int mode = CENTER);
  int PhasenSpektrumQFT(QuatMatrix& input, Image& alpha, Image& beta, Image& delta, int mode = CENTER);
}
#endif
