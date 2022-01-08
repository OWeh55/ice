/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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

#include <stdexcept>

#include "FourierTrafo.h"
#include "FourierTrafo2D.h"

using namespace std;

#define FNAME "FourierTrafo2D"
namespace ice
{
  void FourierTrafo2D::setParameter(int newRows, int newCols,
                                    bool newForward, bool newCentered)
  {
    rows = newRows;
    cols = newCols;

    real.resize(rows, cols);
    imag.resize(rows, cols);

    forward = newForward;
    centered = newCentered;
    setStatePara();
  }

  void  FourierTrafo2D::setInput(const ice::matrix<double>& v)
  {
    checkParameter(v.rows(), v.cols());

    real = v;
    imag.set(0);
    setStateData();
  }

  void FourierTrafo2D::setInput(const ice::matrix<double>& vr,
                                const ice::matrix<double>& vi)
  {
    checkParameter(vr.rows(), vr.cols());

    real = vr;
    imag = vi;
    setStateData();
  }

  void FourierTrafo2D::setInput(const ice::matrix<int>& v, double factor)
  {
    checkParameter(v.rows(), v.cols());

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = v[y][x] * factor;
        }

    imag.set(0);
    setStateData();
  }

  void FourierTrafo2D::setInput(const ice::matrix<int>& vr,
                                const ice::matrix<int>& vi, double factor)
  {
    checkParameter(vr.rows(), vr.cols());

    for (int y = 0; y < rows; ++y)
      {
        for (int x = 0; x < cols; ++x)
          {
            real[y][x] = vr[y][x] * factor;
            imag[y][x] = vi[y][x] * factor;
          }
      }
    setStateData();
  }

  void FourierTrafo2D::setInput(const Image& v, double factor, int sign)
  {
    checkParameter(v.ysize, v.xsize);

    int v0 = (sign == SIGNED) ? (v.maxval + 1) / 2 : 0;
    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = (v.getPixel(x, y) - v0) * factor;
        }
    imag.set(0.0);
    setStateData();
  }

  void FourierTrafo2D::setInput(const Image& vr, const Image& vi,
                                double factor, int sign)
  {
    checkParameter(vr.ysize, vr.xsize);

    int vr0 = (sign == SIGNED) ? (vr.maxval + 1) / 2 : 0;
    int vi0 = (sign == SIGNED) ? (vi.maxval + 1) / 2 : 0;
    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = (vr.getPixel(x, y) - vr0) * factor;
          imag[y][x] = (vi.getPixel(x, y) - vi0) * factor;
        }
    setStateData();
  }

  void FourierTrafo2D::setInput(const ImageD& v)
  {
    checkParameter(v.ysize, v.xsize);

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = v.getPixel(x, y);
        }

    imag.set(0);
    setStateData();
  }

  void FourierTrafo2D::setInput(const ImageD& vr, const ImageD& vi)
  {
    checkParameter(vr.ysize, vr.xsize);

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = vr.getPixel(x, y);
          imag[y][x] = vi.getPixel(x, y);
        }
    setStateData();
  }

  void FourierTrafo2D::setInput(const Matrix& v)
  {
    checkParameter(v.rows(), v.cols());

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = v[y][x];
        }

    imag.set(0);
    setStateData();
  }

  void FourierTrafo2D::setInput(const Matrix& vr, const Matrix& vi)
  {
    checkParameter(vr.rows(), vr.cols());

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          real[y][x] = vr[y][x];
          imag[y][x] = vi[y][x];
        }
    setStateData();
  }

  void FourierTrafo2D::checkParameter(int nRows, int nCols)
  {
    if ((mState & sPara) == 0)  // not set
      setParameter(nRows, nCols);
    else
      {
        if (cols != nCols || rows != nRows)
          {
            throw logic_error("wrong input dimension");
          }
      }
  }

  void FourierTrafo2D::transform() const
  {
    FourierTrafo trRow(cols, forward, centered);
    for (int y = 0; y < rows; ++y)
      {
        trRow.setInputFromRow(real, imag, y);
        trRow.getResultToRow(real, imag, y);
      }

    FourierTrafo trCol(rows, forward, centered);
    for (int x = 0; x < cols; ++x)
      {
        trCol.setInputFromColumn(real, imag, x);
        trCol.getResultToColumn(real, imag, x);
      }
    setStateDone();
  }

  void FourierTrafo2D::setStateDone() const
  {
    mState = sDone | sPara | sData;
  }

  void FourierTrafo2D::setStatePara() const
  {
    mState = (mState | sPara) & ~sDone;
  }

  void FourierTrafo2D::setStateData() const
  {
    mState = (mState | sData) & ~sDone;
  }

  void FourierTrafo2D::checkDone() const
  {
    if ((mState & sData) == 0)
      throw IceException(FNAME, "data incomplete");
    if ((mState & sPara) == 0)
      throw IceException(FNAME, "parameter not set");
    if ((mState & sDone) == 0)
      transform();
  }

  void FourierTrafo2D::getResult(ice::matrix<double>& dstre,
                                 ice::matrix<double>& dstim)
  {
    checkDone();
    dstre = real;
    dstim = imag;
  }

  double FourierTrafo2D::getResult(ice::matrix<double>& dstre)
  {
    checkDone();
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

  void FourierTrafo2D::getResult(const Image& dstre,
                                 const Image& dstim,
                                 int mode, int sign)
  {
    ImageD resReal;
    resReal.create(cols, rows);
    ImageD resImag;
    resImag.create(cols, rows);
    getResult(resReal, resImag);
    ConvImgDImg(resReal, dstre, mode, sign);
    ConvImgDImg(resImag, dstim, mode, sign);
  }

  double FourierTrafo2D::getResult(const Image& dstre,
                                   int mode, int sign)
  {
    ImageD resReal;
    resReal.create(cols, rows);
    double imagValue = getResult(resReal);
    ConvImgDImg(resReal, dstre, mode, sign);
    return imagValue;
  }

  void FourierTrafo2D::getResult(ImageD& dstre,
                                 ImageD& dstim)
  {
    checkDone();

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          // std::cout << x << " " << y << std::endl;
          dstre.setPixel(x, y, real[y][x]);
          dstim.setPixel(x, y, imag[y][x]);
        }
  }

  double FourierTrafo2D::getResult(ImageD& dstre)
  {
    checkDone();

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
}
