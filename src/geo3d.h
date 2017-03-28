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
  Klassen zur Repräsentation geometrischer Elemente
*/
#ifndef GEO3D_H
#define GEO3D_H

#include "Vector3d.h"
#include "Vector.h"

namespace ice
{
  /*------------abstrakte Klasse Geometrisches 3D-Objekt -------------------*/
  class GeoObject3d
  {
  protected:
    Vector3d pos;

  public:
    GeoObject3d() : pos(0, 0, 0) {};
    GeoObject3d(const GeoObject3d& p): pos(p.pos) {};
    GeoObject3d(const Vector3d& p): pos(p) {};
    GeoObject3d(double xp, double yp, double zp): pos(Vector3d(xp, yp, zp)) {};

    explicit GeoObject3d(const Vector& v): pos(v) {};
    explicit GeoObject3d(double p[]): pos(p) {};

    virtual ~GeoObject3d() {};
    virtual double X() const
    {
      return pos.x;
    }
    virtual double Y() const
    {
      return pos.y;
    }
    virtual double Z() const
    {
      return pos.z;
    }

    virtual void setX(double xv)
    {
      pos.x = xv;
    }
    virtual void setY(double yv)
    {
      pos.y = yv;
    }
    virtual void setZ(double zv)
    {
      pos.z = zv;
    }

    virtual void set(const Vector& v)
    {
      pos = v;
    }

    virtual Vector3d& Pos()
    {
      return pos;
    }
    virtual const Vector3d& Pos() const
    {
      return pos;
    }

    virtual void Shift(double dx, double dy, double dz);
    virtual void Shift(const Vector3d& v);

    virtual Vector3d closestPoint(const Vector3d& p) const
    {
      return pos;
    }

    virtual double Distance(double dx, double dy, double dz) const
    {
      return Distance(Vector3d(dx, dy, dz));
    };

    virtual double Distance(const Vector3d& ob) const
    {
      return (closestPoint(ob) - ob).Length();
    };

    virtual double Distance(const GeoObject3d& ob) const
    {
      return Distance(ob.pos);
    };

    virtual double Volume() const = 0;
  };
}
#endif
