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
//   *************************************************
//   *   Konturverwaltung                            *
//   *                                               *
//   *                                               *
//   *   (C) m. schubert 11/93                       *
//   *          neubauer 3/93                        *
//   *          Ortmann 10/99                        *
//   *************************************************

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "Contur.h"
#include "contfeat.h"
#include "numbase.h"
#include "gentrans.h"
#include "analygeo.h"

#include "contools.h"

namespace ice
{
#define FNAME "LineContur"
  Contur LineContur(int s1[2], int s2[2])
  {
    Contur c(s1[0], s1[1]);
    c.add(s2[0], s2[1]);
    return c;
  }
  Contur LineContur(IPoint p1, IPoint p2)
  {
    Contur c(p1);
    c.add(p2);
    return c;
  }
  Contur LineContur(int x, int y, int x1, int y1)
  {
    Contur c(x, y);
    c.add(x1, y1);
    return c;
  }
#undef FNAME
}
