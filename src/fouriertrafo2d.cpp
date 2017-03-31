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

#include "fouriertrafo.h"
#include "fouriertrafo2d.h"

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
    state = sPara;
  }

  void FourierTrafo2D::transformIfNeeded() throw(std::logic_error)
  {
    if (state >= sDone)
      {
        return;
      }

    if (state != sInput)
      {
        throw logic_error("no input specified");
      }

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
    state = sDone;
  }

  void FourierTrafo2D::setInput(const ice::matrix<double>& v)
  {
    checkParameter(v.rows(), v.cols());

    real = v;
    imag.set(0);
    state = sInput;
  }

  void FourierTrafo2D::setInput(const ice::matrix<double>& vr,
                                const ice::matrix<double>& vi)
  {
    checkParameter(vr.rows(), vr.cols());

    real = vr;
    imag = vi;
    state = sInput;
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
    state = sInput;
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
    state = sInput;
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
    imag.set(0);
    state = sInput;
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
    state = sInput;
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
    state = sInput;
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
    state = sInput;
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
    state = sInput;
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
    state = sInput;
  }

  void FourierTrafo2D::checkParameter(int nRows, int nCols)
  {
    if (state < sPara)   // not set
      {
        setParameter(nRows, nCols);
      }
    else
      {
        if (cols != nCols || rows != nRows)
          {
            throw logic_error("wrong input dimension");
          }
      }
  }
}
