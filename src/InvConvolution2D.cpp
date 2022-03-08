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

#include "InvConvolution2D.h"
#include "FourierTrafo2D.h"

using namespace std;

#define CNAME "InvConvolution2D"
namespace ice
{
  void InvConvolution2D::setParameter(int newRows, int newCols,
                                      double newBeta, bool newCentered)
  {
    VWorker2D::setParameter(newRows, newCols);
    if (centered != newCentered || beta != newBeta)
      {
        centered = newCentered;
        beta = newBeta;
        resultValid = false;
      }
  }

  void InvConvolution2D::transform() const
  {
    if (input[1].empty())
      throw IceException(CNAME, "input not set");

    result[0].resize(rows, cols);
    result[1].clear();

    FourierTrafo2D tr(rows, cols, true, centered);

    tr.setInput(input[0]);
    matrix<double> s1r;
    matrix<double> s1i;
    tr.getResult(s1r, s1i);

    tr.setInput(input[1]);
    matrix<double> s2r;
    matrix<double> s2i;
    tr.getResult(s2r, s2i);

    matrix<double> s3r(rows, cols);
    matrix<double> s3i(rows, cols);

    double factor = sqrt(rows * cols);

    for (int y = 0; y < rows; ++y)
      for (int x = 0; x < cols; ++x)
        {
          double r1 = s1r[y][x];
          double i1 = s1i[y][x];
          double r2 = s2r[y][x];
          double i2 = s2i[y][x];
          double denominator = r2 * r2 + i2 * i2;
          if (beta == 0.0) // inverse filterung
            {
              if (denominator != 0)
                {
                  // z3 = (1/sqrt(N)) * z1 / z2
                  s3r[y][x] = (r1 * r2 + i1 * i2) / denominator * factor;
                  s3i[y][x] = (-r1 * i2 + i1 * r2) / denominator * factor;
                }
              else
                {
                  s3r[y][x] = 0.0;
                  s3i[y][x] = 0.0;
                }
            }
          else  // restoration with contraints
            {
              denominator += beta * beta;
              s3r[y][x] = (r2 * r1 + i2 * i1) / denominator * factor;
              s3i[y][x] = (-r1 * i2 + r2 * i1) / denominator * factor;
            }
        }

    FourierTrafo2D trInv(rows, cols, false, centered);
    trInv.setInput(s3r, s3i);
    trInv.getResult(result[0]);
  }

  void InvConvolution2D::deconvolute(const ice::matrix<double>& src1,
                                     const ice::matrix<double>& src2,
                                     ice::matrix<double>& dst)
  {
    InvConvolution2D iconv2d;
    iconv2d.setInput(src1, src2);
    iconv2d.getResult(dst);
  }


} // namespace ice
