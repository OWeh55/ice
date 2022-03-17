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
#include <stdio.h>
#include "simplex6.h"

namespace ice
{
  void hfall(
    int     heapnum,
    int*     key,
    int*     iheap,
    int*     heap,
    int     cur
  )
  {
    int child, child1;
    int heapcur0 = heap[cur];
    int keyheapcur0 = key[heapcur0];

    child = 2 * cur;

    while (child <= heapnum)
      {
        child1 = child + 1;

        if (child < heapnum && key[heap[child1]] < key[heap[child]])
          {
            child = child1;
          }

        if (keyheapcur0 <= key[heap[child]])
          {
            break;
          }

        heap[cur] = heap[child];
        iheap[heap[cur]] = cur;
        cur = child;
        child = 2 * cur;
      }

    heap[cur] = heapcur0;
    iheap[heap[cur]] = cur;
  }

  void hrise(
    int*     key,
    int*     iheap,
    int*     heap,
    int     cur
  )
  {
    int parent;
    int heapcur0 = heap[cur];
    int keyheapcur0 = key[heapcur0];

    parent = cur / 2;

    while (parent > 0)
      {
        if (key[heap[parent]] <= keyheapcur0)
          {
            break;
          }

        heap[cur] = heap[parent];
        iheap[heap[cur]] = cur;
        cur = parent;
        parent = cur / 2;
      }

    heap[cur] = heapcur0;
    iheap[heap[cur]] = cur;
  }

  void hcheck(int cnt, int heapnum, int* key, int* iheap, int* heap)
  {
    int cur, parent;

    for (cur = heapnum; cur > 1; cur--)
      {
        parent = cur / 2;

        if (key[heap[cur]] < key[heap[parent]])
          {
            printf("%d: key[heap[%d]] = %d, key[heap[%d]] = %d\n",
                   cnt, cur, key[heap[cur]],
                   parent, key[heap[parent]]);
          }
      }

    for (cur = 1; cur <= heapnum; cur++)
      {
        if (iheap[heap[cur]] != cur)
          {
            printf("%d: heap[%d] = %d, iheap[%d] = %d\n",
                   cnt, cur, heap[cur],
                   heap[cur], iheap[heap[cur]]);
          }
      }
  }
}
