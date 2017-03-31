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
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <limits.h>

#ifndef WIN32
#include <stdlib.h>
#endif

#include "defs.h"
#include "message.h"
#include "macro.h"
#include "conturfunctions.h"
#include "histogram.h"
#include "arith.h"
#include "fitfn.h"
#include "filter.h"

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
// die Anzahl der Nachbarn des Aufsetzpunktes mit einem Grauwert!=0 zurck.

  int GetEnviron(Image img, int x, int y, int dx, int dy, int mask[])
  {
    int xx, yy, xi, yi, xa, ya, index = 0, count = 0;

    xi = -(dx - 1) / 2 + x;
    yi = -(dy - 1) / 2 + y;
    xa = (dx - 1) / 2 + x;
    ya = (dy - 1) / 2 + y;

    for (yy = yi; yy <= ya; yy++)
      for (xx = xi; xx <= xa; xx++)
        {
          if (!mask[index] || xx > img->xsize || yy > img->ysize || xx < 0 || yy < 0) mask[index] = -1;
          else if ((mask[index] = GetVal(img, xx, yy)) && (abs(xx - x) < 2) && (abs(yy - y) < 2)) count++;

          index++;
        }

    return count - 1;
  }

/////////////////////////////////////////////////////////////////////////
// Bestimmen der 0 zu >0 Uebergaenge in der Zahlenfolge p1,p2,...,p8,p1

  int Trans(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
  {
    int count = 0;

    if (p2 > 0 && p1 == 0) count = 1;

    if (p3 > 0 && p2 == 0) count++;

    if (p4 > 0 && p3 == 0) count++;

    if (p5 > 0 && p4 == 0) count++;

    if (p6 > 0 && p5 == 0) count++;

    if (p7 > 0 && p6 == 0) count++;

    if (p8 > 0 && p7 == 0) count++;

    if (p1 > 0 && p8 == 0) count++;

    return count;
  }

  /**********************************************************************

      Thinning nach JAIN
      Die Objekte im Binaerbild img1 werden verduennt.
      Ist der Zeiger auf das Ergebnisbild (img2) NULL, so wird das Ergebnis-
      bild neu angelegt. Bei fehlerfreier Ausfuehrung wird ein Zeiger auf
      das Ergebnisbild, ansonsten NULL zurueckgegeben.

  ***********************************************************************/

#define FNAME "Thinning"

  int Thinning(Image img1, Image img2)
  {
    Image imgs, imgd;
    int x, y, changed, neighbors, m[5][5];
    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(img1, img2, dx, dy));

    imgd = NewImg(img1->xsize, img1->ysize, 1);

    if (!IsImg(imgd))
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    imgs = img2;

    wloop(img1, x, y)
    {
      if (GetVal(img1, x, y) > 0) PutVal(imgd, x, y, 1);
      else PutVal(imgd, x, y, 0);
    }

    do
      {
        CopyImg(imgd, imgs);
        changed = 0;

        for (x = 2; x < imgd->xsize - 2; x++)
          for (y = 2; y < imgd->ysize - 2; y++)
            if (GetVal(imgs, x, y))
              {
                memset(m, 1, 25 * sizeof(int));
                neighbors = GetEnviron(imgs, x, y, 5, 5, &m[0][0]);

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

    wloop(imgs, x, y)
    {
      memset(m, 1, 25 * sizeof(int));

      if (GetVal(imgs, x, y) > 0)
        {
          int val = imgs->maxval;

          if ((neighbors = GetEnviron(imgs, x, y, 3, 3, &m[0][0])) > 1)
            {
              for (int i = 0; i < 5; i++)
                for (int j = 0; j < 5; j++)
                  if (m[i][j] == -1) m[i][j] = 0;

              if (Trans(m[0][0], m[0][1], m[0][2], m[1][0], m[1][3], m[1][2], m[1][1], m[0][3]) == 2 &&
                  (!m[0][0] || !m[1][3] || (m[0][1] && m[1][0]) || (m[0][3] && m[1][2])) &&
                  (!m[0][2] || !m[1][1] || (m[1][0] && m[1][2]) || (m[0][1] && m[0][3])) &&
                  (!m[0][1] || !m[1][2]) &&
                  (!m[1][0] || !m[0][3]) &&
                  ((m[0][1] && m[0][3]) ||
                   (m[0][1] && m[1][0]) ||
                   (m[1][0] && m[1][2]) ||
                   (m[1][2] && m[0][3])))
                val = 0;
            }

          PutVal(imgs, x, y, val);
        }
    }

    FreeImg(imgd);
    return OK;
  }
#undef FNAME

  /**********************************************************************

        Interne Funktion fuer Skeleton

  ***********************************************************************/


//////////////////////////////////////////////////////////////////////////
// Rotation eines Bildes
// (typ=RI_90GRAD,RI_180GRAD,RI_270GRAD)
// Achtung! Bildzeiger wird veraendert!

#define RI_90GRAD   1
#define RI_180GRAD  2
#define RI_270GRAD  3

#define FNAME "RotateImg"

  int RotateImg(Image& img, short typ)
  {
    if (!IsImg(img) || (typ != RI_90GRAD && typ != RI_180GRAD && typ != RI_270GRAD))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return false;
      }

    int val, xx, yy, x, y;

    Image img2;

    if (typ != RI_180GRAD)
      {
        img2 = NewImg(img->ysize, img->xsize, img->maxval);

        if (!IsImg(img2))
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return false;
          }

        if (typ == RI_270GRAD)
          wloop(img, x, y)
          PutVal(img2, img->ysize - y - 1, x, GetVal(img, x, y));
        else
          wloop(img, x, y)
          PutVal(img2, y, img->xsize - x - 1, GetVal(img, x, y));

        FreeImg(img);
        img = img2;

      }
    else
      {

        int yend = (img->ysize % 2 == 0) ? img->ysize / 2 : (img->ysize + 1) / 2;

        for (y = 0; y < yend; y++)
          for (x = 0; x < img->xsize; x++)
            {
              val = GetVal(img, x, y);
              PutVal(img, x, y, GetVal(img, (xx = img->xsize - x - 1), (yy = img->ysize - y - 1)));
              PutVal(img, xx, yy, val);
            }
      }

    return true;
  }

