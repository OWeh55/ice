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
          {
            throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
          }

        IMatrix m = ConturPointlist(c);

        for (int i = 0; i < m.rows(); i++)
          {
            TransformAndRound(tr, m[i][0], m[i][1]);
          }

        return PointlistContur(m, true);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "TransformList"
  void TransformList(const Trafo& tr, Matrix& m)
  {
    Matrix temp;
    TransformList(tr, m, temp);
    m = temp;
  }

  void TransformList(const Trafo& tr, const Matrix& m1, Matrix& m2)
  {
    if (&m1 == &m2)
      {
        return TransformList(tr, m2);
      }

    if (tr.dimSource > m1.cols())
      throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);

    m2 = Matrix(m1.rows(), tr.dimTarget);

    for (int i = 0; i < m1.rows(); i++)
      {
        Vector v = m1[i](0, tr.dimSource - 1);
        m2[i] = tr * v;
      }
  }
#undef FNAME
}
