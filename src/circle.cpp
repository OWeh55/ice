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
#include <math.h>

#include "defs.h"
#include "message.h"
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
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

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
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    r = vr;
    return OK;
  }
#undef FNAME
}
