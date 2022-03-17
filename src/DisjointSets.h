#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <vector>
#include "forest.h"

namespace ice
{
  class DisjointSets
  {
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

    DisjointSets(const DisjointSets& s) = delete;

  public:

    DisjointSets(int s): sarray(s)
    {
      init();
    };

    ~DisjointSets()
    {
    }

    int find(int x) const
    {
      // Finds the representative of the set, given an element
      int r = sarray[x];

      if (r != x)
        {
          // Wurzel noch nicht gefunden, rekursiver abstieg
          r = find(r);
          // gefundene Wurzel direkt als Vater eintragen
          // dadurch wird der Weg für die nächste Suche
          // abgekürzt
          sarray[x] = r;
        }

      return r;
    }

    int linkRoot(int b1, int b2)
    {
      // combine 2 sets, given their _representative_
      sarray[b2] = b1;
      return b1;
    }

    int link(int s1, int s2)
    {
      // combine 2 sets, given one element of each set
      return linkRoot(find(s1), find(s2));
    }
  protected:
    mutable std::vector<int> sarray;

    void init()
    {
      // initialisation - every element is root of it´s own tree
      for (unsigned int i = 0; i < sarray.size(); i++)
        {
          sarray[i] = i;
        }
    }

  }; // class DisjointSets
} // namespace ice
#endif
