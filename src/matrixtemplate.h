#ifndef MATRIX_TEMPLATE_H
#define MATRIX_TEMPLATE_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <initializer_list>

#include "defs.h"
#include "IceException.h"
#include "numbase.h"

namespace ice
{
  /**
   * simple and efficient matrix class.
   * This matrix class is a simple implementation as template
   * with element type as parameter.
   * This class shall be used for all new implementations.
   * @tparam T type of elements
   */
  template<typename T>
  class matrix
  {
  public:
    /**
     * default constructor.
     * After default construction matrix is invalid. Use resize or
     * init to create a real matrix
     */
    matrix() {}

    /**
     * constructor with size and optional initialization.
     * A matrix of the given size is created and possibly initialized.
     * @param r number of rows of the matrix.
     * @param c number of columns of the matrix.
     * @param init mode of initialization.
     * @see init()
     */
    matrix(int r, int c):
      nColumns(c), nRows(r), data(new T[r * c])
    {
    }

    matrix(int r, int c, int init):
      nColumns(c), nRows(r), data(new T[r * c])
    {
      initMatrix(init);
    }

    /**
     * copy constructor.
     */
    matrix(const matrix& m): nColumns(m.nColumns), nRows(m.nRows)
    {
      data = new T[nColumns * nRows];

      for (int i = 0; i < nRows * nColumns; i++)
        {
          data[i] = m.data[i];
        }
    }

    /**
     * type conversion of elements
     */
    template<typename Tsource>
    explicit matrix(const matrix<Tsource>& m): nColumns(m.cols()), nRows(m.rows())
    {
      data = new T[nColumns * nRows];

      for (int i = 0; i < nRows * nColumns; i++)
        {
          data[i] = (T)m[i / nColumns][i % nColumns] ;
        }
    }

    /**
     * move constructor.
     */
    matrix(matrix&& m)
    {
      swap(*this, m);
    }

    /**
     * constructor with sizes and initializer list.
     * Constructs a matrix of the giver size and initializes
     * element with values from the initializer list.
     * @param r number of rows of the matrix.
     * @param c number of columns of the matrix.
     * @param l linear list of values.
     */
    matrix(int r, int c, const std::initializer_list<T>& l):
      nColumns(c), nRows(r), data(new T[r * c])
    {
      int i = 0;
      for (auto p = l.begin(); p != l.end(); ++p)
        {
          data[i++] = *p;
        }
    }

    /**
     * destructor.
     */
    ~matrix()
    {
      delete [] data;
    };

    /**
     * initialization of a given matrix.
     * @param mode initialization mode \n
     *  mode = 0   all values zero.\n
     *  mode = 1   unit matrix.\n
     */

    void initMatrix(int mode)
    {
      if (mode >= 0)
        {
          for (int i = 0; i < nColumns * nRows; i++)
            {
              data[i] = 0.0;
            }
          if (mode == 1) // unity matrix
            for (int i = 0; i < nColumns * nRows; i += nColumns + 1)
              {
                data[i] = 1.0;
              }
        }
    }

    /**
     * resize matrix preserving values if possible.
     * @param r number of rows of the matrix.
     * @param c number of columns of the matrix.
     */

    void resize(int r, int c)
    {
      matrix<T> newmat(r, c); // the new matrix
      if (data != nullptr)   // copy needed ?
        {
          // common range
          int nr = std::min(r, nRows);
          int nc = std::min(c, nColumns);
          for (int r = 0; r < nr; ++r)
            for (int c = 0; c < nc; ++c)
              {
                newmat[r][c] = (*this)[r][c];
              }
        }
      swap(newmat, *this);
    }

    /**
     * resize matrix and possibly initialization.
     * Creates new matrix with given size. Old content is lost!
     * @param r number of rows of the matrix.
     * @param c number of columns of the matrix.
     * @param init mode of initialization.\n
     *  mode = 0   all values zero.\n
     *  mode = 1   unit matrix.\n
     *
      */

    void init(int r, int c, int init = -1)
    {
      delete [] data;
      nColumns = c;
      nRows = r;
      data = new T[r * c];
      initMatrix(init);
    }

