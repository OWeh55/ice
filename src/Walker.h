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
#ifndef ICE_WALKER_H
#define ICE_WALKER_H

#include <vector>

#include "Point.h"

namespace ice
{
  /**
   * class for handling of a set of pixel.
   * A Walker is a IPoint, which "walks" through a set of pixels.
   * With Walkers one may organize loops like \n
   * for (walker.init();!walker.ready();walker.next()) ... \n
   * Base class Walker is abstract since it lacks a real walking
   * mechanism
   */
  class Walker: public IPoint
  {
  public:
    /**
     * default c'tor.
     * Initializes current point and referencePoit with origin.
     */
    Walker(): IPoint(0, 0), ready_(false), referencePoint(0, 0) {}
    /**
     * c'tor with IPoint.
     * Initializes current point and referencePoint with this point.
     */
    Walker(IPoint p): IPoint(p), ready_(false), referencePoint(p) {}
    /**
     * c'tor with point coordinates x,y.
     * Initializes current point and referencePoint with x,y.
     * @param x,y initial coordinates
     */
    Walker(int x, int y): IPoint(x, y), ready_(false), referencePoint(x, y) {}

    /**
     * d'tor.
     */
    virtual ~Walker() {}

    /**
     * move Walker to given point.
     * @param p new referencePoint
     */
    virtual void moveTo(IPoint p) = 0;

    /**
     * [re-]init Walker.
     * Using init a Walker starts with first point (again).
     */
    virtual void init() = 0;

    /**
     * walk to next point.
     */
    virtual void next() = 0;

    /**
     * walk to n points further.
     * Equivalent to calling next() n times.
     */
    virtual void next(int n)
    {
      for (int i = 0; i < n && ! ready_; ++i)
        {
          next();
        }
    }

    /**
     * walk to previous point.
     */
    virtual void prev() = 0;

    /**
     * walk n points backward().
     * Equivalent to calling prev() n times.
     */
    virtual void prev(int n)
    {
      for (int i = 0; i < n && ! ready_; ++i)
        {
          prev();
        }
    }

    /**
     * walker has finished walk.
     * Becomes true after calling next() while standing at the
     * last point. if ready() is true, the current
     * value/position is undefined
     */
    virtual bool ready() const
    {
      return ready_;
    }

  protected:
    bool ready_;
    IPoint referencePoint;
  };
}
#endif
