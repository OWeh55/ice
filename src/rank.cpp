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

/*
 *  Funktionen zur Bestimmung von Rangwerten
 *  und zur Rangfilterung
 *
 *  m.schubert 07/93
 *  W.Ortmann 08/02, 02/13
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "macro.h"
#include "message.h"
#include "arith.h"

#include "rank.h"

namespace ice
{

  class minimalHistogram
  {
  private:
    minimalHistogram(const minimalHistogram& mh);

  public:
    minimalHistogram(int classes, int nValues, int rank):
      rankindex(0),
      addval(rank)
    {
      counter = new int[classes];
      counter[0] = nValues;
      for (int i = 1; i < classes; i++)
        counter[i] = 0;
    }

    ~minimalHistogram()
    {
      delete counter;
    }

    int get()
    {
      while (addval >= counter[rankindex])
        {
          addval -= counter[rankindex];
          rankindex++;
        }

      while (addval < 0)
        {
          rankindex--;
          addval += counter[rankindex];
        }

      return rankindex;
    }

    int addsub(int val1, int val2)
    {
      counter[val1]++;
      if (val1 < rankindex)
        addval--;

      counter[val2]--;
      if (val2 < rankindex)
        addval++;

      return 0;
    }

  private:
    int* counter;
    int rankindex;
    int addval;
  };

#define FNAME "SubRankImg"

  void PutValL(const Image& img, int x, int y, int val)
  {
    if (val < 0)
      PutVal(img, x, y, 0);
    else
      PutVal(img, x, y, val);
  }

  /*************************************************/
  int SubRankImg(Image img, int fsize, int rank, Image imgd)
  {
    int x, y, i;
    int dx, dy;
    int x1, y1, x2, y2;

    if ((fsize & 1) == 0 || rank < 0 || rank >= fsize * fsize)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int size2 = fsize / 2;

    RETURN_ERROR_IF_FAILED(MatchImg(img, imgd, dx, dy));

    x1 = size2;
    x2 = imgd->xsize - 1 - size2; /* Fenster fuer Rangfilter */
    y1 = size2;
    y2 = imgd->ysize - 1 - size2;

    if (imgd == img)
      {
        /*temporaeres Quellbild*/
        img = NewImg(imgd, true);
      }

    for (y = 0; y < y1; y++)
      for (x = 0; x < imgd->xsize; x++)  /*Rand zuruecksetzen*/
        PutVal(imgd, x, y, 0);

    for (y = y2; y < imgd->ysize; y++)
      for (x = 0; x < imgd->xsize; x++)
        PutVal(imgd, x, y, 0);

    for (x = 0; x < x1; x++)
      for (y = 0; y < imgd->ysize; y++)
        PutVal(imgd, x, y, 0);

    for (x = x2; x < imgd->xsize; x++)
      for (y = 0; y < imgd->ysize; y++)
        PutVal(imgd, x, y, 0);


    minimalHistogram rh(img.maxval + 1, fsize * fsize, rank);

    x = x1;
    y = y1;

    for (int i = x - size2; i <= x + size2; i++)
      for (int j = y - size2; j <= y + size2; j++)
        rh.addsub(GetVal(img, i, j), 0);

    PutValL(imgd, x, y, GetVal(img, x, y) - rh.get());

    while (y <= y2)
      {
        while (x < x2)
          {
            for (i = y - size2; i <= y + size2; i++)
              {
                rh.addsub(GetVal(img, x + size2 + 1, i), GetVal(img, x - size2, i));
              }

            x++;

            PutValL(imgd, x, y, GetVal(img, x, y) - rh.get());
          }

        y++;

        if (y > y2) break;

        for (i = x - size2; i <= x + size2; i++)
          {
            rh.addsub(GetVal(img, i, y + size2), GetVal(img, i, y - size2 - 1));
          }

        PutValL(imgd, x, y, GetVal(img, x, y) - rh.get());

        while (x > x1)
          {
            for (i = y - size2; i <= y + size2; i++)
              {
                rh.addsub(GetVal(img, x - size2 - 1, i), GetVal(img, x + size2, i));
              }

            x--;

            PutValL(imgd, x, y, GetVal(img, x, y) - rh.get());
          }

        y++;

        if (y > y2) break;

        for (i = x - size2; i <= x + size2; i++)
          {
            rh.addsub(GetVal(img, i, y + size2), GetVal(img, i, y - size2 - 1));
          }

        PutValL(imgd, x, y, GetVal(img, x, y) - rh.get());
      }

    return OK;
  }
