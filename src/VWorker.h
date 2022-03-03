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
#ifndef VWORKER_H
#define VWORKER_H

#include <vector>
#include "matrixtemplate.h"
#include "Point.h"
namespace ice
{
  // abstract base class for operations on vectors like fourier transform
  // or convolution
  //
  class VWorker
  {
  public:
    VWorker(): size(0), resultValid(false)
    {}

    VWorker(int size): size(size), resultValid(false)
    {}

    virtual ~VWorker()
    {
    }

    VWorker(const VWorker& ft) = delete;
    const VWorker& operator=(const VWorker& ft) = delete;

    virtual void setParameter(int newSize);

    // assign input data in different forms
    // c array
    void setInput(const double* v, int n);
    void setInput(int i, const double* v, int n);
    void setInput(const double* vr, const double* vi, int n);

    // vector<double>
    void setInput(const std::vector<double>& v);
    void setInput(int i, const std::vector<double>& v);
    void setInput(const std::vector<double>& vr,
                  const std::vector<double>& vi);

    // vector<Point>
    void setInput(const std::vector<Point>& v);

    // vector<int>
    void setInput(const std::vector<int>& v, double factor = 1.0);
    void setInput(int i, const std::vector<int>& v, double factor = 1.0);
    void setInput(const std::vector<int>& vr,
                  const std::vector<int>& vi, double factor = 1.0);

    // input from row of 2d matrix
    void setInputFromRow(const ice::matrix<double>& v, int row);
    void setInputFromRow(int i, const ice::matrix<double>& v, int row);
    void setInputFromRow(const ice::matrix<double>& vr,
                         const ice::matrix<double>& vi, int row);

    void setInputFromRow(const ice::matrix<int>& v,
                         int row, double factor = 1.0);
    void setInputFromRow(int i, const ice::matrix<int>& v,
                         int row, double factor = 1.0);
    void setInputFromRow(const ice::matrix<int>& vr,
                         const ice::matrix<int>& vi,
                         int row, double factor = 1.0);

    // input from column of 2d matrix
    void setInputFromColumn(const ice::matrix<double>& v, int col);
    void setInputFromColumn(int i, const ice::matrix<double>& v, int col);
    void setInputFromColumn(const ice::matrix<double>& vr,
                            const ice::matrix<double>& vi, int col);

    void setInputFromColumn(const ice::matrix<int>& v, int col,
                            double factor = 1.0);
    void setInputFromColumn(int i, const ice::matrix<int>& v, int col,
                            double factor = 1.0);
    void setInputFromColumn(const ice::matrix<int>& vr,
                            const ice::matrix<int>& vi,
                            int col, double factor = 1.0);

    // get result
    void getResult(double* v1,
                   double* v2, int n) const;

    void getResult(double* v1, int n) const;

    void getResult(std::vector<double>& v1,
                   std::vector<double>& v2) const;

    void getResult(std::vector<double>& v1) const;

    void getResult(std::vector<Point>& v1) const;

    void getResultFromReal(std::vector<double>& v1,
                           std::vector<double>& v2) const;

    void getResultFromImag(std::vector<double>& v1,
                           std::vector<double>& v2) const;

    // put result into row of 2d matrix
    void getResultToRow(ice::matrix<double>& m, int row) const;
    void getResultToRow(ice::matrix<double>& m1,
                        ice::matrix<double>& m2, int row) const;

    // put result into column of 2d matrix
    void getResultToColumn(ice::matrix<double>& m, int col) const;
    void getResultToColumn(ice::matrix<double>& m1,
                           ice::matrix<double>& m2, int col) const;

  protected:
    virtual void transform() const = 0;

    void checkIndex(unsigned int i) const;
    void checkParameter(int size);

    // check if transform is done and result valid
    // used in getResult*
    void checkDone() const;

    int size = 0;
    // operation need 2 inputs
    std::array<std::vector<double>, 2> input;

    // result may have up to 2 output vectors
    // mutable to allow calculation at the moment of
    // request for result (getResult* const)
    mutable bool resultValid = false;
    mutable std::array<std::vector<double>, 2> result;
  };
}
#endif
