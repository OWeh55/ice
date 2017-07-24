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
/* Bewertung der Übereinstimmung eines Peak-Bildes mit dem
   Einheitsimpuls */

/* ttbb 18.06.1998 */
/* Wolfgang Ortmann 2016 */

#include <algorithm>

#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "macro.h"
#include "IceException.h"
#include "numbase.h"

#include "PeakVal.h"
#include "PeakDtct.h"

// Gibt im Fehlerfall DBL_MAX zurück.
// Wenn es kein Peak gibt (Markierungsbild vollständig gesetzt oder konstantes Bild),
// gibt die Funktion als Peakkordinaten (-1,-1) und als Bewertung 0.0 zurück.

namespace ice
{
#define FNAME "PeakValuation"

  double PeakValuation(const Image& img,
                       double& x0, double& y0,
                       int mode, int graynull, bool zykl)
  {
    Image mark;
    mark.create(img);
    mark.set(0);
    return PeakValuation(img, mark, x0, y0, mode, graynull, zykl);
  }

  double PeakValuation(const Image& img, const Image& mi,
                       double& x0, double& y0,
                       int mode, int graynull, bool zykl)
  {
    if (!IsImg(img) ||
        (mode != PN_NORMAL && mode != PN_CONVOLUTION) ||
        graynull < 0 || graynull > img.maxval)
      throw IceException(FNAME, M_WRONG_PARAM);

    double   value = 0, pg0 = 0, noise = 0;

    int    pnum = 0;

    int     ming = img.maxval, maxg = 0, count = 0, maxabs = 0;

    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        {
          if (!IsImg(mi) || GetVal(mi, x, y) == 0)
            {
              int g = GetVal(img, x, y);
              if (ming > g)
                {
                  ming = g;
                }

              if (maxg < g)
                {
                  maxg = g;
                }

              if (maxabs < abs(g - graynull))
                {
                  maxabs = abs(g - graynull);
                }

            }
        }

    if (ming == maxg)
      {
        x0 = -1;
        y0 = -1;
        return 0.0;
      }

    for (int x = 0; x < img.xsize - 1; x++)
      for (int y = 0; y < img.ysize - 1; y++)
        {

          if (!IsImg(mi) || GetVal(mi, x, y) == 0)
            {
              int g = GetVal(img, x, y);
              int g1 = g - GetVal(img, x + 1, y);
              int g2 = g - GetVal(img, x, y + 1);
              int  g3 = g - GetVal(img, x + 1, y + 1);
              noise += abs(g1) + abs(g2) + abs(g3);
              count++;
            }

        }

    noise /= (6 * count);
    noise /= double(maxabs);

    PeakList pl = ImgPeakList(img, mi, 10, 0, zykl, (int)(noise + 0.5), IPL_STANDARD, graynull, mode);

    if (pl == NULL)
      {
        return DBL_MAX;
      }

    if (pl->grw < 0)   // Keine Peaks gefunden
      {
        FreePeakList(pl);
        x0 = -1;
        y0 = -1;
        return 0.0;
      }

    x0 = pl->x;
    y0 = pl->y;

    PeakList plptr = pl;

    int panz = 0;

    while (plptr)
      {
        panz++;
        plptr = plptr->next;
      }

    double* grwd = new double[panz];

    if (grwd == NULL)
      throw IceException(FNAME, M_NO_MEM);

    plptr = pl;
    panz = 0;

    while (plptr)
      {
        grwd[panz] = (plptr->grw - graynull) / double(maxabs);
        panz++;
        plptr = plptr->next;
      }

    plptr = pl;

    do
      {

        if (pnum == 0)
          {
            value = pg0 = (grwd[pnum] -
                           noise * (atan(std::max(plptr->sx, plptr->sy) * 256.0 /
                                         std::min(img.xsize, img.ysize)) / M_PI + 0.5));

          }

        else
          {

            double b1 = grwd[pnum] + noise * (0.5 - atan(std::max(plptr->sx, plptr->sy) * 256.0 / std::min(img.xsize, img.ysize)) / M_PI);

            value += (pow(b1, 8) / pow((double)pnum, 4));

          }

        pnum++;

        plptr = plptr->next;

      }
    while (plptr);

    delete []grwd;

    FreePeakList(pl);

    return pg0 / value;

  }

#undef FNAME
}
