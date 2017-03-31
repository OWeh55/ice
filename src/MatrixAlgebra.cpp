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

#define ERR(f,m,r,ret) { throw IceException(f,m,r); }

using std::vector;
namespace ice
{

#define FNAME "Matrix::CholeskyInverse"
  Matrix CholeskyInverse(const Matrix& mat)
  {
    try
      {
        int dimension = mat.cols();

        if (dimension != mat.rows())
          throw IceException(FNAME, M_NO_SQUARE, WRONG_PARAM);

        Matrix l = CholeskyDecomposition(mat);

        Matrix h(dimension, dimension);
        Matrix lt(dimension, dimension);

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

        Matrix inverse(dimension, dimension);
        Vector x(dimension);

        for (int n = 0; n < dimension; n++)
          {
            x.Set(0.0);
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
    catch (IceException& ex)
      {
        throw IceException(ex, FNAME);
      }
  }
#undef FNAME

#define FNAME "Matrix::CholeskyDecomposition"
  Matrix CholeskyDecomposition(const Matrix& mat)
  {
    int dimension = mat.cols();

    if (mat.rows() != dimension)
      throw IceException(FNAME, M_NO_SQUARE);

    Matrix res(dimension, dimension);
    res.Set(0.0);

    Matrix hilf2(mat);

    double sum;

    for (int i = 0; i < dimension; i++)
      for (int j = i; j < dimension; j++)
        {
          sum = hilf2.at(i).at(j);

          for (int k = i - 1; k >= 0; k--)
            {
              sum -= hilf2.at(i).at(k) * hilf2.at(j).at(k);
            }

          if (i == j)
            {
              if (sum <= 0.0)
                {
                  ERR(FNAME, M_WRONG_MATRIX, WRONG_PARAM, res);
                }

              res.at(i).at(i) = sqrt(sum);
            }
          else
            {
              hilf2.at(j).at(i) = sum / res.at(i).at(i);
            }
        }

    for (int i = 0; i < dimension; i++)
      for (int j = (i + 1); j < dimension; j++)
        {
          res.at(j).at(i) = hilf2.at(j).at(i);
        }

    return res;
  }
#undef FNAME

#define FNAME "Matrix::IsPositivDefinit()"
  bool IsPositivDefinit(const Matrix& mat)
  {
    int dimension = mat.cols();

    if (dimension != mat.rows())
      throw IceException(FNAME, M_WRONG_MATRIX);

    Matrix hilf2(mat);

    Matrix h(dimension, dimension);
    h.Set(0);

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

                h[i][i] = sqrt(sum);
              }
            else
              {
                hilf2[j][i] = sum / h[i][i];
              }
          }
      }

    return true;
  }
#undef FNAME

  bool hasInverse(const Matrix& mat)
  {
    int dim = mat.cols();

    if (dim != mat.rows())
      {
        return false;
      }

    double* Mat = new double[dim * dim];

    for (int j = 0; j < dim; j++)
      for (int i = 0; i < dim; i++)
        {
          Mat[j * dim + i] = mat[j][i];
        }

    IF_FAILED(InvertMatrix(Mat, dim, Mat))
    {
      delete [] Mat;
      return false;
    }

    delete [] Mat;
    return true;
  }

