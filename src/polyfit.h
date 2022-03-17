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
/************************************************
 * Kontursegmentierung und Anpassung von        *
 * Liniensegmenten                              *
 * (Die Eckpunkte des Polygonzuges werden durch *
 * rekursive Teilung der Kurve bestimmt)        *
 *                                              *
 *   (C) m.schubert 5.95                        *
 ************************************************/

#ifndef _POLYFIT_H
#define _POLYFIT_H

#include "Contur.h"
#include "fit.h"

namespace ice
{
  Segment LineSegContur(Contur c, int mlng, double mdist);
  Segment LineSegPointList(PointList pl, int closed, int mlng, double mdist);
  PointList FitPolygonContur(Contur c, int mlng, double mdist);
  PointList FitPolygonPointList(PointList pl, int closed, int mlng, double mdist);
}
#endif
