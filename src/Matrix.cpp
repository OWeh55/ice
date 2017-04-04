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
#include <assert.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "matrix_function.h"
#include "numbase.h"

#include "Matrix.h"

using namespace std;

#define PRECISION 1e-200
#ifndef _MSC_VER
#define FILE_SEP ','
#else
#define FILE_SEP '#'
#endif

#define ERR(f,m,r,ret) { throw IceException("Matrix::" f,m,r); }
#define ERR0(f,m,r) { throw IceException("Matrix::" f,m,r); }

namespace ice
{
  // Konstruktoren
#define FNAME "Matrix::Matrix"
  Matrix::Matrix(): nRows(0), nColumns(0), data(nullptr)
  {
  }

  Matrix::Matrix(int n, int m, int init): nRows(n), nColumns(m), data(nullptr)
  {
    if (n < 0)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    if (n > 0)
      {
        data = new Vector* [n];
      }

    for (int i = 0; i < n; i++)
      {
        data[i] = new Vector(m);
      }

    if (init == 1)
      {
        // initialize as unity matrix
        for (int i = 0; i < m && i < n; i++)
          {
            (*data[i])[i] = 1.0;
          }
      }
  }

  Matrix::Matrix(int n, int m, double* init): nRows(n), nColumns(m), data(nullptr)
  {
    if (n < 0)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    if (n > 0)
      {
        data = new Vector* [n];
      }

    for (int i = 0; i < n; i++)
      {
        data[i] = new Vector(m);
      }

    double* iptr = init;

    for (int i = 0; i < n; i++)
      for (int k = 0; k < m; k++)
        {
          (*data[i])[k] = *iptr++;
        }

  }

