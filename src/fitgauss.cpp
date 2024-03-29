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
      throw IceException(FNAME, M_WRONG_DIMENSION);

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
      throw IceException(FNAME, M_WRONG_DIMENSION);

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
      throw IceException(FNAME, M_WRONG_DIMENSION);

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
    try
      {
        // "dreiecksmatrix" zu richtigen Matrix vervollständigen
        for (int i = 1; i < dim; i++)
          for (int j = 0; j < i; j++)
            {
              sum[i][j] = sum[j][i];
            }

        para = SolveLinearEquation(Matrix(sum), Vector(gsum));

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
    RETHROW;
  }
#undef FNAME
#define FNAME "GaussFit::getResult"
  void GaussFit::getResult(vector<double>& v) const
  {
    if (!finished)
      throw IceException(FNAME, M_NOT_FINISHED);
    v = para;
  }

  void GaussFit::getResult(Vector& v) const
  {
    if (!finished)
      throw IceException(FNAME, M_NOT_FINISHED);
    v = Vector(para);
  }

  double GaussFit::Variance() const
  {
    if (!finished)
      throw IceException(FNAME, M_NOT_FINISHED);
    return var / wsum;
  }
#undef FNAME

} // namespace ice
