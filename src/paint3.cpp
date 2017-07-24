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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#include "IceException.h"
#include "macro.h"
#include "Contur.h"
#include "contools.h"
#include "contfeat.h"
#include "arith.h"
#include "numbase.h"

#include "drawline.h"
#include "paint.h"

namespace ice
{
  /**********************************************************************
    Werkzeuge zum grafischen Darstellen von
    eindimensionalen Datenmengen

      InitFuncImage1D()
      PutFuncImage1D()
      FreeFuncImage1D()
      PaintFuncImage1D()
  ***********************************************************************/

  /**********************************************************************/
#define FNAME "InitFuncImage1D"
  FuncImage1D InitFuncImage1D
  (int nbr, double upper, double step, char* str, char* strx, char* stry)
  {
    FuncImage1D fi;
    int i;

    if (nbr < 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    fi = (FuncImage1D)malloc(sizeof(struct FuncImage1D_));

    if (fi == NULL)
      throw IceException(FNAME, M_NO_MEM);

    fi->nbr = nbr;
    fi->upper = upper;
    fi->step = step;
    fi->entry = (double*)malloc(nbr * sizeof(double));
    strcpy(fi->str, str);
    strcpy(fi->strx, strx);
    strcpy(fi->stry, stry);
    fi->entry = (double*)malloc(nbr * sizeof(double));

    if (fi->entry == NULL)
      throw IceException(FNAME, M_NO_MEM);

    for (i = 0; i < nbr; i++)
      {
        fi->entry[i] = 0;
      }

    return fi;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "PutFuncImage1D"
  int PutFuncImage1D(FuncImage1D fi, int i, double val)
  {
    if ((i < 0) || (i > fi->nbr - 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    fi->entry[i] = val;
    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "PainFuncImage1D"
  Image PaintFuncImage1D(FuncImage1D fi, double fac, int mode)
  {

#define CHARX 8
#define CHARY 8
#define GAPX 5
#define GAPY 5
#define LAST 5

    Image img;
    double min, max;
    int i;
    double diff;
    char smin[20], smax[20];
    char down[20], top[20];
    size_t l, l1, l2;
    int sizey, h;

    if (mode != DEFAULT)
      throw IceException(FNAME, M_WRONG_PARAM);

    min = HUGE_VAL;
    max = -HUGE_VAL;

    /* Bestimmung der maximalen und minimalen Einträge */
    for (i = 0; i < fi->nbr; i++)
      {
        if (fi->entry[i] < min)
          {
            min = fi->entry[i];
          }

        if (fi->entry[i] > max)
          {
            max = fi->entry[i];
          }
      }

    diff = max - min;
    sprintf(smin, "%g", min);
    sprintf(smax, "%g", max);
    l1 = strlen(smin);
    l2 = strlen(smax);
    l = std::max(l1, l2);
    sizey = (int)((double)(fi->nbr) * fac);
    img = NewImg(fi->nbr + l * CHARX + 3 * GAPX + LAST + 1, sizey + 4 * CHARY + 6 * GAPY + 1, 255);
    img.set(0);
    Line(l * CHARX + 2 * GAPX, 2 * CHARY + 3 * GAPY,
         l * CHARX + 2 * GAPX, 2 * CHARY + 3 * GAPY + LAST + sizey,
         255, DEFAULT, img);
    Line(l * CHARX + 2 * GAPX, 2 * CHARY + 3 * GAPY + LAST + sizey,
         l * CHARX + 2 * GAPX + fi->nbr + LAST, 2 * CHARY + 3 * GAPY + LAST + sizey,
         255, DEFAULT, img);

    if (l1 <= l2)
      {
        Text(smax, GAPX, 2 * CHARY + 3 * GAPY + LAST - CHARY / 2, 255, 0, img);
      }
    else
      {
        Text(smax, GAPX + (l1 - l2)*CHARX, 2 * CHARY + 3 * GAPY + LAST - CHARY / 2, 255, 0, img);
      }

    if (l1 < l2) Text(smin, GAPX + (l2 - l1)*CHARX,
                        2 * CHARY + 3 * GAPY + LAST + sizey - CHARY / 2, 255, 0, img);
    else  Text(smin, GAPX,
                 2 * CHARY + 3 * GAPY + LAST + sizey - CHARY / 2, 255, 0, img);

    l1 = strlen(fi->strx);
    Text(fi->strx, l * CHARX + 2 * GAPX + fi->nbr + LAST - l1 * CHARX,
         3 * CHARY + 5 * GAPY + LAST + sizey, 255, 0, img);
    Text(fi->stry, GAPX, CHARY + 2 * GAPY, 255, 0, img);

    for (i = 0; i < fi->nbr; i++)
      Line(l * CHARX + 2 * GAPX + i + 1, 2 * CHARY + 3 * GAPY + LAST + sizey,
           l * CHARX + 2 * GAPX + i + 1, 2 * CHARY + 3 * GAPY + LAST + sizey -
           (int)((fi->entry[i] - min) / diff * (double)(sizey)), 255, DEFAULT, img);

    sprintf(down, "%g", fi->upper);
    sprintf(top, "%g", fi->upper + (double)(fi->nbr - 1)*fi->step);
    Text(down, l * CHARX + 2 * GAPX, 2 * CHARY + 4 * GAPY + LAST + sizey, 255, 0, img);
    l1 = strlen(top);
    Text(top, 3 * GAPX + l * CHARX + fi->nbr + LAST - l1 * CHARX,
         2 * CHARY + 4 * GAPY + sizey + LAST, 255, 0, img);
    l1 = strlen(fi->str);
    h = ((3 * GAPX + CHARX + fi->nbr + LAST) / CHARX / 2 - l1 / 2) * CHARX;
    Text(fi->str, h, GAPY, 255, 0, img);
    Line(2 * GAPX + CHARX * l - 2, 2 * CHARY + 3 * GAPY + LAST,
         2 * GAPX + CHARX * l + 2, 2 * CHARY + 3 * GAPY + LAST, 255, DEFAULT, img);
    Line(2 * GAPX + CHARX * l + fi->nbr, 2 * CHARY + 3 * GAPY + sizey + LAST - 2,
         2 * GAPX + CHARX * l + fi->nbr, 2 * CHARY + 3 * GAPY + sizey + LAST + 2, 255, DEFAULT, img);
    return img;
  }
#undef FNAME
  /******************************************************************/
#define FNAME "FreeFuncImage1D"
  int FreeFuncImage1D(FuncImage1D fi)
  {
    if (fi->entry != NULL)
      {
        free(fi->entry);
      }

    if (fi != NULL)
      {
        free(fi);
      }

    return OK;
  }
#undef FNAME
}