    /**
     * Set all elements to a given value.
     * @param value value to fill matrix with.
     */
    void set(T value)
    {
      for (int i = 0; i < nRows * nColumns; i++)
        {
          data[i] = value;
        }
    }

    //*****************************************************

    /**
     * Get row of matrix as T* .
     * This allows element access with indices row and column
     * like m[r][c] = 13;
     */
    T* operator[](int r)
    {
      return &data[r * nColumns];
    }
    /**
     * Get row of matrix as const T* .
     * This allows element access with indices row and column
     * like m[r][c] = 13;
     */
    const T* operator[](int r) const
    {
      return &data[r * nColumns];
    }

    /**
     * swap matrices
     */
    friend void swap(matrix& m1, matrix& m2)
    {
      std::swap(m1.nRows, m2.nRows);
      std::swap(m1.nColumns, m2.nColumns);
      std::swap(m1.data, m2.data);
    }

    /**
     * copy assign operator.
     */
    const matrix<T>& operator =(const matrix& s)
    {
      matrix h(s);
      swap(*this, h);
      return *this;
    }

    /**
     * move assign operator.
     */
    const matrix<T>& operator =(matrix&& s)
    {
      swap(*this, s);
      return *this;
    }

    /**
     * Get pointer to const data.
     */
    const T* getData() const
    {
      return data;
    }

    /**
     * Get pointer to data.
     */
    T* getData()
    {
      return data;
    }

    /**
     * Get number of columns.
     */
    int cols() const
    {
      return nColumns;
    }

    /**
     * Get number of rows.
     */
    int rows() const
    {
      return nRows;
    }

    bool empty() const
    {
      return nColumns == 0 || nRows == 0;
    }

    /**
     * Create matrix from part of *this matrix.
     * @param r1, r2 range of rows
     * @param c1, c2 range of columns
     */
    matrix<T> operator()(int r1, int c1, int r2, int c2)
    {
      int newrows = r2 - r1 + 1;
      int newcols = c2 - c1 + 1;
      matrix<double> tm(newrows, newcols);
      for (int r = 0; r < newrows; ++r)
        for (int c = 0; c < newcols; ++c)
          {
            tm[r][c] = data[(r + r1) * nColumns + (c + c1)];
          }
      return tm;
    }

    /**
     * Get data from row
     * @param rowNr selected row number
     * @param array destination for row data
     */
    template<typename TA>
    void getRow(int rowNr, TA& array) const
    {
      for (int c = 0; c < nColumns; c++)
        array[c] = (*this)[rowNr][c];
    }

    /**
     * Get data from row
     * @param rowNr selected row number
     * @param array destination for row data
     * @param factor values are multiplied with factor
     */
    template<typename TA>
    void getRow(int rowNr, TA& array, double factor) const
    {
      for (int c = 0; c < nColumns; c++)
        array[c] = (*this)[rowNr][c] * factor;
    }

    /**
     * Get data from column
     * @param columnNr selected column number
     * @param array destination for column data
     */

    template<typename TA>
    void getColumn(int columnNr, TA& array) const
    {
      for (int r = 0; r < nRows; r++)
        array[r] = (*this)[r][columnNr];
    }

    /**
     * Get data from column
     * @param columnNr selected column number
     * @param array destination for column data
     * @param factor values are multiplied with factor
     */

    template<typename TA>
    void getColumn(int columnNr, TA& array, double factor) const
    {
      for (int r = 0; r < nRows; r++)
        array[r] = (*this)[r][columnNr] * factor;
    }

    //*************************************************************

    /**
     * Get transposed matrix.
     */
    matrix<T> operator!() const
    {
      matrix<T> res(nColumns, nRows);

      for (int r = 0; r < nRows; ++r)
        for (int c = 0; c < nColumns; ++c)
          {
            res[c][r] = (*this)[r][c];
          }

      return res;
    }

    /**
     * Elementwise sum of matrices.
     */
    friend matrix<T> operator+(const matrix<T>& lhs,
                               const matrix<T>& rhs)
    {
      matrix<T> res(lhs);
      res += rhs;
      return res;
    }

