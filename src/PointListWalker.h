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
#ifndef ICE_WALKER_POINTLIST_H
#define ICE_WALKER_POINTLIST_H

#include <vector>

#include "Point.h"
#include "Walker.h"
#include "Contur.h"

namespace ice
{
  /**
   * Walker with list of points to visit.
   * referencePoint is the first point of the list.
   * @see Walker
   */
  class PointListWalker: public Walker
  {
  public:
    /**
     * default c'tor.
     * Walker is not usable yet, because point list is empty.
     */
    PointListWalker(): Walker(), pl(), idx(0), idx0(0)
    {
    }

    explicit PointListWalker(IPoint p): Walker(p), pl(), idx(0), idx0(0)
    {
    }

    /**
     * c'tor with point list as std::vector<IPoint>.
     * @param v vector with points
     * @param idx0 index of start point. Walking starts here. List
     * is handled cyclic.
     */
    PointListWalker(const std::vector<IPoint>& v, int idx0 = 0):
      Walker(v[idx0]), pl(v), idx(0), idx0(idx0)
    {
    }

    /**
     * c'tor with Contur.
     * Creates Walker for all contour points.
     */
    explicit PointListWalker(const Contur& cp):
      Walker(cp.Start()), idx(0), idx0(0)
    {
      cp.getPoints(pl);
      init();
    }

    /**
     * set pointlist to given vector<IPoint>.
     */
    virtual void setPointList(const std::vector<IPoint>& v, int idx0p = 0)
    {
      pl = v;
      idx0 = idx0p;
      referencePoint = pl[idx0p];
      init();
    }

    virtual void moveTo(IPoint p) override;

    virtual void setStartIndex(int idx0p = 0)
    {
      idx0 = normalizedIndex(idx0p);
      init();
    }

    virtual void init() override
    {
      setIndex(idx0);
      ready_ = false;
    }

    using Walker::next;
    virtual void next() override
    {
      unsigned int newIdx = normalizedIndex(idx + 1);
      if (newIdx == idx0)
        {
          ready_ = true;
        }
      setIndex(newIdx);
    }

    using Walker::prev;
    virtual void prev() override
    {
      unsigned int newIdx = normalizedIndex(idx - 1);
      if (newIdx == idx0)
        {
          ready_ = true;
        }
      setIndex(newIdx);
    }

  protected:
    int normalizedIndex(int idx)
    {
      idx = idx % (int)pl.size();
      if (idx < 0)
        {
          idx += (int)pl.size();
        }
      return idx;
    }

    void setIndex(int newIndex)
    {
      idx = newIndex;
      x = pl[idx].x;
      y = pl[idx].y;
    }

    std::vector<IPoint> pl; /**< point list */
    unsigned int idx; /**< index of current point */
    unsigned int idx0; /**< index of start point */
  };

  /**
   * Walker for points in 4-neighborhood.
   * referencePoint is central point.
   * @see Walker
   */
  class Neighbor4Walker: public PointListWalker
  {
  public:
    Neighbor4Walker(IPoint pp);
  };

  /**
   * Walker for points in 8-neighborhood.
   * referencePoint is central point.
   * @see Walker
   */
  class Neighbor8Walker: public PointListWalker
  {
  public:
    Neighbor8Walker(IPoint pp);
    Neighbor8Walker(int x, int y);
  };

}
#endif
