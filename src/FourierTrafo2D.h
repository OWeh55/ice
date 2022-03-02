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
#include "VWorker2D.h"
#include "IceException.h"
#include "matrixtemplate.h"

namespace ice
{
  class FourierTrafo2D: public VWorker2D
  {
  public:
    FourierTrafo2D(): VWorker2D(), forward(true), centered(true) {}

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

    virtual ~FourierTrafo2D()
    {
    }

    void setParameter(int newRows, int newCols,
                      bool newForward = true, bool newCentered = true);

    FourierTrafo2D(const FourierTrafo2D& ft) = delete;
    const FourierTrafo2D& operator=(const FourierTrafo2D& ft) = delete;

  private:
    void transform() const;

    bool forward;
    bool centered;
  };

}
#endif
