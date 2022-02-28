/*
 * ICE - C++ - Library for image processing
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

using namespace std;
#include "VWorker.h"

namespace ice
{
#define CNAME "VWorker"

  void VWorker::checkIndex(unsigned int i) const
  {
    if (i > 1)
      throw IceException(CNAME, "input index out of range");
  }

  void VWorker::setParameter(int newSize)
  {
    size = newSize;
    resultValid = false;
  }

  void VWorker::setInput(const double* v, int n)
  {
    setInput(0, v, n);
  }

  void VWorker::setInput(int i, const double* v, int n)
  {
    checkIndex(i);
    checkParameter(n);
    input[i].resize(size);
    for (int k = 0; k < size; k++)
      input[i][k] = v[k];
    resultValid = false;
  }

  void VWorker::setInput(const double* vr, const double* vi, int n)
  {
    setInput(0, vr, n);
    setInput(1, vi, n);
  }

  void VWorker::setInput(const std::vector<double>& v)
  {
    setInput(0, v);
  }

  void VWorker::setInput(int i, const std::vector<double>& v)
  {
    checkIndex(i);
    checkParameter(v.size());
    input[i].resize(size);
    for (int k = 0; k < size; k++)
      input[i][k] = v[k];
    resultValid = false;
  }

  void VWorker::setInput(const std::vector<double>& vr,
                         const std::vector<double>& vi)
  {
    setInput(0, vr);
    setInput(1, vi);
  }

  void VWorker::setInput(const std::vector<Point>& v)
  {
    checkParameter(v.size());
    input[0].resize(size);
    input[1].resize(size);
    for (int k = 0; k < size; k++)
      {
        input[0][k] = v[k].x;
        input[0][k] = v[k].y;
      }
    resultValid = false;
  }

  void VWorker::setInput(const std::vector<int>& v, double factor)
  {
    setInput(0, v, factor);
  }

  void VWorker::setInput(int i, const std::vector<int>& v, double factor)
  {
    checkIndex(i);
    checkParameter(v.size());
    input[i].resize(size);
    for (int k = 0; k < size; k++)
      input[i][k] = v[k];
    resultValid = false;
  }

  void VWorker::setInput(const std::vector<int>& vr,
                         const std::vector<int>& vi, double factor)
  {
    setInput(0, vr, factor);
    setInput(1, vi, factor);
  }

  void VWorker::setInputFromRow(const ice::matrix<double>& m, int row)
  {
    setInputFromRow(0, m, row);
  }

  void VWorker::setInputFromRow(int i, const ice::matrix<double>& m, int row)
  {
    checkIndex(i);
    checkParameter(m.cols());
    input[i].resize(size);
    m.getRow(row, input[i]);
    resultValid = false;
  }

  void VWorker::setInputFromRow(const ice::matrix<double>& m1,
                                const ice::matrix<double>& m2, int row)
  {
    setInputFromRow(0, m1, row);
    setInputFromRow(1, m2, row);
  }


  void VWorker::setInputFromRow(const ice::matrix<int>& m, int row, double factor)
  {
    setInputFromRow(0, m, row, factor);
  }

  void VWorker::setInputFromRow(int i, const ice::matrix<int>& m, int row, double factor)
  {
    checkIndex(i);
    checkParameter(m.cols());
    input[i].resize(size);
    m.getRow(row, input[i], factor);
    resultValid = false;
  }

  void VWorker::setInputFromRow(const ice::matrix<int>& m1,
                                const ice::matrix<int>& m2, int row, double factor)
  {
    setInputFromRow(0, m1, row, factor);
    setInputFromRow(1, m2, row, factor);
  }


  void VWorker::setInputFromColumn(const ice::matrix<double>& m, int col)
  {
    setInputFromColumn(0, m, col);
  }

  void VWorker::setInputFromColumn(int i, const ice::matrix<double>& m, int col)
  {
    checkIndex(i);
    checkParameter(m.rows());
    input[i].resize(size);
    m.getColumn(col, input[i]);
    resultValid = false;
  }

  void VWorker::setInputFromColumn(const ice::matrix<double>& m1,
                                   const ice::matrix<double>& m2, int col)
  {
    setInputFromColumn(0, m1, col);
    setInputFromColumn(1, m2, col);
  }


  void VWorker::setInputFromColumn(const ice::matrix<int>& m, int col, double factor)
  {
    setInputFromColumn(0, m, col, factor);
  }

  void VWorker::setInputFromColumn(int i, const ice::matrix<int>& m, int col, double factor)
  {
    checkIndex(i);
    checkParameter(m.rows());
    input[i].resize(size);
    m.getColumn(col, input[i], factor);
    resultValid = false;
  }

  void VWorker::setInputFromColumn(const ice::matrix<int>& m1,
                                   const ice::matrix<int>& m2, int col, double factor)
  {
    setInputFromColumn(0, m1, col, factor);
    setInputFromColumn(1, m2, col, factor);
  }

  void VWorker::checkParameter(int sourceSize)
  {
    if (size == 0) // not set
      {
        setParameter(sourceSize);
      }
    else
      {
        if (size != sourceSize)
          throw IceException(CNAME, M_VECTORDIM);
      }
  }

  void VWorker::checkDone() const
  {
    if (!resultValid)
      {
        if (size == 0)
          throw IceException(CNAME, "parameter not set");
        if (input[0].empty())
          throw IceException(CNAME, "input not set");
        transform();
        resultValid = true;
      }
  }

  void VWorker::getResult(double* v1,
                          double* v2, int n) const
  {
    checkDone();

    if (result[1].empty())
      throw IceException(CNAME, "result only one dimensional");

    if (size != n)
      throw IceException(CNAME, M_VECTORDIM);

    for (int k = 0; k < size; k++)
      {
        v1[k] = result[0][k];
        v2[k] = result[1][k];
      }
  }

  void VWorker::getResult(double* v, int n) const
  {
    checkDone();

    if (size != n)
      throw IceException(CNAME, M_VECTORDIM);

    for (int k = 0; k < size; k++)
      {
        v[k] = result[0][k];
      }
  }

  void VWorker::getResult(std::vector<double>& v1,
                          std::vector<double>& v2) const
  {
    checkDone();

    if (result[1].empty())
      throw IceException(CNAME, "result only one dimensional");

    v1.resize(size);
    v2.resize(size);

    for (int k = 0; k < size; k++)
      {
        v1[k] = result[0][k];
        v2[k] = result[1][k];
      }
  }

  void VWorker::getResult(std::vector<double>& v) const
  {
    checkDone();

    v.resize(size);

    for (int k = 0; k < size; k++)
      {
        v[k] = result[0][k];
      }
  }

  void VWorker::getResult(std::vector<Point>& vp) const
  {
    checkDone();

    if (result[1].empty())
      throw IceException(CNAME, "result only one dimensional");

    vp.resize(size);

    for (int k = 0; k < size; k++)
      {
        vp[k] = Point(result[0][k], result[1][k]);
      }
  }

  // put Result into row of 2d matrix
  void VWorker::getResultToRow(ice::matrix<double>& v, int row) const
  {
    checkDone();

    vector<double> res;
    getResult(res);
    for (int i = 0; i < size; ++i)
      {
        v[row][i] = res[i];
      }
  }

  void VWorker::getResultToRow(ice::matrix<double>& vr,
                               ice::matrix<double>& vi, int row) const
  {
    checkDone();

    vector<double> resr;
    vector<double> resi;
    getResult(resr, resi);
    for (int i = 0; i < size; ++i)
      {
        vr[row][i] = resr[i];
        vi[row][i] = resi[i];
      }
  }

  // put Result into column of 2d matrix
  void VWorker::getResultToColumn(ice::matrix<double>& v, int col) const
  {
    checkDone();

    vector<double> res;
    getResult(res);
    for (int i = 0; i < size; ++i)
      {
        v[i][col] = res[i];
      }
  }

  void VWorker::getResultToColumn(ice::matrix<double>& vr,
                                  ice::matrix<double>& vi, int col) const
  {
    checkDone();

    vector<double> resr;
    vector<double> resi;
    getResult(resr, resi);
    for (int i = 0; i < size; ++i)
      {
        vr[i][col] = resr[i];
        vi[i][col] = resi[i];
      }
  }

}
