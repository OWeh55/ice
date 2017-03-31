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

// lists.cpp
// Funktionen zur Arbeit mit Listen
// Ortmann 10/1999
//

#include "defs.h"
#include "macro.h"
#include "IceException.h"
#include "contools.h"
#include "numbase.h"
#include "Matrix.h"
#include "lists.h"

namespace ice
{

#define FNAME "CheckPointlist"
  int CheckPointlist(const Matrix& m)
  {
    if (m.cols() < 2)
      {
        throw IceException(FNAME, M_WRONG_POINTLIST, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return OK;
  }
#undef FNAME

#define FNAME "CheckPolygon"
  int CheckPolygon(const Matrix& m)
  {
    RETURN_ERROR_IF_FAILED(CheckPointlist(m));

    if (m.rows() < 3)
      {
        throw IceException(FNAME, M_WRONG_POINTLIST, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return OK;
  }
#undef FNAME

#define FNAME "ConturPointlist"
  IMatrix ConturPointlist(const Contur& c, int diff, bool close)
  {
    IMatrix pl(0, 2);

    if (diff < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return pl;
      }

    std::vector<IPoint> vpl;
    c.getPoints(vpl, true);

    // jeden diff. Punkt anhängen ausser Endpunkt
    for (unsigned int i = 0; i < vpl.size() - 1; i += diff)
      {
        pl.Append(IVector(vpl[i]));
      }

    // Endpunkt muss angehängt werden, wenn er ungleich dem Startpunkt ist
    // oder wenn dies explizit durch close=true verlangt wird
    if (! c.isClosed() || close)
      {
        pl.Append(IVector(c.End()));
      }

    return pl;
  }
#undef FNAME

#define FNAME "PointlistContur"
  Contur PointlistContur(const Matrix& pl, int close)
  {
    int i, x0, y0, x, y;
    Contur c;

    RETURN_IF_FAILED(CheckPointlist(pl), c);

    x0 = RoundInt(pl[0][0]);
    y0 = RoundInt(pl[0][1]);

    c.SetStart(x0, y0);

    for (i = 1; i < pl.rows(); i++)
      {
        x = RoundInt(pl[i][0]);
        y = RoundInt(pl[i][1]);
        c.Add(x, y);
      }

    if (close)
      {
        c.Add(x0, y0);
      }

    return c;
  }

  Contur PointlistContur(const IMatrix& pl, int close)
  {
    int i, x, y, x0, y0;
    Contur c;
    RETURN_IF_FAILED(CheckPointlist(pl), c);

    x0 = pl[0][0];
    y0 = pl[0][1];

    c.SetStart(x0, y0);

    for (i = 1; i < pl.rows(); i++)
      {
        x = pl[i][0];
        y = pl[i][1];
        c.Add(x, y);
      }

    if (close)
      {
        c.Add(x0, y0);
      }

    return c;
  }
#undef FNAME
}
