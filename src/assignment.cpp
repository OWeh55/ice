/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "assignment.h"
#include "numbase.h"
#include "defs.h"
#include "IceException.h"
#include "Matrix.h"

namespace ice
{
#define FNAME "ReorderList"
  int ReorderList(Matrix& feat, const IMatrix& ref)
  {
    // sortiert die Liste feat um nach der durch ref vorgegebenen
    // Ordnung: originale Zeile mit dem Index ref[i][0] erscheint
    // im Ergebnis in Zeile ref[i][1]
    if (feat.rows() != ref.rows())
      {
        throw IceException(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
      }

    int i;
    Matrix f1 = feat;

    for (i = 0; i < ref.rows(); i++)
      {
        feat[ref[i][1]] = f1[ref[i][0]];
      }

    return OK;
  }
#undef FNAME
#define FNAME "ReferenceCosts"
  double ReferenceCosts(const Matrix& costs, const IMatrix& pairs)
  {
    int i;
    double sum = 0;

    for (i = 0; i < pairs.rows(); i++)
      {
        sum += costs[pairs[i][0]][pairs[i][1]];
      }

    return sum;
  }
#undef FNAME
}
