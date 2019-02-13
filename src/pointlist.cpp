/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
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

#include "pointlist.h"

/*
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "fit.h"
#include "contools.h"
#include "numbase.h"
#include "vector.h"
#include "matrix.h"
#include "rooteig.h"
#include "analygeo.h"
#include "matdef.h"
#include "lineqn.h"
*/

using namespace std;

namespace ice
{
  /*
   *  Datentruktur Punktliste
   */
#define FNAME "NewPointList"
  PointList NewPointList(int lng)
  {
    PointList pl;

    pl = (PointList) malloc(sizeof(struct PointList_));

    if (pl == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    pl->lng = lng;
    pl->xptr = (double*)malloc(lng * sizeof(double));
    pl->yptr = (double*)malloc(lng * sizeof(double));
    pl->wptr = (double*)malloc(lng * sizeof(double));
    return pl;
  }
#undef FNAME

  /*** PutPoint() ******************************************************/
#define FNAME "PutPoint"
  int PutPoint(PointList pl, int adr, double x, double y, double weight)
  {
    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    if (adr < 0 || adr > pl->lng)
      throw IceException(FNAME, M_WRONG_INDEX);

    if (weight < 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    pl->xptr[adr] = x;
    pl->yptr[adr] = y;
    pl->wptr[adr] = weight;
    return OK;
  }
#undef FNAME

  /*** FreePointList() *******************************/
#define FNAME "FreePointList"
  int FreePointList(PointList pl)
  {
    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    if (pl->lng != 0)
      {
        free(pl->xptr);
        free(pl->yptr);
        free(pl->wptr);
      }

    free(pl);
    return OK;
  }
#undef FNAME

  /*** ConturPointList() **********************************/
#define FNAME "ConturPointList"
  PointList ConturPointList(const Contur& c, int diff)
  {
    PointList pl;
    int i;
    int x, y;
    double* xp, *yp;
    double* wp;

    if (diff < 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    pl = (PointList)malloc(sizeof(struct PointList_));

    if (pl == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    pl->lng = c.Number() / diff + 1;
    pl->xptr = (double*)malloc(pl->lng * sizeof(double) + 1);
    pl->yptr = (double*)malloc(pl->lng * sizeof(double) + 1);
    pl->wptr = (double*)malloc(pl->lng * sizeof(double) + 1);

    if ((pl->xptr == nullptr) || (pl->yptr == nullptr) || (pl->wptr == nullptr))
      {
        throw IceException(FNAME, M_NO_MEM);
        free(pl);
        return nullptr;
      }

    xp = pl->xptr;
    yp = pl->yptr;
    wp = pl->wptr;
    x = c.Start().x;
    y = c.Start().y;
    *xp++ = x;
    *yp++ = y;
    *wp++ = 1;

    for (i = diff; i <= c.Number(); i += diff)
      {
        c.getPoint(i, x, y);
        *xp++ = x;
        *yp++ = y;
        *wp++ = 1.0;
      }

    return pl;
  }
#undef FNAME

  /*
   * Datenstruktur Segmentliste
   */

  int FreeSegmentList(Segment sl)
  {
    while (sl->prev != nullptr)
      {
        sl = sl->prev;
      }

    while (sl->next != nullptr)
      {
        sl = sl->next;
        free(sl->prev);
      }

    free(sl);
    return (OK);
  }
  /************************************************/
  Segment AddSegment(Segment segl, Segment seg)
  {
    if (segl == nullptr)
      {
        return (seg);
      }

    while (segl->next != nullptr)
      {
        segl = segl->next;
      }

    segl->next = seg;

    if (seg != nullptr)
      {
        seg->prev = segl;
      }

    return (seg);
  }
  /************************************************/
  Segment FirstSegment(Segment seg)
  {
    if (seg == nullptr)
      {
        return (nullptr);
      }

    while (seg->prev != nullptr)
      {
        seg = seg->prev;
      }

    return (seg);
  }
  /************************************************/
  Segment NextSegment(Segment s)
  {
    if (s == nullptr)
      {
        return (nullptr);
      }

    return (s->next);
  }
  /************************************************/

#define FNAME "PointList2Matrix"
  int PointList2Matrix(PointList pl, int ad1, int ad2, Matrix& m)
  {
    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    if (ad1 < 0 || ad1 > pl->lng - 1 || ad2 < 0 || ad2 > pl->lng - 1 || pl->lng <= 0)
      throw IceException(FNAME, M_WRONG_INDEX);

    m = Matrix(0, 3);

    m.append(Vector(pl->xptr[ad1], pl->yptr[ad1], pl->wptr[ad1]));

    for (int i = ad1 + 1; i != ad2; i = (i + 1) % pl->lng)
      {
        m.append(Vector(pl->xptr[i], pl->yptr[i], pl->wptr[i]));
      }

    return OK;
  }
#undef FNAME

#define FNAME "PointList2vector"
  int PointList2vector(PointList pl, int ad1, int ad2, vector<PointValue>& vpv)
  {
    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    if (ad1 < 0 || ad1 > pl->lng - 1 ||
        ad2 < 0 || ad2 > pl->lng - 1 || pl->lng <= 0)
      throw IceException(FNAME, M_WRONG_INDEX);

    vpv.clear();

    vpv.push_back(PointValue(pl->xptr[ad1], pl->yptr[ad1], pl->wptr[ad1]));

    for (int i = ad1 + 1; i != ad2; i = (i + 1) % pl->lng)
      {
        vpv.push_back(PointValue(pl->xptr[i], pl->yptr[i], pl->wptr[i]));
      }

    return OK;
  }
#undef FNAME

#define FNAME "Matrix2PointList"
  PointList Matrix2PointList(const Matrix& m)
  {
    if ((m.cols() < 2))
      throw IceException(FNAME, M_MATRIXFORMAT);

    PointList pl = NewPointList(m.rows());

    if (m.cols() > 2) // Gewicht vorhanden
      for (int i = 0; i < m.rows(); i++)
        {
          PutPoint(pl, i, m[i][0], m[i][1], m[i][2]);
        }
    else
      for (int i = 0; i < m.rows(); i++)
        {
          PutPoint(pl, i, m[i][0], m[i][1], 1.0);
        }

    return pl;
  }
#undef FNAME
}
