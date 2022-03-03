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

#include "VWorker2D.h"

using namespace std;

#define CNAME "VWorker2D"
namespace ice
{
  void VWorker2D::setParameter(int newRows, int newCols)
  {
    rows = newRows;
    cols = newCols;
    resultValid = false;
  }

  void VWorker2D::checkIndex(unsigned int i) const
  {
    if (i > 1)
      throw IceException(CNAME, "input index out of range");
  }


  void  VWorker2D::setInput(const ice::matrix<double>& v)
  {
    setInput(0, v);
  }

  void  VWorker2D::setInput(int i, const ice::matrix<double>& v)
  {
    checkIndex(i);
    checkParameter(v.rows(), v.cols());
    input[i] = v;
    resultValid = false;
  }

  void VWorker2D::setInput(const ice::matrix<double>& vr,
                           const ice::matrix<double>& vi)
  {
    checkParameter(vr.rows(), vr.cols());

    input[0] = vr;
    input[1] = vi;
    resultValid = false;
  }

  void VWorker2D::setInput(const ice::matrix<int>& v, double factor)
  {
    setInput(v, factor);
  }

  void VWorker2D::setInput(int i, const ice::matrix<int>& v, double factor)
  {
    checkIndex(i);
    checkParameter(v.rows(), v.cols());

    input[i].resize(rows, cols);

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          input[i][y][x] = v[y][x] * factor;
        }
    resultValid = false;
  }

  void VWorker2D::setInput(const ice::matrix<int>& vr,
                           const ice::matrix<int>& vi, double factor)
  {
    setInput(0, vr, factor);
    setInput(1, vi, factor);
  }

  void VWorker2D::setInput(const Image& v, double factor, int sign)
  {
    setInput(0, v, factor, sign);
  }

  void VWorker2D::setInput(int i, const Image& v, double factor, int sign)
  {
    checkIndex(i);
    checkParameter(v.ysize, v.xsize);
    input[i].resize(rows, cols);
    int v0 = (sign == SIGNED) ? (v.maxval + 1) / 2 : 0;
    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          input[i][y][x] = (v.getPixel(x, y) - v0) * factor;
        }
    resultValid = false;
  }

  void VWorker2D::setInput(const Image& vr, const Image& vi,
                           double factor, int sign)
  {
    setInput(0, vr, factor, sign);
    setInput(1, vi, factor, sign);
  }

  void VWorker2D::setInput(const ImageD& v)
  {
    setInput(0, v);
  }

  void VWorker2D::setInput(int i, const ImageD& v)
  {
    checkIndex(i);
    checkParameter(v.ysize, v.xsize);
    input[i].resize(rows, cols);
    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          input[i][y][x] = v.getPixel(x, y);
        }
    resultValid = false;
  }

  void VWorker2D::setInput(const ImageD& vr, const ImageD& vi)
  {
    setInput(0, vr);
    setInput(1, vi);
  }

  void VWorker2D::setInput(const Matrix& v)
  {
    setInput(0, v);
  }

  void VWorker2D::setInput(int i, const Matrix& v)
  {
    checkIndex(i);
    checkParameter(v.rows(), v.cols());

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          input[i][y][x] = v[y][x];
        }
    resultValid = false;
  }

  void VWorker2D::setInput(const Matrix& vr, const Matrix& vi)
  {
    setInput(0, vr);
    setInput(1, vi);
  }

  void VWorker2D::checkParameter(int nRows, int nCols)
  {
    if (cols == 0 || rows == 0)
      setParameter(nRows, nCols);
    else
      {
        if (cols != nCols || rows != nRows)
          {
            throw logic_error("wrong input dimension");
          }
      }
  }

  void VWorker2D::getResult(ice::matrix<double>& dstre,
                            ice::matrix<double>& dstim)
  {
    checkDone();
    dstre = result[0];
    dstim = result[1];
  }

  void VWorker2D::getResult(ice::matrix<double>& dstre)
  {
    checkDone();
    dstre = result[0];
  }

  void VWorker2D::getResult(const Image& dstre,
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

  void VWorker2D::getResult(const Image& dstre,
                            int mode, int sign)
  {
    ImageD resReal;
    resReal.create(cols, rows);
    getResult(resReal);
    ConvImgDImg(resReal, dstre, mode, sign);
  }

  void VWorker2D::getResult(ImageD& dstre,
                            ImageD& dstim)
  {
    checkDone();

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          // std::cout << x << " " << y << std::endl;
          dstre.setPixel(x, y, result[0][y][x]);
          dstim.setPixel(x, y, result[1][y][x]);
        }
  }

  void VWorker2D::getResult(ImageD& dstre)
  {
    checkDone();

    for (int i = 0; i < rows; ++i)
      for (int j = 0; j < cols; ++j)
        dstre.setPixel(j, i, result[0][i][j]);
  }

  void VWorker2D::checkDone() const
  {
    if (!resultValid)
      {
        if (rows == 0 || cols == 0)
          throw IceException(CNAME, "parameter not set");
        if (input[0].empty())
          throw IceException(CNAME, "input not set");
        transform();
        resultValid = true;
      }
  }
}
