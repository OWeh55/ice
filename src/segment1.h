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

  Region RegionGrowing(IPoint p, const Image& orig,
                       int MaxSize = INT_MAX, bool findMin = false);
  Region RegionGrowing(int x, int y, const Image& orig, int maxSize = INT_MAX,
                       bool findMin = false);

  // Ergebnis ist Markierung im Bild mark
  void RegionGrowing(int x, int y, const Image& orig, const Image& mark,
                     int val = 1, int  maxSize = INT_MAX, bool findMin = false);
  void RegionGrowing(IPoint p, const Image& orig, const Image& mark,
                     int val = 1, int  maxSize = INT_MAX, bool findMin = false);
}
#endif

