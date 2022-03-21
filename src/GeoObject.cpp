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

#include "IceException.h"
#include "macro.h"

#include "Vector.h"

#include "GeoObject.h"

#include "numbase.h"
#include "defs.h"
#include "numbase.h"
#include "base.h"

namespace ice
{
#define FNAME "GeoObject::GeoObject"
  GeoObject::GeoObject(const Vector& v)
  {
    if (v.Size() < 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    p.x = v[0];
    p.y = v[1];
  }
#undef FNAME

  void GeoObject::Shift(double dx, double dy)
  {
    p.x += dx;
    p.y += dy;
  }

}
