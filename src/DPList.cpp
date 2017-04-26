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
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "IceException.h"

#include "DPList.h"

namespace ice
{

#define BLOCKSIZE 100

#define FNAME "NewDPointList"

  DPointList NewDPointList()
  {

    DPointList pl = new DPointList_;

    if (pl == NULL)
      throw IceException(FNAME, M_NO_MEM);

    pl->xptr = new int[BLOCKSIZE];

    if (pl->xptr == NULL)
      {
        delete pl;
        throw IceException(FNAME, M_NO_MEM);
      }

    pl->yptr = new int[BLOCKSIZE];

    if (pl->yptr == NULL)
      {
        delete [](pl->xptr);
        delete pl;
        throw IceException(FNAME, M_NO_MEM);
      }

    pl->wptr = new int[BLOCKSIZE];

    if (pl->wptr == NULL)
      {
        delete [](pl->xptr);
        delete [](pl->yptr);
        delete pl;
        throw IceException(FNAME, M_NO_MEM);
      }

    pl->lng = 0;
    pl->aktlng = 100;

    pl->x = pl->y = pl->w = 0;

    return pl;
  }

#undef FNAME

#define FNAME "DAddPoint"

  int DAddPoint(DPointList pl, int x, int y, int w)
  {

    if (pl == NULL)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (pl->aktlng == pl->lng)
      {

        pl->aktlng += BLOCKSIZE;
        int* ptr = (int*)realloc(pl->xptr, pl->aktlng * sizeof(int));

        if (ptr)
          {
            pl->xptr = ptr;
          }
        else
          throw IceException(FNAME, M_NO_MEM);

        ptr = (int*)realloc(pl->yptr, pl->aktlng * sizeof(int));

        if (ptr)
          {
            pl->yptr = ptr;
          }
        else
          throw IceException(FNAME, M_NO_MEM);

        ptr = (int*)realloc(pl->wptr, pl->aktlng * sizeof(int));

        if (ptr)
          {
            pl->wptr = ptr;
          }
        else
          throw IceException(FNAME, M_NO_MEM);
      }

    pl->xptr[pl->lng] = pl->x = x;
    pl->yptr[pl->lng] = pl->y = y;
    pl->wptr[pl->lng] = pl->w = w;

    pl->lng++;

    return true;
  }

#undef FNAME

#define FNAME "DDelLastPoint"

  int DDelPoint(DPointList pl, int num)
  {

    if (pl == NULL || num >= pl->lng)
      throw IceException(FNAME, M_WRONG_PARAM);

    pl->lng--;
    int anz = (pl->lng - num) * sizeof(int);

    if (anz)
      {
        memmove(pl->xptr + num, pl->xptr + num + 1, anz);
        memmove(pl->yptr + num, pl->yptr + num + 1, anz);
        memmove(pl->wptr + num, pl->wptr + num + 1, anz);
      }

    if (pl->lng > 0)
      {
        pl->x = pl->xptr[pl->lng - 1];
        pl->y = pl->yptr[pl->lng - 1];
        pl->w = pl->wptr[pl->lng - 1];
      }

    return true;
  }

#undef FNAME

#define FNAME "FreePointList"
  int FreePointList(DPointList pl)
  {

    if (pl == NULL)
      throw IceException(FNAME, M_WRONG_PARAM);

    delete [](pl->xptr);
    delete [](pl->yptr);
    delete [](pl->wptr);
    delete pl;
    return true;
  }
#undef FNAME
}
