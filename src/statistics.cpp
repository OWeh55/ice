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
// multivariate statistics
// neub 1/94 - non oop version
// Ortmann Jun 1999,Feb 2007 - oop version

#include <fstream>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <vector>

#include "defs.h"
#include "IceException.h"
#include "numbase.h"
#include "matdef.h"
#include "statistics.h"

//-------------------------------------------------------
//          The class Statistics
//-------------------------------------------------------

namespace ice
{
#define FNAME "Statistics::Statistics"
  Statistics::Statistics(int dimension)
  {
    if (dimension < 1)
      {
        throw IceException(FNAME, M_WRONG_DIMENSION);
        dim = 0;
      }
    else
      {
        Init(dimension);
      }
  }
#undef FNAME

  Statistics::~Statistics()
  {
  }
//------------------------------------------------------
#define FNAME "Statistics::Init"
  int Statistics::Init()
  {
    if (dim < 1)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    sweight = 0.0;

    akk.clear();
    akk.resize(dim, 0);

    sakk.clear();
    sakk.resize(dim, akk);

    return OK;
  }

  int Statistics::Init(int dimension)
  {
    if (dimension < 1)
      {
        throw IceException(FNAME, M_WRONG_DIMENSION);
      }

    dim = dimension;
    return Init();
  }
#undef FNAME
#define FNAME "Put"
  int Statistics::put(const Vector& val, double weight)
  {
    if (weight < 0.0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (val.Size() != dim)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    for (int i = 0; i < dim; i++)
      {
        akk[i] += val.at(i) * weight;
      }

    double weight_squared = Sqr(weight);

    for (int i = 0; i < dim; i++)
      {
        double val_i = val[i] * weight_squared;

        for (int j = i; j < dim; j++)
          {
            sakk[i][j] += val_i * val[j];

            if (i != j)
              {
                sakk[j][i] = sakk[i][j];
              }
          }
      }

    sweight += weight;
    return OK;
  }

  int Statistics::put(const std::vector<double>& val, double weight)
  {
    if (weight < 0.0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((int)val.size() != dim)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    for (int i = 0; i < dim; i++)
      {
        akk[i] += val[i] * weight;
      }

    double weight_squared = Sqr(weight);

    for (int i = 0; i < dim; i++)
      {
        double val_i = val[i] * weight_squared;

        for (int j = i; j < dim; j++)
          {
            sakk[i][j] += val_i * val[j];

            if (i != j)
              {
                sakk[j][i] = sakk[i][j];
              }
          }
      }

    sweight += weight;
    return OK;
  }

  int Statistics::put(const Matrix& vals)
  {
    if (vals.cols() != dim)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    for (int i = 0; i < vals.rows(); i++)
      {
        put(vals[i]);
      }

    return OK;
  }
#undef FNAME
#define FNAME "Statistics::getMean"
  std::vector<double> Statistics::getMean() const
  {
    std::vector<double> res(dim);
    if (sweight == 0.0)
      throw IceException(FNAME, M_STAT_NOENTRY);

    double inv_weight = 1 / sweight;

    for (int i = 0; i < dim; i++)
      {
        res[i] = akk[i] * inv_weight;
      }

    return res;
  }
#undef FNAME
#define FNAME "Mean"
  Vector Mean(const Statistics& st)
  {
    if (st.sweight == 0.0)
      throw IceException(FNAME, M_STAT_NOENTRY);

    Vector mean(st.dim);
    double inv_weight = 1 / st.sweight;

    for (int i = 0; i < st.dim; i++)
      {
        mean[i] = st.akk[i] * inv_weight;
      }

    return mean;
  }
#undef FNAME
#define FNAME "Weight"
  double Weight(const Statistics& st)
  {
    return st.sweight;
  }
#undef FNAME
#define FNAME "Statistics::getCovariance"
  matrix<double> Statistics::getCovariance() const
  {
    matrix<double> res(dim, dim);
    if (sweight == 0.0)
      throw IceException(FNAME, M_STAT_NOENTRY);

    double nrm = 1 / sweight;

    for (int i = 0; i < dim; i++)
      for (int j = i; j < dim; j++)
        {
          double cov = (sakk[i][j] - akk[i] * akk[j] * nrm) * nrm;
          res[j][i] = res[i][j] = cov;
        }

    return res;
  }
#undef FNAME
#define FNAME "Covariance"
  Matrix Covariance(const Statistics& st)
  {
    int i, j;
    Matrix res(st.dim, st.dim);

    if (st.sweight == 0.0)
      throw IceException(FNAME, M_STAT_NOENTRY);

    double nrm = 1 / st.sweight;

    for (i = 0; i < st.dim; i++)
      for (j = i; j < st.dim; j++)
        {
          double cov = (st.sakk[i][j] - st.akk[i] * st.akk[j] * nrm) * nrm;
          res.at(i).at(j) = cov;

          if (i != j)
            {
              res.at(j).at(i) = cov;
            }
        }

    return res;
  }
#undef FNAME
#define FNAME "Statistics::getCorrelation"
  matrix<double> Statistics::getCorrelation() const
  {

    if (sweight == 0.0)
      throw IceException(FNAME, M_STAT_NOENTRY);
    try
      {
        matrix<double> res = getCovariance();

        for (int i = 0; i < dim; i++)
          {
            if (res[i][i] < 1e-32)
              throw IceException(FNAME, M_ZERO_VARIANZ);
          }

        for (int i = 0; i < dim; i++)
          for (int j = i + 1; j < dim; j++)
            {
              res[i][j] = res[i][j] / sqrt(res[i][i] * res[j][j]);
              res[j][i] = res[i][j];
            }

        for (int i = 0; i < dim; i++)
          {
            res[i][i] = 1.0;
          }

        return res;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Correlation"
  Matrix Correlation(const Statistics& st)
  {
    if (st.sweight == 0.0)
      throw IceException(FNAME, M_STAT_NOENTRY);

    try
      {
        Matrix res = Covariance(st);

        for (int i = 0; i < st.dim; i++)
          {
            if (res[i][i] < 1e-32)
              throw IceException(FNAME, M_ZERO_VARIANZ);
          }

        for (int i = 0; i < st.dim; i++)
          for (int j = i + 1; j < st.dim; j++)
            {
              res[i][j] = res[i][j] / sqrt(res[i][i] * res[j][j]);
              res[j][i] = res[i][j];
            }

        for (int i = 0; i < st.dim; i++)
          {
            res[i][i] = 1.0;
          }

        return res;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Write(Statistics)"
  void Write(const Statistics& st, const std::string& fn)
  {
    try
      {
        std::ofstream out(fn.c_str());
        out << st;
        out.close();
      }
    catch (std::exception& ex)
      {
        throw IceException(FNAME, ex.what());
      }
  }
#undef FNAME
#define FNAME "operator << (Statistics)"
  std::ostream& operator<<(std::ostream& out, const Statistics& st)
  {
    out.precision(8);
    out << st.dim << FILE_SEP << st.sweight << std::endl ;
    out << Vector(st.akk) << std::endl;

    Matrix sakk(st.dim, st.dim);

    for (int i = 0; i < st.dim; i++)
      for (int j = 0; j < st.dim; j++)
        {
          sakk[i][j] = st.sakk[i][j];
        }

    out << sakk << std::endl ;
    return out;
  }
#undef FNAME
#define FNAME "Read(Statistics)"
  void Read(Statistics& st, const std::string& fn)
  {
    try
      {
        std::ifstream inp(fn.c_str());
        inp >> st;
        inp.close();
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "operator >> (Statistics)"
  std::istream& operator >> (std::istream& inp, Statistics& st)
  {
    char c;
    inp >> st.dim >> c;

    if ((c != ',') && (c != '#'))
      {
        inp.clear(std::ios::badbit);
        throw IceException(FNAME, M_WRONG_FILEFORMAT);
      }

    inp >> st.sweight;
    Vector akk;
    Matrix sakk;
    inp >> akk >> sakk;

    st.akk.resize(akk.size());

    for (unsigned int i = 0; i < akk.size(); i++)
      {
        st.akk[i] = akk[i];
      }

    st.sakk.resize(sakk.rows(), std::vector<double>(sakk.cols()));

    for (int i = 0; i < sakk.rows(); i++)
      for (int j = 0; j < sakk.cols(); j++)
        {
          st.sakk[i][j] = sakk[i][j];
        }

    return inp;
  }
#undef FNAME
}
#undef FNAME
