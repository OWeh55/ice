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

#include "IceException.h"
#include "defs.h"

#include "Vector.h"

#include "lmdif.h"

namespace ice
{

  static bool running = false; // to avoid nested calls
  static Vector* varvec;     //
  static Vector* funcresult;
  static LMFunc* function;

#define FNAME "LMDif"
  static int lmfunction1(int m, int n, double x[], double fva[])
  {
    int i;

    for (i = 0; i < n; i++)
      {
        (*varvec)[i] = x[i];
      }

    function(*varvec, *funcresult);

    for (i = 0; i < m; i++)
      {
        fva[i] = (*funcresult)[i];
      }

    return 0;
  }

  int LMDif(Vector& variable, int optnumber, LMFunc* fcn, int funcdim, int& inumber, int maxiter)
  {
    int i;

    if (running)
      {
        throw IceException(FNAME, M_NOT_NESTED, ERROR);
      }

    if ((optnumber < 1) || (optnumber > variable.Size() || funcdim < optnumber))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    running = true;
    varvec = new Vector(variable);
    funcresult = new Vector(funcdim);

    double* x = new double[optnumber];

    for (i = 0; i < optnumber; i++)
      {
        x[i] = variable[i];
      }

    function = fcn;

    int info;

    lmdifx(lmfunction1, funcdim, optnumber, x,
           DBL_EPSILON, DBL_EPSILON, DBL_EPSILON,
           maxiter, DBL_EPSILON,
           100.0, 0, &info, &inumber,
           funcdim);

    if (info == 0)
      {
        /* Fehler sollte nicht auftreten, da vorher getestet */
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM); // Fehler melden
        info = ERROR; // Rückgabewert vorbereiten
      }

    for (i = 0; i < optnumber; i++)
      {
        //      printf("%d:%g\n",i,x[i]);
        variable[i] = x[i];
      }

    delete varvec;
    delete funcresult;

    delete [] x;

    running = false;
    return info;
  }

  int LMDif(Vector& variable, int optnumber, LMFunc* fcn, int funcdim)
  {
    int inumber;
    return  LMDif(variable, optnumber, fcn, funcdim, inumber);
  }

}