#undef FNAME
  /*************************************************/
#define FNAME "RankImg"
  int RankImg(Image img, int fsizex, int fsizey, int rank, Image imgd)
  {
    int x, y, i, j;
    int xl, xr;
    int x1, y1, x2, y2;
    int dx, dy;

    if (((fsizex & 1) == 0) || ((fsizey & 1) == 0))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int sizex = fsizex / 2;
    int sizey = fsizey / 2;

    RETURN_ERROR_IF_FAILED(MatchImg(img, imgd, dx, dy));

    x1 = sizex;
    x2 = imgd->xsize - 1 - sizex; /* Fenster fuer Rangfilter */
    y1 = sizey;
    y2 = imgd->ysize - 1 - sizey;

    if (imgd == img)
      {
        /* temporaeres Quellbild */
        img = NewImg(imgd, true);
      }

    for (y = 0; y < y1; y++)
      for (x = 0; x < imgd->xsize; x++)  /*Rand zuruecksetzen*/
        PutVal(imgd, x, y, 0);

    for (y = y2; y < imgd->ysize; y++)
      for (x = 0; x < imgd->xsize; x++)
        PutVal(imgd, x, y, 0);

    for (x = 0; x < x1; x++)
      for (y = 0; y < imgd->ysize; y++)
        PutVal(imgd, x, y, 0);

    for (x = x2; x < imgd->xsize; x++)
      for (y = 0; y < imgd->ysize; y++)
        PutVal(imgd, x, y, 0);

    x = x1;
    y = y1;
    xl = x - sizex;
    xr = x + sizex;

    minimalHistogram rh(img.maxval + 1, fsizex * fsizey, rank); /*Histogramm fuer Rangfilter*/

    for (j = y - sizey; j <= y + sizey; j++) /*.. initialisieren*/
      for (i = xl; i <= xr; i++)
        rh.addsub(GetVal(img, i, j), 0);

    PutVal(imgd, x, y, rh.get()); /*erster Punkt*/

    while (y <= y2)
      {
        while (x < x2)                    /*Zeile vorwaerts*/
          {
            for (i = y - sizey; i <= y + sizey; i++)
              {
                rh.addsub(GetVal(img, xr + 1, i), GetVal(img, xl, i));
              }

            x++;
            xr++;
            xl++;
            PutVal(imgd, x, y, rh.get());
          }

        y++;                             /*naechste Zeile*/

        if (y > y2) break;

        for (i = xl; i <= xr; i++)
          {
            rh.addsub(GetVal(img, i, y + sizey), GetVal(img, i, y - sizey - 1));
          }

        PutVal(imgd, x, y, rh.get());

        while (x > x1)                      /*Zeile rückwärts*/
          {
            for (i = y - sizey; i <= y + sizey; i++)
              {
                rh.addsub(GetVal(img, xl - 1, i), GetVal(img, xr, i));
              }

            x--;
            xl--;
            xr--;
            PutVal(imgd, x, y, rh.get());
          }

        y++;                               /*nächste Zeile*/

        if (y > y2) break;

        for (i = xl; i <= xr; i++)
          {
            rh.addsub(GetVal(img, i, y + sizey), GetVal(img, i, y - sizey - 1));
          }

        PutVal(imgd, x, y, rh.get());
      }

    return OK;
  }

  int RankImg(Image img, int size, int rank, Image imgd)
  {
    return RankImg(img, size, size, rank, imgd);
  }
#undef FNAME

#define FNAME "MedianImg"
  int MedianImg(Image img, int size, Image imgd)
  {
    int ret;

    if ((size & 1) == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int rank = (size * size) / 2;
    RETURN_ERROR_IF_FAILED(ret = RankImg(img, size, rank, imgd));
    return ret;
  }
#undef FNAME
}
