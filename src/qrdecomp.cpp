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
#include "Matrix.h"
#include "qrdecomp.h"

#include "numbase.h"
#include "IceException.h"

namespace ice
{

#define FNAME "QRDecompositionPacked"
  int QRDecompositionPacked(const Matrix& A,
                            Matrix& a, Vector& c, Vector& d)
  {
    int i, j, k;
    double scale, sigma, sum, tau, val;

    int dim = A.cols();

    if (A.rows() != dim)
      throw IceException(FNAME, M_NO_SQUARE);

    a = A;
    c = Vector(dim);
    d = Vector(dim);

    for (k = 0; k < dim - 1; k++)   // all but last column
      {
        scale = 0.0;

        for (i = k; i < dim; i++)
          {
            scale = std::max(scale, fabs(a[i][k]));
          }

        if (scale == 0.0)
          {
            // Singularity in kth column
            c[k] = d[k] = 0.0;
          }
        else
          {
            sum = 0.0;

            for (i = k; i < dim; i++)
              {
                val = a[i][k] / scale;
                a[i][k] = val;
                sum += val * val;
              }

            sigma = fabs(sqrt(sum));

            if (a[k][k] < 0)
              {
                sigma = -sigma;
              }

            a[k][k] += sigma;
            c[k] = sigma * a[k][k];
            d[k] = -scale * sigma;

            for (j = k + 1; j < dim; j++)
              {
                sum = 0.0;

                for (i = k; i < dim; i++)
                  {
                    sum += a[i][k] * a[i][j];
                  }

                tau = sum / c[k];

                for (i = k; i < dim; i++)
                  {
                    a[i][j] -= tau * a[i][k];
                  }
              }
          }
      }

    d[dim - 1] = a[dim - 1][dim - 1]; // last column
    return OK;
  }

  int QRDecomposition(const Matrix& A, Matrix& Q, Matrix& R)
  {
    int dim = A.cols();

    if (A.rows() != dim)
      throw IceException(FNAME, M_NO_SQUARE);

    int i, j, k;

    Vector c(dim);
    Vector d(dim);

    QRDecompositionPacked(A, R, c, d);

    Q = Matrix(dim, dim);
    Matrix Qi(dim, dim);
    Vector Ui(dim);

    for (i = 0; i < dim - 1; i++)
      {
        double cn;

        if (c[i] != 0.0)
          {
            cn = 1 / c[i];
          }
        else
          {
            cn = 0.0;
          }

        for (j = 0; j < i; j++)
          {
            Ui[j] = 0.0;
          }

        for (j = i; j < dim; j++)
          {
            Ui[j] = R[j][i];
          }

        for (j = 0; j < dim; j++)
          for (k = 0; k < dim; k++)
            {
              Qi[j][k] = -cn * Ui[j] * Ui[k];

              if (k == j)
                {
                  Qi[j][k] += 1.0;
                }
            }

        if (i == 0)
          {
            Q = Qi;
          }
        else
          {
            Q = Q * Qi;
          }
      }

// Obere Dreiecksmatrix R
    for (i = 0; i < dim; i++)
      for (j = i + 1; j < dim; j++)
        {
          R[j][i] = 0.0;
        }

    for (i = 0; i < dim; i++)
      {
        R[i][i] = d[i];

        if (d[i] < 0)
          {
            // invert ith line in R and ith column in Q
            for (j = 0; j < dim; j++)
              {
                R[i][j] = -R[i][j];
                Q[j][i] = -Q[j][i];
              }
          }
      }

    return OK;
  }

} // namespace ice
