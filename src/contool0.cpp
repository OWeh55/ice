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
//   *************************************************
//   *   Conttool0.cpp                               *
//   *   low level contur functions                  *
//   *          Ortmann 10/99                        *
//   *************************************************

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "Contur.h"
#include "contfeat.h"
#include "numbase.h"
#include "gentrans.h"
#include "analygeo.h"

#include "contools.h"

namespace ice
{
//--------------------------------------------------------------------
//    Gegenstueck zur ICE-Funktion Freeman.
//    Bestimmt den Freemancode fuer die Richtung (x,y)->(xs,ys).
//    Die Punkte muessen Nachbarn im Sinne der 8er-Nachbarschaft sein.
//    Gibt im Fehlerfall -1 zurueck.
//--------------------------------------------------------------------

#define FNAME "GetFreeman"

  char GetFreeman(int x, int y, int xs, int ys)
  {

    const signed char FC[3][3] = {{5, 4, 3}, {6, -1, 2}, {7, 0, 1}};

    int dx = xs - x;
    int dy = ys - y;

    if (abs(dy) > 1 || abs(dx) > 1 || (dx == 0 && dy == 0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    return FC[dx + 1][dy + 1];
  }

#undef FNAME

//
}
