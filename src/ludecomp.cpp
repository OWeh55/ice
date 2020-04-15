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

/*
 * Functions for LU-Decomposition of matrices
 * and solving linear equation systems
 */

#include "Matrix.h"
#include "ludecomp.h"

#include "IceException.h"
#include "math.h"

namespace ice
{

#define FNAME "LUDecompositionPacked"

#define LU(i,j) LU.at(i).at(j)
  //#define LU(i,j) LU[i][j]

  void LUDecompositionPacked(const Matrix& a,
                             Matrix& LU,
                             IVector& indx,
                             bool pivot)
  {
    // liefert die gepackte Darstellung der Zerlegung einer Matrix
    // in eine untere Dreiecksmatrix L und eine obere Dreieckmatrix U
    // Die Ergebnismatrix LU enthält in den Elementen LU_ij:
    // i<=j : LU_ij=U_ij
    // i>j: LU_ij=L_ij
    // Die Elemente L_ii der Hauptdiagonale von L sind gleich 1.0

    int dim = a.cols(); // Dimension merken

    if (dim != a.rows())   // auf quadratische Matrix testen
      throw IceException(FNAME, M_NO_SQUARE);

    try
      {
        LU = a;

        Vector vv(dim);
        indx.Resize(dim);

        int dsign = 1;

        for (int i = 0; i < dim; i++)
          {
            double big = 0.0;

            for (int j = 0; j < dim; j++)
              {
                double temp = fabs(LU(i, j));

                if (temp > big)
                  {
                    big = temp;
                  }
              }

            if (big == 0)
              throw IceException(FNAME, M_MATRIX_SINGULAR);

            vv[i] = 1.0 / big;
          }

        for (int j = 0; j < dim; j++)   // loop over all columns
          {
            for (int i = 0; i < j; i++)
              {
                double sum = LU(i, j);

                for (int k = 0; k < i; k++)
                  {
                    sum -= LU(i, k) * LU(k, j);
                  }

                LU(i, j) = sum;
              }

            double big = 0.0;
            int imax = 0; // avoid warning

            for (int i = j; i < dim; i++)
              {
                double sum = LU(i, j);

                for (int k = 0; k < j; k++)
                  {
                    sum -= LU(i, k) * LU(k, j);
                  }

                LU(i, j) = sum;
                double dum = vv[i] * fabs(sum);

                if (dum >= big)
                  {
                    big = dum;
                    imax = i;
                  }
              }

            if (pivot)
              {
                if (j != imax)
                  {
                    LU.ExchangeRow(j, imax);
                    dsign = -dsign;
                    vv[imax] = vv[j];
                  }

                indx[j] = imax;
              }
            else
              {
                indx[j] = j;
              }

            if (LU(j, j) == 0)
              throw IceException(FNAME, M_MATRIX_SINGULAR);

            if (j < dim - 1)
              {
                double dum = 1.0 / LU(j, j);

                for (int i = j + 1; i < dim; i++)
                  {
                    LU(i, j) *= dum;
                  }
              }
          } // all columns
      }
    RETHROW;
  }

  void LUDecompositionPacked(const matrix<double>& a,
                             matrix<double>& LU,
                             std::vector<int>& indx,
                             bool pivot)
  {
    // liefert die gepackte Darstellung der Zerlegung einer Matrix
    // in eine untere Dreiecksmatrix L und eine obere Dreieckmatrix U
    // Die Ergebnismatrix LU enthält in den Elementen LU_ij:
    // i <= j : LU_ij = U_ij
    // i >  j : LU_ij = L_ij
    // Die Elemente L_ii der Hauptdiagonale von L sind gleich 1.0

    int dim = a.cols(); // Dimension merken

    if (dim != a.rows())   // auf quadratische Matrix testen
      throw IceException(FNAME, M_NO_SQUARE);

    try
      {
        LU = a;

        std::vector<double> vv(dim);
        indx.resize(dim);

        int dsign = 1;

        for (int i = 0; i < dim; i++)   // loop over all rows
          {
            double big = 0.0;

            for (int j = 0; j < dim; j++)
              {
                double temp = fabs(LU[i][j]);

                if (temp > big)
                  {
                    big = temp;
                  }
              }

            if (big == 0)
              throw IceException(FNAME, M_MATRIX_SINGULAR);

            vv[i] = 1.0 / big;
          }

        for (int j = 0; j < dim; j++)   // loop over all columns
          {
            for (int i = 0; i < j; i++)
              {
                double sum = LU[i][j];

                for (int k = 0; k < i; k++)
                  {
                    sum -= LU[i][k] * LU[k][j];
                  }

                LU[i][j] = sum;
              }

            double big = 0.0;
            int imax = 0;

            for (int i = j; i < dim; i++)
              {
                double sum = LU[i][j];

                for (int k = 0; k < j; k++)
                  {
                    sum -= LU[i][k] * LU[k][j];
                  }
                LU[i][j] = sum;

                double dum = vv[i] * fabs(sum);

                if (dum >= big)
                  {
                    big = dum;
                    imax = i;
                  }
              }

            if (pivot)
              {
                if (j != imax)
                  {
                    for (int c = 0; c < LU.cols(); ++c)
                      {
                        std::swap(LU[j][c], LU[imax][c]);
                      }
                    dsign = -dsign;
                    vv[imax] = vv[j];
                  }

                indx[j] = imax;
              }
            else
              {
                indx[j] = j;
              }

            if (LU[j][j] == 0.0)
              throw IceException(FNAME, M_MATRIX_SINGULAR);

            if (j < dim - 1)
              {
                double dum = 1.0 / LU[j][j];

                for (int i = j + 1; i < dim; i++)
                  {
                    LU[i][j] *= dum;
                  }
              }
          } // all columns
      }
    RETHROW;
  }

