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
#ifndef __ICE_COLOR_TABLE
#define __ICE_COLOR_TABLE

#include "limits.h"
#include "defs.h"

namespace ice
{
  class ColorTable
  {
  public:
    static constexpr int maxEntries = SHRT_MAX;
    // constructor/destructor
    ColorTable(int MaxIndex);
    ~ColorTable();

    // get/set colour values at certain indices
    int getColor(int Index,
                 unsigned char& Red, unsigned char& Green, unsigned char& Blue) const;
    int setColor(int Index,
                 unsigned char  Red, unsigned char  Green, unsigned char  Blue);

    // rearrange colortable:
    //
    //    0 .. FirstIndex         = Greyvalue 255
    //    FirstIndex .. LastIndex = Greyvalue 255 to 0 (gradually)
    //    LastIndex .. MaxIndex   = Greyvalue 0
    //
    //  Note: Existing changes in the table are overwritten!
    int setLinear(int FirstIndex, int LastIndex);

    // create an overlay color table
    int setTable(int type = 0);

  protected:

    // maximum of our table's index, also denotes our colour table's size
    int MaxIndex;

    // ptr to our table
    unsigned char* RTable;
    unsigned char* GTable;
    unsigned char* BTable;

  };

}
#endif
