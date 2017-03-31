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

#include "filter.h"

namespace ice
{
  bool clearpixel1(int bitmask)
  {
    int white = 0; /* Anzahl der weissen Nachbarn */

    for (int i = 0x1; i < 0x100; i = i << 1)
      {
        if (!(i & bitmask))
          {
            white++;
          }
      }

    /* diese erste Bedingung weicht von Zhang/Suen ab*/
    if ((white < 3) || (white > 6))
      {
        return false;
      }

    if ((bitmask & 0x15) == 0x15)
      {
        return false;
      }

    if (((bitmask & 0x54) == 0x54))
      {
        return false;
      }

    int transitions = 0;

    for (int i = 0x01, j = 0x02; i < 0x100; i = i << 1, j = j << 1)
      {
        if (((bitmask & i) == 0) && ((bitmask & j) == j))
          {
            transitions++; /* 01-Muster */
          }
      }

    return transitions == 1;
  }

  bool clearpixel2(int bitmask)
  {
    int white = 0; /* Anzahl der weissen Nachbarn */

    for (int i = 0x1; i < 0x100; i = i << 1)
      {
        if (!(i & bitmask))
          {
            white++;
          }
      }

    /* diese erste Bedingung weicht von Zhang/Suen ab*/
    if ((white < 3) || (white > 6))
      {
        return false;
      }

    if ((bitmask & 0x51) == 0x51)
      {
        return false;
      }

    if (((bitmask & 0x45) == 0x45))
      {
        return false;
      }

    int transitions = 0;

    for (int i = 1, j = 2; i < 0x100; i = i << 1, j = j << 1)
      {
        if (((bitmask & i) == 0) && ((bitmask & j) == j))
          {
            transitions++; /* 01-Muster */
          }
      }

    return transitions == 1;
  }

  int getNeighbors(const Image& img, int x, int y)
  {
    int p = 0;

    /* Nachbarn ermitteln -> bitmaske */
    if (GetValUnchecked(img, x, y - 1))
      {
        p |= 0x101;
      }

    if (GetValUnchecked(img, x + 1, y - 1))
      {
        p |= 0x02;
      }

    if (GetValUnchecked(img, x + 1, y))
      {
        p |= 0x04;
      }

    if (GetValUnchecked(img, x + 1, y + 1))
      {
        p |= 0x08;
      }

    if (GetValUnchecked(img, x, y + 1))
      {
        p |= 0x10;
      }

    if (GetValUnchecked(img, x - 1, y + 1))
      {
        p |= 0x20;
      }

    if (GetValUnchecked(img, x - 1, y))
      {
        p |= 0x40;
      }

    if (GetValUnchecked(img, x - 1, y - 1))
      {
        p |= 0x80;
      }

    return p;
  }

