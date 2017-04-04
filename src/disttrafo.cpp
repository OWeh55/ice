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

#include "distance.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "contools.h"
#include "IMatrix.h"
#include "Point.h"

using namespace std;

namespace ice
{
#define FNAME "EDistance"
  int Randpunkt(const Image& inr, vector<IPoint>& la, int x, int y)
  {
    int pnr;
    pnr = inr.getPixel(x, y);

    if (pnr == 0)
      {
        la.push_back(IPoint(x, y));
        pnr = la.size();
        inr.setPixel(x, y, pnr);
      }

    return pnr;
  }

  void Neuerpunkt(vector<IPoint>& la, int x, int y,
                  const Image& inr, int rindex,
                  const Image& dist, int dist2, int step2, double fac,
                  Image& dir, Freeman freeman)
  {
    if (GetVal(inr, x, y) == 0)
      {
        int x0 = 0;
        int y0 = 0;
        la.push_back(IPoint(x, y));
        PutVal(inr, x, y, rindex);

        if (step2 == 0) PutVal(dist, x + x0, y + y0,
                                 Min(RoundInt(sqrt((double)dist2)*fac), dist->maxval));
        else
          {
            if (step2 < 0)
              {
                if (dist2 <= -step2)
                  {
                    PutVal(dist, x + x0, y + y0, dist->maxval);
                  }
              }
            else
              {
                if (dist2 > step2)
                  {
                    PutVal(dist, x + x0, y + y0, dist->maxval);
                  }
              }
          }

        if (IsImg(dir))
          {
            PutVal(dir, x, y, freeman.Int());
          }
      }
  }

  int DistanceTransform(const Image& orig, const Image& dist,
                        int mode, double step,
                        int pegl, double fac,
                        Image dir)
  {
    int xs, ys;
    double maxdist;

    if (!IsImg(orig))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }

