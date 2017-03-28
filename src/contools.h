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
#ifndef _CONTOOLS_H
#define _CONTOOLS_H
//
//   Werkzeuge zur Erzeugung und Behandlung
//   von Konturen
//
//   Ortmann 10/99
//   M. Schubert 20.11.92
//
#define FREEMAN_INLINE

#include "Contur.h"
/********************************************************************
  contool1.c
    LineContur()

  contool2.c
    CircleContur()
    EllipseContur()
    CircleSegmentContur()
    EllipsenSegmentContur()

  remark : SelContur() has been moved to contool_vis.cpp/h
********************************************************************/

//
// Funktionen der Konturverwaltung (System)
//

namespace ice
{
  /*
   *  Gegenstueck zur ICE-Funktion Freeman.
   *  Bestimmt den Freemancode fuer die Richtung (x,y)->(xs,ys).
   *  Die Punkte muessen Nachbarn im Sinne der 8er-Nachbarschaft sein.
   *  Gibt im Fehlerfall -1 zurueck.
   */
  char GetFreeman(int x, int y, int xs, int ys);

  /************************************************
   *      Funktionen zur Konturerzeugung          *
   ************************************************/

  Contur LineContur(int p1[2], int p2[2]);
  Contur LineContur(int x, int y, int x1, int y1);
  Contur LineContur(IPoint p1, IPoint p2);

  /*** contool2.c ***********/
  Contur CircleContur(double* par);
  Contur EllipseContur(double* par);
  Contur CircleSegmentContur(double* par);
  Contur EllipseSegmentContur(double* par);
}
#endif


