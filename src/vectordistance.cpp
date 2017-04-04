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
#include <math.h>

#include "macro.h"
#include "vectordistance.h"

namespace ice
{
  double EuclideanVectorDistance::distance(const double* x, const double* y, int n) const
  {
    double dist      = 0.0;

    for (int i = 0 ; i < n ; i++)
      {
        double diff = (x[i] - y[i]);
        dist += diff * diff;
      }

    return sqrt(dist);
  }

  double SquaredEuclideanVectorDistance::distance(const double* x, const double* y, int n) const
  {
    double dist = 0.0;

    for (int i = 0 ; i < n ; i++)
      {
        double diff = (x[i] - y[i]);
        dist += diff * diff;
      }

    return dist;
  }

  double CityBlockVectorDistance::distance(const double* x, const double* y, int n) const
  {
    double dist      = 0.0;

    for (int i = 0 ; i < n ; i++)
      {
        double diff = (x[i] - y[i]);
        dist += fabs(diff);
      }

    return dist;
  }

  double EarthMoverUnNormalizedVectorDistance::distance(const double* x, const double* y, int n) const
  {
    double dist = 0.0;
    double sumx = 0.0;
    double sumy = 0.0;

    for (int i = 0 ; i < n ; i++)
      {
        sumx += x[i];
        sumy += y[i];
        dist += fabs(sumy - sumx);
      }

    return dist;
  }

  double EarthMoverVectorDistance::distance(const double* x, const double* y, int n) const
  {
    double sumx = 0.0;
    double sumy = 0.0;

    for (int i = 0 ; i < n ; i++)
      {
        sumx += x[i];
        sumy += y[i];
      }

    double dist = 0.0;
    double partialsumx = 0.0;
    double partialsumy = 0.0;

    for (int i = 0 ; i < n ; i++)
      {
        partialsumx += x[i];
        partialsumy += y[i];
        dist += fabs(partialsumy / sumy - partialsumx / sumx);
      }

    return dist;
  }

  double QuadraticFormVectorDistance::distance(const double* x, const double* y, int n) const
  {
    std::vector<double> diff(n);

    for (int i = 0 ; i < n ; i++)
      {
        diff[i] = x[i] - y[i];
      }

    double dist = 0.0;

    for (int i = 0 ; i < n ; i++)
      for (int k = 0 ; k < n ; k++)
        {
          dist += diff[i] * diff[k] * A[i][k];
        }

    return sqrt(dist);
  }

#define FNAME "DistanceMatrix"
  int DistanceMatrix(const Matrix& feat1, const Matrix& feat2, Matrix& dist, int mode)
  {
    int n1 = feat1.rows();
    int n2 = feat2.rows();

    dist = Matrix(n1, n2);

    int dim = feat1.cols();

    if (dim != feat2.cols())
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return WRONG_MATRIX;
      }

    Vector am(dim);

    // calculating distance field
    for (int i = 0; i < n1; ++i)
      {
        am = feat1.at(i);

        for (int j = 0; j < n2; ++j)
          {
            double d = 0.0;

            switch (mode)
              {
              case D_EUCLID:

                for (int k = 0; k < dim; k++)
                  {
                    d += Sqr(am.at(k) - feat2.at(j).at(k));
                  }

                d = sqrt(d);
                break;
              case D_SQUARE:

                for (int k = 0; k < dim; k++)
                  {
                    d += Sqr(am.at(k) - feat2.at(j).at(k));
                  }

                break;
              case D_CITYBLOCK:

                for (int k = 0; k < dim; k++)
                  {
                    d += fabs(am.at(k) - feat2.at(j).at(k));
                  }
              }

            dist.at(i).at(j) = d;
          }
      }

    return OK;
  }

  Matrix DistanceMatrix(const Matrix& feat1, const Matrix& feat2,
                        int mode)
  {
    try {
    Matrix res;
    DistanceMatrix(feat1, feat2, res, mode);
    return res;
    }
    RETHROW;
  }


  Matrix DistanceMatrix(const Matrix& feat, int mode)
  {
    try {
    Matrix res;
    DistanceMatrix(feat, feat, res, mode);
    return res;
    }
    RETHROW;
  }

  int DistanceMatrix(const Matrix& feat1, const Matrix& feat2, Matrix& distmatrix, const VectorDistance& dist)
  {
    int n1 = feat1.rows();
    int n2 = feat2.rows();

    int dim = feat1.cols();

    if (dim != feat2.cols())
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
      }

    distmatrix = Matrix(n1, n2);

    // calculating distance field
    for (int i = 0; i < n1; ++i)
      {
        const Vector& am = feat1.at(i);

        for (int j = 0; j < n2; ++j)
          {
            distmatrix.at(i).at(j) = dist(am, feat2[j]);
          }
      }

    return OK;
  }

  int DistanceMatrix(const std::vector<std::vector<double> > features_1,
                     const std::vector<std::vector<double> > features_2,
                     Matrix& dist)
  {
    unsigned int n1 = features_1.size();
    unsigned int n2 = features_2.size();
    if (n1 < 1 || n2 < 1)
      {
        throw IceException(FNAME, M_EMPTY_POINTLIST, WRONG_PARAM);
        return WRONG_PARAM;
      }

    unsigned int feature_dim = features_1[0].size();
    if (features_2[0].size() != feature_dim)
      {
        throw IceException(FNAME, M_VECTORDIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    dist = Matrix(n1, n2);
    for (unsigned int i = 0; i < n1; ++i)
      {
        for (unsigned int j = 0; j < n2; ++j)
          {
            double sum = 0.0;
            for (unsigned int k = 0; k < feature_dim; k++)
              {
                double dd = features_1[i][k] - features_2[j][k];
                sum += dd * dd;
              }
            dist[i][j] = sqrt(sum);
          }
      }
    return OK;
  }

  Matrix DistanceMatrix(const Matrix& feat1, const Matrix& feat2, const VectorDistance& dist)
  {
    try {
    Matrix res;
    DistanceMatrix(feat1, feat2, res, dist);
    return res;
    }
    RETHROW;
  }

  Matrix DistanceMatrix(const Matrix& feat, const VectorDistance& dist)
  {
    try {
    Matrix res;
    DistanceMatrix(feat, feat, res, dist);
    return res;
    }
    RETHROW;
  }
#undef FNAME
}
