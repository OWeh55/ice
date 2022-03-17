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
/////////////////////////////////////////////////////////////////////
// Torsten Baubach, Mai 1997
// Mathematische Funktionen
/////////////////////////////////////////////////////////////////////

#include "numbase.h"

#include "mathfunc.h"

namespace ice
{
  double PI_2 = M_PI + M_PI;

/////////////////////////////////////////////////////////////////////
// Euklid. Abstand zweier Punkte

  double PointDist(double x1, double y1, double x2, double y2)
  {
    return (sqrt(Sqr((x1) - (x2)) + Sqr((y1) - (y2))));
  }

/////////////////////////////////////////////////////////////////////
// Berechnet zu einem Winkel grad einer Ellipse (a,b) in Haupt-
// achsenlage den zugehoerigen Winkel des Kreises, aus dem die
// Ellipse durch anisotrope Skalierung hervorging.

  double Transform(double grad, double a, double b)
  {

    double ergebnis, cgrad, rad;

    cgrad = cos(rad = Arcus(grad));

    if (b == 0)
      {
        return -10000;
      }

    ergebnis = atan(a * tan(rad) / b);

    if (cgrad < 0)
      {
        ergebnis += M_PI;
      }

    if (ergebnis < 0)
      {
        ergebnis += 2 * M_PI;
      }

    return Degree(ergebnis);
  }

/////////////////////////////////////////////////////////////////////
// Verschiebt die Grad-Angabe in das Intervall [0,360)

  void TestDegree(double& grad)
  {
    grad = fmod(grad, 360.0) + (grad < 0 ? 360 : 0);
  }
}
