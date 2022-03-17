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

#include "defs.h"
#include "macro.h"
#include "IceException.h"

#include "Trafo.h"
#include "contools.h"
#include "lists.h"
#include "Matrix.h"

#include "geo.h"

//--------------------------------------------
namespace ice
{
//-----------------------------
// Several ways to use a Trafo
//-----------------------------

#define FNAME "Transform"
  Contur Transform(const Trafo& tr, const Contur& c)
  {
    try
      {

        Contur res;

        if ((tr.DimSource() != 2) || (tr.DimTarget() != 2))
          throw IceException(FNAME, M_WRONG_DIMENSION);

        IMatrix m = ConturPointlist(c);

        for (int i = 0; i < m.rows(); i++)
          {
            transformAndRound(tr, m[i][0], m[i][1]);
          }

        return PointlistContur(m, true);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "transformList"
  void transformList(const Trafo& tr, Matrix& m)
  {
    Matrix temp;
    transformList(tr, m, temp);
    m = temp;
  }

  void transformList(const Trafo& tr, const Matrix& m1, Matrix& m2)
  {
    if (&m1 == &m2)
      {
        return transformList(tr, m2);
      }

    if (tr.dimSource > m1.cols())
      throw IceException(FNAME, M_WRONG_DIMENSION);

    m2 = Matrix(m1.rows(), tr.dimTarget);

    for (int i = 0; i < m1.rows(); i++)
      {
        Vector v = m1[i](0, tr.dimSource - 1);
        m2[i] = tr * v;
      }
  }
#undef FNAME
}
