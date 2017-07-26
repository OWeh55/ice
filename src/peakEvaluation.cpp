/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002-2017 FSU Jena, Digital Image Processing Group
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

#include <queue>
#include <pointvalue.h>
#include <ImageBase.h>
#include <darith.h>
#include <PointListWalker.h>
#include "peakEvaluation.h"

// Gibt im Fehlerfall DBL_MAX zurück.
// Wenn es kein Peak gibt (Markierungsbild vollständig gesetzt oder konstantes Bild),
// gibt die Funktion als Peakkordinaten (-1,-1) und als Bewertung 0.0 zurück.

namespace ice
{
#define FNAME "PeakEvaluation"

  using namespace std;

  void markMax(const ImageD& peakImage, const Image& mark,
               IPoint max, int markerValue)
  {
    priority_queue<IPointValue> cList;
    double maxV = peakImage.getPixel(max);
    double minV = maxV;
    double deltaV = maxV * 0.01;
    cList.push(IPointValue(max, minV));
    bool ready = false;
    while (!cList.empty() && !ready)
      {
        IPointValue cPoint = cList.top();
        cList.pop();

        // cout << cPoint << mark.xsize << " " << mark.ysize<< endl;
        if (mark.getPixel(cPoint) == 0) // unmarked ?
          {
            double thisValue = cPoint.Value();
            if (thisValue < minV)
              minV = thisValue;

            Neighbor8Walker w8(cPoint);
            bool isMax = true; // thisValue>maxV*0.2;

            for (w8.init(); isMax && !w8.ready(); w8.next())
              {
                // not max if unmarked point in neighborhoud
                // has higher value
                if (peakImage.inside(w8))
                  if (mark.getPixelUnchecked(w8) == 0) // not part of current maximum
                    if (peakImage.getPixelUnchecked(w8) > minV + deltaV)
                      isMax = false;
              }

            if (isMax)
              {
                mark.setPixel(cPoint, markerValue);
                // new candidates
                for (w8.init(); !w8.ready(); w8.next())
                  {
                    if (peakImage.inside(w8))
                      if (mark.getPixelUnchecked(w8) == 0) // not part of maximum
                        cList.push(IPointValue(w8, peakImage.getPixelUnchecked(w8)));
                  }
              }
            else
              ready = true;
          }
      }
  }

#if 0
  double findMaxRegion(const ImageD& peakImage, const Image& mark,
                       IPoint max, int markerValue)
  {
    priority_queue<IPointValue> cList;
    cList.push(IPointValue(max, peakImage.getPixel(max)));
    // vector<IPoint> rand;
    double minV = peakImage.getPixel(max);
    double sumV = 0;
    while (!cList.empty())
      {
        IPointValue cPoint = cList.top();
        cList.pop();
        // cout << cPoint << endl;
        if (mark.getPixel(cPoint) == 0) // unmarked ?
          {

            Neighbor8Walker w8(cPoint);
            bool isMax = true;
            for (w8.init(); isMax && !w8.ready(); w8.next())
              {
                // not max if unmarked point in neighborhoud
                // has higher value
                if (peakImage.inside(w8))
                  if (mark.getPixelUnchecked(w8) == 0) // not part of maximum
                    if (peakImage.getPixelUnchecked(w8) > cPoint.Value())
                      isMax = false;
              }
            if (isMax)
              {
                mark.setPixel(cPoint, markerValue);
                // pixel is part of maximum
                sumV += cPoint.Value();
                if (cPoint.Value() < minV)
                  minV = cPoint.Value();

                for (w8.init(); isMax && !w8.ready(); w8.next())
                  {
                    if (peakImage.inside(w8))
                      if (mark.getPixelUnchecked(w8) == 0) // not part of maximum
                        cList.push(IPointValue(w8, peakImage.getPixelUnchecked(w8)));
                  }
              }
            else
              {
                while (!cList.empty())
                  cList.pop();
                cout << "rand: " << peakImage.getPixel(cPoint) << endl;
                // mark.setPixel(cPoint, mark.maxval);
                // rand.push_back(cPoint);
              }
          }
      }

    // remove marker for outside points
    //  for (const auto& p : rand)
    //    mark.setPixel(p, 0);

    cout << "sum: " << sumV << "  minv: " << minV << endl;

    return sumV;
  }
#endif
  double peakEvaluation(const ImageD& peakImage, Point& p)
  {
    p.x = -1;
    p.y = -1; // p "invalid"

    double minVal = findMin(peakImage);
    //  cout << minVal << endl;
    IPoint maxPoint;
    double maxVal = findMax(peakImage, maxPoint);
    //  cout << maxVal << " " << maxPoint << endl;
    if (maxVal > minVal)   // peak exists
      {
        p.x = (double) maxPoint.x;
        p.y = (double) maxPoint.y;
        Image mark;
        mark.create(peakImage.xsize, peakImage.ysize, 255);
        mark.set(0);
#ifdef MAXDEBUG
        Image pp;
        pp.create(peakImage.xsize, peakImage.ysize, 255);
        ConvImgDImg(peakImage, pp);
        Show(OVERLAY, pp, mark);
#endif
        markMax(peakImage, mark, maxPoint, 1);

        // find second maximum
        IPoint secondMaxPoint = findMax(peakImage, mark);
        if (secondMaxPoint.x < 0 || secondMaxPoint.y < 0)
          return 0.99; // maximum fills whole image (is this good?)
#ifdef MAXDEBUG
        GetChar();
#endif
        return (maxVal - peakImage.getPixel(secondMaxPoint)) / maxVal;
      }
    else return 0;
  }


#undef FNAME
}
