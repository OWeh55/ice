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
#include <iostream>
using namespace std;

#include <math.h>
#include "IceException.h"
#include "macro.h"

#include "line3d.h"

namespace ice
{
  /*
   * Line3d
   */

#define FNAME "Line3d::Line3d"

  Line3d::Line3d() : GeoObject3d(), dp(0, 0, 0), len2(0) { }

  Line3d::Line3d(double xp, double yp, double zp,
                 double x2p, double y2p, double z2p, int typep)
    : GeoObject3d(xp, yp, zp), dp(x2p - xp, y2p - yp, z2p - zp), len2(dp.length2()), type(typep) {}

  Line3d::Line3d(const Line3d& p) : GeoObject3d(p), dp(p.dp), len2(p.len2), type(p.type) {}

  Line3d::Line3d(const Vector3d& p1, const Vector3d& p2, int typep)
    : GeoObject3d(p1), dp(p2 - p1), len2(dp.length2()), type(typep)
  {
  }

  Line3d::Line3d(const Vector& v, int typep) : GeoObject3d(v), type(typep)
  {
    if (v.Size() < 6)
      throw IceException(FNAME, M_WRONG_DIM);

    dp = Vector3d(v[3] - pos.x, v[4] - pos.y, v[5] - pos.z);
    len2 = dp.length2();
  }

  Line3d::Line3d(double p[], int typep) :
    GeoObject3d(p), dp(&p[3]), len2(dp.length2()), type(typep)
  {
  }
#undef FNAME

  Vector3d Line3d::closestPoint(const Vector3d& p) const
  {
    double my;
    return closestPoint(p, my);
  }

  Vector3d Line3d::closestPoint(const Vector3d& p, double& my) const
  {
    Vector3d prel = p - pos;
    my = limitedMy(prel * dp / len2);
    // my = (prel * dp) / dp.length();
    //  std::cout << my << std::endl;
    return RelPoint(my);
  }
}
