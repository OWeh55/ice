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
#ifndef LMDIF_H
#define LMDIF_H

#include <vector>
#include <float.h>
#include <limits.h>

#include "Vector.h"

namespace ice
{

// type of function used by lmdif

  typedef int lmfunc(int m, int n, double x[], double fvac[]);

// lmdif - original Levenberg-Marquardt optimizer adapted from fortran
// direct use not recommended
  int lmdif(lmfunc* fnc, int m, int n, double x[], double fvec[],
            double ftol, double xtol, double gtol, int maxiter, double epsfcn,
            double diag[], int mode, double factor,
            int nprint, int* info, int* nfev,
            double fjac[], int ldfjac,
            int ipvt[], double qtf[]);

// first wrapper for lmdif allocating memory
  int lmdifx(lmfunc* fnc, int m, int n, double x[],
             double ftol, double xtol, double gtol, int maxiter, double epsfcn,
             double factor,
             int nprint, int* info, int* nfev,
             int ldfjac);

// Translate return codes of lmdif to message string
  std::string LMDifMessage(int info);

// function type used in the c++ wrapper for lmdif
  typedef int LMFunc(const Vector& variable, Vector& result);

// functor version of wrapper
  class LMFunctionObject
  {
  public:
    virtual int operator()(Vector& result) const = 0;
    virtual int funcdim() const = 0;
    virtual ~LMFunctionObject() {};
  };

#define LMMAXITER (INT_MAX/2)
// wrappers for lmdif using objects as parameters

  int LMDif(Vector& variable, int optnumber, LMFunc* fcn, int funcdim);
  int LMDif(Vector& variable, int optnumber, LMFunc* fcn, int funcdim,
            int& inumber, int maxiter = LMMAXITER);
  int LMDif(Vector& variable, const IVector& optvar, LMFunc* fcn, int funcdim);
  int LMDif(Vector& variable, const IVector& optvar, LMFunc* fcn, int funcdim,
            int& inumber, int maxiter = LMMAXITER);

  int LMDif(const std::vector<double*>& v, const LMFunctionObject& fcn,
            int maxiter, int& inumber, double epsStepLength = DBL_EPSILON);
  int LMDif(const std::vector<double*>& v, const LMFunctionObject& fcn,
            int maxiter = LMMAXITER);
}
#endif
