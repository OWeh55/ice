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
#ifndef WINDOW3D_H
#define WINDOW3D_H

#include <numbase.h>
#include <Vector3d.h>

namespace ice
{

  class Window3d
  {
  public:
    IVector3d p1, p2;

    Window3d() : p1(0, 0, 0), p2(0, 0, 0) {};
    Window3d(IVector3d m1, IVector3d m2): p1(m1), p2(m2)
    {
      normalize();
    }
    Window3d(int x1, int y1, int z1, int x2, int y2, int z2):
      p1(x1, y1, z1), p2(x2, y2, z2)
    {
      normalize();
    };

    inline int XI() const
    {
      return p1.x;
    }
    inline int XA() const
    {
      return p2.x;
    }

    inline int YI() const
    {
      return p1.y;
    }
    inline int YA() const
    {
      return p2.y;
    }

    inline int ZI() const
    {
      return p1.z;
    }
    inline int ZA() const
    {
      return p2.z;
    }

    inline const IVector3d& P1() const
    {
      return p1;
    }
    inline const IVector3d& P2() const
    {
      return p2;
    }

    IVector3d getSize() const
    {
      return p2 - p1;
    }

    inline bool Inside(const IVector3d& p) const
    {
      return Inside(p.x, p.y, p.z);
    }

    inline bool Inside(int x, int y, int z) const
    {
      if (x < p1.x) return false;
      if (y < p1.y) return false;
      if (z < p1.z) return false;
      if (x > p2.x) return false;
      if (y > p2.y) return false;
      if (z > p2.z) return false;
      return true;
    }

    bool operator!=(const Window3d& s) const
    {
      return (p1 != s.p1) || (p2 != s.p2);
    }

    bool operator==(const Window3d& s) const
    {
      return (s.p1 == p1) && (s.p2 == p2);
    }

    int Shift(int dx, int dy, int dz)
    {
      p1.Shift(dx, dy, dz);
      p2.Shift(dx, dy, dz);
      return OK;
    }
  private:

    void normalize()
    {
      order(p1.x, p2.x);
      order(p1.y, p2.y);
      order(p1.z, p2.z);
    }
  };
}
#endif
