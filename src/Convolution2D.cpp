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

#include "Convolution2D.h"
#include "FourierTrafo2D.h"

using namespace std;

#define CNAME "Convolution2D"
namespace ice
{
  void Convolution2D::setParameter(int newRows, int newCols,
                                   bool newCentered)
  {
    VWorker2D::setParameter(newRows, newCols);
    if (centered != newCentered)
      {
        centered = newCentered;
        resultValid = false;
      }
  }

  void Convolution2D::transform() const
  {
    if (input[1].empty())
      throw IceException(CNAME, "input not set");

    result[0].resize(nRows, nCols);
    result[1].clear();

    FourierTrafo2D tr(nRows, nCols, true, centered);

    tr.setInput(input[0]);
    matrix<double> s1r;
    matrix<double> s1i;
    tr.getResult(s1r, s1i);

    tr.setInput(input[1]);
    matrix<double> s2r;
    matrix<double> s2i;
    tr.getResult(s2r, s2i);

    matrix<double> s3r(nRows, nCols);
    matrix<double> s3i(nRows, nCols);

    double factor = sqrt(nRows * nCols);

    for (int y = 0; y < nRows; ++y)
      for (int x = 0; x < nCols; ++x)
        {
          double r1 = s1r[y][x];
          double i1 = s1i[y][x];
          double r2 = s2r[y][x];
          double i2 = s2i[y][x];
          s3r[y][x] = (r1 * r2 - i1 * i2) * factor;
          s3i[y][x] = (r1 * i2 + i1 * r2) * factor;
        }

    FourierTrafo2D trInv(nRows, nCols, false, centered);
    trInv.setInput(s3r, s3i);
    trInv.getResult(result[0]);
  }

  void Convolution2D::convolute(const ice::matrix<double>& src1,
                                const ice::matrix<double>& src2,
                                ice::matrix<double>& dst)
  {
    Convolution2D conv2d;
    conv2d.setInput(src1, src2);
    conv2d.getResult(dst);
  }


} // namespace ice
