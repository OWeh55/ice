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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <string>

#include "macro.h"
#include "defs.h"
#include "message.h"
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
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

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
