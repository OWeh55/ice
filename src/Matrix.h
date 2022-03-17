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
#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <vector>
#include "matrixtemplate.h"
#include "Vector.h"
#include "IMatrix.h"
#include "based.h"

namespace ice
{
  // class Matrix
  class Matrix
  {
  public:
    Matrix();
    Matrix(int, int, int init = 0);

    Matrix(const Matrix&);  // Kopier-Konstruktor
    Matrix(const IMatrix&);
    explicit Matrix(const Image& img, int mode = RAW, int sign = UNSIGNED);
    explicit Matrix(ImageD imgd);
    explicit Matrix(int rows, int cols, double* data);
    explicit Matrix(const std::vector<Point>& pl);
    explicit Matrix(const std::vector<std::vector<double> >& vv);
    explicit Matrix(const ice::matrix<double>& vv);
    Matrix(const std::vector<Point>& pl, const std::vector<double>& weight);
    ~Matrix();

    void resize(int rows, int cols);

    // operator matrix<double> () const;
    matrix<double> toMatrix() const;

    /* Element-Operator */
    const Vector& operator[](int) const;
    Vector& operator[](int);

    /* Element-Operator (ohne Index-Test) */
    const Vector& at(int i) const
    {
      return *data[i];
    }
    Vector& at(int i)
    {
      return *data[i];
    }

    double MaxVal() const;
    void MinCol(Vector&) const;
    void MinRow(Vector&) const;

    int  WriteC(std::string fn) const;

    void set(double val = 0.0);

    void ExchangeRow(int, int);
    void ExchangeCol(int, int);

    const Matrix& operator=(Matrix m); /* Zuweisungs-Operator */

    Matrix& operator *= (double v);
    Matrix& operator /=(double v);
    Matrix& operator +=(const Matrix& rhs);
    Matrix& operator -=(const Matrix& rhs);

    friend Matrix operator+(const Matrix&, const Matrix&);

    friend Matrix operator-(const Matrix&, const Matrix&);
    friend Matrix operator-(const Matrix&);

    friend Matrix operator*(const Matrix&, double);
    friend Matrix operator*(double, const Matrix&);

    friend Matrix operator !(const Matrix&);

    Matrix operator()(int, int, int, int) const; /* Teil-Matrix */

    friend std::ostream& operator<<(std::ostream&, const Matrix&);
    friend std::istream& operator>>(std::istream&, Matrix&);

    void ReadFromStream(std::istream&);

    inline int rows() const
    {
      return nRows;
    }
    inline int cols() const
    {
      return nColumns;
    }

    double Norm(int mode = 1) const;

    int DeleteRow(int n);
    int DeleteRow(int n1, int n2);
    int DeleteCol(int n);
    int DeleteCol(int n1, int n2);

    int append(const Vector& v);

    int Sort(int col = 0, bool asc = true);

    int AddDyadicProd(const Vector& v, double factor = 1.0);
    int AddDyadicProd(const Vector& v, const Vector& w, double factor = 1.0);
    int Add(const Matrix&);

    Matrix MulTrans(const Matrix&) const;
    Vector MulTrans(const Vector&) const;

    void NormalizeRows();
    void NormalizeCols();

    void SumRows(Vector& sum) const;
    void SumCols(Vector& sum) const;

    void swap(Matrix& second)
    {
      using std::swap;
      swap(nRows, second.nRows);
      swap(nColumns, second.nColumns);
      swap(data, second.data);
    }

    friend void swap(Matrix& first, Matrix& second)
    {
      using std::swap;
      swap(first.nRows, second.nRows);
      swap(first.nColumns, second.nColumns);
      swap(first.data, second.data);
    }

  protected:

    int nRows;
    int nColumns;
    Vector** data;
  };

  Matrix operator+(const Matrix&, const Matrix&);
  Matrix operator-(const Matrix&, const Matrix&);
  Matrix operator*(const Matrix&, double);
  Matrix operator*(double, const Matrix&);
  Matrix operator*(const Matrix&, const Matrix&);
  Vector operator*(const Matrix&, const Vector&);
  Vector operator*(const Vector&, const Matrix&);

  Matrix operator||(const Matrix&, const Matrix&);
  Matrix operator&&(const Matrix&, const Matrix&);
  Matrix operator||(const Vector&, const Matrix&);
  Matrix operator&&(const Vector&, const Matrix&);
  Matrix operator||(const Matrix&, const Vector&);
  Matrix operator&&(const Matrix&, const Vector&);
  Matrix operator||(const Vector&, const Vector&);
  Matrix operator&&(const Vector&, const Vector&);

  Matrix operator !(const Matrix&);

  void setImg(const Image& img, const Matrix& m, int mode = ADAPTIVE, int sign = UNSIGNED);

} // namespace ice
#endif
