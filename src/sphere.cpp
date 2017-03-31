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
#include "Vector.h"
#include "defs.h"
#include "sphere.h"

namespace ice
{
  /*
   * Sphere
   */

#define FNAME "Sphere::Sphere"
  Sphere::Sphere(double xp, double yp, double zp, double rp): GeoObject3d(xp, yp, zp)
  {
    setR(rp);
  }
  Sphere::Sphere(const Vector3d& p, double rp) : GeoObject3d(p)
  {
    setR(rp);
  }
  Sphere::Sphere(const Vector& v) : GeoObject3d(v)
  {
    if (v.Size() < 4)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    setR(v[2]);
  }

  Sphere::Sphere(double p[]): GeoObject3d(p)
  {
    setR(p[2]);
  }
#undef FNAME

#define FNAME "Sphere::setR"
  void Sphere::setR(double rp)
  {
    if (rp < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

    r = rp;
  }
#undef FNAME

  Vector3d Sphere::closestPoint(const Vector3d& p) const
  {
    Vector3d prel = p - pos;
    return pos + prel.Normalized();
  }

  double Sphere::Volume() const
  {
    return M_PI * 4.0 / 3.0 * r * r * r;
  }
}

