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
    VWorker2D::setParameter(newRows, newCols);

    if (forward != newForward)
      {
        forward = newForward;
        resultValid = false;
      }
    if (centered != newCentered)
      {
        centered = newCentered;
        resultValid = false;
      }
  }

  void FourierTrafo2D::transform() const
  {
    result[0].resize(rows, cols);
    result[1].resize(rows, cols);
    FourierTrafo trRow(cols, forward, centered);
    for (int y = 0; y < rows; ++y)
      {
        if (input[1].empty())
          trRow.setInputFromRow(input[0], y);
        else
          trRow.setInputFromRow(input[0], input[1], y);
        trRow.getResultToRow(result[0], result[1], y);
      }

    FourierTrafo trCol(rows, forward, centered);
    for (int x = 0; x < cols; ++x)
      {
        trCol.setInputFromColumn(result[0], result[1], x);
        trCol.getResultToColumn(result[0], result[1], x);
      }
  }
} // namespace ice
