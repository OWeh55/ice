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
// Peaksuche - 1998 Torsten Baumbach

#include <cstdlib>
#include <float.h>
#include <limits.h>

#include "WindowWalker.h"
#include "PeakDtct.h"
#include "MtchTool.h"
#include "DPList.h"
#include "MaxSrch.h"

using namespace std;

namespace ice
{
  static MaxSearch ms = nullptr;

#ifndef GetValDef
#define GetValDef(img,x,y) (((x)>=0&&(y)>=0&&(x)<(img)->xsize&&(y)<(img)->ysize)?GetVal(img,x,y):0)
#endif

#define GetVal_zykl(img,x,y) GetVal(img,(x+img->xsize)%(img->xsize),(y+img->ysize)%(img->ysize))

#define ReallyFramePoint(mask,x,y) (!GetValDef(mask,x-1,y-1) || !GetValDef(mask,x-1,y  ) || !GetValDef(mask,x-1,y+1) || !GetValDef(mask,x  ,y-1) || !GetValDef(mask,x  ,y+1) || !GetValDef(mask,x+1,y-1) || !GetValDef(mask,x+1,y  ) || !GetValDef(mask,x+1,y+1))

#define ReallyFramePoint_zykl(mask,x,y)  (!GetVal_zykl(mask,x-1,y-1) || !GetVal_zykl(mask,x-1,y  ) || !GetVal_zykl(mask,x-1,y+1) || !GetVal_zykl(mask,x  ,y-1) || !GetVal_zykl(mask,x  ,y+1) || !GetVal_zykl(mask,x+1,y-1) || !GetVal_zykl(mask,x+1,y  ) || !GetVal_zykl(mask,x+1,y+1))

#define SORT_X    1
#define SORT_Y    2
#define SORT_VAL  3

#define FNAME "PointHeapPeakDetect"
  // heap class for peak detection - obsolete, don't use this outside this module
  class PointHeapPeakDetect
  {
  public:
    PointHeapPeakDetect(int sortmode)
    {
      if (sortmode != SORT_X && sortmode != SORT_Y &&
          sortmode != SORT_VAL)
        {
          Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        }

      xptr = yptr = wptr = vptr = nullptr;
      lng = 0;
      reslng = 0;
      mode = sortmode;
    }

    ~PointHeapPeakDetect()
    {
      if (xptr != nullptr) free(xptr);
      if (yptr != nullptr) free(yptr);
      if (wptr != nullptr) free(wptr);
    }

    int insert(int x, int y, int val)
    {
      if (lng == reslng)
        {

          int* ptr;

          if (xptr != nullptr)
            ptr = (int*)realloc(xptr, reslng * sizeof(int) + 100 * sizeof(int));
          else
            ptr = (int*)malloc(100 * sizeof(int));

          if (ptr == nullptr)
            {
              Message(FNAME, M_NO_MEM, NO_MEM);
              return ERROR;
            }

          xptr = ptr;

          if (mode == SORT_X) vptr = xptr;

          if (yptr != nullptr)
            ptr = (int*)realloc(yptr, reslng * sizeof(int) + 100 * sizeof(int));
          else
            ptr = (int*)malloc(100 * sizeof(int));

          if (ptr == nullptr)
            {
              Message(FNAME, M_NO_MEM, NO_MEM);
              return ERROR;
            }

          yptr = ptr;

          if (mode == SORT_Y) vptr = yptr;

          if (wptr != nullptr)
            ptr = (int*)realloc(wptr, reslng * sizeof(int) + 100 * sizeof(int));
          else
            ptr = (int*)malloc(100 * sizeof(int));

          if (ptr == nullptr)
            {
              Message(FNAME, M_NO_MEM, NO_MEM);
              return ERROR;
            }

          wptr = ptr;

          if (mode == SORT_VAL) vptr = wptr;

          reslng += 100;
        }

      xptr[lng] = x;
      yptr[lng] = y;
      wptr[lng] = val;

      lng++;

      upsort();
      return OK;
    }

    int getMaximum(int& x, int& y, int& val) const
    {
      if (lng == 0) return ERROR;

      x  = xptr[0];
      y  = yptr[0];
      val = wptr[0];
      return OK;
    }

    void delMaximum()
    {
      if (lng > 1)
        {
          xptr[0] = xptr[lng - 1];
          yptr[0] = yptr[lng - 1];
          wptr[0] = wptr[lng - 1];
        }

      lng--;
      downsort();
    }

    PointHeapPeakDetect& operator =(const PointHeapPeakDetect&) = delete;

  private:

    void upsort()
    {
      if (lng < 2) return;

      int pos_child = lng - 1, pos_parent = (lng - 2) / 2, x, y, w;

      while (pos_child > 0 && vptr[pos_child] > vptr[pos_parent])
        {
          x = xptr[pos_child];
          y = yptr[pos_child];
          w = wptr[pos_child];
          xptr[pos_child] = xptr[pos_parent];
          yptr[pos_child] = yptr[pos_parent];
          wptr[pos_child] = wptr[pos_parent];
          xptr[pos_parent] = x;
          yptr[pos_parent] = y;
          wptr[pos_parent] = w;
          pos_child = pos_parent;
          pos_parent = (pos_child - 1) / 2;
        }
    }

