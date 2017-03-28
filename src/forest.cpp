/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2012 FSU Jena, Digital Image Processing Group
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
#include <iostream>

#include "defs.h"
#include "macro.h"

#include "forest.h"

using namespace std;

namespace ice
{
//*** class Forest ******************************************
  int Forest::nTree(bool ignoreZeroLengthTrees) const
  {
    int ntree = 0;

    for (int i = 0; i < size; i++)
      {
        if (sarray[i] == rootval) // Wurzel
          {
            if (ignoreZeroLengthTrees)
              {
                if (carray[i] != 0)  // ignore isolated points
                  ntree++;
              }
            else
              ntree++;
          }
      }
    return ntree;
  }

  int Forest::getNeighborCount(int i) const
  {
    int res = getRefCount(i); // number of childs

    if (!isRoot(i))           // + father, if not root
      res++;

    return res;
  }

#define FNAME "Forest::setFather"
  // set father of node[x] to newfather
  void Forest::setFather(int x, int newfather)
  {
    if (newfather < rootval || newfather > size || x < 0 || x > size)
      {
        Message(FNAME, M_WRONG_INDEX, WRONG_PARAM);
        return;
      }

    int oldfather = sarray[x];

    // decrease number of references for old father
    if (oldfather != rootval)
      carray[oldfather]--;
    // increase number of references for new father
    if (newfather != rootval)
      carray[newfather]++;

    // now set the link
    sarray[x] = newfather;
  }
}
