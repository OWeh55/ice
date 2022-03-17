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
#ifndef _DRAWLINE_H
#define _DRAWLINE_H
/*
 * Funktionen zum Zeichnen von Geraden und Strecken
 *
 *    Line()
 *    HesseLine()
 *
 */

#include "base.h"
#include "Vector.h"
#include "Point.h"

namespace ice
{

#define NO_PAINT -1

//  int Line(int x1, int y1, int x2, int y2, int val, int mode, const Image & img);
  int Line(int x1, int y1, int x2, int y2, int val, const Image& img);

  int Line(double x1, double y1, double x2, double y2, int val, int mode, const Image& img);
  int Line(double x1, double y1, double x2, double y2, int val, const Image& img);

  int Line(const IVector& p1, const IVector& p2, int val, int mode, const Image& img);
  int Line(const IVector& p1, const IVector& p2, int val, const Image& img);

  int Line(Point p1, Point p2, int val, int mode, const Image& img);
  int Line(Point p1, Point p2, int val, const Image& img);

  int Line(IPoint p1, IPoint p2, int val, int mode, const Image& img);
  int Line(IPoint p1, IPoint p2, int val, const Image& img);

  int HesseLine(double p, double phi, int val, const Image& img);
  int HesseLine(const Vector& p, int val, const Image& img);
}
#endif
