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
#ifndef _REGION2_H
#define _REGION2_H
//
// Die Klasse Region zur Beschreibung von 2d-Regionen als Sammlung
// diskreter Punkte

#include <deque>
#include <vector>

#include "defs.h"
#include "base.h"
#include "Matrix.h"
#include "geo_ob.h"
#include "Contur.h"
#include "Window.h"
#include "moments.h"
#include "rangeset.h"

namespace ice
{
  //--------------------------------------------------------------------
  class Region
  {
  public:
    Region() {}
    explicit Region(const Contur& c);

    int getMinY() const;
    int getMaxY() const;
    int getMaxX() const;
    int getMinX() const;

    int getWidth() const;
    int getHeight() const;
    void getRect(int& xi, int& yi, int& xa, int& ya) const;

    void add(int x, int y);
    void add(IPoint p)
    {
      return add(p.x, p.y);
    }
    void add(Point p)
    {
      return add(RoundInt(p.x), RoundInt(p.y));
    }

    void add(const Window& w);
    void add(int x1, int y1, int x2, int y2);

    void add(const Region& r);

    void del(int x, int y);
    void del(IPoint p)
    {
      return del(p.x, p.y);
    }

    void del(int x1, int y1, int x2, int y2);
    void del(const Region& r);

    void intersect(const Region& r);

    bool inside(IPoint p) const
    {
      return inside(p.x, p.y);
    }

    bool inside(int xp, int yp) const
    {
      int yh = yp - y0;

      if (yh < 0)
        {
          return false;
        }

      if (yh >= (int)sl.size())
        {
          return false;
        }

      return sl[yh].inside(xp);
    }

    bool inside(int yp) const
    {
      int yh = yp - y0;

      if (yh < 0)
        {
          return false;
        }

      if (yh >= (int)sl.size())
        {
          return false;
        }

      return true;
    }

    int draw(const Image& img, int val) const;
    int calcMoments(Moments& m) const;

    friend std::ostream& operator<<(std::ostream&, const Region& s);

    const Region& operator +=(const Region& r2)
    {
      this->add(r2);
      return *this;
    }

    friend Region operator +(const Region& r1, const Region& r2)
    {
      Region res(r1);
      res.add(r2);
      return res;
    }

    const Region operator &=(const Region& r2)
    {
      this->intersect(r2);
      return *this;
    }

    friend Region operator &(const Region& r1, const Region& r2)
    {
      Region res(r1);
      res.intersect(r2);
      return res;
    }

    const Region& operator -=(const Region& r2)
    {
      this->del(r2);
      return *this;
    }

    friend Region operator -(const Region& r1, const Region& r2)
    {
      Region res(r1);
      res.del(r2);
      return res;
    }

    void getPoints(std::vector<IPoint>& pl) const;
    void getSegments(std::vector<RowSegment>& sl) const;

    int getArea() const
    {
      int area = 0;
      for (int i = 0; i < (int)sl.size(); ++i)
        {
          area += sl[i].getArea();
        }
      return area;
    }

    Point getCenter() const;

  protected:
    int newY(int y);
    int trimY();

    std::deque<RangeSet> sl;
    int y0;
  };
} // namespace
#endif
