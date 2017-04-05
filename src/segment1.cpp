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

#include "segment1.h"
#include "arith.h"
#include "histogram.h"

#include "pointvalue.h"
#include "PointListWalker.h"
#include "IceException.h"
#include "macro.h"

// --------------------------------------------------------------------------

namespace ice
{
#if 0
  class PointValue
  {
  public:
    IPoint p;
    int grw;

    PointValue(IPoint pp, int grey = 0): p(pp), grw(grey) {}
  private:
  };
#endif

// class with function object for comparison of TaskObjects
  class ComparePoints
  {
  public :
    int operator()(const IPointValue& a, const IPointValue& b)
    {
      return a.Value() > b.Value();
    }
  };

  // Punktprioritaetswarteschlange
  typedef priority_queue<IPointValue, deque<IPointValue> , ComparePoints> PointQueue;

  typedef deque<struct STPoint> FIFOList;

  /* Funktionen RegionGrow
   * - Saatpunkt: (x,y)
   * - Originalbild: orig
   * - Ergebnisbild: Mark
   * - Zulässige Standardabweichung der Grauwerte
   * - Anzahl der maximal zu betrachtenden Punkte: maxSize
   */

  // GRWThresh - Schwellwert für die Differenz der Grauwerte im Objekt

#define FNAME "RegionGrowGrw"
  void Collect(const Image& orig,
               const Image& mark, IPoint p, int refgrw, PointQueue& PQ)
  {
    if (orig.inside(p))
      if (mark.getPixel(p) == 0)
        {
          PQ.push(IPointValue(p, abs(refgrw - orig.getPixel(p))));
          mark.setPixel(p, 1);
        }
  }

  Region RegionGrowGrw(int x, int y, const Image& orig,
                       double stdmax, int  maxSize)
  {
    return RegionGrowGrw(IPoint(x, y), orig, stdmax, maxSize);
  }

  Region RegionGrowGrw(IPoint p, const Image& orig,
                       double stdmax, int  maxSize)
  {
    Region Res;

    if ((!orig.inside(p)) || (maxSize <= 0) ||
        (stdmax <= 0))   // Parametertestung
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM); // Parameterwerte ungültig -> Fehlermeldung
      }

    Histogram stat(orig.maxval + 1);

    PointQueue PQ;      // Priority Queue der Randpunkte

    int refgrw = GetVal(orig, p);
    PQ.push(IPointValue(p, 0)); // Startpunkt in Qeue eintragen

    Image i;
    i.create(orig.xsize, orig.ysize, 1);

    clearImg(i);
    i.setPixel(p, 1);

    int PointCount = 0;

    while ((PointCount < maxSize) && (!PQ.empty()))
      {
        IPoint pn = PQ.top(); // Punkt mit geringstem GRW aus Queue
        PQ.pop();       // aus Queue loeschen

        int count;
        double mean, std;
        int CurrentGRW = GetVal(orig, pn);
        stat.addValue(CurrentGRW);

        if (PointCount > 1)
          {
            stat.Statistic(count, mean, std);
          }
        else
          {
            std = 0.0;
          }

        if (std < stdmax)
          {
            Res.add(pn);
            PointCount++;

            // Nachbarschaft des aktuellen Punktes betrachten
            Neighbor4Walker nw(pn);
            for (nw.init(); !nw.ready(); nw.next())
              // insert unhandled point in queue
              {
                Collect(orig, i, nw, refgrw, PQ);
              }
          }  // (std<stdmax)
        else
          {
            while (!PQ.empty())
              {
                PQ.pop();
              }
          }
      } // while

    return Res;
  }

  int RegionGrowGrw(int x, int y, const Image& orig, const Image& mark, int val,
                    double stdmax, int maxSize)
  {
    return RegionGrowGrw(IPoint(x, y), orig, mark, val, stdmax, maxSize);
  }

  int RegionGrowGrw(IPoint p, const Image& orig, const Image& mark, int val,
                    double stdmax, int maxSize)
  {
    Region res;
    RETURN_ERROR_IF_FAILED(MatchImg(orig, mark));
    RETURN_ERROR_IF_FAILED(res = RegionGrowGrw(p, orig, stdmax, maxSize));
    RETURN_ERROR_IF_FAILED(res.draw(mark, val));
    return OK;
  }

