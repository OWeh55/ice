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

#include "message.h"
#include "macro.h"
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
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
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
      {
        Message(FNAME, M_NOT_INITIALISED, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        dim = 0;
        return WRONG_PARAM;
      }

    dim = dimension;
    return Init();
  }
#undef FNAME
#define FNAME "Put"
  int Statistics::put(const Vector& val, double weight)
  {
    if (weight < 0.0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (val.Size() != dim)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((int)val.size() != dim)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
      {
        Message(FNAME, M_STAT_NOENTRY, ERROR);
        return res;
      }

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
      {
        Message(FNAME, M_STAT_NOENTRY, ERROR);
        return Vector(st.dim);
      }

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
      {
        Message(FNAME, M_STAT_NOENTRY, ERROR);
        return res;
      }

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
      {
        Message(FNAME, M_STAT_NOENTRY, ERROR);
        return res;
      }

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
    matrix<double> res(dim, dim);

    if (sweight == 0.0)
      {
        Message(FNAME, M_STAT_NOENTRY, ERROR);
        return res;
      }

    IF_FAILED(res = getCovariance())
    {
      Message(FNAME, M_0, ERROR);
      return res;
    }

    for (int i = 0; i < dim; i++)
      {
        if (res[i][i] < 1e-32)
          {
            Message(FNAME, M_ZERO_VARIANZ, ERROR);
            return res;
          }
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
#undef FNAME
#define FNAME "Correlation"
  Matrix Correlation(const Statistics& st)
  {
    Matrix res(st.dim, st.dim);

    if (st.sweight == 0.0)
      {
        Message(FNAME, M_STAT_NOENTRY, ERROR);
        return res;
      }

    IF_FAILED(res = Covariance(st))
    {
      Message(FNAME, M_0, ERROR);
      return res;
    }

    for (int i = 0; i < st.dim; i++)
      {
        if (res[i][i] < 1e-32)
          {
            Message(FNAME, M_ZERO_VARIANZ, ERROR);
            return res;
          }
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
#undef FNAME
#define FNAME "Write(Statistics)"
  int Write(const Statistics& st, const std::string& fn)
  {
    std::ofstream out(fn.c_str());
    RETURN_ERROR_IF_FAILED(out << st);
    out.close();
    return OK;
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
  int Read(Statistics& st, const std::string& fn)
  {
    std::ifstream inp(fn.c_str());
    RETURN_ERROR_IF_FAILED(inp >> st);
    inp.close();
    return OK;
  }
#undef FNAME
#define FNAME "operator >> (Statistics)"
  std::istream& operator >> (std::istream& inp, Statistics& st)
  {
    char c;
    inp >> st.dim >> c;

    if ((c != ',') && (c != '#'))
      {
        Message(FNAME, M_WRONG_FILE, WRONG_FILE);
        inp.clear(std::ios::badbit);
        return inp;
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
