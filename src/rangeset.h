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
#ifndef RANGESET_H
#define RANGESET_H

#include <vector>

#include "ImageC.h"
#include "moments.h"
#include "rowsegment.h"

namespace ice
{
  class RangeSet
  {
  public:
    void add(int x1, int x2)
    {
      addB(x1, true);
      addB(x2 + 1, false);
      normalize();
    }

    void add(const RangeSet& rs)
    {
      merge(rs.elements);
      normalize();
    }

    void add(int x)
    {
      add(x, x);
    }

    void del(int x1, int x2)
    {
      addB(x1, false);
      addB(x2 + 1, true);
      normalize();
    }

    void del(const RangeSet& rs)
    {
      merge(rs.elements, true);
      normalize();
    }

    void del(int x)
    {
      del(x, x);
    }

    void intersect(const RangeSet& rs)
    {
      merge(rs.elements);
      normalize(-1);
    }

    bool inside(int x) const
    {
      int idx = findIndex(x, false);
      //  std::cout<<idx<< std::endl;
      return idx >= 0 && elements[idx].b;
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

    friend void swap(RangeSet& rs1, RangeSet& rs2)
    {
      std::swap(rs1.elements, rs2.elements);
    }

    bool empty() const
    {
      return elements.empty();
    }

    void clear()
    {
      elements.clear();
    }

    int getArea() const
    {
      int area = 0;
      for (int i = 0; i < (int)elements.size(); i += 2)
        {
          area += elements[i + 1].x - elements[i].x;
        }
      return area;
    }

    int getMin() const
    {
      if (elements.empty())
        {
          return  -1;
        }
      return elements.front().x;
    }

    int getMax() const
    {
      if (elements.empty())
        {
          return  -1;
        }
      return elements.back().x - 1;
    }

    int draw(int y, const Image& img, int val) const;
    void getPoints(std::vector<IPoint>& points, int y) const;

    void getSegments(std::vector<RowSegment>& sl, int y) const;

    int calcMoments(Moments& m, int y) const;
    friend std::ostream& operator<<(std::ostream& out, const RangeSet& s);

  private:
    class Elem
    {
    public:
      int x;
      bool b;
      Elem() {}
      Elem(int x, bool b): x(x), b(b) {}

      Elem operator!() const
      {
        return Elem(x, !b);
      }
      bool operator<(const Elem& sec) const
      {
        if (x == sec.x)
          {
            return b > sec.b;
          }
        return x < sec.x;
      }
    };

    void merge(const std::vector<Elem>& v2, bool inverted = false);

    int findIndex(const Elem& value, int left, int right) const
    {
      // iterative binary search
      while (right - left > 1)
        {
          int middle = (right + left) / 2;
          if (value < elements[middle])
            {
              right = middle;
            }
          else
            {
              left = middle;
            }
        }
      return left;
    }

    int findIndex(int value, bool b) const
    {
      Elem val(value, b);
      if (elements.empty() || val < elements[0])
        {
          return -1;
        }
      return findIndex(val, 0, elements.size());
    }

    void insertB(int index, int x, bool val)
    {
      // insert new entry/exit point after index
      int last = elements.size(); // last index in extended vector
      elements.resize(last + 1); // add one element
      Elem temp(x, val);
      int idx = index + 1;
      while (idx <= last)
        {
          std::swap(temp, elements[idx]);
          idx++;
        }
    }

    void addB(int x, bool val)
    {
      int idx = findIndex(x, val);
      insertB(idx, x, val);
    }

    void normalize(int startLevel = 0);

    std::vector<Elem> elements;
  };
}
#endif
