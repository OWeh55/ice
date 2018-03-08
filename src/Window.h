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
#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include "defs.h"
#include "IceException.h"

#include "numbase.h"
#include "Point.h"

namespace ice
{

  class Window
  {
  public:
    IPoint p1, p2;

    Window() : p1(0, 0), p2(0, 0) {};
    Window(IPoint m1, IPoint m2) : p1(m1), p2(m2)
    {
      normalize();
    }
    Window(int x1, int y1, int x2, int y2): p1(x1, y1), p2(x2, y2)
    {
      normalize();
    };

    Window(IPoint center, int size)
    {
      int size1 = size / 2;
      int size2 = size - size1 - 1;
      p1 = center - IPoint(size1, size1);
      p2 = center + IPoint(size2, size2);
    };

    inline int Width() const
    {
      return p2.x - p1.x + 1;
    }
    inline int Height() const
    {
      return p2.y - p1.y + 1;
    }

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

    inline IPoint center() const
    {
      return IPoint((p1.x + p2.x + 1) / 2, (p1.y + p2.y + 1) / 2);
    }

    int nPixel() const
    {
      return Width() * Height();
    }

    inline bool inside(const IPoint& p) const
    {
      return inside(p.x, p.y);
    }

    inline bool inside(const Point& p) const
    {
      return inside(RoundInt(p.x), RoundInt(p.y));
    }

    inline bool inside(int x, int y) const
    {
      return !((x < p1.x) || (y < p1.y) || (x > p2.x) || (y > p2.y));
    }

    bool operator!=(const Window& s) const
    {
      return (p1 != s.p1) || (p2 != s.p2);
    }

    bool operator==(const Window& s) const
    {
      return (s.p1 == p1) && (s.p2 == p2);
    }

    void shift(int dx, int dy)
    {
      p1.shift(dx, dy);
      p2.shift(dx, dy);
    }

    friend std::ostream& operator<<(std::ostream& os, const Window& w)
    {
      os << "<" << w.p1 << " " << w.p2 << ">";
      return os;
    }

  private:

    void normalize()
    {
      order(p1.x, p2.x);
      order(p1.y, p2.y);
    }

  };
}
#endif
