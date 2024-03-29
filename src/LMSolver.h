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
#ifndef LMSOLVER_H
#define LMSOLVER_H
// C
#include <limits.h>
#include <float.h>
#include <cmath>
// C++
#include <stdexcept>
#include <vector>
// ice
#include <matrixtemplate.h>

namespace ice
{
  class LMFunctor
  {
  protected:
    int state;
  public:
    enum state_t { init, next, jacobian };
    LMFunctor(): state(0) {}
    virtual int operator()(const std::vector<double>& x,
                           std::vector<double>& result) const = 0;
    virtual int getDimension() const = 0;
    virtual void setState(int s)
    {
      state = s;
    }
    virtual int getState() const
    {
      return state;
    }
  };

  class LMSolver
  {
  public:
    LMSolver(LMFunctor& fn): fn(fn), functionDim(fn.getDimension()), stop(0)
    {
    }

    void solve(std::vector<double>& x);
    void solve(std::vector<double>& x, const std::vector<int>& select);

    int getNIterations() const
    {
      return nIterations;
    }

    int getInfo() const
    {
      return stop;
    }

    double getErrorValue() const
    {
      return errorValue;
    }

    void setStopConditions(double ftol, double xtol, double gtolp, int maxIter)
    {
      if (ftol > 0)
        {
          ftol2 = ftol * ftol;
        }
      if (xtol > 0)
        {
          xtol2 = xtol * xtol;
        }
      if (gtolp > 0)
        {
          gtol = gtolp;
        }
      if (maxIter > 0)
        {
          maxIterations = maxIter;
        }
    }

    void setEpsilon(double e)
    {
      eps = e;
    }

  private:
    bool getJacobian(const std::vector<double>& x,
                     const std::vector<int>& select,
                     const std::vector<double>& fv0,
                     ice::matrix<double>& jm) const;

    LMFunctor& fn;
    int functionDim;
    int nVars;

    // parameters for stop of iteration
    double ftol2 = 1e-16; // actual and predicted reductions < ftol
    double xtol2 = 1e-16; // step width between two consecutive iterates < xtol
    double gtol = 1e-5;   // cosine of the between FVEC and partial derivatives < gtol
    int maxIterations = INT_MAX; // maximum number of iterations

    // stepwidth for calculation of jacobi matrix
    double eps = DBL_EPSILON * 1e4;

    // additional results
    int nIterations = -1;    // number of iterations
    int stop = -1;           // stop condition
    double errorValue = -1;  // remaining error
  };
}
#endif