#define FNAME "FindPivot"
  int FindPivot(const Matrix& m, int i, int j1)
  {
    int jo = 0;
    double max;
    max = 0;

    if (m.rows() == 0)
      {
        ERR(FNAME, M_WRONG_MATRIX, WRONG_PARAM, -1);
      }

    if ((j1 < 0) || (j1 >= m.cols()))
      {
        ERR(FNAME, M_WRONG_PARAM, WRONG_PARAM, -1);
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
    int i, j, k;

    if (m.cols() != m.rows())
      {
        ERR(FNAME, M_NO_SQUARE, WRONG_PARAM, 0);
      }

    if (m.rows() == 1)
      {
        return m[0][0];
      }

    int dim = m.rows();
    double det = 1.0;
    double sign = 1.0;

    Matrix h(m);

    double hilf = 0;
    double max, s, q, pivot;
    IVector p_k(dim);

    for (k = 0; k < dim; k++)
      {
        max = 0;
        p_k[k] = 0;

        for (i = k; i < dim; i++)
          {
            s = 0;

            for (j = k; j < dim; j++)
              {
                s = s + fabs(h[i][j]);
              }

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
          {
            return 0;
          }

        if (p_k[k] != k)
          {
            sign = -sign;

            for (j = 0; j < dim; j++)
              {
                hilf = h[k][j];
                h[k][j] = h[p_k[k]][j];
                h[p_k[k]][j] = hilf;
              }
          }

        pivot = h[k][k];

        for (j = (k + 1); j < dim; j++)
          {
            double faktor = (-(h[j][k]) / pivot);

            for (i = 0; i < dim; i++)
              {
                h[j][i] = h[j][i] + (h[k][i] * faktor);
              }
          }

        for (j = (k + 1); j < dim; j++)
          {
            h[j][k] = 0;
          }
      }

    det = sign;

    for (k = 0; k < dim; k++)
      {
        det = det * h[k][k];
      }

    return det;
  }

  double Determinant(const matrix<double>& m)
  {
    if (m.cols() != m.rows())
      {
        ERR(FNAME, M_NO_SQUARE, WRONG_PARAM, 0);
      }

    if (m.rows() == 1)
      {
        return m[0][0];
      }

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
              {
                s = s + fabs(h[i][j]);
              }

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
          {
            return 0;
          }

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
              {
                h[j][i] = h[j][i] + (h[k][i] * faktor);
              }
          }

        for (int j = (k + 1); j < dim; j++)
          {
            h[j][k] = 0;
          }
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
    if (m.cols() != m.rows())
      {
        ERR(FNAME, M_NO_SQUARE, WRONG_PARAM, 0);
      }

    int dimension = m.cols();
    Matrix l;

    RETURN_IF_FAILED(l = CholeskyDecomposition(m), 0.0);

    double det = 1;

    for (int k = 0; k < dimension; k++)
      {
        det = det * l[k][k];
      }

    return det * det;
  }
#undef FNAME

#define FNAME "SolveLinEqu"
  int SolveLinEqu1(const Matrix& m, const Vector& v, Vector& res)
  {
    // Matrix is square, v has correct size

    Matrix LU;
    IVector indx;

    // LU-Zerlegung
    IF_FAILED(LUDecompositionPacked(m, LU, indx, true))
    {
      throw IceException(FNAME, M_0, ERROR);
      return ERROR;
    }
    // Lösen von L*U*x=i
    res = LUSolve(LU, indx, v);
    return OK;
  }

  int SolveLinearEquation1(const matrix<double>& A,
                           const std::vector<double>& b,
                           std::vector<double>& x)
  {
    // Matrix is square, v has correct size

    matrix<double> LU;
    vector<int> index;

    // LU-Zerlegung
    IF_FAILED(LUDecompositionPacked(A, LU, index, true))
    {
      throw IceException(FNAME, M_0, ERROR);
      return ERROR;
    }
    // Lösen von L*U*x=b
    x = LUSolve(LU, index, b);
    return OK;
  }

  Vector SolveLinEqu(const Matrix& m, const Vector& v)
  {
    int rc;
    Vector res(v);

    if (v.Size() != m.rows())
      {
        ERR(FNAME, M_WRONG_MATRIX, WRONG_PARAM, res);
      }

    if (m.cols() > m.rows())
      {
        ERR(FNAME, M_WRONG_MATRIX, WRONG_PARAM, res);
      }

    // Ausgleichsrechnung bei überbestimmten Gleichungsystemen
    if (m.cols() < m.rows())
      {
        Matrix a = m.MulTrans(m); // m^T * m
        Vector i = m.MulTrans(v); // m^T * v
        rc = SolveLinEqu1(a, i, res);
      }
    else
      {
        rc = SolveLinEqu1(m, v, res);
      }

    if (rc != OK)
      {
        throw IceException(FNAME, M_0, ERROR);
        return res;
      }

    return res;
  }

#undef FNAME
#define FNAME "SolveLinearEquation"
  std::vector<double> SolveLinearEquation(const matrix<double>& m,
                                          const std::vector<double>& b)
  {
    std::vector<double> res(m.cols());

    if ((int)b.size() != m.rows())
      {
        ERR(FNAME, M_WRONG_MATRIX, WRONG_PARAM, res);
      }

    if (m.cols() > m.rows())
      {
        ERR(FNAME, M_WRONG_MATRIX, WRONG_PARAM, res);
      }

    int rc;

    // Ausgleichsrechnung bei überbestimmten Gleichungsystemen
    if (m.cols() < m.rows())
      {
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
        rc = SolveLinearEquation1(a, bb, res);
      }
    else
      {
        rc = SolveLinearEquation1(m, b, res);
      }

    if (rc != OK)
      {
        throw IceException(FNAME, M_0, ERROR);
        return res;
      }

    return res;
  }
#undef FNAME
} // namespace ice
