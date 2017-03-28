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

/*
 * mathfunc.h
 * Torsten Baumbach, Mai 1997
 * Mathematische Funktionen
 */

#ifndef _CADIS_MATHEMATIC_H_
#define _CADIS_MATHEMATIC_H_

namespace ice
{
  extern double PI_2;

  double PointDist(double x1, double y1, double x2, double y2);

  double Transform(double grad, double a, double b);
// Berechnet zu einem Winkel grad einer Ellipse (a,b) in Haupt-
// achsenlage den zugehoerigen Winkel des Kreises, aus dem die
// Ellipse durch anisotrope Skalierung hervorging.

  void   TestDegree(double& grad);
// Verschiebt die Grad-Angabe in das Intervall [0,360)
}
#endif
