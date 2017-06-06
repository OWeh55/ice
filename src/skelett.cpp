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

#include <array>

#include "morph.h"
#include "WindowWalker.h"

namespace ice
{
  int getWhite(int bitmask)
  {
    int white = 0;
    for (int i = 0x1; i < 0x100; i = i << 1)
      {
        if (!(i & bitmask))
          white++;
      }
    return white;
  }

  int getTransitions(int bitmask)
  {
    int transitions = 0;
    int mask1 = 1;
    int mask2 = 2;
    for (int i = 0; i < 8; i++)
      {
        if (((bitmask & mask1) == 0) && ((bitmask & mask2) > 0))
          {
            transitions++; /* 01-Muster */
          }
        mask1 <<= 1;
        mask2 <<= 1;
        if (mask2 > 255)
          mask2 = 1;
      }
    return transitions;
  }

  bool clearpixel1(int bitmask)
  {
    int white = getWhite(bitmask);

    /* diese erste Bedingung weicht von Zhang/Suen ab*/
    if ((white < 3) || (white > 6))
      // if ((white < 2) || (white > 6))  // Zhang/Suen
      {
        return false;
      }

    if ((bitmask & 0x15) == 0x15) // ORU
      {
        return false;
      }

    if (((bitmask & 0x54) == 0x54)) // LUR
      {
        return false;
      }

    return getTransitions(bitmask) == 1;
  }

  bool clearpixel2(int bitmask)
  {
    int white = getWhite(bitmask);

    /* diese erste Bedingung weicht von Zhang/Suen ab*/
    if ((white < 3) || (white > 6))
      // if ((white < 2) || (white > 6))  // Zhang/Suen
      {
        return false;
      }

    if ((bitmask & 0x51) == 0x51) // LOU
      {
        return false;
      }

    if (((bitmask & 0x45) == 0x45)) //LOR
      {
        return false;
      }

    return getTransitions(bitmask) == 1;
  }

#if 0
  int getNeighbors(PixelType1** tdata, int x, int y)
  {
    int p = 0;

    /* Nachbarn ermitteln -> bitmaske */
    if (tdata[y - 1][x]) p |= 0x01;
    if (tdata[y - 1][x + 1])  p |= 0x02;
    if (tdata[y][x + 1]) p |= 0x04;
    if (tdata[y + 1][x + 1]) p |= 0x08;
    if (tdata[y + 1][x]) p |= 0x10;
    if (tdata[y + 1][x - 1]) p |= 0x20;
    if (tdata[y][x - 1]) p |= 0x40;
    if (tdata[y - 1][x - 1]) p |= 0x80;

    return p;
  }
#else
  int getNeighbors(PixelType1** tdata, int x, int y)
  {
    return (tdata[y - 1][x] & 1) * 0x01 |
           (tdata[y - 1][x + 1] & 1) * 0x02 |
           (tdata[y][x + 1] & 1) * 0x04 |
           (tdata[y + 1][x + 1] & 1) * 0x08 |
           (tdata[y + 1][x] & 1) * 0x10 |
           (tdata[y + 1][x - 1] & 1) * 0x20 |
           (tdata[y][x - 1] & 1) * 0x40 |
           (tdata[y - 1][x - 1] & 1) * 0x80;
  }
#endif

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
  void skeletonImg(const Image& pic, const Image& skelett, int lvl, int nIter)
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

        std::array<bool, 256> clearPixel1;
        std::array<bool, 256> clearPixel2;

        for (int mask = 0; mask < 256; mask++)
          {
            clearPixel1[mask] = clearpixel1(mask);
            clearPixel2[mask] = clearpixel2(mask);
          }

        /* Iterativ verduennen nach modifiziertem Zhang/Suen-Algorith. */

        int nIterations = 0;
        int changes = 0;
        do
          {
            nIterations++;
            changes = 0;

            // only inner points (left upper to right lower)
            for (int y = 1; y < dimy - 1; y++)
              for (int x = 1; x < dimx - 1; x++)
                if (tempData[y][x] > 0) // object point
                  {
                    // get binary pattern of neighbours as int
                    int p = getNeighbors(tempData, x, y);

                    if (clearPixel1[p])
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

                    if (clearPixel2[p])
                      {
                        tempData[y][x] = 1;
                        changes++;
                      }
                  }

            // clear marked pixel
            clearMarkedPixel(tempData, dimx, dimy);
          }
        while (changes > 0 && nIterations < nIter);   // until no more changes occur

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

