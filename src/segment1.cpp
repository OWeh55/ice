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
  typedef priority_queue<IPointValue, deque<IPointValue> , ComparePoints> PointQueueMin;
  typedef priority_queue<IPointValue, deque<IPointValue> > PointQueueMax;

  typedef deque<struct STPoint> FIFOList;

  /* Funktionen RegionGrowDeviation
   * - Saatpunkt: (x,y)
   * - Originalbild: orig
   * - Ergebnisbild: mark
   * - Zulässige Standardabweichung der Grauwerte
   * - Anzahl der maximal zu betrachtenden Punkte: maxSize
   */

#define FNAME "RegionGrowDeviation"
  void collect(const Image& orig,
               const Image& mark, IPoint p,
               int refgrw,
               PointQueueMin& PQ)
  {
    if (orig.inside(p))
      if (mark.getPixel(p) == 0)
        {
          PQ.push(IPointValue(p, abs(refgrw - orig.getPixel(p))));
          mark.setPixel(p, 1);
        }
  }

  Region RegionGrowDeviation(int x, int y, const Image& orig,
                             double stdmax, int  maxSize)
  {
    return RegionGrowDeviation(IPoint(x, y), orig, stdmax, maxSize);
  }

  Region RegionGrowDeviation(IPoint p, const Image& orig,
                             double stdmax, int  maxSize)
  {
    Region Res;

    if ((!orig.inside(p)) || (maxSize <= 0) ||
        (stdmax <= 0))   // Parametertestung
      throw IceException(FNAME, M_WRONG_PARAM);

    Histogram stat(orig.maxval + 1);

    PointQueueMin PQ;      // Priority Queue der Randpunkte

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
            stat.getStatistics(count, mean, std);
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
                collect(orig, i, nw, refgrw, PQ);
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

  void RegionGrowDeviation(int x, int y, const Image& orig, const Image& mark, int val,
                           double stdmax, int maxSize)
  {
    RegionGrowDeviation(IPoint(x, y), orig, mark, val, stdmax, maxSize);
  }

  void RegionGrowDeviation(IPoint p, const Image& orig, const Image& mark, int val,
                           double stdmax, int maxSize)
  {
    try
      {
        checkSizes(orig, mark);
        Region res = RegionGrowDeviation(p, orig, stdmax, maxSize);
        res.draw(mark, val);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "RegionGrowing"
  void collect(const Image& orig, const Image& mark,
               IPoint p, bool findMin,
               PointQueueMin& pq, int& sum)
  {
    // point inside image?
    if (orig.inside(p))
      // point not seen yet?
      if (mark.getPixel(p) == 0)
        {
          int value =  orig.getPixel(p);
          if (!findMin)
            value = -value;
          pq.push(IPointValue(p, value));
          sum += value;
          mark.setPixel(p, 1); // bearbeitet
        }
  }

  Region RegionGrowing(int xp, int yp, const Image& orig, int maxSize, bool findMin)
  {
    IPoint p(xp, yp);
    return RegionGrowing(p, orig, maxSize, findMin);
  }

  Region RegionGrowing(IPoint p, const Image& orig, int maxSize, bool findMin)
  {
    if ((!orig.inside(p)) || (maxSize <= 0))   // Parametertestung
      throw IceException(FNAME, M_WRONG_PARAM);

    Image himg;
    himg.create(orig.xsize, orig.ysize, 1);
    clearImg(himg);

    vector<IPoint> collectedPoints;

    PointQueueMin candidatePoints;
    // int startval = orig.getPixel(p);

    int sumRegion = 0;
    int sumCandidates = 0;

    // startpunkt ist erster Kandidat
    collect(orig, himg, p, findMin, candidatePoints, sumCandidates);

    int nPoints = 0;

    double maxDiff = 0;
    int maxPoints = 0;

    while ((nPoints < maxSize) && (!candidatePoints.empty()))
      {
        // Punkt mit geringster Grauwertabweichung aus Queue lesen
        IPointValue pn = candidatePoints.top();
        int value = pn.Value();

        // remove from candidates
        candidatePoints.pop();
        sumCandidates -= value;

        // add to region
        nPoints++;
        // .. In Liste fuer Objektpunkte
        collectedPoints.push_back(pn);
        sumRegion += value;  // zum Objekt hinzufügen

        // Nachbarschaft des aktuellen Punktes betrachten
        // unbearbeitete Punkte in Queue aufnehmen
        Neighbor4Walker nw(pn);
        for (nw.init(); !nw.ready(); nw.next())
          {
            // insert point in queue (if not yet seen)
            collect(orig, himg, nw, findMin, candidatePoints, sumCandidates);
          }

        if (!candidatePoints.empty())
          {
            // durchschnittlichen Grauwert der Region
            double valRegion = (double)sumRegion / nPoints;
            // durchschnittlicher Grauwert der Kandidatenpunkte
            double valCandidates = (double)sumCandidates / candidatePoints.size();

            double diff = fabs(valRegion - valCandidates);
            // cout << nPoints << " " << diff << endl;
            // neues Maximum ?
            if (diff > maxDiff)
              {
                maxDiff = diff;       // Wert des Maximums merken
                maxPoints = nPoints;   // und Zahl der zur Region gehörenden Punkte
              }
          }
      }

    Region res;
    for (int j = 0; j < maxPoints; j++)
      {
        res.add(collectedPoints[j]);
      }

    return res;
  }

  void RegionGrowing(int x, int y, const Image& orig,
                     const Image& mark, int val,
                     int  maxSize, bool findMin)
  {
    try
      {
        checkSizes(orig, mark);
        Region res = RegionGrowing(x, y, orig, maxSize, findMin);
        res.draw(mark, val);
      }
    RETHROW;
  }

  void RegionGrowing(IPoint p, const Image& orig,
                     const Image& mark, int val,
                     int  maxSize, bool findMin)
  {
    RegionGrowing(p.x, p.y, orig, mark, val, maxSize, findMin);
  }
#undef FNAME
}
