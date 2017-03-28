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
#ifndef LINE3D_H
#define LINE3D_H

#include "geo3d.h"
#include "Vector.h"
#include "Vector3d.h"

namespace ice
{
  /*--------------- Gerade, Strecke oder Strahl  -------------------*/
  class Line3d: public GeoObject3d
  {
  public:
    // Bitmasken:
    static const int begin_l = 1 << 0;  // anfang begrenzt
    static const int end_l   = 1 << 1;  // ende begrenzt
    // lineseg-typen
    static const int line    = 0;               // Gerade - unlimitiert
    static const int ray     = begin_l;         // Strahl - anfang limitiert
    static const int inv_ray = end_l;           // inverser Strahl - ende limitiert (nur der Vollst344ndigkeit wegen)
    static const int segment = begin_l | end_l; // Strecke

  protected:
    Vector3d dp;
    double len2;
    int type;

  public:
    Line3d();
    Line3d(const Line3d& p);

    Line3d(double xp, double yp, double zp,
           double x2p, double y2p, double z2p,
           int typep = segment);

    Line3d(const Vector3d& p1, const Vector3d& p2, int typep = segment);

    explicit Line3d(const Vector& v, int typep = segment);
    explicit Line3d(double d[], int typep = segment);

    // X1 und Y1 als Synonym für X und Y
    // wegen Symmetrie der Endpunkte
    double X1() const
    {
      return pos.x;
    }
    double Y1() const
    {
      return pos.y;
    }
    double Z1() const
    {
      return pos.z;
    }

    double DX() const
    {
      return dp.x;
    }
    double DY() const
    {
      return dp.y;
    }
    double DZ() const
    {
      return dp.z;
    }

    double X2() const
    {
      return X() + dp.x;
    }
    double Y2() const
    {
      return Y() + dp.y;
    }
    double Z2() const
    {
      return Z() + dp.z;
    }

    void setX2(double vr);
    void setY2(double vr);
    void setZ2(double vr);
    void setP2(const Vector3d& p);

    virtual void setType(int typep = segment)
    {
      type = typep;
    }

    // Punkt in Parameter-Darstellung
    virtual Vector3d RelPoint(double my) const
    {
      return pos + dp * my;
    }

    virtual double limitedMy(double my) const
    {
      // begrenzt das my der Parameterdarstellung auf einen dem Typ
      // entsprechenden Bereich
      if ((my < 0.0) && (type & begin_l)) my = 0.0;

      if ((my > 1.0) && (type & end_l)) my = 1.0;

      return my;
    }

    virtual bool limitMys(double& my1, double& my2) const
    {
      // tested ob der Bereich my1..my2 Teile von *this umfasst
      // wenn ja: begrenzt my1..my2 auf den Bereich von *this
      if (my1 > my2)
        {
          std::swap(my1, my2);
        }

      if (type & begin_l)
        {
          if (my2 < 0.0) return false;

          if (my1 < 0.0)
            my1 = 0.0;
        }

      if (type & end_l)
        {
          if (my1 > 1.0) return false;

          if (my2 > 1.0)
            my2 = 1.0;
        }

      return true;
    }

    virtual Vector3d closestPoint(const Vector3d& p) const;
    virtual Vector3d closestPoint(const Vector3d& p, double& my) const;

    virtual double Length() const
    {
      return dp.length();
    }

    virtual double Length2() const
    {
      return dp.length2();
    }

    virtual Vector3d operator()(double p) const
    {
      return pos + dp * p;
    }
    virtual const Vector3d& P1() const
    {
      return pos;
    }

    virtual const Vector3d& DP() const
    {
      return dp;
    }

    virtual Vector3d P2() const
    {
      return (pos + dp);
    }

    virtual double Volume() const
    {
      return 0;
    }
  };
}
#endif