    void downsort()
    {
      if (lng < 2) return;

      int pos_child1 = 1, pos_child2 = 2, pos_next, pos_parent = 0, x, y, w;

      while (pos_child1 < lng)
        {
          if (pos_child2 == lng)
            {
              if (vptr[pos_child1] <= vptr[pos_parent]) break;
              else pos_next = pos_child1;
            }
          else
            {
              if (vptr[pos_child1] <= vptr[pos_parent] &&
                  vptr[pos_child2] <= vptr[pos_parent]) break;

              pos_next = (vptr[pos_child1] > vptr[pos_child2]) ? pos_child1 : pos_child2;
            }

          x = xptr[pos_next];
          y = yptr[pos_next];
          w = wptr[pos_next];
          xptr[pos_next] = xptr[pos_parent];
          yptr[pos_next] = yptr[pos_parent];
          wptr[pos_next] = wptr[pos_parent];
          xptr[pos_parent] = x;
          yptr[pos_parent] = y;
          wptr[pos_parent] = w;
          pos_parent = pos_next;
          pos_child1 = pos_parent + pos_parent + 1;
          pos_child2 = pos_child1 + 1;
        }
    }

    int*  xptr;
    int*  yptr;
    int*  wptr;
    int*  vptr;

    int   lng;
    int   reslng;
    int   mode;
  };
#undef FNAME
// ---------------------------------------------------------------------
// Anlegen einer neuen _PeakList-Struktur, initialisieren der Member und
// Rückgabe eines Zeigers auf diese Struktur
// Bei Fehler (Speichermangel) ist Rückgabe == nullptr
// ---------------------------------------------------------------------
#define FNAME "NewPeakList"
  PeakList NewPeakList()
  {
    PeakList pl = new _PeakList;

    if (pl == nullptr)
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return nullptr;
      }

    pl->next = nullptr;
    pl->prev = nullptr;
    pl->x = pl->y = pl->grw = -1;
    pl->sx = pl->sy = pl->sxy = -1;

    return pl;
  }
#undef FNAME

