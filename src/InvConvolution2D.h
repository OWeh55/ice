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
#ifndef INVCONVOLUTION2D_H
#define INVCONVOLUTION2D_H

#include <vector>
#include "VWorker2D.h"

namespace ice
{
  class InvConvolution2D : public VWorker2D
  {
  public:
    InvConvolution2D(int rows, int cols, double beta = 0.0, bool centered = true):
      VWorker2D(rows, cols), beta(beta), centered(centered)
    {
    }

    InvConvolution2D(): VWorker2D(), beta(0.0), centered(true)
    {}

    virtual ~InvConvolution2D()
    {
    }

    InvConvolution2D(const InvConvolution2D& ft) = delete;
    const InvConvolution2D& operator=(const InvConvolution2D& ft) = delete;

    void setParameter(int rows, int cols,
                      double beta = 0.0, bool centered = true);

    // simplified call for vector<double>
    static void deconvolute(const ice::matrix<double>& src1,
                            const ice::matrix<double>& src2,
                            ice::matrix<double>& dst);

  private:
    virtual void transform() const;
    double beta;
    bool centered;
  };
}
#endif