  Matrix::Matrix(const Matrix& m): nRows(m.nRows), nColumns(m.nColumns), data(nullptr)
  {
    if (nRows > 0)
      {
        data = new Vector* [nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(*m.data[i]);
      }
  }

  Matrix::Matrix(const IMatrix& m): nRows(m.rows()), nColumns(m.cols()), data(nullptr)
  {
    if (nRows > 0)
      {
        data = new Vector*[nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(m[i]);
      }
  }

  Matrix::Matrix(const Image& img, int mode, int sign): nRows(0), nColumns(0), data(nullptr)
  {
    if (!IsImg(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return;
      }

    nRows = img->ysize;
    nColumns = img->xsize;

    if (nRows > 0)
      {
        data = new Vector* [nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(nColumns);
      }

    int goff = 0;
    double factor = 1;

    if (sign == SIGNED)
      {
        goff = (img->maxval + 1) / 2;
      }

    if (mode == NORMALIZED)
      {
        factor = 4.0 / (img->maxval + 1);
      }

    for (int i = 0; i < nColumns; i++)
      for (int j = 0; j < nRows; j++)
        {
          (*data[j])[i] = (double)(GetVal(img, i, j) - goff) * factor;
        }
  }

  Matrix::Matrix(ImageD imgd): nRows(0), nColumns(0), data(nullptr)
  {
    if (!IsImgD(imgd))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return;
      }

    nRows = imgd.ysize;
    nColumns = imgd.xsize;

    if (nRows > 0)
      {
        data = new Vector* [nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(nColumns);
      }

    for (int i = 0; i < nColumns; i++)
      for (int j = 0; j < nRows; j++)
        {
          (*data[j])[i] = GetValD(imgd, i, j);
        }
  }

  Matrix::Matrix(const std::vector<Point>& pl): nRows(0), nColumns(0), data(nullptr)
  {
    nRows = pl.size();
    nColumns = 2;

    if (nRows > 0)
      {
        data = new Vector* [nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(nColumns);
      }

    for (int j = 0; j < nRows; j++)
      {
        (*data[j])[0] = pl[j].x;
        (*data[j])[1] = pl[j].y;
      }
  }

  Matrix::Matrix(const std::vector<std::vector<double> >& vv)
  {
    nRows = vv.size();
    nColumns = vv[0].size();

    if (nRows > 0)
      {
        data = new Vector* [nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(nColumns);
      }

    for (int i = 0; i < nRows; i++)
      for (int j = 0; j < nColumns; j++)
        {
          (*data[i])[j] = vv[i][j];
        }
  }

  Matrix::Matrix(const std::vector<Point>& pl,
                 const std::vector<double>& weight): nRows(0), nColumns(0), data(nullptr)
  {
    if (pl.size() != weight.size())
      {
        throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
        return;
      }

    nRows = pl.size();
    nColumns = 3;

    if (nRows > 0)
      {
        data = new Vector* [nRows];
      }

    for (int i = 0; i < nRows; i++)
      {
        data[i] = new Vector(nColumns);
      }

    for (int j = 0; j < nRows; j++)
      {
        (*data[j])[0] = pl[j].x;
        (*data[j])[1] = pl[j].y;
        (*data[j])[2] = weight[j];
      }
  }


#undef FNAME
  Matrix::~Matrix()
  {
    if (data != nullptr)
      {
        for (int i = 0; i < nRows; i++)
          {
            delete data[i];
          }

        delete [] data;
      }
  }

  void Matrix::resize(int r, int c)
  {
    Matrix newmat(r, c);
    int nr = Min(r, nRows);
    int nc = Min(c, nColumns);
    for (int r = 0; r < nr; ++r)
      for (int c = 0; c < nc; ++c)
        {
          newmat[r][c] = (*this)[r][c];
        }
    swap(newmat);
  }

  void Matrix::Set(double val)
  {
    for (int i = 0; i < nRows; i++)
      {
        data[i]->Set(val);
      }
  }

  const Matrix& Matrix::operator=(Matrix m)
  {
    swap(m);
    return *this;
  }

#define FNAME "Matrix::WriteC()"
  int Matrix::WriteC(string name) const
  {
    int i, j;
    Matrix g = *this;
    string fname = name + ".cpp";
    ofstream dest(fname.c_str());

    if (!dest)
      {
        throw IceException(FNAME, M_FILE_OPEN, WRONG_FILE);
        return WRONG_FILE;
      }

    dest << "double " << name << "_data[]" ;
    dest << "={ /* Matrix data */" << endl;

    for (i = 0; i < g.rows(); i++)
      {
        for (j = 0; j < g.cols(); j++)
          {
            dest << g[i][j];

            if (j < g.cols() - 1)
              {
                dest << ',';
              }
          }

        if (i < g.rows() - 1)
          {
            dest << ',';
          }

        dest << endl;
      }

    dest << "};" << endl << endl;

    dest << "Matrixmem " << name << "={" << endl;
    dest << g.rows() << ", /* number of rows */" << endl;
    dest << g.cols() << ", /* number of cols */" << endl;
    dest << name << "_data, /* Matrix data */ };" << endl;

    dest.close();
    return OK;
  }
#undef FNAME

  const Vector& Matrix::operator[](int i) const
  {
    if ((i < 0) || (i >= nRows))
      {
        ERR("operator[]", "Wrong index", WRONG_PARAM, *data[0]);
      }

    return *data[i];
  }

  Vector& Matrix::operator[](int i)
  {
    if ((i < 0) || (i >= nRows))
      {
        ERR("operator[]", "Wrong index", WRONG_PARAM, *data[0]);
      }

    return *data[i];
  }

  Matrix Matrix::operator()(int i1, int j1, int i2, int j2) const
  {
    int i;

    if (
      (i1 < 0) || (i1 > i2 + 1) || (i2 >= nRows) ||
      (j1 < 0) || (j1 > j2 + 1) || (j2 >= nColumns)
    )
      {
        ERR("operator()", "Wrong index", WRONG_PARAM, *this);
      }

    Matrix tm(i2 - i1 + 1, j2 - j1 + 1);

    for (i = i1; i <= i2; i++)
      {
        *(tm.data[i - i1]) = (*data[i])(j1, j2);
      }

    return tm;
  }

  void Matrix::ExchangeRow(int i1, int i2)
  {
    Vector* h;

    if ((i1 < 0) || (i1 >= nRows) || (i2 < 0) || (i2 >= nRows))
      {
        ERR0("ExchangeRow()", "Wrong index", WRONG_PARAM);
      }

    h = data[i1];
    data[i1] = data[i2];
    data[i2] = h;
  }

  void Matrix::ExchangeCol(int i1, int i2)
  {
    double h;
    int j;

    if ((i1 < 0) || (i1 >= nColumns) || (i2 < 0) || (i2 >= nColumns))
      {
        ERR0("ExchangeCol()", "Wrong index", WRONG_PARAM);
      }

    for (j = 0; j < nRows; j++)
      {
        h = (*data[j])[i1];
        (*data[j])[i1] = (*data[j])[i2];
        (*data[j])[i2] = h;
      }
  }

#define FNAME "Matrix::Norm"
  double Matrix::Norm(int mode) const
  {
    try
      {
        double* dptr;
        double* hptr;
        double norm;

        dptr = new double[nRows * nColumns];

        hptr = dptr;

        for (int i = 0; i < nRows; i++)
          for (int j = 0; j < nColumns; j++)
            {
              *(hptr++) = (*data[i])[j];
            }

        try
          {
            NormMatrix(dptr, nRows, nColumns, mode, &norm);
          }
        catch (IceException& ex)
          {
            delete [] dptr;
            throw IceException(ex, "Matrix::Norm");
          }

        delete [] dptr;
        return norm;
      }
    RETHROW;
  }
#undef FNAME

  double Matrix::MaxVal() const
  {
    int i, j;
    double max = (*data[0])[0];

    for (i = 0; i < nRows; i++)
      for (j = 0; j < nColumns; j++)
        if ((*data[i])[j] > max)
          {
            max = (*data[i])[j];
          }

    return max;
  }

  void Matrix::MinCol(Vector& x) const
  {
    x.Resize(nColumns);

    for (size_t j = 0 ; j < (size_t)nColumns; j++)
      {
        double minval = (*data[0])[j];

        for (size_t i = 1 ; i < (size_t)nRows  ; i++)
          if ((*data[i])[j] < minval)
            {
              minval = (*data[i])[j];
            }

        x[j] = minval;
      }
  }

  void Matrix::MinRow(Vector& x) const
  {
    x.Resize(nRows);

    for (size_t i = 0 ; i < (size_t)nRows ; i++)
      {
        double minval = (*data[i])[0];

        for (size_t j = 1 ; j < (size_t)nColumns ; j++)
          if ((*data[i])[j] < minval)
            {
              minval = (*data[i])[j];
            }

        x[i] = minval;
      }
  }

  void Matrix::SumRows(Vector& sum) const
  {
    sum.Resize(nRows);

    for (size_t i = 0 ; i < (size_t)nRows ; i++)
      {
        double rowsum = 0.0;

        for (size_t j = 0 ; j < (size_t)nColumns ; j++)
          {
            rowsum += (*data[i])[j];
          }

        sum[i] = rowsum;
      }
  }

  void Matrix::SumCols(Vector& sum) const
  {
    sum.Resize(nColumns);

    for (size_t j = 0 ; j < (size_t)nColumns ; j++)
      {
        double colsum = 0.0;

        for (size_t i = 0 ; i < (size_t)nRows ; i++)
          {
            colsum += (*data[i])[j];
          }

        sum[j] = colsum;
      }
  }

  void Matrix::NormalizeCols()
  {
    for (size_t j = 0 ; j < (size_t)nColumns ; j++)
      {
        double sum = 0.0;

        for (size_t i = 0 ; i < (size_t)nRows ; i++)
          {
            sum += (*data[i])[j];
          }

        if (fabs(sum) > 1e-20)
          for (size_t i = 0 ; i < (size_t)nRows ; i++)
            {
              (*data[i])[j] /= sum;
            }
      }
  }

  void Matrix::NormalizeRows()
  {
    for (size_t i = 0 ; i < (size_t)nRows ; i++)
      {
        double sum = 0.0;

        for (size_t j = 0 ; j < (size_t)nColumns ; j++)
          {
            sum += (*data[i])[j];
          }

        if (fabs(sum) > 1e-20)
          for (size_t j = 0 ; j < (size_t)nColumns ; j++)
            {
              (*data[i])[j] /= sum;
            }
      }
  }

  Matrix operator+(const Matrix& m1, const Matrix& m2)
  {
    int i;
    Matrix res(m1.rows(), m1.cols());

    if ((m1.rows() != m2.rows()) || (m1.cols() != m2.cols()))
      {
        ERR("operator+", "Format doesn't match", WRONG_PARAM, res);
      }

    for (i = 0; i < m1.rows(); i++)
      {
        *res.data[i] = *m1.data[i] + *m2.data[i];
      }

    return res;
  }

  Matrix operator-(const Matrix& m1, const Matrix& m2)
  {
    int i;
    Matrix res(m1.rows(), m1.cols());

    if ((m1.rows() != m2.rows()) || (m1.cols() != m2.cols()))
      {
        ERR("operator+", "Format doesn't match", WRONG_PARAM, res);
      }

    for (i = 0; i < m1.rows(); i++)
      {
        *res.data[i] = *m1.data[i] - *m2.data[i];
      }

    return res;
  }

  Matrix operator-(const Matrix& m1)
  {
    int i;
    Matrix res(m1.rows(), m1.cols());

    for (i = 0; i < m1.rows(); i++)
      {
        *res.data[i] = -*m1.data[i];
      }

    return res;
  }


  Matrix operator *(double f, const Matrix& m)
  {
    int i;
    Matrix res(m.rows(), m.cols());

    for (i = 0; i < m.rows(); i++)
      {
        *res.data[i] = f** m.data[i];
      }

    return res;
  }

  Matrix operator *(const Matrix& m, double f)
  {
    int i;
    Matrix res(m.rows(), m.cols());

    for (i = 0; i < m.rows(); i++)
      {
        *res.data[i] = f** m.data[i];
      }

    return res;
  }

  Matrix operator *(const Matrix& m1, const Matrix& m2)
  {
    // Matrizen-Multiplikation
    int i, j, k;
    double sum;

    if (m1.cols() != m2.rows())
      {
        ERR("operator*", "Format doesn't match", WRONG_PARAM, m1);
      }

    Matrix res(m1.rows(), m2.cols());

    for (i = 0; i < m1.rows(); i++)
      for (j = 0; j < m2.cols(); j++)
        {
          sum = 0;

          for (k = 0; k < m1.cols(); k++)
            {
              sum += m1.at(i).at(k) * m2.at(k).at(j);
            }

          res[i][j] = sum;
        }

    return res;
  }

  Matrix& Matrix::operator *=(double v)
  {
    for (int i = 0; i < rows(); i++)
      {
        *data[i] *= v;
      }

    return *this;
  }

  Matrix& Matrix::operator /=(double v)
  {
    for (int i = 0; i < rows(); i++)
      {
        *data[i] *= v;
      }

    return *this;
  }

  Matrix& Matrix::operator +=(const Matrix& rhs)
  {
    for (int i = 0; i < rows(); i++)
      {
        *data[i] += *rhs.data[i];
      }

    return *this;
  }

  Matrix& Matrix::operator -=(const Matrix& rhs)
  {
    for (int i = 0; i < rows(); i++)
      {
        *data[i] -= *rhs.data[i];
      }

    return *this;
  }

#define FNAME "Matrix:MulTrans"
  Matrix Matrix::MulTrans(const Matrix& m2) const
  {
    // Matrizen-Multiplikation
    int i, j, k;
    Matrix res(cols(), m2.cols());

    if (rows() != m2.rows())
      {
        ERR(FNAME, M_MATRIXFORMAT, WRONG_PARAM, res);
      }

#if 0
    double sum;

    for (i = 0; i < cols(); i++)
      for (j = 0; j < m2.cols(); j++)
        {
          sum = 0;

          for (k = 0; k < rows(); k++)
            {
              sum += (*data[k]).at(i) * m2.at(k).at(j);
            }

          res[i][j] = sum;
        }

#else

    for (k = 0; k < rows(); k++)
      for (i = 0; i < cols(); i++)
        for (j = 0; j < m2.cols(); j++)
          {
            res[i][j] += (*data[k]).at(i) * m2.at(k).at(j);
          }

#endif
    return res;
  }

  Vector Matrix::MulTrans(const Vector& v) const
  {
    // Multiplikation (transponierte Matrix)*Vector
    int i, j;
    double sum;
    Vector res(cols());

    if (rows() != v.Size())
      {
        ERR(FNAME, M_MATRIXFORMAT, WRONG_PARAM, res);
      }

    for (i = 0; i < cols(); i++)
      {
        sum = 0;

        for (j = 0; j < rows(); j++)
          {
            sum += (*data[j]).at(i) * v[j];
          }

        res[i] = sum;
      }

    return res;
  }

#undef FNAME

  Vector operator *(const Vector& v, const Matrix& m)
  {
    // Multiplikation Matrix^T * Vector
    int i, j;

    double sum;

    if (m.rows() != v.Size())
      {
        ERR("operator*", "Format doesn't match", WRONG_PARAM, v);
      }

    Vector res(m.cols());

    for (i = 0; i < m.cols(); i++)
      {
        sum = 0;

        for (j = 0; j < v.Size(); j++)
          {
            sum += m[j][i] * v[j];
          }

        res[i] = sum;
      }

    return res;
  }

  Vector operator *(const Matrix& m, const Vector& v)
  {
    // Multiplikation Matrix*Vector
    int i, j;

    double sum;

    if (m.cols() != v.Size())
      {
        ERR("operator*", "Format doesn't match", WRONG_PARAM, v);
      }

    Vector res(m.rows());

    for (i = 0; i < m.rows(); i++)
      {
        sum = 0;

        for (j = 0; j < v.Size(); j++)
          {
            sum += m[i][j] * v[j];
          }

        res[i] = sum;
      }

    return res;
  }

  Matrix operator ||(const Matrix& m1, const Matrix& m2)
  {
    // matrizen nebeneinanderstellen und verknuepfen
    int i, j;

    if (m1.rows() != m2.rows())
      {
        ERR("operator||", "Format doesn't match", WRONG_PARAM, m1);
      }

    Matrix res(m1.rows(), m1.cols() + m2.cols());

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

  Matrix operator &&(const Matrix& m1, const Matrix& m2)
  {
    // matrizen untereinander stellen und verknuepfen
    int i, j;

    if (m1.cols() != m2.cols())
      {
        ERR("operator&&", "Format doesn't match", WRONG_PARAM, m1);
      }

    Matrix res(m1.rows() + m2.rows(), m1.cols());

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

  Matrix operator ||(const Matrix& m1, const Vector& v2)
  {
    // Vector an Matrix rechts anhaengen
    int i, j;

    if (m1.rows() != v2.Size())
      {
        ERR("operator||", "Format doesn't match", WRONG_PARAM, m1);
      }

    Matrix res(m1.rows(), m1.cols() + 1);

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

  Matrix operator &&(const Matrix& m1, const Vector& v2)
  {
    // Vector an Matrix unten anhaengen
    int i, j;

    if (m1.cols() != v2.Size())
      {
        ERR("operator&&", "Format doesn't match", WRONG_PARAM, m1);
      }

    Matrix res(m1.rows() + 1, m1.cols());

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

  int Matrix::Append(const Vector& v)
  {
    Vector** ndata;

    if (nColumns != v.Size())
      {
        ERR("Append", "Wrong nRowsension", WRONG_PARAM, WRONG_PARAM);
      }

    ndata = (Vector**)realloc(data, (nRows + 1) * sizeof(Vector*));

    if (ndata == nullptr)
      {
        ERR("Matrix::Append", M_NO_MEM, NO_MEM, NO_MEM);
      }

    data = ndata;
    ndata[nRows] = new Vector(v);
    nRows++;
    return OK;
  }

  Matrix operator ||(const Vector& v1, const Matrix& m2)
  {
    // Vector und Matrix "nebeneinander stellen" und verknuepfen
    int i, j;

    if (v1.Size() != m2.rows())
      {
        ERR("operator||", "Format doesn't match", WRONG_PARAM, m2);
      }

    Matrix res(m2.rows(), m2.cols() + 1);

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

  Matrix operator &&(const Vector& v1, const Matrix& m2)
  {
    // Vector und Matrix untereinander stellen und verkn�pfen
    int i, j;

    if (v1.Size() != m2.cols())
      {
        ERR("operator&&", "Format doesn't match", WRONG_PARAM, m2);
      }

    Matrix res(m2.rows() + 1, m2.cols());

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

  Matrix operator ||(const Vector& v1, const Vector& v2)
  {
    // 2 Vectoren nebeneinanderstellen und verknuepfen
    int i;
    Matrix res(v1.Size(), 2);

    if (v1.Size() != v2.Size())
      {
        ERR("operator||", "Format doesn't match", WRONG_PARAM, res);
      }

    for (i = 0; i < v2.Size(); i++)
      {
        res[i][0] = v1[i];
        res[i][1] = v2[i];
      }

    return res;
  }

  Matrix operator &&(const Vector& v1, const Vector& v2)
  {
    // 2 Vectoren untereinander stellen und verknuepfen
    int i;
    Matrix res(2, v1.Size());

    if (v1.Size() != v2.Size())
      {
        ERR("operator&&", "Format doesn't match", WRONG_PARAM, res);
      }

    for (i = 0; i < v1.Size(); i++)
      {
        res[0][i] = v1[i];
        res[1][i] = v2[i];
      }

    return res;
  }

  Matrix operator !(const Matrix& m)
  {
    // Matrix transponieren
    Matrix res(m.cols(), m.rows());

    for (int i = 0; i < m.rows(); i++)
      for (int j = 0; j < m.cols(); j++)
        {
          res[j][i] = m[i][j];
        }

    return res;
  }

  static int sortcolumn;
  static int compar(const void* p1, const void* p2)
  {
    Vector* vp1 = *(Vector**)p1;
    Vector* vp2 = *(Vector**)p2;
    return Sign((*vp1)[sortcolumn] - (*vp2)[sortcolumn]);
  }
  static int comparinverse(const void* p1, const void* p2)
  {
    Vector* vp1 = *(Vector**)p1;
    Vector* vp2 = *(Vector**)p2;
    return Sign((*vp2)[sortcolumn] - (*vp1)[sortcolumn]);
  }

#define FNAME "Matrix::Sort"
  int Matrix::Sort(int colnr, bool asc)
  {
    if (colnr >= nColumns)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    sortcolumn = colnr;

    if (asc)
      {
        qsort(data, nRows, sizeof(Vector*), compar);
      }
    else
      {
        qsort(data, nRows, sizeof(Vector*), comparinverse);
      }

    return OK;
  }
#undef FNAME

#define FNAME "Matrix::Add"
  int Matrix::Add(const Matrix& m)
  {
    if ((m.rows() != nRows) || (m.cols() != nColumns))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int i = 0 ; i < nRows ; i++)
      for (int j = 0 ; j < nColumns ; j++)
        {
          (*data[i])[j] += (*m.data[i])[j];
        }

    return OK;
  }
#undef FNAME

#define FNAME "Matrix::AddDyadicProd"
  int Matrix::AddDyadicProd(const Vector& v, double factor)
  {
    // *this += v * !v
    if ((v.Size() != nRows) || (v.Size() != nColumns))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int i = 0 ; i < nRows ; i++)
      for (int j = i ; j < nColumns ; j++)
        {
          double prod = v.at(i) * v.at(j) * factor;
          (*data[i])[j] += prod;
          (*data[j])[i] += prod;
        }

    return OK;
  }
#undef FNAME

#define FNAME "Matrix::AddDyadicProd"
  int Matrix::AddDyadicProd(const Vector& v, const Vector& w, double factor)
  {
    // *this += v * !w
    if ((v.Size() != nRows) || (w.Size() != nColumns))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int i = 0 ; i < nRows ; i++)
      for (int j = 0 ; j < nColumns ; j++)
        {
          double prod = v.at(i) * w.at(j) * factor;
          (*data[i])[j] += prod;
        }

    return OK;
  }
#undef FNAME

  ostream& operator<<(ostream& os, const Matrix& m)
  {
    int i;
    os << "<" << endl ;

    for (i = 0; i < m.nRows; i++)
      {
        os << " " << *m.data[i];

        if (i < m.nRows - 1)
          {
            os << FILE_SEP ;
          }

        os << endl ;
      }

    os << ">" << endl ;
    return os;
  }

#define FNAME "operator>>(Matrix)"
  istream& operator>>(istream& is, Matrix& m)
  {
    Vector v;
    char c;
    int first = true;

    is >> c;

    if (c != '<')
      {
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
        is.clear(ios::badbit);
        return is;
      }

    is.ignore(1, '\n'); // ignore first line "<\n"

    if (is.peek() == '>')
      {
        // empty Matrix
        m = Matrix();
        is.ignore(2, '\n');
        return is;
      }

    do
      {
        is >> v >> c ;

        if (first)
          {
            m = Matrix(0, v.Size());
          }

        m.Append(v);
        first = false;

        if ((c != ',') && (c != '#') && (c != '>'))
          {
            throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
            is.clear(ios::badbit);
            return is;
          }
      }
    while (c != '>');

    return is;
  }
#undef FNAME
#define FNAME "Matrix::ReadFromStream"
  void Matrix::ReadFromStream(istream& is)
  {
    Vector v;
    char c;

    is >> c;

    if (c != '<')
      {
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
        is.clear(ios::badbit);
      }

    is.ignore(1, '\n'); // ignore first line "<\n"

    if (is.peek() == '>')
      {
        // empty Matrix
        //m=Matrix();
        is.ignore(2, '\n');
      }

    do
      {
        is >> v >> c ;

        if (v.Size() != cols())
          {
            throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
            is.clear(ios::badbit);
          }

        Append(v);

        if ((c != ',') && (c != '#') && (c != '>'))
          {
            throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
            is.clear(ios::badbit);
          }
      }
    while (c != '>');
  }
#undef FNAME
#define FNAME "Matrix::DeleteCol"
  int Matrix::DeleteCol(int n)
  {
    if ((n < 0) || (n >= nColumns))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    *this = (*this)(0, 0, nRows - 1, n - 1) || (*this)(0, n + 1, nRows - 1, nColumns - 1);
    return OK;
  }

  int Matrix::DeleteCol(int n1, int n2)
  {
    int i;

    if ((n1 < 0) || (n1 >= nColumns) || (n2 < 0) || (n2 >= nColumns))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (n2 < n1)
      {
        std::swap(n1, n2);
      }

    for (i = n2; i >= n1; i--)
      {
        DeleteCol(i);
      }

    return OK;
  }
#undef FNAME
#define FNAME "Matrix::DeleteRow"
  int Matrix::DeleteRow(int n)
  {
    if ((n < 0) || (n >= nRows))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    delete data[n];

    for (int i = n + 1; i < nRows; i++)
      {
        data[i - 1] = data[i];
      }

    Vector** ndata;

    if (nRows > 1)
      {
        ndata = (Vector**)realloc(data, (nRows - 1) * sizeof(Vector*));
      }
    else
      {
        ndata = nullptr;
        free(data);
      }

    nRows = nRows - 1;
    data = ndata;
    return OK;
  }

  int Matrix::DeleteRow(int n1, int n2)
  {
    if ((n1 < 0) || (n1 >= nRows) || (n2 < 0) || (n2 >= nRows))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (n2 < n1)
      {
        std::swap(n1, n2);
      }

    for (int i = n2; i >= n1; i--)
      {
        DeleteRow(i);
      }

    return OK;
  }
#undef FNAME
#define FNAME "SetImg"
  int SetImg(const Image& img, const Matrix& m, int mode, int sign)
  {
    if (!IsImg(img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sx = m.cols();
    int sy = m.rows();

    if ((img->xsize < sx) || (img->ysize < sy))
      {
        throw IceException(FNAME, M_WRONG_WINDOW, WRONG_PARAM);
        return WRONG_PARAM;
      }

    double minv = m[0][0], maxv = m[0][0];

    if (mode == ADAPTIVE)
      {
        for (int y = 0; y < sy; y++)
          for (int x = 0; x < sx; x++)
            {
              if (m[y][x] > maxv)
                {
                  maxv = m[y][x];
                }

              if (m[y][x] < minv)
                {
                  minv = m[y][x];
                }
            }
      }

    double inmaxval;
    double offset = 0.0;
    double factor = 1.0;

    if (sign == SIGNED)
      {
        switch (mode)
          {
          case ADAPTIVE:
            inmaxval = Max(maxv, -minv);
            factor = (img->maxval / 2) / inmaxval;
            offset = (img->maxval + 1) / factor / 2.0;
            break;
          case RAW:
            factor = 1.0;
            offset = (img->maxval + 1) / 2.0;
            break;
          case NORMALIZED:
            factor = (img->maxval + 1) / 4.0;
            offset = 2.0;
            break;
          }
      }
    else
      {
        switch (mode)
          {
          case ADAPTIVE:
            offset = -minv;
            factor = img->maxval / (maxv - minv);
            break;
          case RAW:
            factor = 1.0;
            offset = 0.0;
            break;
          case NORMALIZED:
            factor = (img->maxval + 1) / 4.0;
            offset = 0;
            break;
          }
      }

    int ival;

    for (int y = 0; y < sy; y++)
      for (int x = 0; x < sx; x++)
        {
          ival = RoundInt((m[y][x] + offset) * factor);
          ival = Min(Max(0, ival), img->maxval);
          PutVal(img, x, y, ival);
        }

    return OK;
  }
#undef FNAME
}
