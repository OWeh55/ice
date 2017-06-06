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

#include <memory.h>

#ifndef WIN32
#include <stdlib.h>
#endif

#include "defs.h"
#include "IceException.h"
#include "WindowWalker.h"
#include "morph.h"

#include "icefunc.h"

using namespace std;

namespace ice
{
  /**********************************************************************

        Interne Funktionen fuer Thinning

  ***********************************************************************/

//////////////////////////////////////////////////////////////////////////
// Lesen der dx x dy -Umgebung des Punktes (x,y) im Bild img.
// In mask[] wird die Umgebung eingetragen und die Funktion liefert
// die Anzahl der Nachbarn des Aufsetzpunktes mit einem Grauwert!=0 zurueck.

  int getNeighbours(const Image& img, int x, int y, int dx, int dy, int mask[5][5])
  {
    int index = 0, count = 0;

    int x0 = -(dx - 1) / 2 + x;
    int y0 = -(dy - 1) / 2 + y;

    for (int yi = 0; yi < dy; yi++)
      {
        int yy = y0 + yi;
        for (int xi = 0; xi < dx; xi++)
          {
            int xx = x0 + xi;
            if (!mask[xi][yi] || xx > img->xsize || yy > img->ysize || xx < 0 || yy < 0)
              {
                mask[xi][yi] = -1;
              }
            else if ((mask[xi][yi] = GetVal(img, xx, yy)) && (abs(xx - x) < 2) && (abs(yy - y) < 2))
              {
                count++;
              }

            index++;
          }
      }

    return count - 1;
  }

/////////////////////////////////////////////////////////////////////////
// Bestimmen der 0 zu >0 Uebergaenge in der Zahlenfolge p1, p2, ..., p8, p1

  int Trans(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
  {
    int count = 0;

    if (p2 > 0 && p1 == 0)
      {
        count = 1;
      }

    if (p3 > 0 && p2 == 0)
      {
        count++;
      }

    if (p4 > 0 && p3 == 0)
      {
        count++;
      }

    if (p5 > 0 && p4 == 0)
      {
        count++;
      }

    if (p6 > 0 && p5 == 0)
      {
        count++;
      }

    if (p7 > 0 && p6 == 0)
      {
        count++;
      }

    if (p8 > 0 && p7 == 0)
      {
        count++;
      }

    if (p1 > 0 && p8 == 0)
      {
        count++;
      }

    return count;
  }

  /**********************************************************************

      Thinning nach JAIN
      Die Objekte im Binaerbild img1 werden verduennt.

  ***********************************************************************/

#define FNAME "Thinning"

  void Thinning(const Image& img1, const Image& img2)
  {
    try
      {
        int x, y, changed, neighbors;

        int dx, dy;
        checkSizes(img1, img2, dx, dy);

        Image imgd;
        imgd.create(img1.xsize, img1.ysize, 1);

        binImg(img1, imgd);

        int m[5][5];
        do
          {
            CopyImg(imgd, img2);
            changed = 0;

            for (x = 2; x < imgd->xsize - 2; x++)
              for (y = 2; y < imgd->ysize - 2; y++)
                if (GetVal(img2, x, y))
                  {
                    memset(m, 1, 25 * sizeof(int));
                    neighbors = getNeighbours(img2, x, y, 5, 5, m);

                    if ((2 <= neighbors) && (neighbors <= 6))
                      {
                        if (Trans(m[1][2], m[1][1], m[2][1], m[3][1], m[3][2], m[3][3], m[2][3], m[1][3]) == 1)
                          {
                            if ((!m[1][2]) || (!m[2][1]) || (!m[2][3]) || (Trans(m[0][2], m[0][1], m[1][1], m[2][1], m[2][2], m[2][3], m[1][3], m[0][3])) != 1)
                              {
                                if ((!m[1][2]) || (!m[2][1]) || (!m[3][2]) || (Trans(m[1][1], m[1][0], m[2][0], m[3][0], m[3][1], m[3][2], m[2][2], m[1][2])) != 1)
                                  {
                                    PutVal(imgd, x, y, 0);
                                    changed = 1;
                                  }
                              }
                          }
                      }
                  }
          }
        while (changed);

        for (int y = 0; y < img2.ysize; y++)
          for (int x = 0; x < img2.xsize; x++)
            {
              memset(m, 1, 25 * sizeof(int));

              if (GetVal(img2, x, y) > 0)
                {
                  int val = img2.maxval;

                  if ((neighbors = getNeighbours(img2, x, y, 3, 3, m)) > 1)
                    {
                      for (int i = 0; i < 5; i++)
                        for (int j = 0; j < 5; j++)
                          if (m[i][j] == -1)
                            {
                              m[i][j] = 0;
                            }

                      if (Trans(m[0][0], m[0][1], m[0][2], m[1][0], m[1][3], m[1][2], m[1][1], m[0][3]) == 2 &&
                          (!m[0][0] || !m[1][3] || (m[0][1] && m[1][0]) || (m[0][3] && m[1][2])) &&
                          (!m[0][2] || !m[1][1] || (m[1][0] && m[1][2]) || (m[0][1] && m[0][3])) &&
                          (!m[0][1] || !m[1][2]) &&
                          (!m[1][0] || !m[0][3]) &&
                          ((m[0][1] && m[0][3]) ||
                           (m[0][1] && m[1][0]) ||
                           (m[1][0] && m[1][2]) ||
                           (m[1][2] && m[0][3])))
                        {
                          val = 0;
                        }
                    }

                  PutVal(img2, x, y, val);
                }
            }
      }
    RETHROW;
  }
#undef FNAME

  /******************************************************************

     Relaxation zur Erstellung lokal kontrastierter Bilder

  *******************************************************************/

#define FNAME "RelaxImg"
  void RelaxImg(const Image& img, const Image& imgd, int n)
  {
    try
      {
        int dx, dy, maxval;
        checkImage(img, imgd, dx, dy, maxval);
        Image maxImage;
        maxImage.create(img);
        Image minImage;
        minImage.create(img);
        MinMaxImg(img, n, n, minImage, maxImage);

        WindowWalker ww(img);
        for (ww.init(); !ww.ready(); ww.next())
          {
            int min = minImage.getPixel(ww);
            int max = maxImage.getPixel(ww);
            int mean = (max + min) / 2;
            if (img.getPixel(ww) <= mean)
              imgd.setPixel(ww, min);
            else
              imgd.setPixel(ww, max);
          }

      }
    RETHROW;
  }

#undef FNAME
}
