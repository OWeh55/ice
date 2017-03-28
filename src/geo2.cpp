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
#include "message.h"

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
    //double hf, x, y, xt, yt;
    int i;

    Contur res;

    if ((tr.DimSource() != 2) || (tr.DimTarget() != 2))
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return res;
      }

    IMatrix m;
    IfFailed(m = ConturPointlist(c))
    {
      Message(FNAME, M_0, ERROR);
      return res;
    }

    for (i = 0; i < m.rows(); i++)
      {
        TransformAndRound(tr, m[i][0], m[i][1]);
        /*
        x=m[i][0]; y=m[i][1];
        hf=x*tr.m[2][0]+y*tr.m[2][1]+tr.m[2][2];
        xt=(x*tr.m[0][0]+y*tr.m[0][1]+tr.m[0][2])/hf;
        yt=(x*tr.m[1][0]+y*tr.m[1][1]+tr.m[1][2])/hf;
        m[i][0]=RoundInt(xt); m[i][1]=RoundInt(yt);
        */
      }

    return PointlistContur(m, true);
  }
#undef FNAME
#define FNAME "TransformList"
  int TransformList(const Trafo& tr, Matrix& m)
  {
    Matrix temp;
    ReturnErrorIfFailed(TransformList(tr, m, temp));
    m = temp;
    return OK;
  }

  int TransformList(const Trafo& tr, const Matrix& m1, Matrix& m2)
  {
    if (&m1 == &m2)
      return TransformList(tr, m2);

    if (tr.dimSource > m1.cols())
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    m2 = Matrix(m1.rows(), tr.dimTarget);
    int i;

    for (i = 0; i < m1.rows(); i++)
      {
        Vector v = m1[i](0, tr.dimSource - 1);
        m2[i] = tr * v;
      }

    return OK;
  }
#undef FNAME
}
