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
#ifndef _PEAK1D_
#define _PEAK1D_

#include "Vector.h"

namespace ice
{
// Vector v enthält die diskrete 1D-Funktion (Funktionswerte)
// cyclic gibt an, ob die Funktion als zyklisch anzusehen ist.
// Die Funktion gibt einen IVector/vector<int> mit den Indizes der Maxima innerhalb des
// Feldes values zurück.
// Existiert kein Maximum (konstante Funktion) wird ein leerer Vector zurückgegeben.

  ice::IVector peak1d(const ice::Vector& v, bool cyclic, double noise);
  std::vector<int> peak1d(const std::vector<double>& v, bool cyclic, double noise);
}
#endif
