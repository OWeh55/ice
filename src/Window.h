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
#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include "defs.h"
#include "IceException.h"

#include "numbase.h"
#include "Point.h"

namespace ice
{
  /**
   * Window is a class for rectangular areas
   * the area is described by upper left and lower right corner
   * as IPoint.\n
   */

  class Window
  {
  public:
    /**
     * upper left corner of window
     */
    IPoint p1;
    /**
     * lower right corner of window
     */
    IPoint p2;

    /**
     * standard constructor.
     * standard constructor creates a window of size 0 at origin
     */

    Window() : p1(0, 0), p2(0, 0) {};

    /**
     * constructor with two opposite corners.
     * @param m1, m2 two opposite corners
     */
    Window(IPoint m1, IPoint m2) : p1(m1), p2(m2)
    {
      normalize();
    }

    /**
     * c'tor with minimum and maximum coordinate values.
     */
    Window(int x1, int y1, int x2, int y2): p1(x1, y1), p2(x2, y2)
    {
      normalize();
    };

    /**
     * c'tor with given center and size of window.
     * @param center central point of window
     * @param size number of pixels in both directions
     */
    Window(IPoint center, int size)
    {
      int size1 = size / 2;
      int size2 = size - size1 - 1;
      p1 = center - IPoint(size1, size1);
      p2 = center + IPoint(size2, size2);
    };

    /**
     * width of window.
     * width of window = number of pixels in x direction.
     */
    inline int width() const
    {
      return p2.x - p1.x + 1;
    }

    /**
     * width of window.
     * width of window = number of pixels in x direction.
     * upper case version for backward compatibility only
     */
    inline int Width() const
    {
      return p2.x - p1.x + 1;
    }

    /**
     * height of window.
     * height of window = number of pixels in y direction.
     */
    inline int height() const
    {
      return p2.y - p1.y + 1;
    }

    /**
     * height of window.
     * height of window = number of pixels in y direction.
     * upper case version for backward compatibility only
     */
    inline int Height() const
    {
      return p2.y - p1.y + 1;
    }

    /**
     * minimum x coordinate.
     */
    inline int xMin() const
    {
      return p1.x;
    }

    /**
     * maximum x coordinate.
     */
    inline int xMax() const
    {
      return p2.x;
    }

    /**
     * minimum y coordinate.
     */
    inline int yMin() const
    {
      return p1.y;
    }

    /**
     * maximum y coordinate.
     */
    inline int yMax() const
    {
      return p2.y;
    }

    /**
     * minimum x coordinate.
     * this version exists for backward compatibility only
     * @see xMin
     */
    inline int XI() const
    {
      return p1.x;
    }

    /**
     * maximum x coordinate.
     * this version exists for backward compatibility only
     * @see xMax
     */
    inline int XA() const
    {
      return p2.x;
    }

    /**
     * minimum y coordinate.
     * this version exists for backward compatibility only
     * @see yMin
     */
    inline int YI() const
    {
      return p1.y;
    }

    /**
     * maximum y coordinate.
     * this version exists for backward compatibility only
     * @see yMax
     */
    inline int YA() const
    {
      return p2.y;
    }

    /**
     * center of the window
     */
    inline IPoint center() const
    {
      return IPoint((p1.x + p2.x + 1) / 2, (p1.y + p2.y + 1) / 2);
    }

    /**
     * number of pixel in window
     */
    int nPixel() const
    {
      return width() * height();
    }

    /**
     * point inside window? .
     * @param p point to check
     */
    inline bool inside(const IPoint& p) const
    {
      return inside(p.x, p.y);
    }

    /**
     * point inside window? .
     * @param p point to check
     */
    inline bool inside(const Point& p) const
    {
      return inside(RoundInt(p.x), RoundInt(p.y));
    }

    /**
     * point inside window? .
     * @param x,y coordinates of point to check
     */
    inline bool inside(int x, int y) const
    {
      return !((x < p1.x) || (y < p1.y) || (x > p2.x) || (y > p2.y));
    }

    /**
     * windows differ? .
     * @param s second window
     */
    bool operator!=(const Window& s) const
    {
      return (p1 != s.p1) || (p2 != s.p2);
    }

    /**
     * check for equality.
     * @param s rhs window
     */
    bool operator==(const Window& s) const
    {
      return (s.p1 == p1) && (s.p2 == p2);
    }

    /**
     * shift window.
     * @param dx, dy value to shift window
     */
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
