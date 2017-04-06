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
//
//   Funktionen mit Konturen
//
//
//   Ortmann 3/00
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "conturfunctions.h"
#include "contfeat.h"
#include "numbase.h"
#include "contools.h"
#include "conturPolygon.h"
#include "lists.h"

namespace ice
{
#define FNAME "ConvexContur"
  Contur ConvexContur(const Contur& c, int mode)
  {
    try
      {
        int x1, y1, x2, y2;
        int ready;
        Contur result;

        if (!c.isValid())
          throw IceException(FNAME, M_INVALID_CONTUR);

        c.getRect(x1, y1, x2, y2);
        IMatrix pl[2];
        IMatrix poly = IMatrix(0, 2);
        pl[0] = ConturPointlist(c);

        int xlen = x2 - x1 + 1;
        IVector maxy(xlen);
        IVector miny(xlen);
        maxy.set(y1);
        miny.set(y2);
        int ap;
        int ind, sind;
        int dx1, dy1, dx2, dy2;

        for (ap = 0; ap < pl[0].rows(); ap++)
          {
            ind = pl[0][ap][0] - x1;

            if (pl[0][ap][1] > maxy[ind])
              {
                maxy[ind] = pl[0][ap][1];
              }

            if (pl[0][ap][1] < miny[ind])
              {
                miny[ind] = pl[0][ap][1];
              }
          }

        pl[0] = IMatrix(xlen, 2);

        for (ind = 0; ind < xlen; ind++)
          {
            pl[0][ind][0] = x1 + ind;
            pl[0][ind][1] = maxy[ind];
          }

        int i1 = 0;
        int i2 = 1;

        do
          {
            ready = true;
            pl[i2] = IMatrix(0, 2);
            pl[i2].Append(IVector(pl[i1][0][0], pl[i1][0][1]));

            for (ind = 1; ind < pl[i1].rows() - 1; ind++)
              {
                dx1 = pl[i1][ind - 1][0] - pl[i1][ind][0];
                dy1 = pl[i1][ind - 1][1] - pl[i1][ind][1];
                dx2 = pl[i1][ind][0] - pl[i1][ind + 1][0];
                dy2 = pl[i1][ind][1] - pl[i1][ind + 1][1];

                if (dx1 * dy2 - dy1 * dx2 < 0)
                  {
                    pl[i2].Append(IVector(pl[i1][ind][0], pl[i1][ind][1]));
                  }
                else
                  {
                    ready = false;
                  }
              }

            pl[i2].Append(IVector(pl[i1][pl[i1].rows() - 1][0],
                                  pl[i1][pl[i1].rows() - 1][1]));

            if (i1 == 0)
              {
                i1 = 1;
                i2 = 0;
              }
            else
              {
                i1 = 0;
                i2 = 1;
              }
          }
        while (!ready);

        if (mode == CC_FAST)
          {
            result = Contur(x1, maxy[0]);

            for (ind = 0; ind < pl[i1].rows(); ind++)
              {
                result.Add(pl[i1][ind][0], pl[i1][ind][1]);
              }

          }
        else if (mode == CC_EXACT)
          {
            for (ind = 0; ind < pl[i1].rows(); ind++)
              {
                poly.Append(IVector(pl[i1][ind][0], pl[i1][ind][1]));
              }
          }

        pl[i1] = IMatrix(xlen, 2);

        for (ind = 0, sind = xlen - 1; ind < xlen; ind++, sind--)
          {
            pl[i1][ind][0] = x1 + sind;
            pl[i1][ind][1] = miny[sind];
          }

        do
          {
            ready = true;
            pl[i2] = IMatrix(0, 2);
            pl[i2].Append(IVector(pl[i1][0][0], pl[i1][0][1]));

            for (ind = 1; ind < pl[i1].rows() - 1; ind++)
              {
                dx1 = pl[i1][ind - 1][0] - pl[i1][ind][0];
                dy1 = pl[i1][ind - 1][1] - pl[i1][ind][1];
                dx2 = pl[i1][ind][0] - pl[i1][ind + 1][0];
                dy2 = pl[i1][ind][1] - pl[i1][ind + 1][1];

                if (dx1 * dy2 - dy1 * dx2 < 0)
                  {
                    pl[i2].Append(IVector(pl[i1][ind][0], pl[i1][ind][1]));
                  }
                else
                  {
                    ready = false;
                  }
              }

            pl[i2].Append(IVector(pl[i1][pl[i1].rows() - 1][0],
                                  pl[i1][pl[i1].rows() - 1][1]));

            if (i1 == 0)
              {
                i1 = 1;
                i2 = 0;
              }
            else
              {
                i1 = 0;
                i2 = 1;
              }
          }
        while (!ready);

        if (mode == CC_FAST)
          {
            for (ind = 0; ind < pl[i1].rows(); ind++)
              {
                result.Add(pl[i1][ind][0], pl[i1][ind][1]);
              }

            result.Add(x1, maxy[0]); // Contur schliessen
          }
        else if (mode == CC_EXACT)
          {
            for (ind = 0; ind < pl[i1].rows(); ind++)
              {
                poly.Append(IVector(pl[i1][ind][0], pl[i1][ind][1]));
              }

            conturFromPolygon(poly, result);
          }

        if (result.isHole() && !c.isHole())
          {
            result.InvDir();
          }

        if (!result.isHole() && c.isHole())
          {
            result.InvDir();
          }

        return result;
      }
    RETHROW;
  }
#undef FNAME
}
