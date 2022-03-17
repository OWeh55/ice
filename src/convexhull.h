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
#ifndef _CONVEXHULL_H
#define _CONVEXHULL_H

#include <vector>
#include "IceException.h"
#include "defs.h"

namespace ice
{
#define FNAME "ConvexHull"
  template<typename T>
  bool orderX(const T* p1, const T* p2)
  {
    return p1->x < p2->x;
  }

  template<typename T>
  void EliminateNotch(std::vector<const T*>& pl, bool upper)
  {
    bool has_changed;
    do
      {
        has_changed = false;
        unsigned int i = 0;
        while (i < pl.size() - 2)
          {
            T p1 = *pl[i];
            T p2 = *pl[i + 1];
            T p3 = *pl[i + 2];

            double prod = (p2 - p1).Cross(p3 - p2);

            bool eliminate = upper ? (prod >= 0) : (prod <= 0);
            if (eliminate)
              {
                pl.erase(pl.begin() + i + 1);
                has_changed = true;
              }
            else
              {
                i++;
              }
          }
      }
    while (has_changed);
  }

  template<typename T>
  void Unify(std::vector<const T*>& pl, bool upper)
  {
    unsigned i = 0;
    while (i < pl.size() - 1)
      {
        T p1 = *pl[i];
        T p2 = *pl[i + 1];
        if (p1.x == p2.x)
          {
            if (upper ? p1.y > p2.y : p1.y < p2.y)
              {
                pl.erase(pl.begin() + i + 1);
              }
            else
              {
                pl.erase(pl.begin() + i);
              }
          }
        else
          {
            i++;
          }
      }
  }

  template<typename T>
  void ConvexHull(std::vector<T> pl, std::vector<T>& cpl)
  {
    if (pl.size() == 0)
      throw IceException(FNAME, M_EMPTY_POINTLIST);

    if (pl.size() <= 3)
      {
        cpl = pl;
        return;
      }

    std::vector<const T*> upper(pl.size());  // pointlist for upper Limit of convex hull
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        upper[i] = &pl[i];
      }
    sort(upper.begin(), upper.end(), orderX<T>);
    std::vector<const T*> lower = upper;

    Unify(upper, true);
    EliminateNotch<T>(upper, true);

    Unify(lower, false);
    EliminateNotch<T>(lower, false);

    cpl.clear();

    if (lower[0] != upper[0])
      {
        cpl.push_back(*lower[0]);
      }

    for (unsigned int i = 1; i < lower.size(); i++)
      {
        cpl.push_back(*lower[i]);
      }

    if (lower.back() != upper.back())
      {
        cpl.push_back(*upper.back());
      }

    for (int i = upper.size() - 2; i >= 0; i--)
      {
        cpl.push_back(*upper[i]);
      }
  }
#undef FNAME
}
#endif
