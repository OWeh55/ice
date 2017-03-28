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
#ifndef _CONVEX_HULL_
#define _CONVEX_HULL_

#include "pointlist.h"
#include "Matrix.h"

namespace ice
{
/// Test, ob sich die Seiten zweier Polygone schneiden => POLY_INTERSECT
//  Wenn sie sich nicht schneiden, wird für konvexe Polygone noch entschieden,
//  ob, ein Polygon im anderen liegt (POLY_1_IN_2 oder POLY_2_IN_2). Ansonsten
//  wird POLY_DISJUNKT zurueckgegeben

#define POLY_ERROR  -1
#define POLY_DISJUNKT 0
#define POLY_INTERSECT  1
#define POLY_1_IN_2 2
#define POLY_2_IN_1 3

  int ConvexPolygonsIntersect(PointList pl1, PointList pl2);

// Bestimmt die konvexe Huelle (geordnete Liste der Eckpunkte) einer
// beliebigen (diskreten) Punktmenge in O(n).

  PointList ConvexHull(PointList pl);
  Matrix ConvexHull(const Matrix& pl);
  Contur ConvexHull(const Contur& c);

  //  int ConvexHull(std::vector<Point> pl, std::vector<Point> &cpl);
}
#endif