// -------------------------------------------------------------------------
// Interne Funktionen zur Peaksuche
// -------------------------------------------------------------------------

  void Get2DGaussParams(Image img, Image imgo,
                        double x0, double y0, int wxi, int wyi, int wxa, int wya,
                        double& sx, double& sy, double& sxy, int xmin)
  {

    int x, y;
    int g, c = 0;
    double volume = 0;

    for (x = wxi; x <= wxa; x++)
      for (y = wyi; y <= wya; y++)
        if (GetVal(imgo, x, y))
          {
            c++;
            volume += GetVal(img, x, y);
          }

    volume -= (c * xmin);

    sx = sy = sxy = 0;

    for (x = wxi; x <= wxa; x++)
      for (y = wyi; y <= wya; y++)
        if (GetVal(imgo, x, y))
          {
            sx += Sqr(x - x0) * (g = (GetVal(img, x, y) - xmin));
            sy += Sqr(y - y0) * g;
            sxy += (x - x0) * (y - y0) * g;
          }

    if (volume != 0)
      {
        sx /= volume;
        sx = sqrt(sx);
        sy /= volume;
        sy = sqrt(sy);

        if (sx * sy != 0) sxy /= volume * sx * sy;
        else sxy = 0;
      }
    else
      {
        sx = INT_MAX;
        sy = INT_MAX;
        sxy = 0;
      }
  }

  const int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1};
  const int dy[8] = { -1, -1, -1, 0, 0, 1, 1, 1};

  const int adx[8] = { -1, 1, 1, 0, 0, -1, -1, 0};
  const int ady[8] = { -1, 0, 0, 1, 1, 0, 0, -1};

  int FindPeakArea(Image orig, Image mask, Image mask2,
                   int xp, int yp, int gp, int zykl,
                   int& wxi, int& wyi, int& wxa, int& wya,
                   long& count, PointHeapPeakDetect& heap_frame,
                   int& c_frame, int& xmin, int noise)
  {
    DPointList pl = NewDPointList();
    PointHeapPeakDetect heap(SORT_VAL);

    if (heap.insert(xp, yp, gp) != OK)
      {
        FreePointList(pl);
        return false;
      }

    int xx, yy, x, y, g, gg, gn, flag = false;

    count = 1;
    wxi = wxa = xp;
    wyi = wya = yp;
    c_frame = 0;
    xmin = orig->maxval;

    PutVal(mask, xp, yp, 1);

    if (!GetVal(mask2, xp, yp))
      {
        if (ms != nullptr) RemovePointFromMaxSearch(ms, xp, GetVal(orig, xp, yp));

        PutVal(mask2, xp, yp, 1);
      }

    if (!zykl)   // --- nicht-zyklisches Bildmodell ---------------------------
      {

        while (heap.getMaximum(x, y, g) == OK)
          {

            heap.delMaximum();
            gn = g + noise;

            for (int r = 0; r < 8; r++)
              {
                xx = x + dx[r];
                yy = y + dy[r];

                if (xx >= 0 && yy >= 0 && xx < orig->xsize && yy < orig->ysize)
                  {
                    if (!GetVal(mask, xx, yy))
                      {
                        if ((gg = GetVal(orig, xx, yy)) <= gn)
                          {

                            if (heap.insert(xx, yy, min(gg, g)) != OK)
                              {
                                FreePointList(pl);
                                return false;
                              }

                            count++;
                            PutVal(mask, xx, yy, 1);

                            if (!GetVal(mask2, xx, yy))
                              {
                                if (ms != nullptr) RemovePointFromMaxSearch(ms, xx, gg);

                                PutVal(mask2, xx, yy, 1);
                              }

                            if (wxi > xx) wxi = xx;

                            if (wyi > yy) wyi = yy;

                            if (wxa < xx) wxa = xx;

                            if (wya < yy) wya = yy;

                            if (gg < xmin) xmin = gg;
                          }
                        else flag = true;
                      }
                  }
              }

            if (flag)
              {
                if (heap_frame.insert(x, y, g) != OK)
                  {
                    FreePointList(pl);
                    return false;
                  }

                c_frame++;
                flag = false;

                if (GetVal(orig, x, y) > g)
                  {
                    if (!DAddPoint(pl, x, y, g))
                      {
                        FreePointList(pl);
                        return false;
                      }
                  }
              }
          }
      }
    else   // ---- zyklisches Bildmodell --------------------------------------
      {

        while (heap.getMaximum(x, y, g) == OK)
          {

            heap.delMaximum();
            gn = g + noise;

            for (int r = 0; r < 8; r++)
              {
                xx = (x + dx[r] + orig->xsize) % (orig->xsize);
                yy = (y + dy[r] + orig->ysize) % (orig->ysize);

                if (!GetVal(mask, xx, yy))
                  {
                    if ((gg = GetVal(orig, xx, yy)) <= gn)
                      {

                        if (heap.insert(xx, yy, min(gg, g)) != OK)
                          {
                            FreePointList(pl);
                            return false;
                          }

                        count++;
                        PutVal(mask, xx, yy, 1);

                        if (!GetVal(mask2, xx, yy))
                          {
                            if (ms != nullptr) RemovePointFromMaxSearch(ms, xx, gg);

                            PutVal(mask2, xx, yy, 1);
                          }

                        if (wxi > xx) wxi = xx;

                        if (wyi > yy) wyi = yy;

                        if (wxa < xx) wxa = xx;

                        if (wya < yy) wya = yy;

                        if (gg < xmin) xmin = gg;
                      }
                    else flag = true;
                  }
              }

            if (flag)
              {
                if (heap_frame.insert(x, y, g) != OK)
                  {
                    FreePointList(pl);
                    return false;
                  }

                c_frame++;
                flag = false;

                if (GetVal(orig, x, y) > g)
                  {
                    if (!DAddPoint(pl, x, y, g))
                      {
                        FreePointList(pl);
                        return false;
                      }
                  }
              }
          }
      }

    FreePointList(pl);
    return true;
  }

  int ResizePeakArea(Image orig, Image mask, int zykl, int gp, int noise, PointHeapPeakDetect& heap)
  {

    int x, y, g, gg, gn;

    if (!zykl)   // --- nicht-zyklisches Bildmodell ---------------------------
      {

        while (heap.getMaximum(x, y, g) == OK)
          {

            heap.delMaximum();
            gn = g + noise;

            for (int r = 0; r < 8; r++)
              {
                x += adx[r];
                y += ady[r];

                if (x >= 0 && y >= 0 && x < orig->xsize && y < orig->ysize)
                  {
                    if (GetVal(mask, x, y))
                      {
                        if ((gg = GetVal(orig, x, y)) <= gn && gg != gp)
                          {
                            if (heap.insert(x, y, min(gg, g)) != OK) return false;

                            PutVal(mask, x, y, 0);
                          }
                      }
                  }
              }
          }
      }
    else   // ---- zyklisches Bildmodell --------------------------------------
      {

        while (heap.getMaximum(x, y, g) == OK)
          {

            heap.delMaximum();
            gn = g + noise;

            for (int r = 0; r < 8; r++)
              {
                x = (x + adx[r] + orig->xsize) % (orig->xsize);
                y = (y + ady[r] + orig->ysize) % (orig->ysize);

                if (GetVal(mask, x, y))
                  {
                    if ((gg = GetVal(orig, x, y)) <= gn && gg != gp)
                      {
                        if (heap.insert(x, y, min(gg, g)) != OK) return false;

                        PutVal(mask, x, y, 0);
                      }
                  }
              }
          }
      }

    return true;
  }

