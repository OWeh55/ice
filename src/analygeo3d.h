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
#ifndef _ANALYGEO3D_H
#define _ANALYGEO3D_H

#include "Vector3d.h"
#include "sphere.h"
#include "line3d.h"

namespace ice
{
  double Distance(const Vector3d& p1, const Vector3d& p2);
  double Distance(const Vector3d& p1, const Line3d& l2);
  double Distance(const Line3d& l1, const Vector3d& l2);
  double Distance(const Line3d& l1, const Line3d& l2);
  double Distance(const Vector3d& p, const Line3d& l, double& par);

  int Intersection(const Line3d& l1, const Line3d& l2,
                   Vector3d& p, double& dist, Vector& res);
  int Intersection(const Line3d& l1, const Line3d& l2,
                   Vector3d& p, double& dist);
  int Intersection(const Line3d& l1, const Line3d& l2, Vector3d& p);

  int Intersection(const Line3d& l, const Sphere& s, Vector3d& p1, Vector3d& p2);
}
#endif