    /**
     * Add matrix to this matrix.
     */
    matrix<T>& operator +=(const matrix<T>& rhs)
    {
      for (int i = 0; i < nColumns * nRows; i++)
        {
          data[i] += rhs.data[i];
        }
      return *this;
    }

    /**
     * Elementwise difference of matrices.
     */
    friend matrix<T> operator-(const matrix<T>& lhs,
                               const matrix<T>& rhs)
    {
      matrix<T> res(lhs);
      res -= rhs;
      return res;
    }

    /**
     * Subtract matrix from this matrix.
     */
    matrix<T>& operator -=(const matrix<T>& rhs)
    {
      for (int i = 0; i < nColumns * nRows; i++)
        {
          data[i] -= rhs.data[i];
        }

      return *this;
    }

    /**
     * Create matrix with negated elements.
     */
    matrix<T> operator-() const
    {
      matrix<T> res(nRows, nColumns);

      for (int i = 0; i < nColumns * nRows; i++)
        {
          res.data[i] = -data[i];
        }

      return res;
    }

    /**
     * product of matrix with scalar.
     */
    matrix<T> operator*(T v) const
    {
      matrix<T> res(nRows, nColumns);

      for (int i = 0; i < nColumns * nRows; i++)
        {
          res.data[i] = data[i] * v;
        }

      return res;
    }

#define FNAME "matrix<T>::operator*"
    /**
     * product of matrices.
     */
    friend matrix<T> operator*(const matrix<T>& lhs,
                               const matrix<T>& rhs)
    {
      matrix<T> res(lhs.nRows, rhs.nColumns);

      if (lhs.nColumns != rhs.nRows)
        throw IceException(FNAME, M_WRONG_DIMENSION);

      for (int i = 0; i < lhs.nRows; i++)
        for (int j = 0; j < rhs.nColumns; j++)
          {
            double sum = 0;

            for (int k = 0; k < lhs.nColumns; k++)
              sum += lhs.data[i * lhs.nColumns + k] *
                     rhs.data[k * rhs.nColumns + j];

            res[i][j] = sum;
          }

      return res;
    }

    /**
     * product of matrix and vector.
     */
    friend std::vector<T> operator*(const std::vector<T>& lhs,
                                    const matrix<T>& rhs)
    {
      std::vector<T> res(rhs.nColumns);

      if ((int)lhs.size() != rhs.nRows)
        throw IceException(FNAME, M_WRONG_DIMENSION);

      for (int j = 0; j < rhs.nColumns; ++j)
        {
          double sum = 0;
          for (int k = 0; k < rhs.nRows; ++k)
            {
              sum += lhs[k] * rhs.data[k * rhs.nColumns + j];
            }

          res[j] = sum;
        }

      return res;
    }

    /**
     * product of matrix and vector.
     */
    friend std::vector<T> operator*(const matrix<T>& lhs,
                                    const std::vector<T>& rhs)
    {
      std::vector<T> res(lhs.nRows);

      if (lhs.nColumns != (int)rhs.size())
        throw IceException(FNAME, M_WRONG_DIMENSION);

      for (int i = 0; i < lhs.nRows; ++i)
        {
          double sum = 0;

          for (int k = 0; k < lhs.nColumns; ++k)
            {
              sum += lhs.data[i * lhs.nColumns + k] * rhs[k];
            }

          res[i] = sum;
        }

      return res;
    }
#undef FNAME

    //*******************************************************************

    template<typename T2>
    matrix<T>& operator *= (T2 d)
    {
      for (int i = 0; i < nColumns * nRows; i++)
        {
          data[i] *= d;
        }

      return *this;
    }

    template<typename T2>
    friend matrix<T> operator*(const matrix<T>& lhs,
                               T2 rhs)
    {
      matrix<T> res(lhs);
      res *= rhs;
      return res;
    }

    template<typename T2>
    friend matrix<T> operator*(T2 lhs,
                               const matrix<T>& rhs)
    {
      return rhs * lhs;
    }

  protected:
    int nColumns = 0;
    int nRows = 0;
    T* data = nullptr;
  };
}
#endif
