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

#include "rangeset_map.h"

using namespace std;

namespace ice
{
  int RangeSetMap::draw(int y, const Image& img, int val) const
  {
    eemap::const_iterator i = eep.begin();
    while (i != eep.end())
      {
        int x1 = i->first;
        ++i;
        int x2 = i->first;
        ++i;
        for (int x = x1; x < x2; x++)
          {
            img.setPixelClipped(x, y, val);
          }
      }

    return OK;
  }

  void RangeSetMap::getPoints(vector<IPoint>& points, int y) const
  {
    eemap::const_iterator i = eep.begin();
    while (i != eep.end())
      {
        int x1 = i->first;
        ++i;
        int x2 = i->first;
        ++i;
        for (int x = x1; x < x2; x++)
          {
            points.push_back(IPoint(x, y));
          }

      }
  }

  void RangeSetMap::getSegments(vector<RowSegment>& sl, int y) const
  {
    eemap::const_iterator i = eep.begin();
    while (i != eep.end())
      {
        int x1 = i->first;
        ++i;
        int x2 = i->first;
        ++i;
        sl.push_back(RowSegment(x1, x2 - 1, y));
      }
  }

  int RangeSetMap::calcMoments(Moments& m, int y) const
  {
    eemap::const_iterator i = eep.begin();
    while (i != eep.end())
      {
        int x1 = i->first;
        ++i;
        int x2 = i->first;
        ++i;
        m.Add(y, x1, x2 - 1);
      }

    return OK;
  }

  void RangeSetMap::normalize(int startLevel)
  {
    int level = startLevel;

#if 0
    eemap new_eep;
    for (eemap::const_iterator i = eep.begin(); i != eep.end(); ++i)
      {
        // cout << i->first << ": " << i->second << endl;
        int level2 = level + i->second;
        if (level <= 0 && level2 > 0)
          {
            new_eep[i->first] = 1;
          }
        else if (level > 0 && level2 <= 0)
          {
            new_eep[i->first] = -1;
          }
        level = level2;
      }
    eep = new_eep;
#endif

    for (eemap::iterator i = eep.begin(); i != eep.end(); ++i)
      {
        // cout << i->first << ": " << i->second << endl;
        int level2 = level + i->second;
        if (level <= 0 && level2 > 0)
          {
            i->second = 1;
          }
        else if (level > 0 && level2 <= 0)
          {
            i->second = -1;
          }
        else
          {
            i->second = 0;
          }
        level = level2;
      }

    eemap::iterator i;
    do
      {
        i = eep.begin();
        while (i != eep.end() && i->second != 0)
          {
            ++i;
          }

        if (i != eep.end())
          {
            eep.erase(i);
          }
      }
    while (i != eep.end());
  }

  void RangeSetMap::merge(const eemap& map2, bool inverted)
  {
    if (inverted)
      for (eemap::const_iterator i = map2.begin(); i != map2.end(); ++i)
        {
          eep[i->first] -= i->second;
        }

    else
      for (eemap::const_iterator i = map2.begin(); i != map2.end(); ++i)
        {
          eep[i->first] += i->second;
        }
  }

  std::ostream& operator<<(std::ostream& out, const RangeSetMap& s)
  {
    RangeSetMap::eemap::const_iterator i = s.eep.begin();
    while (i != s.eep.end())
      {
        int x1 = i->first;
        ++i;
        int x2 = i->first;
        ++i;
        out << x1 << ".." << (x2 - 1) << "  ";
      }
    out << std::endl;
    return out;
  }
}
