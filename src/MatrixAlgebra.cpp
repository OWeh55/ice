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
#include <stdlib.h>
#include <math.h>
#include <fstream>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "matrix_function.h"
#include "numbase.h"
#include "ludecomp.h"

#include "MatrixAlgebra.h"

#define PRECISION 1e-200

using std::vector;

namespace ice
{
#define FNAME "Cholesky"
  bool Cholesky(matrix<double>& mat)
  {
    int dimension = mat.cols();

    if (mat.rows() != dimension)
      throw IceException(FNAME, M_NO_SQUARE);

    for (int i = 0; i < dimension; i++)
      for (int j = 0; j < i; j++)
        {
          if (fabs(mat[i][j] - mat[j][i]) > 1e-14) // symmetric?
            return false;
        }

    for (int i = 0; i < dimension; i++)
      for (int j = 0; j <= i; j++)
        {
          double sum = mat[i][j];

          for (int k = 0; k < j; k++)
            sum -= mat[i][k] * mat[j][k];

          if (j < i)
            mat[i][j] = sum / mat[j][j];
          else
            {
              if (sum <= 0.0)
                {
                  return false;
                }
              mat[i][i] = sqrt(sum);
            }
        }

    for (int i = 0; i < dimension; i++)
      for (int j = (i + 1); j < dimension; j++)
        {
          mat[i][j] = 0.0;
        }

    return true;
  }
#undef FNAME
#define FNAME "CholeskyDecomposition"
  Matrix CholeskyDecomposition(const Matrix& mat)
  {
    matrix<double> m(mat);
    m = CholeskyDecomposition(m);
    return Matrix(m);
  }