    if (!IsImg(dist))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
      }

    if (orig == dist)
      {
        throw IceException(FNAME, M_SAME_IMAGE, WRONG_PARAM);
      }

    RETURN_ERROR_IF_FAILED(MatchImg(orig, dist, xs, ys));

    maxdist = RoundInt(sqrt((double)(xs * xs + ys * ys)));

    if (fac <= 0.0)
      {
        fac = dist->maxval / maxdist;
      }

    if ((mode < 1) || (mode > 2))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    if (fabs(step) > maxdist)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    if (step < 0.0)
      {
        maxdist = RoundInt(-step);
      }

    int step2 = RoundInt(step * step);

    if (step < 0)
      {
        step2 = -step2;
      }

    if (pegl == -1)
      {
        pegl = (orig->maxval + 1) / 2;
      }

    if ((pegl < 0) || (pegl > orig->maxval))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    if (IsImg(dir))
      {
        if ((xs != dir->xsize) || (ys != dir->ysize))
          {
            throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
          }

        if ((orig == dir) || (dist == dir))
          {
            throw IceException(FNAME, M_SAME_IMAGE, WRONG_PARAM);
          }

        if (dir->maxval < 8)
          {
            throw IceException(FNAME, M_LOWRANGE, WRONG_PARAM);
          }
      }

    Image ibin, inr;
    ibin = NewImg(xs, ys, 2);

    if (!IsImg(ibin))
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
      }

    inr = NewImg(xs, ys, xs * ys / 2);

    if (!IsImg(inr))
      {
        FreeImg(ibin);
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
      }

    int io, iu;

    if (mode == 1)
      {
        io = 1;
        iu = 0;
      }
    else     /*mode==2*/
      {
        io = 0;
        iu = 1;
      }

    for (int y = 0; y < ys; ++y)
      for (int x = 0; x < xs; ++x)
        {
          if (orig.getPixel(x, y) >= pegl)
            {
              ibin.setPixelUnchecked(x, y, io);
            }
          else
            {
              ibin.setPixelUnchecked(x, y, iu);
            }
        }

    SetImg(inr, 0);
    SetImg(dist, 0);

    if (IsImg(dir))
      {
        SetImg(dir, 8);
      }

    if (step < 0)
      {
        for (int y = 0; y < ys; ++y)
          for (int x = 0; x < xs; ++x)
            {
              if (ibin.getPixel(x, y) == 0)
                {
                  dist.setPixel(x, y, dist->maxval);
                }
            }
      }

    vector<IPoint> rp;
    vector<IPoint> aplist, aqlist;
    int b1, b2;
    // alle Randpunkte aufsammeln
    int ind;

    for (int y = 0; y < ys; y++)
      {
        b1 = GetVal(ibin, 0, y);

        for (int x = 1; x < xs; x++)
          {
            b2 = GetVal(ibin, x, y);

            if (b2 != b1)
              {
                if (b1 == 0)
                  {
                    ind = Randpunkt(inr, rp, x - 1, y);
                    Neuerpunkt(aplist, x, y,
                               inr, ind,
                               dist, 1, step2, fac,
                               dir, 4);
                  }
                else
                  {
                    ind = Randpunkt(inr, rp, x, y);
                    Neuerpunkt(aplist, x - 1, y,
                               inr, ind,
                               dist, 1, step2, fac,
                               dir, 0);
                  }
              }

            b1 = b2;
          }
      }

    for (int x = 0; x < xs; x++)
      {
        b1 = GetVal(ibin, x, 0);

        for (int y = 1; y < ys; y++)
          {
            b2 = GetVal(ibin, x, y);

            if (b2 != b1)
              {
                if (b1 == 0)
                  {
                    ind = Randpunkt(inr, rp, x, y - 1);
                    Neuerpunkt(aplist, x, y,
                               inr, ind,
                               dist, 1, step2, fac,
                               dir, 6);
                  }
                else
                  {
                    ind = Randpunkt(inr, rp, x, y);
                    Neuerpunkt(aplist, x, y - 1,
                               inr, ind,
                               dist, 1, step2, fac,
                               dir, 2);
                  }
              }

            b1 = b2;
          }
      }

    int adist = 1;

    int xt, yt;
    Freeman tdir; // neuer zu testender Punkt und Richtung dorthin

    int xn, yn;

    int ndist; // abstand dieses nachbarn vom randpunkt
    int xr, yr, ir; // Randpunkt (index und koordinaten)
    int mindist2, mindist, minindex;
    Freeman mindir;

    while ((aplist.size() > 0) && (adist < maxdist))
      {
        //   Printf("adist:%d aplist.rows():%d\n",adist,aplist.rows());
        int aindex = 0;

        while (aindex < (int)aplist.size())
          {
            // schon bestimmten Punkt ermitteln
            int x = aplist[aindex].x;
            int y = aplist[aindex].y;

            tdir = Freeman(0);

            for (int dircount = 0; dircount < 4; dircount++)   // 4 Richtungen
              {
                tdir.move(x, y, xt, yt); // nachbarpunkt bestimmen

                if ((xt >= 0) && (xt < xs) && (yt >= 0) && (yt < ys)) // Wenn innerhalb...
                  if (GetVal(inr, xt, yt) == 0)   // ..und distanz noch nicht bestimmt
                    {
                      // alle nachbarn abfragen
                      mindist2 = (adist + 10) * (adist + 10); // ist immer größer
                      mindir = Freeman(-2);
                      minindex = 0;

                      for (int ndir = 0; ndir < 8; ndir += 2)
                        {
                          Freeman(ndir).move(xt, yt, xn, yn);

                          if ((xn >= 0) && (xn < xs) && (yn >= 0) && (yn < ys))   // Wenn innerhalb...
                            {
                              ir = GetVal(inr, xn, yn);

                              if (ir != 0)
                                {
                                  xr = rp[ir - 1].x;
                                  yr = rp[ir - 1].y;
                                  ndist = (xt - xr) * (xt - xr) + (yt - yr) * (yt - yr);

                                  if (ndist < mindist2)
                                    {
                                      mindist2 = ndist;
                                      mindir = Freeman(ndir);
                                      minindex = ir;
                                    }
                                }
                            }
                        }

                      mindist = RoundInt(sqrt((double)mindist2));

                      if (mindist == adist)   // abstand nicht vergrößert
                        {
                          Neuerpunkt(aplist, xt, yt,
                                     inr, minindex,
                                     dist, mindist2, step2, fac,
                                     dir, mindir);
                        }
                      else
                        {
                          Neuerpunkt(aqlist, xt, yt,
                                     inr, minindex,
                                     dist, mindist2, step2, fac,
                                     dir, mindir);
                        }
                    }

                tdir += 2;
              }

            aindex++; // nächster Punkt der Liste
          }

        adist++; // "nächste" Distanz
        swap(aplist, aqlist);
        aqlist.clear();
      }

    FreeImg(inr);
    FreeImg(ibin);
    return OK;
  }
#undef FNAME
}
