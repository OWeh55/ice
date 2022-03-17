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
 *
 *  Projektive Transformation von
 *         - Punkten
 *         - Konturen
 *         - Bildern
 *
 *  obsolete now, replaced by class Trafo
 *
 *  m.schubert 7.93/ neubauer 6/95
 *
 */
#ifndef _TRANS_H
#define _TRANS_H

#include "base.h"
#include "Contur.h"

namespace ice
{
  /* Projektive Transformation eines Punktes */
  double* TransPoint
  (double p1[2], double Trans[3][3], double p2[2]);

  /* Projektive Transformation einer Kontur */
  Contur TransContur(const Contur& c, double Trans[3][3]);

  /* Projektive Transformation eines Bildes */
  int TransImg(const Image& imgs, double Trans[3][3], int mode, const Image& imgd);
  int TransAffineImg(const Image& imgs, double Trans[3][3], int mode, const Image& imgd);
}
#endif
