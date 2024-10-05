#ifndef POINTLIST_H
#define POINTLIST_H
/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
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
//
// definition for OLD datastructur PointList
// only for compatibility !!!!!!!
// use Matrix or vector<Point>/vector<PointValue> instead

#include <vector>
#include "Contur.h"
#include "Matrix.h"
#include "matrix_function.h"
#include "pointvalue.h"

namespace ice
{
//*******Datenstrukturen********************************************/
  typedef struct PointList_
  {
    int lng;                         /*Anzahl der Punkte*/
    double* xptr;                    /*x-Koordinaten*/
    double* yptr;                    /*y-Koordinaten*/
    double* wptr;                    /*Gewichte*/
  }* PointList;

  /******************************************************************/
  typedef struct Segment_
  {
    double          p0[2], p1[2]; /*Anfangs- und Endpunkt*/
    /* Segmenttyp: 1 - Geradensegment*/
    /*             2 - Kreissegment*/
    /*             3 - Ellipsensegment*/
    int             typ;
    /* Parameter */
    /*Geradensegment: p,phi*/
    /*Kreissegment:   xm,ym,r,psi1,psi2*/
    /*Ellipsensegment:xm,ym,a,b,phi,psi1,psi2*/
    double          par[7];
    struct Segment_* prev, * next;    /*Listenverkettung*/
  }* Segment;

  /******* Funktionen zu Datenstrukturen (pointlist.cpp) **************/

  PointList NewPointList(int len);
  int PutPoint(PointList pl, int adr, double x, double y, double weight = 1.0);
  int FreePointList(PointList pl);

  PointList ConturPointList(const Contur& c, int diff = 1);

  int FreeSegmentList(Segment sl);
  Segment AddSegment(Segment sl, Segment s);
  Segment FirstSegment(Segment sl);
  Segment NextSegment(Segment sl);

  /******************************************************************/

  int PointList2Matrix(PointList pl, int ad1, int ad2, Matrix& m);
  int PointList2vector(PointList pl, int ad1, int ad2,
                       std::vector<PointValue>& vpv);
  PointList Matrix2PointList(const Matrix& m);

}
#endif
