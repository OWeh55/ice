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

#include "visual/xio.h"

#include "visual/hist_vis.h"

namespace ice
{

//
//  Wolfgang Ortmann 11/99
//
#define FNAME "PrintHist"
  void printHist(const Hist& h)
  {
    int i;
    double kgrenzeneu = 0, kgrenzealt;

    if (!h.isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

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
  };
#undef FNAME
}
