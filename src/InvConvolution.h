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
#ifndef INVCONVOLUTION_H
#define INVCONVOLUTION_H

#include <vector>
#include "VWorker.h"

namespace ice
{
  class InvConvolution : public VWorker
  {
  public:
    InvConvolution(int size, double beta = 0.0): VWorker(size), beta(beta)
    {
    }

    InvConvolution(): VWorker(), beta(0.0)
    {}

    virtual ~InvConvolution()
    {
    }

    InvConvolution(const InvConvolution& ft) = delete;
    const InvConvolution& operator=(const InvConvolution& ft) = delete;

    // simplified call for vector<double>
    static void deconvolute(const std::vector<double>& src1,
                            const std::vector<double>& src2,
                            std::vector<double>& dst,
                            double beta = 0.0);

  private:
    double beta;
    virtual void transform() const;
  };
}
#endif
