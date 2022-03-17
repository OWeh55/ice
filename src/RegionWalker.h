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
#ifndef ICE_REGION_WALKER_H
#define ICE_REGION_WALKER_H

#include <vector>

#include "Walker.h"
#include "region.h"

namespace ice
{
  /**
   * Walker class for regions / areas in images
   */
  class RegionWalker: public Walker
  {
  public:
    /**
     * c'tor creates an (incomplete) Walker with reference point p
     */
    explicit RegionWalker(IPoint p): Walker(p) {}

    /**
     * c'tor creates a Walker for a Region
     */
    explicit RegionWalker(const Region& region)
    {
      region.getSegments(segments);

      // use first Point as reference point
      referencePoint = IPoint(segments[0].x1, segments[0].y);

      // convert coordinate to coordinates relative to reference point
      relativateSegments();

      init();
    }

    /**
     * c'tor creates a Walker for the inside of a Contur
     */
    explicit RegionWalker(const Contur& c)
    {
      Region region(c);
      region.getSegments(segments);

      // use first Point as reference point
      referencePoint = IPoint(segments[0].x1, segments[0].y);

      // convert coordinate to coordinates relative to reference point
      relativateSegments();

      init();
    }

    void init() override
    {
      idx = 0;
      newidx();
      x = x1;
      ready_ = false;
    }

    virtual void moveTo(IPoint p) override
    {
      referencePoint = p;
      init();
    }

    using Walker::next;
    virtual void next() override
    {
      x++;
      if (x > x2)
        {
          idx++;
          if (idx >= (int)segments.size())
            {
              init();
              ready_ = true;
            }
          else
            {
              newidx();
              x = x1;
            }
        }
    }

    using Walker::prev;
    virtual void prev() override
    {
      x--;
      if (x < x1)
        {
          idx--;
          if (idx < 0)
            {
              init();
              ready_ = true;
            }
          else
            {
              newidx();
              x = x2;
            }
        }
    }

  protected:
    /**
     * calculate segment coordinates relative to reference point
     */
    void relativateSegments()
    {
      // convert coordinates in segments to coordinates relative
      // to reference point
      for (int i = 0; i < (int)segments.size(); i++)
        {
          segments[i].x1 -= referencePoint.x;
          segments[i].x2 -= referencePoint.x;
          segments[i].y -= referencePoint.y;
        }
    }

    void newidx()
    {
      x1 = segments[idx].x1 + referencePoint.x;
      x2 = segments[idx].x2 + referencePoint.x;
      y = segments[idx].y + referencePoint.y;
    }

    std::vector<RowSegment> segments;
    int idx;
    int x1, x2; // caching borders of current segment
  };

  /**
   * specialized RegionWalker for discs / inside of circle
   */
  class DiskWalker: public RegionWalker
  {
  private:
    virtual void makeSegments(int px, int py, double rad);

  public:
    /**
     * c'tor creates DiskWalker from center and radius
     */
    DiskWalker(IPoint center, double rad): RegionWalker(center)
    {
      makeSegments(center.x, center.y, rad);
      init();
    }

    /**
     * c'tor creates DiskWalker from center coordinates and radius
     */
    DiskWalker(int x, int y, double rad): RegionWalker(IPoint(x, y))
    {
      makeSegments(x, y, rad);
      init();
    }
  };
}
#endif
