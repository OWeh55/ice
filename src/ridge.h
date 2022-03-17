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
