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

namespace ice
{
#define FNAME "FunctionWithFitting::Fit"
  int FunctionWithFitting::Fit(const Matrix& m)
  {
    int samples = m.rows();

    if (m.cols() < 2)
      throw IceException(FNAME, M_WRONG_DIM);

    FitInit();

    if (m.cols() > 2)
      for (int i = 0; i < samples; i++)
        {
          FitVal(m[i][0], m[i][1], m[i][2]);
        }
    else
      for (int i = 0; i < samples; i++)
        {
          FitVal(m[i][0], m[i][1]);
        }

    FitFinish();
    return OK;
  }
#undef FNAME
//===========================================================================
  Polynom::Polynom(const Vector& v): FunctionWithFittingG(v.Size()),
    ord(v.Size() - 1)
  {
  }

  void Polynom::FitInit(int n)
  {
    if (n > 0)
      {
        ord = n;
      }

    FunctionWithFittingG::FitInit(ord + 1);
  }

  void Polynom::FitInit()
  {
    FitInit(0);
  }

  void Polynom::FitVal(double x, double g, double w)
  {
    int i;
    Vector p(ord + 1);
    double px = 1;

    for (i = 0; i <= ord; i++)
      {
        p[i] = px;
        px *= x;
      }

    FunctionWithFittingG::FitVal(p, g, w);
  }

#define FNAME "Polynom::operator()"
  double Polynom::operator()(double x) const
  {
    double res = 0;

    // horner schema
    //    for (int i = ord - 1; i >= 0; i--)
    for (int i = ord; i >= 0; i--)
      {
        res = res * x + co[i];
      }

    return res;
  }
#undef FNAME
//============================================================================
#define FNAME "Polynom2d::Polynom2d"
  Polynom2d::Polynom2d()
  {
    co = Vector(1);
    ord = 0;
  }

  Polynom2d::Polynom2d(int n)
  {
    vsize = (n + 1) * (n + 2) / 2;
    co = Vector(vsize);
    ord = n;
  }

  Polynom2d::Polynom2d(const Vector& v)
  {
    co = v;
    vsize = v.Size();
    ord = 0;

    while ((ord + 1) * (ord + 2) / 2 < vsize)
      {
        ord++;
      }

    if ((ord + 1) * (ord + 2) / 2 != vsize)
      throw IceException(FNAME, M_WRONG_DIM);
  }
#undef FNAME
  void Polynom2d::FitInit(int n)
  {
    if (n > 0)
      {
        ord = n;
        vsize = (ord + 1) * (ord + 2) / 2;
      }

    Function2dWithFittingG::FitInit(vsize);
  }

  void Polynom2d::FitInit()
  {
    FitInit(0);
  }

  void Polynom2d::FitVal(double x, double y, double g, double w)
  {
    Vector p(vsize);
    double px = 1, py = 1;
    Vector xx(ord + 1); // Vektoren mit Potenzen
    Vector yy(ord + 1);

    for (int i = 0; i <= ord; i++)
      {
        xx[i] = px;
        px *= x;
        yy[i] = py;
        py *= y;
      }

    int k = 0;

    for (int i = 0; i <= ord; i++)
      {
        for (int j = 0; j <= i; j++)
          {
            p[k] = xx[i - j] * yy[j];
            k++;
          }
      }

    Function2dWithFittingG::FitVal(p, g, w);
  }

#define FNAME "Polynom2d::operator()"
  double Polynom2d::operator()(double x, double y) const
  {
    double px = 1, py = 1;
    double res = 0;
    Vector xx(ord + 1);
    Vector yy(ord + 1);

    for (int i = 0; i <= ord; i++)
      {
        xx[i] = px;
        px *= x;
        yy[i] = py;
        py *= y;
      }

    int k = 0;

    for (int i = 0; i <= ord; i++)
      {
        for (int j = 0; j <= i; j++)
          {
            res += xx[i - j] * yy[j] * co[k];
            k++;
          }
      }

    return res;
  }
}
#undef FNAME