#define TestAndAdd2(x,y,m,pl) if (x>=0 && y>=0 && x<mask->xsize && y<mask->ysize && GetVal(mask,x,y)==1) { PutVal(mask,x,y,2); DAddPoint(pl,x,y,1); }
#define TestAndAdd2_zykl(x,y,m,pl) xxx=(x+mask->xsize)%mask->xsize;yyy=(y+mask->ysize)%mask->ysize; if (GetVal(mask,xxx,yyy)==1) { PutVal(mask,xxx,yyy,2); DAddPoint(pl,xxx,yyy,1); }

  int KeepOnlyPeakArea(Image mask, int xp, int yp,
                       int wxi, int wyi, int wxa, int wya,
                       int zykl)
  {

    int x, y, xxx, yyy;
    PutVal(mask, xp, yp, 2);

    if (zykl)
      {
        DPointList pl = NewDPointList();

        if (pl == nullptr) return false;

        DAddPoint(pl, xp, yp, 0);

        do
          {
            int point = pl->lng - 1;
            x = pl->x;
            y = pl->y;
            TestAndAdd2_zykl(x + 1, y - 1, mask, pl);
            TestAndAdd2_zykl(x + 1, y  , mask, pl);
            TestAndAdd2_zykl(x + 1, y + 1, mask, pl);
            TestAndAdd2_zykl(x  , y - 1, mask, pl);
            TestAndAdd2_zykl(x  , y + 1, mask, pl);
            TestAndAdd2_zykl(x - 1, y - 1, mask, pl);
            TestAndAdd2_zykl(x - 1, y  , mask, pl);
            TestAndAdd2_zykl(x - 1, y + 1, mask, pl);
            DDelPoint(pl, point);
          }
        while (pl->lng > 0);

        FreePointList(pl);

        for (x = wxi; x <= wxa; x++)
          for (y = wyi; y <= wya; y++)
            if (GetVal(mask, x, y) == 2) PutVal(mask, x, y, 1);
            else PutVal(mask, x, y, 0);
      }
    else
      {
        DPointList pl = NewDPointList();

        if (pl == nullptr) return false;

        DAddPoint(pl, xp, yp, 0);

        do
          {
            int point = pl->lng - 1;
            x = pl->x;
            y = pl->y;
            TestAndAdd2(x + 1, y - 1, mask, pl);
            TestAndAdd2(x + 1, y  , mask, pl);
            TestAndAdd2(x + 1, y + 1, mask, pl);
            TestAndAdd2(x  , y - 1, mask, pl);
            TestAndAdd2(x  , y + 1, mask, pl);
            TestAndAdd2(x - 1, y - 1, mask, pl);
            TestAndAdd2(x - 1, y  , mask, pl);
            TestAndAdd2(x - 1, y + 1, mask, pl);
            DDelPoint(pl, point);
          }
        while (pl->lng > 0);

        FreePointList(pl);

        for (x = wxi; x <= wxa; x++)
          for (y = wyi; y <= wya; y++)
            if (GetVal(mask, x, y) == 2) PutVal(mask, x, y, 1);
            else PutVal(mask, x, y, 0);

      }

    return true;
  }

  static int CorrectPeakPos(Image img, int delta, double& xp, double& yp, Image img2)
  {

    // iterativ das Plateau und dessen Mittelwert bestimmen

    double sumx = 0, sumy = 0, sum = 0;

    DPointList pl_temp = NewDPointList();

    if (pl_temp == nullptr) return false;

    DPointList pl_all = NewDPointList();

    if (pl_all == nullptr) return false;

    DAddPoint(pl_temp, (int)Round(xp), (int)Round(yp), 0);

    int g = GetVal(img, (int)Round(xp), (int)Round(yp));

    do
      {
        int point = pl_temp->lng - 1;
        int x = pl_temp->x;
        int y = pl_temp->y;
        PutVal(img2, x, y, 1);

        for (int r = 0; r < 8; r++)
          {
            int xx = x + dx[r];
            int yy = y + dy[r];

            if (xx >= 0 && yy >= 0 && xx < img->xsize && yy < img->ysize &&
                !GetVal(img2, xx, yy) && GetVal(img, xx, yy) >= g - delta)
              {
                if (!DAddPoint(pl_temp, xx, yy, 0))
                  {
                    FreePointList(pl_temp);
                    FreePointList(pl_all);
                    return false;
                  }

                PutVal(img2, xx, yy, 1);
              }
          }

        DDelPoint(pl_temp, point);

        if (!DAddPoint(pl_all, x, y, 0))
          {
            FreePointList(pl_temp);
            FreePointList(pl_all);
            return false;
          }

        sumx += x;
        sumy += y;
        sum++;

      }
    while (pl_temp->lng > 0);

    FreePointList(pl_temp);

    sumx /= sum;
    sumy /= sum;

    // Den Punkt des Plateaus bestimmen, der dem Mittelwert am nächsten ist

    if (GetVal(img2, (int)Round(sumx), (int)Round(sumy)) == 0)
      {

        double mindist = DBL_MAX;
        int minx = pl_all->x, miny = pl_all->y;

        for (int i = 0; i < pl_all->lng; i++)
          {
            double dist = Sqr(pl_all->xptr[i] - sumx) + Sqr(pl_all->yptr[i] - sumy);

            if (dist < mindist)
              {
                mindist = dist;
                minx = pl_all->xptr[i];
                miny = pl_all->yptr[i];
              }

            PutVal(img2, pl_all->xptr[i], pl_all->yptr[i], 0);
          }

        xp = minx;
        yp = miny;
      }
    else
      {

        for (int i = 0; i < pl_all->lng; i++)
          PutVal(img2, pl_all->xptr[i], pl_all->yptr[i], 0);

        xp = sumx;
        yp = sumy;
      }

    FreePointList(pl_all);

    return true;
  }

  static int FindPeak(Image img, Image mask1, Image mask2, Image mask3,
                      double& xp, double& yp, int& gp, int zykl,
                      int& wxi, int& wyi, int& wxa, int& wya,
                      long& count, int noise, int& xmin, PointHeapPeakDetect& heap_frame)
  {

    // Neues Maximum im Bild suchen
    gp = -1;

    int ixp, iyp;

    if (!GetMaximum(ms, img, mask2, ixp, iyp, gp)) return false;

    xp = ixp;
    yp = iyp;

    if (!CorrectPeakPos(img, 0, xp, yp, mask3)) return false;

    if (IsImg(mask1) && IsImg(mask2))
      {
        // Maximalen Peakbereich suchen und in mask1 markieren
        int c_frame;
        FindPeakArea(img, mask1, mask2, ixp, iyp, gp, zykl, wxi, wyi, wxa, wya, count, heap_frame, c_frame, xmin, noise);
      }

    return true;
  }

