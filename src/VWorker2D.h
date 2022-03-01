/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011-2017 FSU Jena, Digital Image Processing Group
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

#ifndef VWORKER2D_H
#define VWORKER2D_H

#include <stdexcept>
#include <vector>
#include "matrixtemplate.h"

#include "IceException.h"

#include "Matrix.h"
#include "base.h"
#include "based.h"

namespace ice
{
  class VWorker2D
  {
  public:
    VWorker2D(): rows(0), cols(0) {}

    VWorker2D(int rows, int cols)
    {
      setParameter(rows, cols);
    }

    template<typename Tmatrix>
    VWorker2D(Tmatrix mat, bool forward = true, bool centered = true)
    {
      setParameter(mat.rows(), mat.cols(), forward, centered);
      setInput(mat);
    }

    template<typename Tmatrix>
    VWorker2D(Tmatrix matr, Tmatrix mati, bool forward = true, bool centered = true)
    {
      setParameter(matr.rows(), matr.cols(), forward, centered);
      setInput(matr, mati);
    }

    ~VWorker2D()
    {
    }

    void setParameter(int newRows, int newCols);

    void setInput(const ice::matrix<double>& v);
    void setInput(int i, const ice::matrix<double>& v);
    void setInput(const ice::matrix<double>& vr, const ice::matrix<double>& vi);

    void setInput(const ice::matrix<int>& v, double factor = 1.0);
    void setInput(int i, const ice::matrix<int>& v, double factor = 1.0);
    void setInput(const ice::matrix<int>& vr, const ice::matrix<int>& vi,
                  double factor = 1.0);

    void setInput(const Image& v, double factor = 1.0, int sign = SIGNED);
    void setInput(int i, const Image& v, double factor = 1.0, int sign = SIGNED);
    void setInput(const Image& vr, const Image& vi, double factor = 1.0, int sign = SIGNED);

    void setInput(const ImageD& v);
    void setInput(int i, const ImageD& v);
    void setInput(const ImageD& vr, const ImageD& vi);

    void setInput(const Matrix& v);
    void setInput(int i, const Matrix& v);
    void setInput(const Matrix& vr, const Matrix& vi);

    void getResult(ice::matrix<double>& dstre,
                   ice::matrix<double>& dstim);

    void getResult(ice::matrix<double>& dstre);

    void getResult(const Image& dstre,
                   const Image& dstim,
                   int mode = RAW, int sign = SIGNED);

    void getResult(const Image& dstre,
                   int mode = RAW, int sign = SIGNED);

    void getResult(ImageD& dstre,
                   ImageD& dstim);

    void getResult(ImageD& dstre);

    VWorker2D(const VWorker2D& ft) = delete;
    const VWorker2D& operator=(const VWorker2D& ft) = delete;

  protected:
    virtual void transform() const = 0;

    void checkIndex(unsigned int i) const;
    void checkParameter(int nRows, int nCols);

    void checkDone() const;

    int rows = 0;
    int cols = 0;

    std::array<ice::matrix<double>, 2> input;

    mutable bool resultValid = false;
    mutable std::array<ice::matrix<double>, 2> result;
  };
}
#endif
