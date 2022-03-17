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