#undef FNAME

  /*********************************************************************

      Skeleton

      Skeletierung des Binaerbildes img nach _imgd.
      Ist _imgd==NULL wird das Ergebnisbild neu angelegt.
      Rueckgabe: NULL bei Fehler, sonst Zeiger auf das Ergebnisbild.
      mode3 gibt die Richtung (VERT,HORZ oder HORZ_VERT) an.
      Mit mode wird die Behandlung der Minima angegeben:
          Eintragen des ersten Minima eines Plateus: FIRST_EXTREMA
          letztes:                                   LAST_EXTREMA
          mittlerer:                                 MID_EXTREMA
          Gesamtes Plateau:                          ALL_EXTREMA
      Dito mode2 fuer die Maxima.
      thresh gibt die Grauwertdifferenz an, um die sich zwei
      aufeinanderfolgende Extrema unterscheiden mussen.
      diff ist nur im Zusammenhang mit dem Modus ALL_EXTREMA fuer
      die Maxima interessant. diff muss aus [0,1] sein und bestimmt
      die relative zulaessige Grauwertdifferenz fuer Pixel neben dem
      Plateau, die noch zum Plateau zugehoerig zaehlen.


  **********************************************************************/

#define FNAME "Skeleton"

  int Skeleton(Image img,
               int mode, int mode2, int mode3,
               int thresh, double diff, Image imgd)
  {
    // Parameter pruefen
    if (!IsImg(img) ||
        (fabs(diff) >= 1) ||
        (thresh < 0 || thresh >= img->maxval) ||
        (mode != ALL_EXTREMA && mode != FIRST_EXTREMA && mode != LAST_EXTREMA && mode != MID_EXTREMA && mode != NO_EXTREMA) ||
        (mode2 != ALL_EXTREMA && mode2 != FIRST_EXTREMA && mode2 != LAST_EXTREMA && mode2 != MID_EXTREMA && mode2 != NO_EXTREMA) ||
        (mode == NO_EXTREMA && mode2 == NO_EXTREMA))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // Initalisierung

    if (!IsImg(imgd) || imgd->maxval != 1 || imgd->xsize != img->xsize || imgd->ysize != img->ysize)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    ClearImg(imgd);

    int* vec = NULL, *vecl = NULL, mem = 0, thr, flag = mode3 > HORZ_VERT;

    if (mode3 > HORZ_VERT) mode3 -= HORZ_VERT;

    if (diff < 0)
      {

        vec = (int*)malloc(mem = max(img->xsize, img->ysize) * sizeof(int));

        if (!vec)
          {
            return ERROR;
          }

        vecl = (int*)malloc(mem);

        if (!vecl)
          {
            free(vec);
            return ERROR;
          }
      }

    int lookfor, pos = 0, posl = 0, max = 0, min = INT_MAX, val, l = 0, y, x;
    double d;

    if (mode3 == HORZ || mode3 == HORZ_VERT)
      {

        for (y = 0; y < img->ysize; y++)
          {

            pos = posl = 0;
            lookfor = 0;
            max = min = GetVal(img, 0, y);

            if (diff < 0)
              memset(vec, 0, mem);

            for (x = 1; x < img->xsize; x++)
              {

                val = GetVal(img, x, y);

                if (lookfor != -1) // suche maxima
                  {

                    if (val > max)
                      {
                        max = val;
                        pos = x;
                      }

                    if (val == max)
                      {
                        posl = x;
                      }

                    if (max - val > thresh || (lookfor && x == img->xsize - 1))
                      {

                        // wesentliches Maxima gefunden

                        if (diff > 0 && mode2 != NO_EXTREMA)
                          {

                            d = max * diff;

                            for (l = pos - 1; l >= 0; l--)
                              {
                                if (abs(max - GetVal(img, l, y)) <= d) pos = l;
                                else break;
                              }

                            for (l = posl + 1; l < img->xsize; l++)
                              {
                                if (abs(max - GetVal(img, l, y)) <= d) posl = l;
                                else break;
                              }
                          }

                        if (diff >= 0)
                          {

                            switch (mode2)
                              {

                              case FIRST_EXTREMA :
                                PutVal(imgd, pos, y, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, (pos + posl) / 2, y, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, posl, y, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, l, y, 1);

                                break;
                              }
                          }
                        else
                          {
                            vec[(pos + posl) / 2] = 1;
                            //vecl[(pos+posl)/2]=val;
                          }

                        max = 0;
                        min = val;
                        pos = x;
                        posl = x;
                        lookfor = -1;
                      }
                  }

                if (lookfor != 1)
                  {

                    if (val < min)
                      {
                        min = val;
                        pos = x;
                      }

                    if (val == min)
                      {
                        posl = x;
                      }

                    if (val - min > thresh || (lookfor && x == img->xsize - 1))
                      {

                        if (diff > 0 && mode != NO_EXTREMA)
                          {

                            d = min * diff;

                            for (l = pos - 1; l >= 0; l--)
                              {
                                if (abs(min - GetVal(img, l, y)) <= d) pos = l;
                                else break;
                              }

                            for (l = posl + 1; l < img->xsize; l++)
                              {
                                if (abs(min - GetVal(img, l, y)) <= d) posl = l;
                                else break;
                              }
                          }

                        if (diff >= 0)
                          {

                            switch (mode)
                              {

                              case FIRST_EXTREMA :
                                PutVal(imgd, pos, y, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, (pos + posl) / 2, y, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, posl, y, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, l, y, 1);

                                break;
                              }
                          }
                        else
                          {
                            vec[(pos + posl) / 2] = -1;
                            //vecl[(pos+posl)/2]=pl;
                          }

                        min = INT_MAX;
                        max = val;
                        pos = x;
                        posl = x;
                        lookfor = 1;
                      }
                  }
              }

            if (diff < 0)
              {

                for (x = 0; x < img->xsize; x++)
                  {

                    if (vec[x] != 0)
                      {

                        val = GetVal(img, x, y);
                        max = GetVal(img, 0, y);

                        pos = posl = x;

                        for (l = x - 1; l >= 0; l--)
                          if (vec[l] != 0)
                            {
                              max = GetVal(img, l, y);
                              break;
                            }

                        if (l > 0)
                          {
                            if (vec[x] > 0)
                              {

                                thr = (int)(val - (val - max) * fabs(diff));

                                for (l = x - 1; l >= 0; l--)
                                  if (GetVal(img, l, y) <= thr) break;
                                  else pos = l;

                              }
                            else
                              {

                                thr = (int)(val + (max - val) * fabs(diff));

                                for (l = x - 1; l >= 0; l--)
                                  if (GetVal(img, l, y) >= thr) break;
                                  else pos = l;
                              }
                          }

                        max = GetVal(img, img->xsize - 1, y);

                        for (l = x + 1; l < img->xsize; l++)
                          if (vec[l] != 0)
                            {
                              max = GetVal(img, l, y);
                              break;
                            }

                        if (l < img->xsize - 1)
                          {

                            if (vec[x] > 0)
                              {

                                thr = (int)(val - (val - max) * fabs(diff));

                                for (l = x + 1; l < img->xsize; l++)
                                  if (GetVal(img, l, y) <= thr) break;
                                  else posl = l;

                              }
                            else
                              {

                                thr = (int)(val + (max - val) * fabs(diff));

                                for (l = x + 1; l < img->xsize; l++)
                                  if (GetVal(img, l, y) >= thr) break;
                                  else posl = l;
                              }
                          }

                        if (vec[x] > 0)
                          {

                            switch (mode2)
                              {
                              case FIRST_EXTREMA :
                                PutVal(imgd, pos, y, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, (pos + posl) / 2, y, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, posl, y, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, l, y, 1);

                                break;
                              }
                          }
                        else
                          {

                            switch (mode)
                              {
                              case FIRST_EXTREMA :
                                PutVal(imgd, pos, y, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, (pos + posl) / 2, y, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, posl, y, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, l, y, 1);

                                break;
                              }

                          }
                      }
                  }
              }
          }
      }

    if (mode3 == VERT || mode3 == HORZ_VERT)
      {

        for (x = 0; x < img->xsize; x++)
          {

            pos = posl = 0;
            lookfor = 0;
            max = min = GetVal(img, x, 0);

            if (diff < 0)
              memset(vec, 0, mem);

            for (y = 1; y < img->ysize; y++)
              {

                val = GetVal(img, x, y);

                if (lookfor == 1) // suche maxima
                  {

                    if (val > max)
                      {
                        max = val;
                        pos = y;
                      }

                    if (val == max)
                      {
                        posl = y;
                      }

                    if (max - val > thresh || (lookfor && y == img->ysize - 1))
                      {

                        // wesentliches Maxima gefunden
                        if (diff > 0 && mode2 != NO_EXTREMA)
                          {

                            d = max * diff;

                            for (l = pos - 1; l >= 0; l--)
                              {
                                if (abs(max - GetVal(img, x, l)) <= d) pos = l;
                                else break;
                              }

                            for (l = posl + 1; l < img->ysize; l++)
                              {
                                if (abs(max - GetVal(img, x, l)) <= d) posl = l;
                                else break;
                              }
                          }

                        if (diff >= 0)
                          {

                            switch (mode2)
                              {

                              case FIRST_EXTREMA :
                                PutVal(imgd, x, pos, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, x, (pos + posl) / 2, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, x, posl, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, x, l, 1);

                                break;
                              }
                          }
                        else
                          {
                            vec[(pos + posl) / 2] = 1;
                            //vecl[(pos+posl)/2]=posl;
                          }

                        max = 0;
                        min = val;
                        pos = y;
                        posl = y;
                        lookfor = -1;
                      }
                  }
                else
                  {

                    if (val < min)
                      {
                        min = val;
                        pos = y;
                      }

                    if (val == min)
                      {
                        posl = y;
                      }

                    if (val - min > thresh || (lookfor && y == img->ysize - 1))
                      {

                        if (diff > 0 && mode != NO_EXTREMA)
                          {

                            d = min * diff;

                            for (l = pos - 1; l >= 0; l--)
                              {
                                if (abs(min - GetVal(img, x, l)) <= d) pos = l;
                                else break;
                              }

                            for (l = posl + 1; l < img->ysize; l++)
                              {
                                if (abs(min - GetVal(img, x, l)) <= d) posl = l;
                                else break;
                              }
                          }

                        if (diff >= 0)
                          {

                            switch (mode)
                              {

                              case FIRST_EXTREMA :
                                PutVal(imgd, x, pos, 1);
                                break;
                              case MID_EXTREMA :
                                PutVal(imgd, x, (pos + posl) / 2, 1);
                                break;
                              case LAST_EXTREMA :
                                PutVal(imgd, x, posl, 1);
                                break;
                              case ALL_EXTREMA :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, x, l, 1);

                                break;
                              }
                          }
                        else
                          {
                            vec[(pos + posl) / 2] = -1;
                            //vecl[(pos+posl)/2]=posl;
                          }

                        min = INT_MAX;
                        max = val;
                        pos = y;
                        posl = y;
                        lookfor = 1;
                      } // if (minima gefunden)

                  } // else fuer Suche nach minima

              } // for (y)

            if (diff < 0)
              {

                for (y = 0; y < img->ysize; y++)
                  {

                    if (vec[y] != 0)
                      {

                        val = GetVal(img, x, y);
                        max = GetVal(img, x, 0);

                        pos = posl = y;

                        for (l = y - 1; l >= 0; l--)
                          if (vec[l] != 0)
                            {
                              max = GetVal(img, x, l);
                              break;
                            }

                        if (l > 0)
                          {

                            if (vec[y] > 0)
                              {

                                thr = (int)(val - (val - max) * fabs(diff));

                                for (l = y - 1; l >= 0; l--)
                                  if (GetVal(img, x, l) <= thr) break;
                                  else pos = l;

                              }
                            else
                              {

                                thr = (int)(val + (max - val) * fabs(diff));

                                for (l = y - 1; l >= 0; l--)
                                  if (GetVal(img, x, l) >= thr) break;
                                  else pos = l;
                              }
                          }

                        max = GetVal(img, x, img->ysize - 1);

                        for (l = y + 1; l < img->ysize; l++)
                          if (vec[l] != 0)
                            {
                              max = GetVal(img, x, l);
                              break;
                            }

                        if (l < img->ysize - 1)
                          {

                            if (vec[y] > 0)
                              {

                                thr = (int)(val - (val - max) * fabs(diff));

                                for (l = y + 1; l < img->ysize; l++)
                                  if (GetVal(img, x, l) <= thr) break;
                                  else posl = l;

                              }
                            else
                              {

                                thr = (int)(val + (max - val) * fabs(diff));

                                for (l = y + 1; l < img->ysize; l++)
                                  if (GetVal(img, x, l) >= thr) break;
                                  else posl = l;
                              }
                          }

                        if (vec[y] > 0)
                          {

                            switch (mode2)
                              {
                              case FIRST_EXTREMA :
                                PutVal(imgd, x, pos, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, x, (pos + posl) / 2, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, x, posl, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, x, l, 1);

                                break;
                              }
                          }
                        else
                          {

                            switch (mode)
                              {
                              case FIRST_EXTREMA :
                                PutVal(imgd, x, pos, 1);
                                break;
                              case MID_EXTREMA   :
                                PutVal(imgd, x, (pos + posl) / 2, 1);
                                break;
                              case LAST_EXTREMA  :
                                PutVal(imgd, x, posl, 1);
                                break;
                              case ALL_EXTREMA   :

                                for (l = pos; l <= posl; l++)
                                  PutVal(imgd, x, l, 1);

                                break;
                              }

                          }
                      }
                  }
              }
          } // for(x)

      } // if (vertikale suche)

    if (diff < 0)
      {
        free(vec);
        free(vecl);
      }

    if (!flag)
      {

        Image temp, temp2;

        temp = NewImg(img, true);

        if (!IsImg(temp))
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return NO_MEM;
          }

        temp2 = NewImg(temp);

        if (!IsImg(temp2))
          {
            FreeImg(temp);
            Message(FNAME, M_NO_MEM, NO_MEM);
            return NO_MEM;
          }

        if (!RotateImg(temp, RI_180GRAD))
          {
            FreeImg(temp);
            return ERROR;
          }

        Skeleton(temp, (mode == FIRST_EXTREMA) ? LAST_EXTREMA : (mode == LAST_EXTREMA) ? FIRST_EXTREMA : mode, (mode2 == FIRST_EXTREMA) ? LAST_EXTREMA : (mode2 == LAST_EXTREMA) ? FIRST_EXTREMA : mode2, HORZ_VERT + mode3, thresh, diff, temp2);

        FreeImg(temp);

        if (!RotateImg(temp2, RI_180GRAD))
          {
            FreeImg(temp2);
            return ERROR;
          }

        MaxImg(imgd, temp2, imgd);
        FreeImg(temp2);
      }

    return OK;
  }

