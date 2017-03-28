/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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

#include "rangeset.h"

using namespace std;

namespace ice
{
  int RangeSet::draw(int y, const Image& img, int val) const
  {
    for (int i = 0; i < (int)elements.size(); i += 2)
      {
        for (int x = elements[i].x; x < elements[i + 1].x; x++)
          img.setPixelClipped(x, y, val);
      }
    return OK;
  }

  void RangeSet::getPoints(vector<IPoint>& points, int y) const
  {
    for (int i = 0; i < (int)elements.size(); i += 2)
      {
        for (int x = elements[i].x; x < elements[i + 1].x; x++)
          points.push_back(IPoint(x, y));
      }
  }

  void RangeSet::getSegments(vector<RowSegment>& sl, int y) const
  {
    for (int i = 0; i < (int)elements.size(); i += 2)
      {
        sl.push_back(RowSegment(elements[i].x, elements[i + 1].x - 1, y));
      }
  }

  int RangeSet::calcMoments(Moments& m, int y) const
  {
    for (int i = 0; i < (int)elements.size(); i += 2)
      {
        m.Add(y, elements[i].x, elements[i + 1].x - 1);
      }

    return OK;
  }

  void RangeSet::normalize(int startLevel)
  {
    int sidx = 0;
    int level = startLevel;
    for (int i = 0; i < (int)elements.size(); ++i)
      {
        if (elements[i].b)
          {
            if (level == 0)
              {
                elements[sidx] = elements[i];
                sidx++;
              }
            level++;
          }
        else
          {
            if (level == 1)
              {
                elements[sidx] = elements[i];
                sidx++;
              }
            level--;
          }
      }
    elements.resize(sidx);
    // cout << elements.size() << endl;
  }

  void RangeSet::merge(const vector<Elem>& v2, bool inverted)
  {
    std::vector<Elem> res(elements.size() + v2.size());
    int sidx1 = 0;
    int size1 = elements.size();
    int sidx2 = 0;
    int size2 = v2.size();
    int didx = 0;
    while (sidx1 < size1 && sidx2 < size2)
      {
        //        if (elements[sidx1].x < v2[sidx2].x)
        if (elements[sidx1] < v2[sidx2])
          {
            res[didx++] = elements[sidx1++];
          }
        else
          {
            if (inverted)
              res[didx++] = !v2[sidx2++];
            else
              res[didx++] = v2[sidx2++];
          }
      }
    while (sidx1 < size1)
      res[didx++] = elements[sidx1++];

    while (sidx2 < size2)
      {
        if (inverted)
          res[didx++] = !v2[sidx2++];
        else
          res[didx++] = v2[sidx2++];
      }
    elements = res;
  }

  std::ostream& operator<<(std::ostream& out, const RangeSet& s)
  {
    for (int i = 0; i < (int)s.elements.size(); i += 2)
      {
        out << s.elements[i].x << ".." << s.elements[i + 1].x - 1 << "  ";
      }

    out << std::endl;
    return out;
  }
}