  int getNeighbors(PixelType1** tdata, int x, int y)
  {
    int p = 0;

    /* Nachbarn ermitteln -> bitmaske */
    if (tdata[y - 1][x])
      {
        p |= 0x101;
      }

    if (tdata[y - 1][x + 1])
      {
        p |= 0x02;
      }

    if (tdata[y][x + 1])
      {
        p |= 0x04;
      }

    if (tdata[y + 1][x + 1])
      {
        p |= 0x08;
      }

    if (tdata[y + 1][x])
      {
        p |= 0x10;
      }

    if (tdata[y + 1][x - 1])
      {
        p |= 0x20;
      }

    if (tdata[y][x - 1])
      {
        p |= 0x40;
      }

    if (tdata[y - 1][x - 1])
      {
        p |= 0x80;
      }

    return p;
  }

#define FNAME "SkeletonImg"
  int SkeletonImg(const Image& pic, const Image& skelett, int lvl)
  {
    int dimx, dimy;
    RETURN_ERROR_IF_FAILED(MatchImg(pic, skelett, dimx, dimy));

    int maxv = skelett->maxval;

    if (maxv < 2)
      {
        throw IceException(FNAME, M_LOWRANGE, ERROR);
        return ERROR;
      }

    BinImg(pic, lvl, skelett); // Ausgangsbild binarisieren

    /* Iterativ verduennen nach modifiziertem Zhang/Suen-Algorith. */

    int nIteration = 0;
    int changes;

    do
      {
        nIteration++;

        changes = 0;

        for (int x = 1; x < dimx - 1; x++)
          {
            for (int y = 1; y < dimy - 1; y++)
              {
                if (GetValUnchecked(skelett, x, y) > 0)
                  {
                    int p = getNeighbors(skelett, x, y);

                    if (clearpixel1(p))
                      {
                        PutVal(skelett, x, y, 1); /* Pixel mark.*/
                        changes++;
                      }
                  }
              }
          }

        /* Nach einem Durchlauf nun alle markierten Pixel loeschen */
        for (int y = 0; y < dimy; y++)
          for (int x = 0; x < dimx; x++)
            {
              if (GetValUnchecked(skelett, x, y) == 1)
                {
                  PutVal(skelett, x, y, 0);
                }
            }

        // Nun von rechts unten nach links oben Skelett berechnen

        for (int x = dimx - 2; x >= 1; x--)
          {
            for (int y = dimy - 2; y >= 1; y--)
              {
                if (GetValUnchecked(skelett, x, y) > 0)
                  {
                    int p = getNeighbors(skelett, x, y);

                    if (clearpixel2(p))
                      {
                        PutVal(skelett, x, y, 1); // Pixel mark.
                        changes++;
                      }
                  }
              }
          }

        // Nach einem Durchlauf nun alle markierten Pixel loeschen
        for (int y = 0; y < dimy; y++)
          for (int x = 0; x < dimx; x++)
            {
              if (GetValUnchecked(skelett, x, y) == 1)
                {
                  PutVal(skelett, x, y, 0);
                }
            }
      }
    while (changes > 0);   // Ende erst, falls keine Aenderung mehr

    return 0;
  }

  int skeletonImg(const Image& pic, const Image& skelett, int lvl)
  {
    int dimx, dimy;
    RETURN_ERROR_IF_FAILED(MatchImg(pic, skelett, dimx, dimy));

    int maxv = skelett->maxval;

    Image timg = NewImg(dimx, dimy, 3);

    BinImg(pic, lvl, timg); // binarisieren und im Zwischenbild speichern

    PixelType1** tdata = (PixelType1**)timg->getDataPtr();

    /* Iterativ verduennen nach modifiziertem Zhang/Suen-Algorith. */

    int nIteration = 0;
    int changes;

    do
      {
        nIteration++;

        changes = 0;

        for (int y = 1; y < dimy - 1; y++)
          {
            for (int x = 1; x < dimx - 1; x++)
              {
                if (tdata[y][x] > 0)
                  {
                    int p = getNeighbors(tdata, x, y);

                    if (clearpixel1(p))
                      {
                        tdata[y][x] = 1; /* Pixel mark.*/
                        changes++;
                      }
                  }
              }
          }

        /* Nach einem Durchlauf nun alle markierten Pixel loeschen */
        for (int y = 0; y < dimy; y++)
          for (int x = 0; x < dimx; x++)
            {
              if (tdata[y][x] == 1)
                {
                  tdata[y][x] = 0;
                }
            }

        // Nun von rechts unten nach links oben Skelett berechnen

        for (int y = dimy - 2; y >= 1; y--)
          {
            for (int x = dimx - 2; x >= 1; x--)
              {
                if (tdata[y][x] > 0)
                  {
                    int p = getNeighbors(tdata, x, y);

                    if (clearpixel2(p))
                      {
                        tdata[y][x] = 1;
                        changes++;
                      }
                  }
              }
          }

        // Nach einem Durchlauf nun alle markierten Pixel loeschen
        for (int y = 0; y < dimy; y++)
          for (int x = 0; x < dimx; x++)
            {
              if (tdata[y][x] == 1)
                {
                  tdata[y][x] = 0;
                }
            }
      }
    while (changes > 0);   // Ende erst, falls keine Aenderung mehr


    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          if (tdata[y][x] > 0)
            {
              PutVal(skelett, x, y, maxv);
            }
          else
            {
              PutVal(skelett, x, y, 0);
            }
        }

    return 0;
  }
}

