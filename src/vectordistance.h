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
#ifndef VECTORDISTANCE_H
#define VECTORDISTANCE_H

#include <vector>
#include "Vector.h"
#include "Matrix.h"

namespace ice
{
  class VectorDistance
  {
  public:
    VectorDistance() {};

    virtual ~VectorDistance() {};

    virtual double distance(const double* x, const double* y, int n) const = 0;
    virtual double distance(const Vector& x, const Vector& y) const
    {
      return distance(x.getDataPointer(), y.getDataPointer(), x.Size());
    };
    virtual double distance(const std::vector<double>& x, const std::vector<double>& y) const
    {
      return distance(&x[0], &y[0], x.size());
    };
    virtual double operator()(const Vector& x, const Vector& y) const
    {
      return distance(x, y);
    }
    virtual double operator()(const std::vector<double>& x, const std::vector<double>& y) const
    {
      return distance(x, y);
    }
  };

  class EuclideanVectorDistance : public VectorDistance
  {
  public:
    using VectorDistance::distance;
    virtual double distance(const double* x, const double* y, int n) const;
  };

  class SquaredEuclideanVectorDistance : public VectorDistance
  {
  public:
    using VectorDistance::distance;
    virtual double distance(const double* x, const double* y, int n) const;
  };

  class CityBlockVectorDistance : public VectorDistance
  {
  public:
    using VectorDistance::distance;
    virtual double distance(const double* x, const double* y, int n) const;
  };

  class EarthMoverUnNormalizedVectorDistance : public VectorDistance
  {
  public:
    using VectorDistance::distance;
    virtual double distance(const double* x, const double* y, int n) const;
  };

  class EarthMoverVectorDistance : public VectorDistance
  {
  public:
    using VectorDistance::distance;
    virtual double distance(const double* x, const double* y, int n) const;
  };

  class QuadraticFormVectorDistance : public VectorDistance
  {
  protected:
    Matrix A;

  public:
    QuadraticFormVectorDistance(const Matrix& _A) : A(_A) {};
    using VectorDistance::distance;
    virtual double distance(const double* x, const double* y, int n) const;
  };

// constants for distance measures
#define D_EUCLID 1
#define D_SQUARE 2
#define D_CITYBLOCK 3

  int DistanceMatrix(const Matrix& feat1, const Matrix& feat2, Matrix& dist, int mode = D_EUCLID);
  Matrix DistanceMatrix(const Matrix& feat1, const Matrix& feat2, int mode = D_EUCLID);
  Matrix DistanceMatrix(const Matrix& feat, int mode = D_EUCLID);

  int DistanceMatrix(const Matrix& feat1, const Matrix& feat2, Matrix& distmatrix,
                     const VectorDistance& dist);
  Matrix DistanceMatrix(const Matrix& feat1, const Matrix& feat2,
                        const VectorDistance& dist);
  Matrix DistanceMatrix(const Matrix& feat, const VectorDistance& dist);

  int DistanceMatrix(const std::vector<std::vector<double> > features_1,
                     const std::vector<std::vector<double> > features_2,
                     Matrix& dist);

#define FNAME "DistanceMatrix"
  template<typename T1, typename T2, typename T3>
  int DistanceMatrix(const T1& feat1, const T2& feat2, T3& dist, int mode)
  {
    int n1 = feat1.rows();
    int n2 = feat2.rows();

    dist.resize(n1, n2);

    int dim = feat1.cols();

    if (dim != feat2.cols())
      {
        Message(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return WRONG_MATRIX;
      }

    // calculating distance field
    for (int i = 0; i < n1; ++i)
      {
        for (int j = 0; j < n2; ++j)
          {
            double d = 0.0;

            switch (mode)
              {
              case D_EUCLID:
                for (int k = 0; k < dim; k++)
                  d += Sqr(feat1[i][k] - feat2[j][k]);

                d = sqrt(d);
                break;

              case D_SQUARE:
                for (int k = 0; k < dim; k++)
                  d += Sqr(feat1[i][k] - feat2[j][k]);


                break;

              case D_CITYBLOCK:

                for (int k = 0; k < dim; k++)
                  d += fabs(feat1[i][k] - feat2[j][k]);
                break;
              }

            dist[i][j] = d;
          }
      }

    return OK;
  }
#undef FNAME
}; // namespace

#endif
