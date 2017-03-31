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
#include "IceException.h"

#include "visual/xio.h"

#include "visual/hist_vis.h"

namespace ice
{

//
//  Wolfgang Ortmann 11/99
//
#define FNAME "PrintHist"
  int PrintHist(const Hist& h)
  {
    int i;
    double kgrenzeneu = 0, kgrenzealt;

    if (!h.isInit)
      {
        throw IceException(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    Printf("          x<%7.3f          %d \n", h.lowerLimit, h.klasse[0]);
    kgrenzealt = h.lowerLimit;
    for (i = 1; i <= h.nClasses; i++)
      {
        kgrenzeneu = kgrenzealt + h.classwidth;
        Printf("   %7.3f<=x<%7.3f          %d \n",
               kgrenzealt, kgrenzeneu, h.klasse[i]);
        kgrenzealt = kgrenzeneu;
      };
    Printf("   %7.3f<=x                %d \n",
           kgrenzeneu, h.klasse[h.nClasses + 1]);
    Printf(" min     :  %9.3e \n", h.min);
    Printf(" max     :  %9.3e \n", h.max);
    return OK;
  };
#undef FNAME
}
