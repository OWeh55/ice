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

#include "base.h"
#include "IceException.h"
#include "macro.h"

#include "Vector.h"
#include "Matrix.h"
#include "defs.h"

#include "fitfn.h"
//=========================================================================

namespace ice
{
#define FNAME "Constant::Constant"

  Constant::Constant(const std::vector<double>& v): FunctionWithFittingG(1)
  {
    if (v.size() < 1)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    co[0] = v[0];
  }
#undef FNAME

  void Constant::FitVal(double x, double g, double w)
  {
    Vector p(1);
    p[0] = 1;
    FunctionWithFittingG::FitVal(p, g, w);
  }

//===========================================================================

#define FNAME "Polynom1O::Polynom1O"
  Polynom1o::Polynom1o(const std::vector<double>& v): FunctionWithFittingG(2)
  {
    if (v.size() < 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    co[0] = v[0];
    co[1] = v[1];
  }
#undef FNAME

  void Polynom1o::FitVal(double x, double g, double w)
  {
    Vector p(2);
    p[0] = 1;
    p[1] = x;
    FunctionWithFittingG::FitVal(p, g, w);
  }

//========================================================================
#define FNAME "Polynom2o::Polynom2o"
  Polynom2o::Polynom2o(const Vector& v): FunctionWithFittingG(3)
  {
    if (v.Size() < 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    co[0] = v[0];
    co[1] = v[1];
    co[2] = v[2];
  }
#undef FNAME

  void Polynom2o::FitVal(double x, double g, double w)
  {
    Vector p(3);
    p[0] = 1;
    p[1] = x;
    p[2] = x * x;
    FunctionWithFittingG::FitVal(p, g, w);
  }
#define FNAME "Polynom2o::getCoefficient"
  int Polynom2o::getCoefficient(double& pa, double& pb, double& pc) const
  {
    pa = co[0];
    pb = co[1];
    pc = co[2];
    return OK;
  }
#undef FNAME
}
