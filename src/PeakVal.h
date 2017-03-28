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
/* Bewertung der Übereinstimmung eines Peak-Bildes mit dem
   Einheitsimpuls */

/* ttbb 18.06.1998 */

#ifndef _DELTA_IMPULS_VALUATION_
#define _DELTA_IMPULS_VALUATION_

#include "defs.h"
#include "base.h"
#include "based.h"

namespace ice
{
#define PN_NORMAL   1
#define PN_CONVOLUTION  2

  /* PeakValuation
     Liefert das Bewertungsmaß für das Peakbild img.
     Im Falle eines Fehlers wird DBL_MAX zurückgegeben.
     mode steuert die Art der Normierung des Bildsignales:
     mode==PN_CONVOLUTION -> Bewertungsmaß ist aus [-1,1]
     mode==PN_NORMAL      -> Bewertungsmaß ist aus [ 0,1]
     bool_zykl=TRUE/FALSE steuert, ob das Peakbild dem 2fach zyklischen Bildmodell entspricht
     Auf (x0,y0) wird die Position des großten lokalen Maximums zurückgegeben.
  */

  double PeakValuation(const Image& img, const Image& mi,
                       double& x0, double& y0,
                       int mode = PN_CONVOLUTION,
                       int graynull = 0, bool zykl = true);

  double PeakValuation(const Image& img,
                       double& x0, double& y0,
                       int mode = PN_CONVOLUTION,
                       int graynull = 0, bool zykl = true);
}
#endif

