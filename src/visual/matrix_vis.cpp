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
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <string.h>

#include "defs.h"
#include "visual/xio.h"

#include "visual/matrix_vis.h"

namespace ice
{
  int PrintMatrix(char* str, double* h, int row, int col)
  {
    int i, j;
    double* dptr;

    Printf("*** %s\n", str);
    dptr = h;
    for (i = 0; i < row; i++)
      {
        for (j = 0; j < col; j++)
          {
            Printf("%f   ", *dptr++);
          }
        Printf("\n");
      }
    Printf("\n\n");
    return OK;
  }

}
