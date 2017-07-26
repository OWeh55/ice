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
//  Kontur-Funktionen
//
//
//   Ortmann 10/99
//   m.schubert 9.93
//
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <limits.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "contools.h"
#include "contfeat.h"

#include "Contur.h"

namespace ice
{
//
// Berechnung einer Segmentliste aus einer Kontur
//

  /*************************************************************************/
  /* Private Daten                                                         */
  /*************************************************************************/
  struct segm
  {
    int y;
    int count;
    int* xlist;
  };
  /*************************************************************************/
  /* Private Funktionen                                                    */
  /*************************************************************************/
  struct segm* gen_seglist(Contur c);
  int append_seglist(struct segm* slist, int x, int y);

  int classify_point(Freeman fr1, Freeman fr2, int hole)
  {
    /*Objekt*/
    static char otab[8][8] =
    {
      {0, 1, 1, 1, 1, 0, 0, 0},
      {0, 1, 1, 1, 1, 2, 2, 2},
      {0, 1, 1, 1, 1, 2, 2, 2},
      {0, 1, 1, 1, 1, 2, 2, 2},
      {1, 0, 0, 0, 0, 1, 1, 1},
      {1, 2, 2, 2, 0, 1, 1, 1},
      {1, 2, 2, 2, 0, 1, 1, 1},
      {1, 2, 2, 2, 0, 1, 1, 1}
    };
    /*Loch*/
    static char htab[8][8] =
    {
      {0, 0, 0, 0, 1, 1, 1, 1},
      {1, 1, 1, 1, 0, 2, 2, 2},
      {1, 1, 1, 1, 0, 2, 2, 2},
      {1, 1, 1, 1, 0, 2, 2, 2},
      {1, 1, 1, 1, 0, 0, 0, 0},
      {0, 2, 2, 2, 1, 1, 1, 1},
      {0, 2, 2, 2, 1, 1, 1, 1},
      {0, 2, 2, 2, 1, 1, 1, 1}
    };

    if (!hole)
      {
        return otab[fr1.Int()][fr2.Int()];
      }
    else
      {
        return htab[fr1.Int()][fr2.Int()];
      }
  }

#define FNAME "ConturSegmentlist"
  IMatrix ConturSegmentlist(const Contur& c)
  {
    int xx, yy, tmp;
    int i, j;
    Freeman fcvon, fczu;
    int PointType;
    int yanz;
    int seganz;
    int aseg;

    IMatrix segmente;

    if (!c.isValid())
      throw IceException(FNAME, M_NOT_INITIALIZED);

    int xmax, xmin, ymax, ymin;
    c.getRect(xmin, ymin, xmax, ymax);

    struct segm* slist;
    yanz = ymax - ymin + 1;

    slist = (struct segm*) calloc(sizeof(struct segm), yanz);

    if (slist == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    /*Segmentliste initialisieren*/
    for (i = 0; i < yanz; i++)
      {
        slist[i].y = ymin + i;
        slist[i].count = 0;
        slist[i].xlist = nullptr;
      }

    xx = c.StartX();
    yy = c.StartY();

    for (i = 0; i < c.Number(); i++)
      {
        fcvon = c.DirCode((i + c.Number() - 1) % c.Number());
        fczu = c.DirCode(i);
        PointType = classify_point(fcvon, fczu, c.isHole());

        for (j = 0; j < PointType; j++)
          {
            if (append_seglist(slist, xx, yy) != OK)
              {
                throw IceException(FNAME, M_NO_MEM);

                for (i = 0; i < yanz; i++)
                  {
                    if (slist[i].xlist != nullptr)
                      {
                        free(slist[i].xlist);
                      }
                  }

                free(slist);
                return segmente;
              }
          }

        fczu.move(xx, yy);
      }

    /*Liste sortieren*/
    for (i = 0; i < yanz; i++)
      {
        int ready = false;

        do
          {
            ready = true;

            for (j = 0; j < slist[i].count - 1; j++)
              {
                if (*(slist[i].xlist + j) > *(slist[i].xlist + j + 1))
                  {
                    tmp = *(slist[i].xlist + j);
                    *(slist[i].xlist + j) = *(slist[i].xlist + j + 1);
                    *(slist[i].xlist + j + 1) = tmp;
                    ready = false;
                  }
              }
          }
        while (!ready);
      }

    seganz = 0;

    for (i = 0; i < yanz; i++)
      for (j = 0; j < slist[i].count - 1; j += 2)
        {
          seganz++;
        }

    segmente = IMatrix(seganz, 3);
    aseg = 0;

    for (i = 0; i < yanz; i++)
      for (j = 0; j < slist[i].count - 1; j += 2)
        {
          segmente[aseg][0] = slist[i].y;
          segmente[aseg][1] = slist[i].xlist[j];
          segmente[aseg][2] = slist[i].xlist[j + 1];
          aseg++;
        }

    for (i = 0; i < yanz; i++)
      {
        if (slist[i].xlist != nullptr)
          {
            free(slist[i].xlist);
          }
      }

    free(slist);
    return segmente;
  }

  int append_seglist(struct segm* slist, int x, int y)
  {
    int* tmp;
    int i, index;
    index = 0;

    while (slist[index].y != y)
      {
        index++;
      }

    y = index;
    tmp = slist[y].xlist;
    slist[y].xlist = (int*) calloc(sizeof(int), slist[y].count + 1);

    if (slist[y].xlist == nullptr)
      {
        return NO_MEM;
      }

    for (i = 0; i < slist[y].count; i++)
      {
        *(slist[y].xlist + i) = *(tmp + i);
      }

    *(slist[y].xlist + slist[y].count) = x;
    slist[y].count++;

    if (tmp != nullptr)
      {
        free(tmp);
      }

    return OK;
  }
#undef FNAME
}