#define FNAME "OptimizeMaskImg"

  int OptimizeMaskImg(Image img, Image src, Image dest,
                      int anz,
                      int wxi, int wyi, int wxa, int wya)
  {

    if (anz == 0) return false;

    int x, y, w, count = 0;

    if (src != dest)
      {
        for (x = wxi; x <= wxa; x++)
          for (y = wyi; y <= wya; y++) PutVal(dest, x, y, GetVal(src, x, y) ? 1 : 0);
      }

    PointHeapPeakDetect heap(SORT_VAL);

    for (x = wxi; x <= wxa; x++)
      for (y = wyi; y <= wya; y++)
        {

          if (!GetVal(src, x, y))
            {
              int g1 = GetValDef(src, x - 1, y - 1) ? 1 : 0;
              int g2 = GetValDef(src, x  , y - 1) ? 1 : 0;
              int g3 = GetValDef(src, x + 1, y - 1) ? 1 : 0;
              int g4 = GetValDef(src, x + 1, y) ? 1 : 0;
              int g5 = GetValDef(src, x + 1, y + 1) ? 1 : 0;
              int g6 = GetValDef(src, x  , y + 1) ? 1 : 0;
              int g7 = GetValDef(src, x - 1, y + 1) ? 1 : 0;
              int g8 = GetValDef(src, x - 1, y) ? 1 : 0;
              int s1 = g2 - g1, s2 = g3 - g2, s3 = g4 - g3, s4 = g5 - g4, s5 = g6 - g5, s6 = g7 - g6, s7 = g8 - g7, s8 = g1 - g8;
              int step = abs(s1) + abs(s2) + abs(s3) + abs(s4) + abs(s5) + abs(s6) + abs(s7) + abs(s8);

              if ((step == 0 && g1 > 0) || (step > 2) || (g1 + g2 + g3 + g4 + g5 + g6 + g7 + g8 == 7))
                {

                  if (heap.insert(x, y, GetVal(img, x, y)) != OK) return false;

                  count++;
                }
            }
        }

    if (count <= anz) return false;

    for (int pn = 0; pn < anz; pn++) heap.delMaximum();

    while (heap.getMaximum(x, y, w) == OK)
      {
        heap.delMaximum();
        PutVal(dest, x, y, 1);
      }

    return true;
  }

#undef FNAME

// Auffinden aller Peaks im Bild mittels eines optimierten Verfahrens

  static int DelObjectFromPoint(Image mask,
                                int x, int y,
                                double* pmx, double* pmy, int zykl)
  {

    DPointList pl = NewDPointList();

    if (pl == nullptr) return false;

    if (!DAddPoint(pl, x, y, 1))
      {
        FreePointList(pl);
        return false;
      }

    int mc = 1;

    if (pmx != nullptr)
      {
        *pmx = x;
        *pmy = y;
      }

    int point = 0;

    if (zykl)
      {
        do
          {
            int x = pl->xptr[point], y = pl->yptr[point];

            for (int r = 0; r < 8; r++)
              {
                int xx = x + dx[r], yy = y + dy[r];
                int xxx = (xx + mask->xsize) % (mask->xsize), yyy = (yy + mask->ysize) % (mask->ysize);

                if (GetVal(mask, xxx, yyy) == 1)
                  {
                    if (!DAddPoint(pl, xxx, yyy, 0))
                      {
                        FreePointList(pl);
                        return false;
                      }

                    if (pmx != nullptr)
                      {
                        *pmx += xx;
                        *pmy += yy;
                        mc++;
                      }

                    PutVal(mask, xxx, yyy, 2);
                  }
              }

            point++;

          }
        while (point < pl->lng);
      }
    else
      {
        do
          {
            int x = pl->xptr[point], y = pl->yptr[point];

            for (int r = 0; r < 8; r++)
              {
                int xx = x + dx[r], yy = y + dy[r];

                if (GetValDef(mask, xx, yy) == 1)
                  {
                    if (!DAddPoint(pl, xx, yy, 0))
                      {
                        FreePointList(pl);
                        return false;
                      }

                    if (pmx != nullptr)
                      {
                        *pmx += xx;
                        *pmy += yy;
                        mc++;
                      }

                    PutVal(mask, xx, yy, 2);
                  }
              }

            point++;

          }
        while (point < pl->lng);
      }

    if (pmx != nullptr)
      {

        *pmx /= mc;
        *pmy /= mc;

        *pmx = fmod(*pmx, mask->xsize);

        if (*pmx < 0) *pmx += mask->xsize;

        *pmy = fmod(*pmy, mask->ysize);

        if (*pmy < 0) *pmy += mask->ysize;

        int imx = (int)Round(*pmx), imy = (int)Round(*pmy);

        if (GetVal(mask, imx, imy) != 2) // Mittelwert liegt nicht auf Plateau
          {
            // Bestimmt naechsten Punkt des Plateaus
            double mindist = DBL_MAX;
            int    pnum = 0;

            for (int i = 0; i < pl->lng; i++)
              {
                double dist = Sqr(*pmx - pl->xptr[i]) + Sqr(*pmy - pl->yptr[i]);

                if (dist < mindist)
                  {
                    mindist = dist;
                    pnum = i;
                  }
              }

            *pmx = pl->xptr[pnum];
            *pmy = pl->yptr[pnum];
          }
      }

    for (int i = 0; i < pl->lng; i++) PutVal(mask, pl->xptr[i], pl->yptr[i], 0);

    FreePointList(pl);

    return true;
  }

