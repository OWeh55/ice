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
#ifndef VWORKER_H
#define VWORKER_H

#include <vector>
#include "matrixtemplate.h"
#include "Point.h"
namespace ice
{
  /** abstract base class for operations on vectors.
   *  Operation has one or two input vectors and one our two results.
   *  Input and output is supported for several data types like
   *  c array, vector or ice::Vector. Operator can be used by
   *  creating operator, set parameter, set input values (setInput) and
   *  retrieve the result (getResult)
   */

  class VWorker
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
    VWorker(): size(0), resultValid(false)
    {}

    /**
     * c'tor with given size of vectors.
     * @param size size of vectors
     */

    VWorker(int size): size(size), resultValid(false)
    {}

    /**
     * copy c'tor deleted
     */
    VWorker(const VWorker& ft) = delete;

    /**
     * d'tor
     */
    virtual ~VWorker()
    {
    }
    ///@}

    /**
     * assignment operator deleted
     */
    const VWorker& operator=(const VWorker& ft) = delete;

    /**
     * change size of vectors
     * input and results become invalid
     */
    virtual void setParameter(int newSize);

    /**
     * @name seting input
     * methods setting the input from c array, vector, ice::Vector
     */
    ///@{
    /**
     * assign c array to first input (index 0)
     * @param v input vector as c array
     * @param n size if c array
     */
    void setInput(const double* v, int n);
    /**
     * assign c array to specific input
     * @param i input number (0/1)
     * @param v input vector as c array
     * @param n size if c array
     */
    void setInput(int i, const double* v, int n);
    /**
     * assign two c arrays to input
     * @param v1 first input vector as c array
     * @param v2 second input vector as c array
     * @param n size if c arrays
     */
    void setInput(const double* v1, const double* v2, int n);

    /**
     * assign vector<double> to first input (index 0).
     * @param v input vector
     */
    void setInput(const std::vector<double>& v);

    /**
     * assign vector<double> to specific input.
     * @param i input number (0/1)
     * @param v input vector
     */
    void setInput(int i, const std::vector<double>& v);

    /**
     * assign two vector<double> to input.
     * @param v1 first input vector
     * @param v2 second input vector
     */
    void setInput(const std::vector<double>& vr,
                  const std::vector<double>& vi);


    /**
     * assign vector<ice::Point> to input.
     * @param v input vector\n
     * x coordinate is used as first vector\n
     * y coordinate is used as second vector
     */
    void setInput(const std::vector<Point>& v);

    // vector<int>
    /**
     * assign vector<int> to first input (index 0).
     * @param v input vector
     * @param factor factor all values are multiplied with
     */
    void setInput(const std::vector<int>& v, double factor = 1.0);
    /**
     * assign vector<int> to specific input.
     * @param i input number (0/1)
     * @param v input vector
     * @param factor factor all values are multiplied with
     */
    void setInput(int i, const std::vector<int>& v, double factor = 1.0);
    /**
     * assign two vector<double> to input.
     * @param v1 first input vector
     * @param v2 second input vector
     * @param factor factor all values are multiplied with
     */
    void setInput(const std::vector<int>& v1,
                  const std::vector<int>& v2, double factor = 1.0);
    ///@}

    /**
     * @name setting input from matrix
     * methods using rows or columns of matrices as input
     */
    ///@{
    // input from row of 2d matrix
    /**
     * assign row of matrix<double> to first input (index 0).
     * @param m input matrix
     * @param row row number
     */
    void setInputFromRow(const ice::matrix<double>& m, int row);
    /**
     * assign row of matrix<double> to specific input.
     * @param i input number (0/1)
     * @param m input matrix
     * @param row row number
     */
    void setInputFromRow(int i, const ice::matrix<double>& m, int row);
    /**
     * assign row of matrix<double> to input.
     * @param m1 input matrix
     * @param m2 input matrix
     * @param row row number
     */
    void setInputFromRow(const ice::matrix<double>& m1,
                         const ice::matrix<double>& m2,
                         int row);

    // input from row of 2d matrix
    /**
     * assign row of matrix<int> to first input (index 0).
     * @param m input matrix
     * @param row row number
     * @param factor factor all values are multiplied with
     */
    void setInputFromRow(const ice::matrix<int>& m,
                         int row, double factor = 1.0);

    /**
     * assign row of matrix<int> to specific input.
     * @param i input number (0/1)
     * @param m input matrix
     * @param row row number
     * @param factor factor all values are multiplied with
     */
    void setInputFromRow(int i, const ice::matrix<int>& m,
                         int row, double factor = 1.0);

    /**
     * assign row of matrix<int> to input.
     * @param m1 input matrix
     * @param m2 input matrix
     * @param row row number
     * @param factor factor all values are multiplied with
     */
    void setInputFromRow(const ice::matrix<int>& m1,
                         const ice::matrix<int>& m2,
                         int row, double factor = 1.0);

    // input from column of 2d matrix
    /**
     * assign column of matrix<double> to first input (index 0).
     * @param m input matrix
     * @param col column number
     */
    void setInputFromColumn(const ice::matrix<double>& m, int col);

