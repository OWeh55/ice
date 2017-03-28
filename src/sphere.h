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
  Klasse zur Repräsentation von Kugeln
*/
#ifndef SPHERE3D_H
#define SPHERE3D_H

#include "Vector.h"
#include "geo3d.h"

namespace ice
{
  /*--------------- Kreis -------------------*/
  class Sphere : public GeoObject3d
  {
  protected:
    double r;

  public:

    Sphere(): GeoObject3d(), r(1) {};
    Sphere(const Sphere& p): GeoObject3d(p), r(p.r) {};
    Sphere(double xp, double yp, double zp, double rp);

    Sphere(const Vector3d& p, double rp);

    explicit Sphere(const Vector& v);
    explicit Sphere(double d[]);

    double R() const
    {
      return r;
    }
    void setR(double vr);

    virtual Vector3d closestPoint(const Vector3d& p) const;
    virtual double Volume() const;
  };
}
#endif
