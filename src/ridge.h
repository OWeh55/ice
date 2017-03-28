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
/*****************************************************
 *  Linienverfolgung                                 *
 *                                                   *
 *                                                   *
 *                                                   *
 *  (C) m.schubert 9.95                              *
 *****************************************************/

#ifndef _RIDGE_H
#define _RIDGE_H

#include "base.h"
#include "Contur.h"

namespace ice
{
  int SearchRidgeStart(Image imgv, Image imgo, int pgl, int diff,
                       int ps[2], int dir);
  Contur CalcRidgeContur(Image imgv, Image imgo, int pgl,
                         int ps[2], int lng, double& meangrad);
  Contur CalcRidgeContur(Image imgv, Image imgo, int pgl,
                         int ps[2], int lng);
}
#endif