#define FNAME "FindAllPeaks"

  static PeakList FindAllPeaks(const Image& img, const Image& mask, int zykl)
  {
    bool noMask = !IsImg(mask);

    Image  mask1;
    mask1.create(img->xsize, img->ysize, 2);
    mask1.set(0);

    // Plateaukandidaten markieren
    if (zykl)
      {
        WindowWalker ww(img);
        for (ww.init(); !ww.ready(); ww.next())
          {
            if (noMask || mask.getPixel(ww) == 0)
              {
                int grw = img.getPixel(ww);
                bool isMax = true;

                for (int r = 0; r < 8 && isMax; r++)
                  {
                    int xx = ww.x + dx[r];
                    int yy = ww.y + dy[r];

                    if ((noMask || mask.getPixel(xx, yy) == 0) &&
                        GetVal_zykl(img, xx, yy) > grw)
                      {
                        isMax = false;
                      }
                  }

                if (isMax)
                  mask1.setPixel(ww, 1);
              }
          }
      }
    else
      {
        WindowWalker ww(img);
        for (ww.init(); !ww.ready(); ww.next())
          {
            if (noMask || mask.getPixel(ww) == 0)
              {
                int grw = img.getPixel(ww);
                bool isMax = true;

                for (int r = 0; r < 8 && isMax; r++)
                  {
                    int xx = ww.x + dx[r];
                    int yy = ww.y + dy[r];

                    if ((noMask || !GetValDef(mask, xx, yy)) && (GetValDef(img, xx, yy) > grw))
                      {
                        isMax = false;
                      }
                  }

                if (isMax)
                  mask1.setPixel(ww, 1);
              }
          }
      }

    // Randplateaus und Plateausenken demarkieren
    if (zykl)
      {
        WindowWalker ww(img);
        for (ww.init(); !ww.ready(); ww.next())
          {
            if (mask1.getPixel(ww) > 0)
              {
                int grw = img.getPixel(ww);
                bool flag = true;

                for (int r = 0; r < 8; r++)
                  {
                    int xx = (ww.x + dx[r] + img->xsize) % (img->xsize);
                    int yy = (ww.y + dy[r] + img->ysize) % (img->ysize);

                    if (!GetVal(mask1, xx, yy) && (!IsImg(mask) || !GetVal(mask, xx, yy))
                        && GetVal(img, xx, yy) == grw)
                      {
                        flag = false;
                        break;
                      }
                  }

                if (!flag)
                  DelObjectFromPoint(mask1, ww.x, ww.y, nullptr, nullptr, true);
              }
          }
      }
    else
      {
        int x, y;
        wloop(mask1, x, y)
        {
          if (GetVal(mask1, x, y))
            {
              int grw = GetVal(img, x, y), flag = true;

              for (int r = 0; r < 8; r++)
                {
                  int xx = x + dx[r];
                  int yy = y + dy[r];

                  if (xx >= 0 && yy >= 0 && xx < img->xsize && yy < img->ysize)
                    {
                      if (!GetVal(mask1, xx, yy) && (!IsImg(mask) || !GetVal(mask, xx, yy))
                          && GetVal(img, xx, yy) == grw)
                        {
                          flag = false;
                          break;
                        }
                    }
                }

              if (!flag) DelObjectFromPoint(mask1, x, y, nullptr, nullptr, false);
            }
        }
      }

    // Plateaus zählen und Peakliste aufbauen
    PeakList pl = nullptr, plseg, next, prev;
    int ii;

    PeakList* index = new PeakList[img->maxval + 1];

    if (index == nullptr)
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        FreeImg(mask1);
        return nullptr;
      }

    for (ii = 0; ii <= img->maxval; ii++)
      index[ii] = nullptr;

    int x, y;
    wloop(mask1, x, y)
    {
      if (GetVal(mask1, x, y))
        {
          plseg = NewPeakList();

          if (plseg == nullptr)
            {
              FreePeakList(pl);
              FreeImg(mask1);
              delete []index;
              return nullptr;
            }

          DelObjectFromPoint(mask1, x, y, &(plseg->x), &(plseg->y), zykl);

          int grw = GetVal(img, (int)Round(plseg->x), (int)Round(plseg->y));
          plseg->grw = grw;

          if (pl == nullptr)
            {
              pl = plseg;
              pl->next = nullptr;
              pl->prev = nullptr;

              for (ii = 0; ii <= grw; ii++) index[ii] = pl;
            }
          else
            {
              if (index[grw] == nullptr) // am Anfang einketten
                {
                  next = pl;
                  pl = plseg;
                  pl->prev = nullptr;
                  pl->next = next;
                  next->prev = pl;

                  for (ii = grw; ii >= 0; ii--)
                    if (index[ii] != nullptr) break;
                    else index[ii] = pl;
                }
              else
                {
                  prev = index[grw];
                  next = index[grw]->next;
                  plseg->prev = prev;
                  plseg->next = next;
                  prev->next = plseg;

                  if (next) next->prev = plseg;

                  for (ii = grw; ii >= 0; ii--)
                    if (index[ii] != prev) break;
                    else index[ii] = plseg;
                }
            }
        }
    }

    delete []index;
    FreeImg(mask1);

    return pl;
  }

#undef FNAME