#undef FNAME

#define FNAME "RegionGrow"
  void Collect(const Image& orig, const Image& mark,
               IPoint p, int refgrw,
               PointQueue& pq, double& sum)
  {
    if (orig.inside(p))
      if (mark.getPixel(p) == 0)
        {
          int grw =  orig.getPixel(p);
          pq.push(IPointValue(p, abs(refgrw - grw)));
          mark.setPixel(p, 1); // bearbeitet
          sum += grw;
        }
  }

  Region RegionGrow(int xp, int yp, const Image& orig, int MaxSize, int refval)
  {
    IPoint p(xp, yp);
    return RegionGrow(p, orig, MaxSize, refval);
  }

  Region RegionGrow(IPoint p, const Image& orig, int MaxSize, int refval)
  {
    Region res;

    if ((!orig.inside(p)) || (MaxSize <= 0))   // Parametertestung
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM); // Parameterwerte ungültig
      }

    Image himg;
    himg.create(orig.xsize, orig.ysize, 1);
    clearImg(himg);

    vector<IPoint> collectedPoints;

    PointQueue Candidates;
    int startval = orig.getPixel(p);

    if (refval < 0)
      {
        refval = startval;
      }

    double GrwSumRegion = 0;
    // startpunkt ist erster Kandidat
    double GrwSumCandidates = startval;

    Candidates.push(IPointValue(p, abs(startval - refval)));

    int PointCount = 0;

    double Max = 0;
    int Maxj = 0;

    while ((PointCount < MaxSize) && (!Candidates.empty()))
      {
        // Punkt mit geringster Grauwertabweichung aus Queue lesen
        IPoint pn = Candidates.top();
        Candidates.pop();
        PointCount++;

        // .. In Liste fuer Objektpunkte
        collectedPoints.push_back(pn);

        int grw = GetVal(orig, pn);

        GrwSumRegion += grw;  // zum Objekt hinzufügen
        GrwSumCandidates -= grw; // aus Kandidatenliste entfernen

        // Nachbarschaft des aktuellen Punktes betrachten
        // unbearbeitete Punkte in Queue aufnehmen
        Neighbor4Walker nw(pn);
        for (nw.init(); !nw.ready(); nw.next())
          // insert unhandled point in queue
          {
            Collect(orig, himg, nw, refval, Candidates, GrwSumCandidates);
          }

        if (Candidates.size() > 2)
          {
            // durchschnittlichen Grauwert der Region
            double GrwReg = GrwSumRegion / PointCount;
            // durchschnittlicher Grauwert der Kandidatenpunkte
            double GrwCandidates = GrwSumCandidates / Candidates.size();

            // neues Maximum ?
            if (fabs(GrwReg - GrwCandidates) > Max)
              {
                Max = fabs(GrwReg - GrwCandidates); // Wert des Maximums merken
                Maxj = PointCount;       // und Zahl der zur Region gehörenden Punkte
              }
          }
      }

    for (int j = 0; j < Maxj; j++)
      {
        res.add(collectedPoints[j]);
      }

    return res;
  }

  int RegionGrow(int x, int y, const Image& orig,
                 const Image& mark, int val,
                 int  maxSize, int refval)
  {
    Region res;
    RETURN_ERROR_IF_FAILED(MatchImg(orig, mark));
    RETURN_ERROR_IF_FAILED(res = RegionGrow(x, y, orig, maxSize, refval));
    RETURN_ERROR_IF_FAILED(res.draw(mark, val));
    return OK;
  }

#undef FNAME
}
