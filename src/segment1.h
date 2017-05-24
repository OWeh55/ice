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

#ifndef SEGMENT1_H
#define SEGMENT1_H

#include <vector>
#include <deque>
#include <queue>
#include <functional>
#include <iostream>
#include <limits.h>

using namespace std;

#include "base.h"
#include "region.h"

namespace ice
{
// Region mit maximal maxSize Pixeln um den Punkt (x,y) aus dem
// Bild orig berechnen.
// Die maximale Grauwertschwankung (Standard-Abweichung) ist
// durch std beschraenkt.

// Ergebnis ist eine Region
  Region RegionGrowDeviation(int x, int y, const Image& orig,
                             double std = 3.0, int maxSize = INT_MAX);

  Region RegionGrowDeviation(IPoint p, const Image& orig,
                             double std = 3.0, int maxSize = INT_MAX);

// Ergebnis ist Markierung im Bild mark
  void RegionGrowDeviation(int x, int y,
                           const Image& Orig, const Image& mark, int val = 1,
                           double std = 3.0, int maxSize = INT_MAX);

  void RegionGrowDeviation(IPoint p,
                           const Image& Orig, const Image& mark, int val = 1,
                           double std = 3.0, int maxSize = INT_MAX);

  // Region mit maximal maxSize Pixeln um den Punkt (x,y) aus dem Bild orig
  // berechnen. Die Grenze der Region bestimmt sich aus der maximalen
  // "Unterscheidbarkeit" vom Untergrund:
  // MittlererGrauwertDerRegion - MittlererGrauwertDerAngrenzendenUntergrundpunkte
  // Ergebnis ist eine Region

  Region RegionGrow(IPoint p, const Image& orig,
                    int MaxSize = INT_MAX, int refval = -1);
  Region RegionGrow(int x, int y, const Image& orig, int maxSize = INT_MAX,
                    int refval = -1);

  // Ergebnis ist Markierung im Bild mark

  void RegionGrow(int x, int y, const Image& orig, const Image& mark,
                  int val = 1, int  maxSize = INT_MAX, int refval = -1);
  void RegionGrow(IPoint p, const Image& orig, const Image& mark,
                  int val = 1, int  maxSize = INT_MAX, int refval = -1);
}
#endif

