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
//  Funktionen zur Arbeit mit Polygonen
//
//   (C) m.schubert 1/96
//       Ortmann 10/99
//
#ifndef _POLYGON_H_
#define _POLYGON_H_

#include <vector>
#include "pointlist.h"
#include "Contur.h"

namespace ice
{
  // functions with PointList
  void drawPolygon(PointList pl, int val, const Image& img);
  int drawPolygon(const Matrix& pl, int val, const Image& img,
                  int marker = 0, int size = 5, int color = -1);

  Contur PolygonContur(PointList pl);
  Contur PolygonContur(const Matrix& pl);

  bool insidePolygon(double p[2], PointList pl);
  bool insidePolygon(const Matrix& m, const Point& p);

  bool isPolygon(PointList pl);
  bool isPolygon(const Matrix& m);

  PointList* ClipPolygon(PointList pl1, PointList pl2, int* n);

  // polygon as (n,2) - Matrix

  Matrix ReducePolygon(const Matrix& pl, int nr, int mode = 1);
  Matrix ReducePolygon(const Contur& c, int nr, int mode = 1);

  Matrix ReducePolygonPrecision(const Matrix& pl, double prec, int mode);
  Matrix ReducePolygonPrecision(const Contur& c, double prec, int mode);

  Matrix FitPolygonContur(const Matrix& pl, const Contur& c,
                          int step = 0, int count = 200);

  // distances of points to polygon
  double DistancePointEdge(double x, double y, const Matrix& pl, int i1, int i2);
  double DistancePointEdge(double x, double y, const Matrix& pl, int i);
  double DistancePointEdge(const Matrix& pl, int i, int i1, int i2);

  double DistancePointPolygon(double x, double y, const Matrix& pl);
  double DistancePointPolygon(double x, double y, const Matrix& pl, int& mine);

  int FindNearestCorner(double x, double y, const Matrix& pl);

  // distances between polygons

#define DPP_MIN 0
#define DPP_MEAN 1
#define DPP_MAX 2
#define DPP_DIR 3

  double DistancePolygonPolygonDir(const Matrix& p1, const Matrix& p2, int mode = DPP_MAX);
  double DistancePolygonPolygon(const Matrix& p1, const Matrix& p2, int mode = DPP_MEAN, int pmode = DPP_MAX);

  double DistanceConturPolygon(const Contur& c, const Matrix& p, int mode = DPP_MEAN, int pmode = DPP_MAX);

  std::vector<Point> FitPolygonPointlist(const std::vector<Point>& poly,
                                         const std::vector<Point>& pl,
                                         int step, int count);

}
#endif
