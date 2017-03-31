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

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "Vector.h"
#include "Matrix.h"
#include "MatrixAlgebra.h"

#include "fitgauss.h"

using namespace std;

namespace ice
{
  GaussFit::GaussFit(int pdim): dim(pdim)
  {
    if (dim > 0)
      {
        Init(dim);
      }
  }

#define FNAME "GaussFit::FitInit"
  void GaussFit::Init(int pdim)
  {
    if (pdim > 0)
      {
        dim = pdim;
      }

    if (dim <= 0)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    values = 0;
    gsquare = 0;
    wsum = 0;
    para.resize(dim);
    for (int i = 0; i < dim; i++)
      {
        para[i] = 0;
      }
    sum.resize(dim);
    for (int i = 0; i < dim; i++)
      {
        sum[i].resize(dim);
        for (int j = 0; j < dim; j++)
          {
            sum[i][j] = 0;
          }
      }
    gsum.resize(dim);
    for (int i = 0; i < dim; i++)
      {
        gsum[i] = 0;
      }

    finished = false;
  }
#undef FNAME
#define FNAME "GaussFit:Value"
  void GaussFit::Value(const vector<double>& p, double v, double w)
  {
    if ((int)p.size() != dim)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    for (int i = 0; i < dim; i++)
      {
        gsum[i] += v * p[i] * w;
      }

    for (int i = 0; i < dim; i++)
      {
        vector<double>& h = sum[i];
        double pi = p[i];

        for (int j = i; j < dim; j++)
          {
            h[j] += pi * p[j] * w;
          }
      }

    gsquare += w * v * v;
    wsum += w;
    values++;

    finished = false;
  }

  void GaussFit::Value(const Vector& p, double value, double w)
  {
    vector<double> v = p;
    Value(v, value, w);
  }
#undef FNAME

#define FNAME "GaussFit:Fit"
  void GaussFit::Fit(const Matrix& p, const Vector& v)
  {
    if ((p.cols() != dim) || (p.rows() != v.Size()))
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    Init();

    for (int i = 0; i < p.rows(); i++)
      {
        Value(p[i], v[i]);
      }

    Finish();
  }
#undef FNAME
#define FNAME "GaussFit::calc"
  void GaussFit::Finish()
  {
    // "dreiecksmatrix" zu richtigen Matrix vervollständigen
    for (int i = 1; i < dim; i++)
      for (int j = 0; j < i; j++)
        {
          sum[i][j] = sum[j][i];
        }

    IF_FAILED(para = SolveLinEqu(Matrix(sum), Vector(gsum)))
    {
      throw IceException(FNAME, M_0, ERROR);
      return;
    }

    // calculate variance
    var = gsquare;

    for (int i = 0; i < dim; i++)
      for (int j = 0; j < dim; j++)
        {
          var += sum[i][j] * para[i] * para[j];
        }

    for (int i = 0; i < dim; i++)
      {
        var -= 2 * gsum[i] * para[i];
      }

    finished = true;
  }
#undef FNAME
#define FNAME "GaussFit::getResult"
  void GaussFit::getResult(vector<double>& v) const
  {
    if (!finished)
      {
        throw IceException(FNAME, M_NOT_FINISHED, ERROR);
        return;
      }
    v = para;
  }

  void GaussFit::getResult(Vector& v) const
  {
    if (!finished)
      {
        throw IceException(FNAME, M_NOT_FINISHED, ERROR);
        return;
      }
    v = Vector(para);
  }

  double GaussFit::Variance() const
  {
    if (!finished)
      {
        throw IceException(FNAME, M_NOT_FINISHED, ERROR);
        return 0;
      }
    return var / wsum;
  }
#undef FNAME

} // namespace ice