// ------------------------------------------------------------------------------
// Sucht Peaks in einem Grauwertbild in monoton fallender Folge ihrer Maxima
// maxanz bestimmt die maximale Anzahl der Peaks, die gesucht werden soll
// mingrw bestimmt den Mindestgrauwert eines lokalen Maximum, um als Peak zu gelten
// zykl legt fest, ob die Bilder dem 2fach zyklischen Modell entsprechen
// noise legt die Höhe von Nebenpeaks fest, um als Rauschen zu gelten
// ------------------------------------------------------------------------------
#define FNAME "ImgPeakList"

  PeakList ImgPeakList(const Image& img, const Image& global_mark,
                       int maxanz, int mingrw, int zykl,
                       int noise, int feat, int gnull, int mode)
  {

    if (!IsImg(img) || maxanz < 0 || mingrw < 0 || noise < 0 ||
        mingrw > img->maxval ||
        (IsImg(global_mark) && (img->xsize != global_mark->xsize ||
                                img->ysize != global_mark->ysize)))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return nullptr;
      }

    if (noise == 0 && feat == IPL_NOFEAT)
      {
        PeakList pl = FindAllPeaks(img, global_mark, zykl);

        if (pl == nullptr) return nullptr;

        if (maxanz > 0)
          {
            PeakList ptr = pl;

            for (int i = 0; ptr != nullptr && i < maxanz; i++)
              ptr = ptr->next;

            if (ptr)
              {
                ptr->prev->next = nullptr;
                ptr->prev = nullptr;
                FreePeakList(ptr); // Schwanz löschen
              }
          }

        return pl;
      }

    // Initialisierung

    PeakList pl = nullptr, plseg = nullptr;
    int      peakanz = 0, gp;
    double   xp = 0, yp = 0;

    Image mask1 = Image(), mask2 = Image(), mask3;
    // mask1 enthält alle Punkte, die zum aktuellen Peak gehören
    // mask2 enthält alle Punkte, die zu irgendeinem Peak gehören

    int use_masks = (maxanz != 1 || (feat & IPL_MAXCONTUR) != 0 ||
                     (feat & IPL_MINCONTUR) != 0 || IsImg(global_mark) ||
                     (feat & IPL_STANDARD) != 0);

    if (use_masks)
      {

        RETURN_NULL_IF_FAILED(mask1 = NewImg(img->xsize, img->ysize, 2));
        ClearImg(mask1);
        IF_FAILED(mask2 = NewImg(img->xsize, img->ysize, 1))
        {
          FreeImg(mask1);
          Message(FNAME, M_0, ERROR);
          return nullptr;
        }
        ClearImg(mask2);
      }

    IF_FAILED(mask3 = NewImg(img->xsize, img->ysize, 1))
    {
      FreeImg(mask1);
      FreeImg(mask2);
      Message(FNAME, M_0, NO_MEM);
      return nullptr;
    }
    ClearImg(mask3);

    int wxi, wyi, wxa, wya;
    long count, imgdim = img->xsize * img->ysize;

    ms = NewMaxSearch(img);

    if (ms == nullptr)
      {
        return nullptr;
      }

    int x, y;

    if (IsImg(global_mark))
      {
        for (x = 0; x < img->xsize; x++)
          for (y = 0; y < img->ysize; y++)
            {
              if (GetVal(global_mark, x, y) != 0)
                {
                  imgdim--;
                  PutVal(mask2, x, y, 1);
                  RemovePointFromMaxSearch(ms, x, GetVal(img, x, y));
                }
            }
      }

    // sonderfall : konstantes bild oder alles markiert?

    int g1 = -1, g, flag = false, flag2 = false;

    for (x = 0; x < img->xsize; x++)
      for (y = 0; y < img->ysize; y++)
        {
          if (!IsImg(mask2) || GetVal(mask2, x, y) == 0)
            {
              flag2 = true;
              g = GetVal(img, x, y);

              if (g1 == -1) g1 = g;
              else if (g1 != g)
                {
                  flag = true;
                  x = y = 10000;
                }
            }
        }

    if (!flag || !flag2)
      {
        pl = NewPeakList();

        if (pl == nullptr)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return nullptr;
          }
        else
          {
            pl->x = pl->y = pl->grw = -1;
            pl->sx = pl->sy = pl->sxy = 0;
            pl->next = pl->prev = nullptr;
            return pl;
          }
      }

    // now lets go

    int num = 1, xmin;

    do
      {

        PointHeapPeakDetect heap_frame(SORT_VAL);

        if (!FindPeak(img, mask1, mask2, mask3,
                      xp, yp, gp, zykl, wxi, wyi, wxa, wya, count, noise, xmin, heap_frame)) break;

        num++;

        if (gp < mingrw) break; // Aktuelles Maximum unterschreitet vorgeg. Mindestgrauwert

        // Peak in Liste aufnehmen

        plseg = NewPeakList();

        if (plseg == nullptr) // Speichermangel (Fehler wurde in NewPeakList gemeldet)
          {
            if (pl) FreePeakList(pl);

            FreeMaxSearch(ms);
            return nullptr;
          }

        if (pl == nullptr) // erstes Element der Liste
          {
            pl = plseg;
          }
        else   // an bisherige Liste anhängen
          {
            pl->next = plseg;
            plseg->prev = pl;
            pl = plseg;
          }

        pl->x = xp;
        pl->y = yp;
        pl->grw = gp;

        if ((feat & IPL_MINCONTUR) || (feat & IPL_MAXCONTUR) || (feat & IPL_STANDARD))
          {

            ConturList c;

            if (feat & IPL_MAXCONTUR)
              {

                int ma = max<int> (maxanz, (img->xsize + img->ysize) / 20);
                int rc = OptimizeMaskImg(img, mask1, mask3, ma, wxi, wyi, wxa, wya);

                while (rc)
                  rc = OptimizeMaskImg(img, mask3, mask3, ma, wxi, wyi, wxa, wya);

                IF_FAILED(c = BinObj2ConturList(maxanz ? mask3 : mask1,
                                                RoundInt(xp), RoundInt(yp)))
                {
                  Message(FNAME, M_0, ERROR);

                  if (pl) FreePeakList(pl);

                  FreeMaxSearch(ms);
                  return nullptr;
                }
                pl->contur_max = c;

                if (maxanz)
                  {
                    for (x = wxi; x <= wxa; x++)
                      for (y = wyi; y <= wya; y++) PutVal(mask3, x, y, 0);
                  }
              }

            if ((feat & IPL_STANDARD || feat & IPL_MINCONTUR) && (count == imgdim || ResizePeakArea(img, mask1, zykl, gp, noise, heap_frame)))
              {

                KeepOnlyPeakArea(mask1, RoundInt(xp), RoundInt(yp),
                                 wxi, wyi, wxa, wya, zykl);

                if (feat & IPL_STANDARD)
                  {
                    Get2DGaussParams(img, mask1, xp, yp, wxi, wyi, wxa, wya,
                                     pl->sx, pl->sy, pl->sxy, xmin);
                  }
                else
                  {
                    pl->sx = pl->sy = DBL_MAX;
                    pl->sxy = 0;
                  }

                if (feat & IPL_MINCONTUR)
                  {

                    int ma = max<int> (maxanz, (img->xsize + img->ysize) / 20);
                    int rc = OptimizeMaskImg(img, mask1, mask3, ma, wxi, wyi, wxa, wya);

                    while (rc)
                      rc = OptimizeMaskImg(img, mask3, mask3, ma, wxi, wyi, wxa, wya);

                    IF_FAILED((c = BinObj2ConturList(maxanz ? mask3 : mask1,
                                                     RoundInt(xp), RoundInt(yp))))
                    {
                      Message(FNAME, M_0, ERROR);

                      if (pl) FreePeakList(pl);

                      FreeMaxSearch(ms);
                      return nullptr;
                    }
                    pl->contur = c;

                    if (maxanz)
                      {
                        for (x = wxi; x <= wxa; x++)
                          for (y = wyi; y <= wya; y++)
                            PutVal(mask3, x, y, 0);
                      }
                  }
              }
            else
              {
                pl->sx = pl->sy = pl->sxy = 0;
              }
          }
        else
          {
            pl->sx = pl->sy = pl->sxy = 0;
          }

        if (IsImg(mask1))
          {
            for (x = wxi; x <= wxa; x++)
              for (y = wyi; y <= wya; y++)
                PutVal(mask1, x, y, 0);
          }

        peakanz++;

        if (peakanz == maxanz) break; // Die vorgeg. max. Anzahl von Peaks ist erreicht

      }
    while (1);   // Schleife wird nur durch break's verlassen

    FreeMaxSearch(ms);

    if (pl == nullptr)
      {
        pl = NewPeakList();

        if (pl == nullptr)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return nullptr;
          }
        else
          {
            pl->x = pl->y = pl->grw = -1;
            pl->sx = pl->sy = pl->sxy = 0;
            pl->next = pl->prev = nullptr;
          }
      }

    while (pl->prev != nullptr) pl = pl->prev; // Zeiger auf Listenanfang setzen

    return pl;
  }
