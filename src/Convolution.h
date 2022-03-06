/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2021 FSU Jena, Digital Image Processing Group
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
#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <vector>
#include "VWorker.h"

namespace ice
{
  class Convolution : public VWorker
  {
  public:
    Convolution(int size, bool centered = false): VWorker(size), centered(centered)
    {
    }

    Convolution(): VWorker(), centered(false)
    {}

    virtual ~Convolution()
    {
    }

    Convolution(const Convolution& ft) = delete;
    const Convolution& operator=(const Convolution& ft) = delete;

    // simplified call for vector<double>
    static void convolute(const std::vector<double>& src1,
                          const std::vector<double>& src2,
                          std::vector<double>& dst);

  private:
    bool centered;
    virtual void transform() const;
  };
}
#endif
