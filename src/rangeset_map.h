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
#ifndef RANGESET_MAP_H
#define RANGESET_MAP_H

#include <vector>
#include <map>
#include "ImageC.h"
#include "moments.h"
#include "rowsegment.h"

namespace ice
{
  class RangeSetMap
  {
  public:
    void add(int x1, int x2)
    {
      ++eep[x1];
      --eep[x2 + 1];
      normalize();
    }

    void add(const RangeSetMap& rs)
    {
      merge(rs.eep);
      normalize();
    }

    void add(int x)
    {
      return add(x, x);
    }

    void del(int x1, int x2)
    {
      --eep[x1];
      ++eep[x2 + 1];
      normalize();
    }

    void del(const RangeSetMap& rs)
    {
      merge(rs.eep, true);
      normalize();
    }

    void del(int x)
    {
      return del(x, x);
    }

    void intersect(const RangeSetMap& rs)
    {
      merge(rs.eep);
      normalize(-1);
    }

    bool inside(int x) const
    {
      eemap::const_iterator iter1 = eep.upper_bound(x);
      if (iter1 == eep.end())
        {
          return false;
        }
      eemap::const_iterator iter2 = iter1;
      --iter2;
      if (iter2 == eep.end())
        {
          return false;
        }

      return iter2->second > 0;
    }

#if 0
    // for debugging only
    std::vector<int> getI() const
    {
      std::vector<int> res;
      for (int i = 0; i < (int)elements.size(); i++)
        {
          res.push_back(elements[i].x);
        }
      return res;
    }

    std::vector<bool> getB() const
    {
      std::vector<bool> res;
      for (int i = 0; i < (int)elements.size(); i++)
        {
          res.push_back(elements[i].b);
        }
      return res;
    }
#endif

    friend void swap(RangeSetMap& rs1, RangeSetMap& rs2)
    {
      std::swap(rs1.eep, rs2.eep);
    }

    bool empty() const
    {
      return eep.empty();
    }

    void clear()
    {
      eep.clear();
    }

    int getArea() const
    {
      int area = 0;
      eemap::const_iterator i = eep.begin();
      while (i != eep.end())
        {
          area -= i->first;
          ++i;
          area += i->first;
          ++i;
        }
      return area;
    }

    int getMin() const
    {
      if (eep.empty())
        {
          return  -1;
        }
      return eep.begin()->first;
    }

    int getMax() const
    {
      if (eep.empty())
        {
          return  -1;
        }
      return eep.rbegin()->first;
    }

    int draw(int y, const Image& img, int val) const;
    void getPoints(std::vector<IPoint>& points, int y) const;

    void getSegments(std::vector<RowSegment>& sl, int y) const;

    int calcMoments(Moments& m, int y) const;
    friend std::ostream& operator<<(std::ostream& out, const RangeSetMap& s);

  private:

    class Integer
    {
    public:
      int b;
      Integer(): b(0) {}
      Integer(int b): b(b) {}

      Integer& operator++()
      {
        ++b;
        return *this;
      }
      Integer& operator--()
      {
        --b;
        return *this;
      }

      Integer& operator+=(Integer sec)
      {
        b += sec.b;
        return *this;
      }
      Integer& operator-=(Integer sec)
      {
        b -= sec.b;
        return *this;
      }

      Integer operator-() const
      {
        return Integer(-b);
      }
      operator int() const
      {
        return b;
      }
    };

    typedef std::map<int, Integer> eemap;

    void merge(const eemap& v2, bool inverted = false);

    void normalize(int startLevel = 0);

    eemap eep;
  };
}
#endif