    /**
    * assign column of matrix<double> to specific input.
    * @param i input number (0/1)
    * @param m input matrix
    * @param col column number
    */
    void setInputFromColumn(int i, const ice::matrix<double>& m, int col);

    /**
    * assign column of matrix<double> to input.
    * @param m1 input matrix
    * @param m2 input matrix
    * @param column column number
    */
    void setInputFromColumn(const ice::matrix<double>& m1,
                            const ice::matrix<double>& m2, int col);

    /**
    * assign column of matrix<int> to first input (index 0).
    * @param m input matrix
    * @param col column number
    * @param factor factor all values are multiplied with
    */
    void setInputFromColumn(const ice::matrix<int>& m, int col,
                            double factor = 1.0);
    /**
     * assign column of matrix<int> to specific input.
     * @param i input number (0/1)
     * @param m input matrix
     * @param col column number
     * @param factor factor all values are multiplied with
     */
    void setInputFromColumn(int i, const ice::matrix<int>& m, int col,
                            double factor = 1.0);

    /**
     * assign column of matrix<int> to input.
     * @param m1 input matrix
     * @param m2 input matrix
     * @param col column number
     * @param factor factor all values are multiplied with
     */
    void setInputFromColumn(const ice::matrix<int>& m1,
                            const ice::matrix<int>& m2,
                            int col, double factor = 1.0);
    ///@}

    /**
     *  @name retrieve result
     *  methods retrieving the result of the operation.
     *  if parameters and inputs are valid but result
     *  not valid yet, the methods initiate the execution of the operation
     */

    ///@{
    // get result
    /**
     * puts result to c arrays.
     * @param v1 c array #1
     * @param v2 c array #2
     * @param n size of c arrays
     */
    void getResult(double* v1,
                   double* v2, int n) const;

    /**
     * puts first result to c array.
     * second result vector is ignored, no matter if it is valid or not
     * @param v c array
     * @param n size of c arrays
     */
    void getResult(double* v, int n) const;

    /**
     * puts result to vector<double>.
     * @param v1 vector #1
     * @param v2 vector #2
     */
    void getResult(std::vector<double>& v1,
                   std::vector<double>& v2) const;

    /**
     * puts first result to vector<double>.
     * second result vector is ignored, no matter if it is valid or not
     * @param v vector
     */
    void getResult(std::vector<double>& v) const;

    /**
     * puts result to vector<Point>.
     * first result vector becomes x coordinate,
     * second result vector becomes y coordinate of Points
     * @param v c array
     */
    void getResult(std::vector<Point>& v) const;
    ///@}

    /**
     *  @name retrieve result to matrix.
     *  methods retrieving the result of the operation and putting it
     *  to columns or rows of matrix.
     *  if parameters and inputs are valid but result
     *  not valid yet, the methods initiate the execution of the operation
     */

    ///@{
    // put result to row of 2d matrix
    /**
     * puts first result to row of matrix<double>.
     * second result vector is ignored, no matter if it is valid or not
     * @param m matrix
     * @param row row number
     */
    void getResultToRow(ice::matrix<double>& m, int row) const;

    /**
     * puts result to row of two matrix<double>.
     * @param m1 matrix
     * @param m2 matrix
     * @param row row number
     */
    void getResultToRow(ice::matrix<double>& m1,
                        ice::matrix<double>& m2, int row) const;

    // put result into column of 2d matrix
    /**
     * puts first result to column of matrix<double>.
     * second result vector is ignored, no matter if it is valid or not
     * @param m matrix
     * @param col column number
     */
    void getResultToColumn(ice::matrix<double>& m, int col) const;
    /**
     * puts result to column of two matrix<double>.
     * @param m1 matrix
     * @param m2 matrix
     * @param col column number
     */
    void getResultToColumn(ice::matrix<double>& m1,
                           ice::matrix<double>& m2, int col) const;
    ///@}

  protected:
    /**
     * transformation of data.
     * must be redefined in derived classes.\n
     * transforms vectors input[0] and input[1] to vectors result[0] and
     * result[1]. if there is only one result, keep vector[1] empty
     */
    virtual void transform() const = 0;

    /**
     * check index of input.
     * @param i input number
     */
    void checkIndex(unsigned int i) const;
    /**
     * check parameter.
     * if not yet set parameter size is set to given value
     * if already set check if equal given value
     * @param size size of [input] vectors
     */
    void checkParameter(int size);

    // check if transform is done and result valid
    // used in getResult*
    /**
     * checks if result is calculated and call transform if necessary
     */
    void checkDone() const;

    int size = 0; ///< dimension of input vectors

    // operation need 2 inputs
    std::array<std::vector<double>, 2> input; ///< input vectors

    // result may have up to 2 output vectors
    // mutable to allow calculation at the moment of
    // request for result (getResult* const)
    mutable bool resultValid = false; ///< output valid?
    mutable std::array<std::vector<double>, 2> result; ///< output of operator
  };
}
#endif
