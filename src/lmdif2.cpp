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

#include <math.h>
#include <float.h>

#include "defs.h"
#include "message.h"
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
      {
        Message(FNAME, M_NOT_NESTED, ERROR);
        return ERROR;
      }

    int optnumber = optvar.Size();

    if ((optnumber < 1) || (optnumber > variable.Size() || funcdim < optnumber))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return ERROR;
      }

    for (i = 0; i < optnumber; i++)
      if (optvar[i] >= variable.Size())
        {
          Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
          return ERROR;
        }

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
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM); // Fehler melden
        info = ERROR; // Rückgabewert vorbereiten
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
