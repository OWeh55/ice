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
#ifndef _NEW_DPOINTLIST_
#define _NEW_DPOINTLIST_

namespace ice
{
  typedef struct DPointList_
  {
    int* xptr;
    int* yptr;
    int* wptr;

    int x; // letzter Wert (zum lesen!)
    int y;
    int w;

    int lng, aktlng;
  }* DPointList;

  DPointList NewDPointList();
  int DAddPoint(DPointList pl, int x, int y, int w);
  int DDelPoint(DPointList pl, int num);
  int FreePointList(DPointList pl);
}
#endif
