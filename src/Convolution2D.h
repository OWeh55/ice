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
#ifndef CONVOLUTION2D_H
#define CONVOLUTION2D_H

#include <vector>
#include "VWorker2D.h"

namespace ice
{
  class Convolution2D : public VWorker2D
  {
  public:
    Convolution2D(int rows, int cols, bool centered = true):
      VWorker2D(rows, cols), centered(centered)
    {
    }

    Convolution2D(): VWorker2D(), centered(true)
    {}

    virtual ~Convolution2D()
    {
    }

    Convolution2D(const Convolution2D& ft) = delete;
    const Convolution2D& operator=(const Convolution2D& ft) = delete;

    void setParameter(int rows, int cols, bool centered = true);

    // simplified call for matrix<double>
    static void convolute(const ice::matrix<double>& src1,
                          const ice::matrix<double>& src2,
                          ice::matrix<double>& dst);

  private:
    virtual void transform() const;
    bool centered;
  };
}
#endif
