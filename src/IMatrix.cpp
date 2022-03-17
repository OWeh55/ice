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
#include <stdlib.h>
#include <math.h>

#include <algorithm>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "IMatrix.h"

#define PRECISION 1e-14

#define vERR(f,m,r,ret) { throw IceException("IMatrix::" f,m); }
#define ERR0(f,m,r) { throw IceException("IMatrix::" f,m); }

namespace ice
{

#define FNAME "IMatrix::Imatrix"
  IMatrix::IMatrix()
  {
    dim = 0;
    dimv = 0;
    data = nullptr;
  }

  IMatrix::IMatrix(const int n, const int m, int init)
  {
    int i;

    if (n < 0)
      {
        ERR0("IMatrix()", "Wrong dimension", WRONG_PARAM);
      }

    dim = n;
    dimv = m;

    if (n > 0)
      {
        data = (IVector**)malloc(n * sizeof(IVector*));
      }
    else
      {
        data = nullptr;
      }

    for (i = 0; i < n; i++)
      {
        data[i] = new IVector(m);
      }

    if (init == 1)
      {
        //initialize as unity matrix
        for (i = 0; i < n && i < m; i++)
          {
            (*data[i])[i] = 1;
          }
      }
  }

  IMatrix::IMatrix(const IMatrix& m)
  {
    int i;
    dim = m.dim;
    dimv = m.dimv;

    if (dim > 0)
      {
        data = (IVector**)malloc(dim * sizeof(IVector*));
      }
    else
      {
        data = nullptr;
      }

    for (i = 0; i < dim; i++)
      {
        data[i] = new IVector(*m.data[i]);
      }
  }

  IMatrix::IMatrix(Image img)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    dim = img->ysize;
    dimv = img->xsize;

    if (dim > 0)
      {
        data = (IVector**)malloc(dim * sizeof(IVector*));
      }
    else
      {
        data = nullptr;
      }

    for (int i = 0; i < dim; i++)
      {
        data[i] = new IVector(dimv);
      }

    for (int x = 0; x < dimv; x++)
      for (int y = 0; y < dim; y++)
        {
          (*data[y])[x] = GetVal(img, x, y);
        }
  }
#undef FNAME
  IMatrix::~IMatrix()
  {
    int i;

    for (i = 0; i < dim; i++)
      {
        delete data[i];
      }

    if (data != nullptr)
      {
        free(data);
      }
  }

  int IMatrix::Append(const IVector& v)
  {
    IVector** ndata;

    if (dimv != v.Size())
      {
        vERR("Append", "Wrong dimension", WRONG_PARAM, WRONG_PARAM);
      }

    ndata = (IVector**)realloc(data, (dim + 1) * sizeof(IVector*));

    if (ndata == nullptr)
      {
        vERR("IMatrix::Append", M_NO_MEM, NO_MEM, NO_MEM);
      }

    data = ndata;
    ndata[dim] = new IVector(v);
    dim++;
    return OK;
  }

  int IMatrix::set(int val)
  {
    for (int i = 0; i < dim; i++)
      {
        data[i]->set(val);
      }

    return OK;
  }

  IMatrix& IMatrix::operator=(const IMatrix& m)
  {
    int i;

    if (&m == this)
      {
        return *this;
      }

    for (i = 0; i < dim; i++)
      {
        delete data[i];
      }

    if (data != nullptr)
      {
        free(data);
      }

    dim = m.dim;
    dimv = m.dimv;

    if (dim > 0)
      {
        data = (IVector**)malloc(dim * sizeof(IVector*));
      }
    else
      {
        data = nullptr;
      }

    for (i = 0; i < dim; i++)
      {
        data[i] = new IVector(*m.data[i]);
      }

    return *this;
  }

  IVector& IMatrix::operator[](int i) const
  {
    if ((i < 0) || (i >= dim))
      {
        vERR("operator[]", "Wrong index", WRONG_PARAM, *data[0]);
      }

    return *data[i];
  }

  IVector& IMatrix::at(int i) const
  {
    return *data[i];
  }

  IMatrix IMatrix::operator()(int i1, int j1, int i2, int j2) const
  {
    int i;

    if (
      (i1 < 0) || (i1 > i2 + 1) || (i2 >= dim) ||
      (j1 < 0) || (j1 > j2 + 1) || (j2 >= dimv)
    )
      {
        vERR("operator()", "Wrong index", WRONG_PARAM, *this);
      }

    IMatrix tm(i2 - i1 + 1, j2 - j1 + 1);

    for (i = i1; i <= i2; i++)
      {
        *tm.data[i - i1] = (*data[i])(j1, j2);
      }

    return tm;
  }

