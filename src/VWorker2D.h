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

#ifndef VWORKER2D_H
#define VWORKER2D_H

#include <stdexcept>
#include <array>
#include <vector>
#include "matrixtemplate.h"

#include "IceException.h"

#include "Matrix.h"
#include "base.h"
#include "based.h"

namespace ice
{
  /** abstract base class for operations on twodimensional data like matrix or Image.
   *  Operation has one or two input matrices and one our two result matrices.
   *  Input and output is supported for several data types like
   *  matrix, ice:Image or ice::ImageD. Operator can be used by
   *  creating instance, set parameter, set input values (setInput) and
   *  retrieve the result (getResult)
   */

  class VWorker2D
  {
  public:
    /**
     * @name constructors and destructors
     */
    ///@{
    /**
     * standard c'tor.
     * size will be determined from input
     */
    VWorker2D(): nRows(0), nCols(0) {}

    /**
     * c'tor with given size of data structure.
     * @param rows number of rows of matrix / height of images
     * @param cols number of columns of matrix / width of images
     */
    VWorker2D(int rows, int cols)
    {
      setParameter(rows, cols);
    }

    /*
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
    */

    /**
     * d'tor
     */

    virtual ~VWorker2D()
    {
    }

    /**
     * copy c'tor deleted
     */

    VWorker2D(const VWorker2D& ft) = delete;

    /**
     * assignment operator deleted
     */

    const VWorker2D& operator=(const VWorker2D& ft) = delete;
    ///@}

    /**
     * change size of datastructure
     * input and results become invalid
     * @param newRows number of rows of matrix / height of images
     * @param newCols number of columns of matrix / width of images
     */
    void setParameter(int newRows, int newCols);

    /**
     * @name seting input
     * methods setting the input from twodimensional datastructure
     */

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
    ///@}

    /**
     *  @name retrieve result
     *  methods retrieving the result of the operation.
     *  if parameters and inputs are valid but result
     *  not valid yet, the methods initiate the execution of the operation
     */


    void getResult(ice::matrix<double>& dstre,
                   ice::matrix<double>& dstim) const;

    void getResult(ice::matrix<double>& dstre) const;

    void getResult(const Image& dstre,
                   const Image& dstim,
                   int mode = RAW, int sign = SIGNED) const;

    void getResult(const Image& dstre,
                   int mode = RAW, int sign = SIGNED) const;

    void getResult(ImageD& dstre,
                   ImageD& dstim) const;

    void getResult(ImageD& dstre) const;

    ///@}

  protected:
    virtual void transform() const = 0;

    void checkIndex(unsigned int i) const;
    void checkParameter(int nRowsP, int nColsP);

    void checkDone() const;

    int nRows = 0;
    int nCols = 0;

    std::array<ice::matrix<double>, 2> input;

    mutable bool resultValid = false;
    mutable std::array<ice::matrix<double>, 2> result;
  };
}
#endif