#undef FNAME

  /**********************************************************************

      Histogrammausgleich

  ***********************************************************************/

#define FNAME "HistogramEqual"
  int HistogramEqual(const Image& img, Image& imgd)
  {
    int dx, dy;
    RETURN_ERROR_IF_FAILED(MatchImg(img, imgd, dx, dy));

    Histogram h(img);

    vector<int> histx(img->maxval + 1);

    double maxval = double(imgd->maxval) / img->xsize / img->ysize;
    int sum = 0;

    for (int i = 0; i <= img->maxval; i++)
      {
        sum += h.getCount(i);
        histx[i] = RoundInt(sum * maxval);
      }

    IPoint p;
    for (p.y = 0; p.y < imgd->ysize; p.y++)
      for (p.x = 0; p.x < imgd->xsize; p.x++)
        {
          PutVal(imgd, p, histx[GetVal(img, p)]);
        }

    return OK;
  }

  int HistogramEqual(Image& imgd)
  {
    return HistogramEqual(imgd, imgd);
  }

#undef FNAME


  /******************************************************************

     Relaxation zur Erstellung lokal kontrastierter Bilder

  *******************************************************************/

///////////////////////////////////////////////////////////////////////////
// Interne Hilfsfunktion
// Bestimmt minimalen/maximalen Grauwert in der se x se - Umgebung des
// Bildpunktes (x,y) im Bild img.

  void EnvColors(Image img, int x, int y, int se, int& mini, int& maxi)
  {
    int val, xi, yi, xa, ya;

    xi = max(x - se, 0);
    yi = max(y - se, 0);
    xa = min(x + se, img->xsize - 1);
    ya = min(y + se, img->ysize - 1);
    maxi = 0;
    mini = img->maxval;

    for (int xx = xi; xx <= xa; xx++)
      for (int yy = yi; yy <= ya; yy++)
        {
          val = GetVal(img, xx, yy);

          if (val > maxi) maxi = val;

          if (val < mini) mini = val;
        }
  }

#define FNAME "RelaxImg"
  int RelaxImg(const Image& img, const Image& imgd, int n)
  {
    int dx, dy;
    RETURN_ERROR_IF_FAILED(MatchImg(img, imgd, dx, dy));
    Image maxImage;
    maxImage.create(img);
    Image minImage;
    minImage.create(img);
    MinMaxImg(img, n, n, minImage, maxImage);

    for (int y = 0; y < dy; y++)
      for (int x = 0; x < dx; x++)
        {
          int min = minImage.getPixel(x, y);
          int max = maxImage.getPixel(x, y);
          int mean = (max + min) / 2;
          if (img.getPixel(x, y) < mean)
            imgd.setPixel(x, y, min);
          else
            imgd.setPixel(x, y, max);
        }

    return OK;
  }

#undef FNAME
}