#if 0

  void IMatrix::Print() const
  {
    int i;

    for (i = 0; i < dim; i++)
      {
        data[i]->Print();
        Printf("\n");
      }
  }
#endif

  int IMatrix::ExchangeRow(int i1, int i2)
  {
    IVector* h;

    if ((i1 < 0) || (i1 >= dim) || (i2 < 0) || (i2 >= dim))
      {
        vERR("ExchangeRow()", "Wrong index", WRONG_PARAM, ERROR);
      }

    h = data[i1];
    data[i1] = data[i2];
    data[i2] = h;
    return OK;
  }

  int IMatrix::ExchangeCol(int i1, int i2)
  {
    int h;
    int j;

    if ((i1 < 0) || (i1 >= dimv) || (i2 < 0) || (i2 >= dimv))
      {
        vERR("ExchangeRow()", "Wrong index", WRONG_PARAM, ERROR);
      }

    for (j = 0; j < dim; j++)
      {
        h = (*data[j])[i1];
        (*data[j])[i1] = (*data[j])[i2];
        (*data[j])[i2] = h;
      }

    return OK;
  }

  int IMatrix::rows() const
  {
    return dim;
  }
  int IMatrix::cols() const
  {
    return dimv;
  }

  IMatrix operator+(const IMatrix& m1, const IMatrix& m2)
  {
    int i;

    if ((m1.rows() != m2.rows()) || (m1.cols() != m2.cols()))
      {
        vERR("operator+", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows(), m1.cols());

    for (i = 0; i < m1.rows(); i++)
      {
        *res.data[i] = *m1.data[i] + *m2.data[i];
      }

    return res;
  }

  IMatrix operator -(const IMatrix& m1, const IMatrix& m2)
  {
    int i;

    if ((m1.rows() != m2.rows()) || (m1.cols() != m2.cols()))
      {
        vERR("operator+", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows(), m1.cols());

    for (i = 0; i < m1.rows(); i++)
      {
        *res.data[i] = *m1.data[i] - *m2.data[i];
      }

    return res;
  }

  IMatrix operator -(const IMatrix& m1)
  {
    int i;
    IMatrix res(m1.rows(), m1.cols());

    for (i = 0; i < m1.rows(); i++)
      {
        *res.data[i] = -*m1.data[i];
      }

    return res;
  }

  IMatrix operator *(const int f, const IMatrix& m)
  {
    int i;
    IMatrix res(m.rows(), m.cols());

    for (i = 0; i < m.rows(); i++)
      {
        *res.data[i] = f** m.data[i];
      }

    return res;
  }

  IMatrix operator *(const IMatrix& m, const int f)
  {
    int i;
    IMatrix res(m.rows(), m.cols());

    for (i = 0; i < m.rows(); i++)
      {
        *res.data[i] = f** m.data[i];
      }

    return res;
  }

  IMatrix operator *(const IMatrix& m1, const IMatrix& m2)
  {
    // Matrizzen-Multiplikation
    int i, j, k;
    int sum;

    if (m1.cols() != m2.rows())
      {
        vERR("operator*", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows(), m2.cols());

    for (i = 0; i < m1.rows(); i++)
      for (j = 0; j < m2.cols(); j++)
        {
          sum = 0;

          for (k = 0; k < m1.cols(); k++)
            {
              sum += m1[i][k] * m2[k][j];
            }

          res[i][j] = sum;
        }

    return res;
  }

  IVector operator *(const IVector& v, const IMatrix& m1)
  {
    // Multiplikation Vector*IMatrix
    return m1 * v;
  }

  IVector operator *(const IMatrix& m1, const IVector& v)
  {
    // Multiplikation IMatrix*Vector
    int i, j;

    int sum;

    if (m1.cols() != v.Size())
      {
        vERR("operator*", "Format doesn't match", WRONG_PARAM, v);
      }

    IVector res(m1.rows());

    for (i = 0; i < m1.rows(); i++)
      {
        sum = 0;

        for (j = 0; j < v.Size(); j++)
          {
            sum += m1[i][j] * v[j];
          }

        res[i] = sum;
      }

    return res;
  }

  IMatrix operator ||(const IMatrix& m1, const IMatrix& m2)
  {
    // matrizzen nebeneinanderstellen und verknuepfen
    int i, j;

    if (m1.rows() != m2.rows())
      {
        vERR("operator||", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows(), m1.cols() + m2.cols());

    for (i = 0; i < m1.rows(); i++)
      {
        for (j = 0; j < m1.cols(); j++)
          {
            res[i][j] = m1[i][j];
          }

        for (j = 0; j < m2.cols(); j++)
          {
            res[i][j + m1.cols()] = m2[i][j];
          }
      }

    return res;
  }

  IMatrix operator &&(const IMatrix& m1, const IMatrix& m2)
  {
    // matrizzen untereinander stellen und verknüpfen
    int i, j;

    if (m1.cols() != m2.cols())
      {
        vERR("operator&&", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows() + m2.rows(), m1.cols());

    for (i = 0; i < m1.cols(); i++)
      {
        for (j = 0; j < m1.rows(); j++)
          {
            res[j][i] = m1[j][i];
          }

        for (j = 0; j < m2.rows(); j++)
          {
            res[j + m1.rows()][i] = m2[j][i];
          }
      }

    return res;
  }

  IMatrix operator ||(const IMatrix& m1, const IVector& v2)
  {
    // Vector an IMatrix rechts anhängen
    int i, j;

    if (m1.rows() != v2.Size())
      {
        vERR("operator||", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows(), m1.cols() + 1);

    for (i = 0; i < m1.rows(); i++)
      {
        for (j = 0; j < m1.cols(); j++)
          {
            res[i][j] = m1[i][j];
          }

        res[i][m1.cols()] = v2[i];
      }

    return res;
  }

  IMatrix operator &&(const IMatrix& m1, const IVector& v2)
  {
    // Vector an IMatrix unten anhängen
    int i, j;

    if (m1.cols() != v2.Size())
      {
        vERR("operator&&", "Format doesn't match", WRONG_PARAM, m1);
      }

    IMatrix res(m1.rows() + 1, m1.cols());

    for (i = 0; i < m1.cols(); i++)
      {
        for (j = 0; j < m1.rows(); j++)
          {
            res[j][i] = m1[j][i];
          }

        res[m1.rows()][i] = v2[i];
      }

    return res;
  }

  IMatrix operator ||(const IVector& v1, const IMatrix& m2)
  {
    // Vector und IMatrix nebeneinanderstellen und verknuepfen
    int i, j;

    if (v1.Size() != m2.rows())
      {
        vERR("operator||", "Format doesn't match", WRONG_PARAM, m2);
      }

    IMatrix res(m2.rows(), m2.cols() + 1);

    for (i = 0; i < m2.rows(); i++)
      {
        res[i][0] = v1[i];

        for (j = 0; j < m2.cols(); j++)
          {
            res[i][j + 1] = m2[i][j];
          }
      }

    return res;
  }

  IMatrix operator &&(const IVector& v1, const IMatrix& m2)
  {
    // Vector und IMatrix untereinander stellen und verknüpfen
    int i, j;

    if (v1.Size() != m2.cols())
      {
        vERR("operator||", "Format doesn't match", WRONG_PARAM, m2);
      }

    IMatrix res(m2.rows() + 1, m2.cols());

    for (i = 0; i < m2.cols(); i++)
      {
        res[0][i] = v1[i];

        for (j = 0; j < m2.rows(); j++)
          {
            res[j + 1][i] = m2[j][i];
          }
      }

    return res;
  }
  IMatrix operator ||(const IVector& v1, const IVector& v2)
  {
    // 2 Vectoren nebeneinanderstellen und verknuepfen
    int i;
    IMatrix res(v1.Size(), 2);

    if (v1.Size() != v2.Size())
      {
        vERR("operator||", "Format doesn't match", WRONG_PARAM, res);
      }

    for (i = 0; i < v2.Size(); i++)
      {
        res[i][0] = v1[i];
        res[i][1] = v2[i];
      }

    return res;
  }

  IMatrix operator &&(const IVector& v1, const IVector& v2)
  {
    // 2 Vectoren untereinander stellen und verknüpfen
    int i;
    IMatrix res(2, v1.Size());

    if (v1.Size() != v2.Size())
      {
        vERR("operator||", "Format doesn't match", WRONG_PARAM, res);
      }

    for (i = 0; i < v1.Size(); i++)
      {
        res[0][i] = v1[i];
        res[1][i] = v2[i];
      }

    return res;
  }

  IMatrix operator !(const IMatrix& m)
  {
    // IMatrix transponieren
    int i, j;
    IMatrix res(m.cols(), m.rows());

    for (i = 0; i < m.rows(); i++)
      for (j = 0; j < m.cols(); j++)
        {
          res[j][i] = m[i][j];
        }

    return res;
  }

  int Determinant(IMatrix m)
  {
    if (m.cols() != m.rows())
      {
        vERR("Determinant()", M_MATRIXFORMAT, WRONG_PARAM, 0);
      }

    if (m.rows() == 1)
      {
        return m[0][0];
      }

    int det = 0;
    int sign = 1;

    for (int i = 0; i < m.rows(); i++)
      {
        det += sign * m[i][0] * Determinant(m(0, 1, i - 1, m.rows() - 1) && m(i + 1, 1, m.rows() - 1, m.rows() - 1));
        sign = -sign;
      }

    return det;
  }

#define FNAME "IMatrix::DeleteRow"
  int IMatrix::DeleteRow(int n)
  {
    int i;

    if ((n < 0) || (n >= dim))
      throw IceException(FNAME, M_WRONG_INDEX);

    delete data[n];

    for (i = n + 1; i < dim; i++)
      {
        data[i - 1] = data[i];
      }

    if (dim > 1)
      {
        data = (IVector**)realloc(data, (dim - 1) * sizeof(IVector*));
      }
    else
      {
        free(data);
        data = nullptr;
      }

    dim = dim - 1;

    return OK;
  }

  int IMatrix::DeleteRow(int n1, int n2)
  {
    int i;

    if ((n1 < 0) || (n1 >= dim) || (n2 < 0) || (n2 >= dim))
      throw IceException(FNAME, M_WRONG_INDEX);

    if (n2 < n1)
      {
        std::swap(n1, n2);
      }

    for (i = n2; i >= n1; i--)
      {
        DeleteRow(i);
      }

    return OK;
  }
#undef FNAME

  int IMatrix::MaxVal() const
  {
    int i, j;
    int max = (*data[0])[0];

    for (i = 0; i < dim; i++)
      for (j = 0; j < dimv; j++)
        if ((*data[0])[0] > max)
          {
            max = (*data[0])[0];
          }

    return max;
  }
}
