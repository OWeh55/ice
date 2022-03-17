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
#ifndef ICE_WINDOW_WALKER_H
#define ICE_WINDOW_WALKER_H

#include <vector>

#include "Walker.h"
#include "Window.h"
#include "base.h"
#include "based.h"
#include "ColorImage.h"

namespace ice
{
  /**
   * Walker class for handling a rectangular area (window).
   * WindowWalker walks line by line through a window.
   * Start point and referencePoint is the upper left corner.
   * @see Walker
   * @see Window
   */
  class WindowWalker: public Walker
  {
  public:
    /**
     * c'tor with Window.
     * Initializes WindowWalker for given window
     * Initializes current point and referencePoint with this point.
     */
    WindowWalker(const Window& w): Walker(w.p1),
      x1(w.p1.x), x2(w.p2.x),
      y1(w.p1.y), y2(w.p2.y)
    {
    }

    /**
     * c'tor with given center and window size.
     * @param center center of window.
     * @param sizeX, sizeY size of window (in pixel). If sizeY is
     * not given a square window is assumed.
     */
    WindowWalker(IPoint center, int sizeX, int sizeY = -1);

    /**
     * c'tor with Image.
     * Initializes WindowWalker for whole image.\n
     * Note, that this does not fix Walker to that image permanently.
     * After usage of moveTo() WindowWalker may produce coordinates
     * outside the image.
     */
    WindowWalker(const Image& image): Walker(0, 0),
      x1(0), x2(image.xsize - 1), y1(0), y2(image.ysize - 1)
    {
    }

    /**
     * c'tor with ImageD.
     * Initializes WindowWalker for whole image.
     */
    WindowWalker(const ImageD& image): Walker(0, 0),
      x1(0), x2(image.xsize - 1), y1(0), y2(image.ysize - 1)
    {
    }

    /**
     * c'tor with ColorImage.
     * Initializes WindowWalker for whole image.
     */
    WindowWalker(const ColorImage& image): Walker(0, 0),
      x1(0), x2(image.xsize - 1), y1(0), y2(image.ysize - 1)
    {
    }

    /**
     * move Walker to given point.
     * @param p new referencePoint
     */
    virtual void moveTo(IPoint p) override
    {
      IPoint dp = p - referencePoint;
      referencePoint = p;
      x1 += dp.x;
      y1 += dp.y;
      x2 += dp.x;
      y2 += dp.y;
      init();
    }

    /**
      * [re-]init WindowWalker.
      * Using init a Walker starts with first point (again).
      *
      */
    virtual void init() override
    {
      x = x1;
      y = y1;
      ready_ = false;
    }

    using Walker::next;
    /**
     * walk to next point.
     */
    virtual void next() override
    {
      x++;
      if (x > x2)
        {
          x = x1;
          y++;
          if (y > y2)
            {
              ready_ = true;
              y = y1;
            }
        }
    }

    using Walker::prev;
    /**
     * walk to previous point.
     */
    virtual void prev() override
    {
      x--;
      if (x < x1)
        {
          x = x2;
          y--;
          if (y < y1)
            {
              ready_ = true;
              y = y2;
            }
        }
    }

  private:
    int x1;
    int x2;
    int y1;
    int y2;
  };
}
#endif
