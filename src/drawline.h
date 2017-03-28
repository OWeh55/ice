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