  matrix<double> CholeskyDecomposition(const matrix<double>& mat)
  {
    try
      {
        matrix<double> result(mat);
        if (Cholesky(result))
          return result;
        else
          throw IceException(FNAME, M_WRONG_MATRIX);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Matrix::CholeskyInverse"
  Matrix CholeskyInverse(const Matrix& mat)
  {
    try
      {
        matrix<double> m(mat);
        m = CholeskyInverse(m);
        return Matrix(m);
      }
    RETHROW;
  }

  matrix<double> CholeskyInverse(const matrix<double>& mat)
  {
    try
      {
        matrix<double> l = CholeskyDecomposition(mat);

        int dimension = l.rows();
        matrix<double> h(dimension, dimension);
        matrix<double> lt(dimension, dimension);

        for (int k = 0; k < dimension; k++)
          {
            for (int i = 0; i < dimension; i++)
              {
                if (k <= i)
                  {
                    h[k][i] = mat[k][i];
                  }
                else
                  {
                    h[k][i] = l[k][i];
                  }

                lt[k][i] = l[i][k];
              }
          }

        matrix<double> inverse(dimension, dimension);

        for (int n = 0; n < dimension; n++)
          {
            vector<double> x(dimension, 0.0);
            x[n] = 1.0;

            for (int i = 0; i < dimension; i++)
              {
                double sum = x[i];

                for (int k = i - 1; k >= 0; k--)
                  {
                    sum -= h[i][k] * x[k];
                  }

                x[i] = sum / l[i][i];
              }

            for (int i = (dimension - 1); i >= 0; i--)
              {
                double sum = x[i];

                for (int k = i + 1; k < dimension; k++)
                  {
                    sum -= h[k][i] * x[k];
                  }

                x[i] = sum / l[i][i];
                inverse[i][n] = x[i];
              }
          }

        return inverse;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Matrix::isPositiveDefinite()"
  bool isPositiveDefinite(const matrix<double>& mat)
  {
    int dimension = mat.cols();

    if (dimension != mat.rows())
      throw IceException(FNAME, M_WRONG_MATRIX);

    matrix<double> hilf2(mat);

    vector<double> h(dimension);

    double sum;

    for (int i = 0; i < dimension; i++)
      {
        for (int j = i; j < dimension; j++)
          {
            sum = hilf2[i][j];

            for (int k = i - 1; k >= 0; k--)
              {
                sum -= hilf2[i][k] * hilf2[j][k];
              }

            if (i == j)
              {
                if (sum <= 0.0)
                  {
                    return false;
                  }

                h[i] = sqrt(sum);
              }
            else
              {
                hilf2[j][i] = sum / h[i];
              }
          }
      }

    return true;
  }

  bool isPositiveDefinite(const Matrix& mat)
  {
    matrix<double> m(mat);
    return isPositiveDefinite(m);
  }
#undef FNAME

#define FNAME "FindPivot"
  int FindPivot(const Matrix& m, int i, int j1)
  {
    int jo = 0;
    double max;
    max = 0;

    if (m.rows() == 0)
      {
        throw IceException(FNAME, M_WRONG_MATRIX);
      }

    if ((j1 < 0) || (j1 >= m.cols()))
      {
        throw IceException(FNAME, M_WRONG_PARAM);
      }

    for (int j = j1; j < m.rows(); j++)
      {
        if (fabs(m[j][i]) > max)
          {
            max = fabs(m[j][i]);
            jo = j;
          }
      }

    if (max < PRECISION)
      {
        return -1;
      }

    return jo;
  }
#undef FNAME

#define FNAME "Determinant"
  double Determinant(const Matrix& m)
  {
    matrix<double> mat(m);
    return Determinant(mat);
  }

  double Determinant(const matrix<double>& m)
  {
    if (m.cols() != m.rows())
      throw IceException(FNAME, M_NO_SQUARE);

    if (m.rows() == 1)
      return m[0][0];

    int dim = m.rows();

    int sign = 1;

    // copy matrix
    matrix<double> h(m);

    double q, pivot;

    vector<int> p_k(dim);

    for (int k = 0; k < dim; k++)
      {
        double max = 0;
        p_k[k] = 0;

        for (int i = k; i < dim; i++)
          {
            double s = 0;

            for (int j = k; j < dim; j++)
              s = s + fabs(h[i][j]);

            if (s != 0)
              {
                q = fabs(h[i][k]) / s;
              }
            else
              {
                q = 0;
              }

            if (q > max)
              {
                max = q;
                p_k[k] = i;
              }
          }

        if (max == 0)
          return 0;

        if (p_k[k] != k)
          {
            sign = -sign;

            for (int j = 0; j < dim; j++)
              {
                double hilf = h[k][j];
                h[k][j] = h[p_k[k]][j];
                h[p_k[k]][j] = hilf;
              }
          }

        pivot = h[k][k];

        for (int j = (k + 1); j < dim; j++)
          {
            double faktor = (-(h[j][k]) / pivot);

            for (int i = 0; i < dim; i++)
              h[j][i] = h[j][i] + (h[k][i] * faktor);
          }

        for (int j = (k + 1); j < dim; j++)
          h[j][k] = 0;
      }

    double det = sign;

    for (int k = 0; k < dim; k++)
      {
        det = det * h[k][k];
      }

    return det;
  }
#undef FNAME
#define FNAME "CholeskyDeterminant"
  double CholeskyDeterminant(const Matrix& m)
  {
    matrix<double> mat(m);
    return CholeskyDeterminant(mat);
  }

  double CholeskyDeterminant(const matrix<double>& m)
  {
    if (m.cols() != m.rows())
      throw IceException(FNAME, M_NO_SQUARE);

    try
      {
        int dimension = m.cols();
        matrix<double> l = CholeskyDecomposition(m);

        double det = 1;

        for (int k = 0; k < dimension; k++)
          {
            det = det * l[k][k];
          }

        return det * det;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "solveLinEqu"
  int solveLinearEquation1(const Matrix& m, const Vector& v, Vector& res)
  {
    // Matrix is square, v has correct size
    try
      {
        Matrix LU;
        IVector indx;

        // LU-Zerlegung
        LUDecompositionPacked(m, LU, indx);

        // Lösen von L*U*x=i
        res = LUSolve(LU, indx, v);
        return OK;
      }
    RETHROW;
  }

  void solveLinearEquation1(const matrix<double>& A,
                            const std::vector<double>& b,
                            std::vector<double>& x)
  {
    // Matrix is square, v has correct size
    try
      {
        matrix<double> LU;
        vector<int> index;

        // LU-Zerlegung
        LUDecompositionPacked(A, LU, index, true);
        // Lösen von L*U*x=b
        x = LUSolve(LU, index, b);
      }
    RETHROW;
  }

  Vector SolveLinearEquation(const Matrix& m, const Vector& v)
  {
    try
      {
        Vector res(v);

        if (v.Size() != m.rows() || m.cols() > m.rows())
          throw IceException(FNAME, M_WRONG_MATRIX);

        // Ausgleichsrechnung bei überbestimmten Gleichungsystemen
        if (m.cols() < m.rows())
          {
            Matrix a = m.MulTrans(m); // m^T * m
            Vector i = m.MulTrans(v); // m^T * v
            solveLinearEquation1(a, i, res);
          }
        else
          {
            solveLinearEquation1(m, v, res);
          }

        return res;
      }
    RETHROW;
  }

#undef FNAME
#define FNAME "solveLinearEquation"
  std::vector<double> solveLinearEquation(const matrix<double>& m,
                                          const std::vector<double>& b)
  {
    std::vector<double> res(m.cols());

    if ((int)b.size() != m.rows())
      {
        throw IceException(FNAME, M_WRONG_MATRIX);
      }

    if (m.cols() > m.rows())
      {
        throw IceException(FNAME, M_WRONG_MATRIX);
      }

    if (m.cols() < m.rows())
      {
        // Ausgleichsrechnung bei überbestimmten Gleichungsystemen
        matrix<double> a = !m * m; // m^T * m
        std::vector<double> bb(m.cols());
        for (int i = 0; i < m.cols(); ++i)
          {
            bb[i] = 0;
            for (int k = 0; k < m.rows(); ++k)
              {
                bb[i] += m[k][i] * b[k];
              }
          }
        solveLinearEquation1(a, bb, res);
      }
    else
      {
        // Ausgleichsrechnung bei überbestimmten Gleichungsystemen
        solveLinearEquation1(m, b, res);
      }

    return res;
  }
#undef FNAME
} // namespace ice
