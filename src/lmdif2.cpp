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

#include <math.h>
#include <float.h>

#include "defs.h"
#include "IceException.h"
#include "Vector.h"
#include "lmdif.h"

namespace ice
{
  static bool running = false; // avoid nested calls
  static Vector* varvec;
  static Vector* funcresult;
  static IVector* optvec;
  static LMFunc* function;

#define FNAME "LMDif"
  static int lmfunction2(int m, int n, double x[], double fva[])
  {
    int i;

    // get parameters from double[] to Vector
    for (i = 0; i < n; i++)
      {
        (*varvec)[(*optvec)[i]] = x[i];
      }

    function(*varvec, *funcresult);

    // return distances as double[]
    for (i = 0; i < m; i++)
      {
        fva[i] = (*funcresult)[i];
      }

    return 0;
  }

  int LMDif(Vector& variable, const IVector& optvar,
            LMFunc* fcn, int funcdim, int& inumber, int maxiter)
  {
    int i;

    if (running)
      throw IceException(FNAME, M_NOT_NESTED);

    int optnumber = optvar.Size();

    if ((optnumber < 1) || (optnumber > variable.Size() || funcdim < optnumber))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    for (i = 0; i < optnumber; i++)
      if (optvar[i] >= variable.Size())
        throw IceException(FNAME, M_WRONG_PARAMETER);

    running = true;
    varvec = new Vector(variable);
    optvec = new IVector(optvar);
    funcresult = new Vector(funcdim);

    double* x = new double[optnumber];

    for (i = 0; i < optnumber; i++)
      {
        x[i] = variable[optvar[i]];
      }

    function = fcn;

    int info;

    lmdifx(lmfunction2, funcdim, optnumber, x,
           DBL_EPSILON, DBL_EPSILON, DBL_EPSILON, maxiter, DBL_EPSILON,
           100.0, 0, &info, &inumber, funcdim);

    if (info == 0)
      {
        /* Fehler sollte nicht auftreten, da vorher getestet */
        throw IceException(FNAME, M_WRONG_PARAMETER);
      }

    for (i = 0; i < optnumber; i++)
      {
        variable[optvar[i]] = x[i];
      }

    delete varvec;
    delete optvec;
    delete funcresult;
    delete [] x;

    running = false;
    return info;
  }

  int LMDif(Vector& variable, const IVector& optvar,
            LMFunc* fcn, int funcdim)
  {
    int inumber;
    return LMDif(variable, optvar, fcn, funcdim, inumber);
  }

// ----------------------------------------------------------------------

}
