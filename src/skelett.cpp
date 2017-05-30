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

#include "macro.h"
#include "morph.h"
#include "WindowWalker.h"

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

  void clearMarkedPixel(PixelType1** tempData, int dimx, int dimy)
  {
    // clear marked pixel
    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          if (tempData[y][x] == 1)
            {
              tempData[y][x] = 0;
            }
        }
  }

#define FNAME "skeletonImg"
  void skeletonImg(const Image& pic, const Image& skelett, int lvl)
  {
    try
      {
        int dimx, dimy;
        checkSizes(pic, skelett, dimx, dimy);

        int maxv = skelett.maxval;

        Image tempImage;
        tempImage.create(dimx, dimy, 3);

        binImg(pic, lvl, tempImage); // binarisieren und im Zwischenbild speichern
        // objects in tempImage have value 3 ( == tempImage.maxval)

        PixelType1** tempData = (PixelType1**)tempImage->getDataPtr();

        /* Iterativ verduennen nach modifiziertem Zhang/Suen-Algorith. */

        int nIteration = 0;
        int changes = 0;
        do
          {
            nIteration++;
            changes = 0;

            // only inner points (left upper to right lower)
            for (int y = 1; y < dimy - 1; y++)
              for (int x = 1; x < dimx - 1; x++)
                if (tempData[y][x] > 0) // object point
                  {
                    // get binary pattern of neighbours as int
                    int p = getNeighbors(tempData, x, y);

                    if (clearpixel1(p))
                      {
                        tempData[y][x] = 1; // mark pixel to "remove"
                        changes++;
                      }
                  }

            // clear marked pixel
            clearMarkedPixel(tempData, dimx, dimy);

            // only inner points (right lower to left upper)
            for (int y = dimy - 2; y >= 1; y--)
              for (int x = dimx - 2; x >= 1; x--)
                if (tempData[y][x] > 0)
                  {
                    int p = getNeighbors(tempData, x, y);

                    if (clearpixel2(p))
                      {
                        tempData[y][x] = 1;
                        changes++;
                      }
                  }

            // clear marked pixel
            clearMarkedPixel(tempData, dimx, dimy);
          }
        while (changes > 0);   // until no more chages occur

        WindowWalker ww(skelett);
        for (ww.init(); !ww.ready(); ww.next())
          {
            if (tempData[ww.y][ww.x] > 0)
              skelett.setPixel(ww, maxv);
            else
              skelett.setPixel(ww, 0);
          }
      }
    RETHROW;
  }
}

