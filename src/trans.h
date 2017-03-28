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
