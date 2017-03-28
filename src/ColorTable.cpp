/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <iostream>
#include "ColorTable.h"
#include "defs.h"

namespace ice
{
  int ColorTable::getColor(int Index,
                           unsigned char& Red, unsigned char& Green, unsigned char& Blue) const
  {
    // std::cout << Index << " " << MaxIndex << std::endl;
    if (Index > MaxIndex) return ERROR;

    Red = RTable[Index];
    Green = GTable[Index];
    Blue = BTable[Index];
    return OK;
  }

  int ColorTable::setColor(int Index, unsigned char Red, unsigned char Green, unsigned char Blue)
  {
    if (Index > MaxIndex) return ERROR;

    RTable[Index] = Red;
    GTable[Index] = Green;
    BTable[Index] = Blue;
    return OK;
  }

  ColorTable::ColorTable(int MaxIndexP) :
    MaxIndex(MaxIndexP)
  {
    RTable = new unsigned char [MaxIndex + 1];
    GTable = new unsigned char [MaxIndex + 1];
    BTable = new unsigned char [MaxIndex + 1];
    setLinear(0, MaxIndex);
  }

  ColorTable::~ColorTable()
  {
    delete[] RTable;
    delete[] GTable;
    delete[] BTable;
  }

  int ColorTable::setLinear(int FirstIndex, int LastIndex)
  {
    int i;

    // catch invalid indices
    if (FirstIndex > LastIndex || LastIndex > MaxIndex) return ERROR;

    // overwrite old colors

    int delta = (LastIndex - FirstIndex);

    for (i = 0; i < FirstIndex; i++)
      RTable[i] = GTable[i] = BTable[i] = 255;

    for (i = LastIndex; i <= MaxIndex; i++)
      RTable[i] = GTable[i] = BTable[i] = 0;

    for (i = FirstIndex; i < LastIndex; i++)
      RTable[i] = GTable[i] = BTable[i] = 255 - ((i - FirstIndex) * 255) / delta;

    return OK;
  }

  int ColorTable::setTable(int type)
  {
    const unsigned char OverlayColorTable[6][3] =
    {
      {255,   0,   0},  // red
      {  0, 255,   0},  // green
      {  0,   0, 255},  // blue
      {255, 255,   0},  // yellow
      {  0, 255, 255},  // cyan
      {255,   0, 255},  // magenta
    };

    switch (type)
      {
      case 0:
        // value 0 == black
        RTable[0] = 0;
        GTable[0] = 0;
        BTable[0] = 0;
        for (int i = 1; i <= MaxIndex; i++)
          {
            int i1 = i - 1;
            int il = i1 % 6;
            int ih = i1 / 6;
            switch (ih % 3)
              {
              case 0:
                // normal
                RTable[i] = OverlayColorTable[il][0];
                GTable[i] = OverlayColorTable[il][1];
                BTable[i] = OverlayColorTable[il][2];
                break;
              case 1:
                // bright, low saturation
                RTable[i] = OverlayColorTable[il][0] / 2 + 127;
                GTable[i] = OverlayColorTable[il][1] / 2 + 127;
                BTable[i] = OverlayColorTable[il][2] / 2 + 127;
                break;
              case 2:
                // dark
                RTable[i] = OverlayColorTable[il][0] / 2;
                GTable[i] = OverlayColorTable[il][1] / 2;
                BTable[i] = OverlayColorTable[il][2] / 2;
                break;
              }
          }
        break;
      }

    return OK;
  }
}
