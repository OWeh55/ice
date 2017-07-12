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
/* Fourier- und Hartley-Transformation (eindimensional)
 * Wolfgang Ortmann, 97, 98
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "fourier.h"
#include "fouriertrafo.h"

using namespace std;

namespace ice
{
#define FNAME "FourierD"
  void FourierD(const double* srcre, const double* srcim,
               int n, int option,
               double* re, double* im)
  {
    try
      {
        FourierTrafo ft(n, option == NORMAL);
        ft.setInput(srcre, srcim, n);
        ft.getResult(re, im, n);
      }
    RETHROW;
  }

  void Fourier(const vector<double>& srcre, const vector<double>& srcim,
              vector<double>& re, vector<double>& im,
              int option)
  {
    int dim = srcre.size();

    if (dim < 1 || dim != (int)srcim.size())
      throw IceException(FNAME, M_WRONG_PARAM);

    if (option != NORMAL && option != INVERS)
      throw IceException(FNAME, M_WRONG_PARAM);

    try
      {
        FourierTrafo ft(dim, option == NORMAL);
        ft.setInput(srcre, srcim);
        ft.getResult(re, im);
      }
    RETHROW;
  }

  void Fourier(vector<double>& re, vector<double>& im,
              int option)
  {
    int dim = re.size();

    if (dim < 1 || dim != (int)im.size())
      throw IceException(FNAME, M_WRONG_PARAM);

    if (option != NORMAL && option != INVERS)
      throw IceException(FNAME, M_WRONG_PARAM);

    try
      {
        FourierTrafo ft(dim, option == NORMAL);
        ft.setInput(re, im);
        ft.getResult(re, im);
      }
    RETHROW;
  }
#undef FNAME
}


