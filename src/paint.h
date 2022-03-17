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
#ifndef _PAINT_H
#define _PAINT_H

#include <string>

#include "base.h"
#include "Vector.h"
#include "Point.h"

namespace ice
{
// rückgabewert, wenn nichts zu Zeichnen (weil außerhalb)
#define NO_PAINT -1

  /*
   *  paint1.cpp
   *    Marker()
   *    drawCircle()
   *    drawEllipse(), drawSuperEllipse
   *
   *  paint2.cpp
   *    drawCircleSegment()
   *    drawEllipsenSegment()
   *    PaintFunc()
   *    Text()
   *
   *  paint3.cpp
   *    InitFuncImage1D()
   *    PutFuncImage1D()
   *    FreeFuncImage1D()
   *    PaintFuncImage1D()
   */

  typedef struct FuncImage1D_
  {
    int nbr;              /* Anzahl der Eintraege */
    double upper;     /* Untere Schranke */
    double step;      /* Schrittweite */
    char str[80];     /* Beschriftung Kopfzeile */
    char strx[80];      /* Beschriftung x-Achse */
    char stry[80];      /* Beschriftung y-Achse */
    double* entry;      /* Liste Funktionswerte */
  }* FuncImage1D;

  int Marker(int option, int x, int y, int val, int size, const Image& img);
  int Marker(int option, const IVector& p, int val, int size, const Image& img);
  int Marker(int mode, const Point& xy, int val, int size, const Image& img);
  int Marker(int mode, const IPoint& xy, int val, int size, const Image& img);

  int drawCircle(double* par, int val1, int val2, int mode, const Image& img);
  int drawEllipse(double* par, int val1, int val2, int mode, const Image& img);
  int drawSuperEllipse(double c, double tr[3][3], int val, const Image& img);
  int drawCircleSegment(double* par, int val1, int val2, int mode, const Image& img);
  int drawEllipseSegment(double* par, int val1, int val2, int mode, const Image& img);
  int PaintFunc(double(*func)(double*, int, int), double* param, int val, const Image& img);

  int Text(const char* text, int x0, int y0, int val, int size, const Image& img);
  int Text(const std::string& txt, int x0, int y0, int val, int size, const Image& img);
  int Text(const std::string& txt, IPoint p, int val, int size, const Image& img);

  FuncImage1D InitFuncImage1D(int nbr, double upper, double step, char* str, char* strx, char* stry);
  int PutFuncImage1D(FuncImage1D fi, int i, double val);
  int FreeFuncImage1D(FuncImage1D fi);
  Image PaintFuncImage1D(FuncImage1D fi, double fac, int mode);
}
#endif