#undef FNAME

// ------------------------------------------------------------------------------
// Gibt den gesamten Speicher einer Peakliste frei
// ------------------------------------------------------------------------------
#define FNAME "FreePeakList"

  void FreePeakList(PeakList pl)
  {
    PeakList ptr = pl;

    if (pl == nullptr)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

    while ((ptr->prev) != nullptr) ptr = ptr->prev;

    do
      {

        pl = ptr->next;
        delete ptr;
        ptr = pl;
      }
    while (ptr != nullptr);
  }

#undef FNAME

//----------------------------------------------------------

#define FNAME "PeakConturList"

  ConturList PeakConturList(const Image& img, int x, int y, int flag, int zykl, int noise)
  {
    ConturList c;

    if (!IsImg(img) || x < 0 || y < 0 || x >= img->xsize || y >= img->ysize ||
        (flag != IPL_MAXCONTUR && flag != IPL_MINCONTUR))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return c;
      }

    Image mask1;
    mask1.create(img->xsize, img->ysize, 3);

    int   wxi, wyi, wxa, wya, c_frame = 0, g = GetVal(img, x, y), xmin;
    long  count = 0;

    if (!IsImg(mask1))
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return c;
      }

    ClearImg(mask1);

    ms = nullptr; // Optimierte Maximumsuche ab dem 2.Peak wird nicht benötigt

    // Maximalen Peakbereich suchen und in mask1 markieren
    PointHeapPeakDetect heap_frame(SORT_VAL);
    FindPeakArea(img, mask1, mask1, x, y, g, zykl,
                 wxi, wyi, wxa, wya, count,
                 heap_frame, c_frame, xmin, noise);

    if (flag == IPL_MINCONTUR && count != (img->xsize * img->ysize))
      ResizePeakArea(img, mask1, zykl, g, noise, heap_frame);

    int rc;

    do
      {
        rc = OptimizeMaskImg(img, mask1, mask1, (img->xsize + img->ysize) / 20,
                             wxi, wyi, wxa, wya);
      }
    while (rc);

    IF_FAILED((c = BinObj2ConturList(mask1, x, y)))
    {
      Message(FNAME, M_0, ERROR);
      FreeImg(mask1);
      return c;
    }

    FreeImg(mask1);

    return c;
  }
#undef FNAME
}