  void LUDecompositionPacked(const Matrix& a, Matrix& LU)
  {
    IVector ndx; // dummy, ungenutzt weil ohne Pivotisierung
    LUDecompositionPacked(a, LU, ndx, false);
  }

  void LUDecompositionPacked(const matrix<double>& a, matrix<double>& LU)
  {
    std::vector<int> ndx; // dummy, ungenutzt weil ohne Pivotisierung
    LUDecompositionPacked(a, LU, ndx, false);
  }
#undef FNAME
#define FNAME "LUDecomposition"
  void LUDecomposition(const Matrix& a, Matrix& L, Matrix& U)
  {
    try
      {
        Matrix LU;
        LUDecompositionPacked(a, LU);

        int i, j;
        L = LU;
        U = LU;

        // entpacken
        // U ist obere Dreiecksmatrix
        for (i = 0; i < U.cols(); i++)
          {
            for (j = i + 1; j < U.rows(); j++)
              {
                U[j][i] = 0;
              }
          }

        // L ist untere Dreiecksmatrix
        for (i = 0; i < L.cols(); i++)
          {
            for (j = 0; j < i; j++)
              {
                L[j][i] = 0;
              }
            L[i][i] = 1.0;
          }
      }
    RETHROW;
  }

  void LUDecomposition(const matrix<double>& a,
                       matrix<double>& L,
                       matrix<double>& U)
  {
    matrix<double> LU;  // packed LU matrix
    try
      {
        LUDecompositionPacked(a, LU);

        int i, j;
        L = LU;
        U = LU;

        // unpack
        // U ist obere Dreiecksmatrix
        for (i = 0; i < U.cols(); i++)
          {
            for (j = i + 1; j < U.rows(); j++)
              {
                U[j][i] = 0;
              }
          }

        // L ist untere Dreiecksmatrix
        for (i = 0; i < L.cols(); i++)
          {
            for (j = 0; j < i; j++)
              {
                L[j][i] = 0;
              }
            L[i][i] = 1.0;
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "LUSolve"
  Vector LUSolve(const Matrix& LU, const IVector& indx, const Vector& b)
  {
    // lösung eines linearen Gleichungssystems, nachdem die Matrix A
    // in die gepackte LU-Matrix transformiert wurde.
    Vector res(b);

    int dim = LU.cols(); // Dimension merken

    if (dim != LU.rows())   // auf quadratische Matrix testen
      throw IceException(FNAME, M_NO_SQUARE);

    if (indx.Size() != dim)   // Größe permutation korrekt
      throw IceException(FNAME, M_WRONG_DIMENSION);

    if (b.Size() != dim)   // Größe inhomogenität korrekt
      throw IceException(FNAME, M_WRONG_DIMENSION);

    int ii = -1;

    for (int i = 0; i < dim; i++)
      {
        int ip = indx[i];
        double sum = res[ip];
        res[ip] = res[i];

        if (ii >= 0)
          {
            for (int j = ii; j < i; j++)
              {
                sum -= LU(i, j) * res[j];
              }
          }
        else if (sum)
          {
            ii = i;
          }

        res[i] = sum;
      }

    for (int i = dim - 1; i >= 0; i--)
      {
        double sum = res[i];

        for (int j = i + 1; j < dim; j++)
          {
            sum -= LU(i, j) * res[j];
          }

        res[i] = sum / LU(i, i);
      }

    return res;
  }

  std::vector<double> LUSolve(const matrix<double>& LU,
                              const std::vector<int>& indx,
                              const std::vector<double>& b)
  {
    // lösung eines linearen Gleichungssystems, nachdem die Matrix A
    // in die gepackte LU-Matrix transformiert wurde.
    std::vector<double> res(b);

    int dim = LU.cols(); // Dimension merken und auf quadratische Matrix testen

    if (dim != LU.rows())
      throw IceException(FNAME, M_NO_SQUARE);

    if ((int)indx.size() != dim)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    if ((int)b.size() != dim)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    int ii = -1;

    for (int i = 0; i < dim; i++)
      {
        int ip = indx[i];
        double sum = res[ip];
        res[ip] = res[i];

        if (ii >= 0)
          {
            for (int j = ii; j < i; j++)
              {
                sum -= LU[i][j] * res[j];
              }
          }
        else if (sum)
          {
            ii = i;
          }

        res[i] = sum;
      }

    for (int i = dim - 1; i >= 0; i--)
      {
        double sum = res[i];

        for (int j = i + 1; j < dim; j++)
          {
            sum -= LU[i][j] * res[j];
          }

        res[i] = sum / LU[i][i];
      }

    return res;
  }

#undef FNAME
}
