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

#ifndef FOURIERTRAFO2D_H
#define FOURIERTRAFO2D_H

#include <stdexcept>
#include <vector>
#include "fourier.h"
#include "Matrix.h"
#include "base.h"
#include "based.h"

namespace ice
{
  class FourierTrafo2D final
  {
  public:
    FourierTrafo2D(): rows(0), cols(0), forward(true), centered(true), state(sNull) {}
    FourierTrafo2D(int rows, int cols, bool forward = true, bool centered = true)
    {
      setParameter(rows, cols, forward, centered);
    }

    template<typename Tmatrix>
    FourierTrafo2D(Tmatrix mat, bool forward = true, bool centered = true)
    {
      setParameter(mat.rows(), mat.cols(), forward, centered);
      setInput(mat);
    }

    template<typename Tmatrix>
    FourierTrafo2D(Tmatrix matr, Tmatrix mati, bool forward = true, bool centered = true)
    {
      setParameter(matr.rows(), matr.cols(), forward, centered);
      setInput(matr, mati);
    }

    ~FourierTrafo2D()
    {
    }

    void setParameter(int newRows, int newCols, bool newForward = true, bool newCentered = true);

    void setInput(const ice::matrix<double>& v);
    void setInput(const ice::matrix<double>& vr, const ice::matrix<double>& vi);

    void setInput(const ice::matrix<int>& v, double factor = 1.0);
    void setInput(const ice::matrix<int>& vr, const ice::matrix<int>& vi,
                  double factor = 1.0);

    void setInput(const Image& v, double factor = 1.0, int sign = SIGNED);
    void setInput(const Image& vr, const Image& vi,
                  double factor = 1.0, int sign = SIGNED);

    void setInput(const ImageD& v);
    void setInput(const ImageD& vr, const ImageD& vi);

    void setInput(const Matrix& v);
    void setInput(const Matrix& vr, const Matrix& vi);

    void getResult(ice::matrix<double>& dstre,
                   ice::matrix<double>& dstim) throw(std::logic_error)
    {
      transformIfNeeded();
      dstre = real;
      dstim = imag;
    }

    double getResult(ice::matrix<double>& dstre) throw(std::logic_error)
    {
      transformIfNeeded();
      dstre = real;
      double imagvalue = 0;
      for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
          {
            double iv = imag[i][j];
            imagvalue += iv * iv;
          }
      return imagvalue;
    }

    void getResult(const Image& dstre,
                   const Image& dstim,
                   int mode = RAW, int sign = UNSIGNED) throw(std::logic_error)
    {
      ImageD resReal;
      resReal.create(cols, rows);
      ImageD resImag;
      resImag.create(cols, rows);
      getResult(resReal, resImag);
      ConvImgDImg(resReal, dstre, mode, sign);
      ConvImgDImg(resImag, dstim, mode, sign);
    }

    double getResult(const Image& dstre,
                     int mode = RAW, int sign = UNSIGNED) throw(std::logic_error)
    {
      ImageD resReal;
      resReal.create(cols, rows);
      double imagValue = getResult(resReal);
      ConvImgDImg(resReal, dstre, mode, sign);
      return imagValue;
    }

    void getResult(ImageD& dstre,
                   ImageD& dstim) throw(std::logic_error)
    {
      transformIfNeeded();

      for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
          {
            // std::cout << x << " " << y << std::endl;
            dstre.setPixel(x, y, real[y][x]);
            dstim.setPixel(x, y, imag[y][x]);
          }
    }

    double getResult(ImageD& dstre) throw(std::logic_error)
    {
      transformIfNeeded();

      double imagSum = 0;
      for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
          {
            // std::cout << i << " " << j << std::endl;
            dstre.setPixel(j, i, real[i][j]);
            double iv = imag[i][j];
            imagSum += iv * iv;
          }
      return imagSum;
    }

    FourierTrafo2D(const FourierTrafo2D& ft) = delete;
    const FourierTrafo2D& operator=(const FourierTrafo2D& ft) = delete;

  private:
    void transformIfNeeded() throw(std::logic_error);
    void checkParameter(int xs, int ys); // throws in case of error

    static const int sNull = 0;
    static const int sPara = 1;
    static const int sInput = 2;
    static const int sDone = 3;

    int rows, cols;
    bool forward;
    bool centered;

    int state = sNull;

    matrix<double> real;
    matrix<double> imag;
  };

  template<typename Ts1, typename Ts2, typename Td1, typename Td2>
  void Fourier2D(const Ts1& sr, const Ts2& si, Td1& dr, Td2& di, int dir)
  {
    int cols = sr.cols();
    int rows = sr.rows();
    for (int r = 0; r < rows; ++r) // all rows
      {
        std::vector<double> vr(cols);
        std::vector<double> vi(cols);
        for (int c = 0; c < cols; ++c)
          {
            vr[c] = sr[r][c];
            vi[c] = si[r][c];
          }
        Fourier(vr, vi, dir);
        for (int c = 0; c < cols; ++c)
          {
            dr[r][c] = vr[c];
            di[r][c] = vi[c];
          }
      }

    for (int c = 0; c < cols; ++c) // all columns
      {
        std::vector<double> vr(rows);
        std::vector<double> vi(rows);
        for (int r = 0; r < rows; ++r)
          {
            vr[r] = dr[r][c];
            vi[r] = di[r][c];
          }
        Fourier(vr, vi, dir);
        for (int r = 0; r < rows; ++r)
          {
            dr[r][c] = vr[r];
            di[r][c] = vi[r];
          }
      }
  }
}
#endif
