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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <string>

#include "macro.h"
#include "defs.h"
#include "IceException.h"
#include "paint.h"
#include "drawline.h"

#include "visual/xio.h"
#include "visual/histogram_vis.h"

namespace ice
{

#define FNAME "PrintHistogram"
  int PrintHistogram(const Histogram& h)
  {
    int i;
    int min, max;
    if (!h.isValid())
      throw IceException(FNAME, M_NOT_INITIALIZED);

    for (i = 0; i < h.nClasses(); i++)
      {
        Printf("   %3d :  %d \n", i, h.getCount(i));
      }

    h.getLimits(min, max);
    Printf(" min     :  %3d \n", min);
    Printf(" max     :  %3d \n", max);
    return OK;
  };
#undef FNAME

}
