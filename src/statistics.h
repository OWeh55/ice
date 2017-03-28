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
//
//  Statistic
//      neub 1/94 - non oop version
//      Ortmann 6/99 - oop version, 03/2013
//
#ifndef _STATIST_H
#define _STATIST_H

#include <iostream>
#include <string>
#include <vector>

#include "matdef.h"
#include "matrixtemplate.h"
#include "Matrix.h"
#include "Vector.h"

namespace ice
{
  // class for multivariate statistics

  template<int dim>
  class statistics
  {
  private:
    int ct;
    double akk[dim];
    double sakk[dim][dim];

  public:
    statistics()
    {
      init();
    }

    ~statistics() {};

    void init() // [re-]initialisation
    {
      ct = 0;

      for (int i = 0; i < dim; i++)
        {
          akk[i] = 0.0;

          for (int k = 0; k < dim; k++)
            sakk[i][k] = 0.0;
        }
    }

    void put(const std::vector<double>& val)
    {
      ct++;

      for (int i = 0; i < dim; i++)
        akk[i] += val[i];

      for (int i = 0; i < dim; i++)
        {
          double vi = val[i];

          for (int j = i; j < dim; j++)
            {
              double sum = sakk[i][j] + vi * val[j];
              sakk[i][j] = sakk[j][i] = sum;
            }
        }
    }

    void put(const Vector& val)
    {
      ct++;

      for (int i = 0; i < dim; i++)
        akk[i] += val[i];

      for (int i = 0; i < dim; i++)
        {
          double vi = val[i];

          for (int j = i; j < dim; j++)
            {
              double sum = sakk[i][j] + vi * val[j];
              sakk[i][j] = sakk[j][i] = sum;
            }
        }
    }

    void put(const Matrix& values)
    {
      for (int i = 0; i < values.rows(); i++)
        put(values[i]);
    }

    std::vector<double> getMean() const
    {
      std::vector<double> res(dim);

      for (int i = 0; i < dim; i++)
        res[i] = akk[i] / ct;

      return res;
    }

    double getCount() const
    {
      return ct;
    }

    matrix<double> getCovariance()
    {
      std::vector<double> mean = getMean();
      matrix<double> res(dim, dim);
      for (int i = 0; i < dim; ++i)
        for (int k = 0; k < dim; ++k)
          res[i][k] = sakk[i][k] / ct - mean[i] * mean[k];
      return res;
    }

    matrix<double> getCorrelation();

    int getDim() const
    {
      return dim;
    }
  };

  class Statistics
  {
  public:
    Statistics() : dim(0) {};
    Statistics(int dim);
    virtual ~Statistics();

    virtual int Init(int dim); // (Re-)initialisation
    virtual int Init(); // reinitialisation

    virtual matrix<double> getCovariance() const ;
    virtual matrix<double> getCorrelation() const ;

    virtual std::vector<double> getMean() const ;

    int put(const Vector& val, double weight = 1.0);
    int put(const std::vector<double>& val, double weight = 1.0);
    int put(const Matrix& val);

    friend Vector Mean(const Statistics& st);
    friend double Weight(const Statistics& st);
    friend Matrix Covariance(const Statistics& st);
    friend Matrix Correlation(const Statistics& st);
    friend int Write(const Statistics& st, const std::string& fn);
    friend int Read(Statistics& st, const std::string& fn);

    friend std::ostream& operator<<(std::ostream&, const Statistics&);
    friend std::istream& operator>>(std::istream&, Statistics&);

    virtual int Dim() const
    {
      return dim;
    }

  protected:

    int dim;                // dimension of feature vector

    double sweight;         // sum of weights
    std::vector<double> akk;         // accumulator
    std::vector<std::vector<double> > sakk; // accumulator (products)
  };

  inline int Put(Statistics& st, const Vector& val, double weight = 1.0)
  {
    return st.put(val, weight);
  }
  inline int Put(Statistics& st, const std::vector<double>& val, double weight = 1.0)
  {
    return st.put(val, weight);
  }
  inline int Put(Statistics& st, const Matrix& val)
  {
    return st.put(val);
  }

}
#endif
