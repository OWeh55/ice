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
#include <math.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "numbase.h"
#include "base.h"
#include "lineseg.h"
#include "paint.h"

#include "circle.h"

namespace ice
{
  /*
   * --------------- Circle --------------------
   */

#define FNAME "Circle::Circle"
  Circle::Circle(const Vector& v) : GeoObject(v)
  {
    if (v.Size() < 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    setR(v[2]);
  }

  Circle::Circle(double p[]): GeoObject(p)
  {
    setR(p[2]);
  }
#undef FNAME

#define FNAME "Circle::setR"
  int Circle::setR(double vr)
  {
    if (vr < 0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    r = vr;
    return OK;
  }
#undef FNAME
}
