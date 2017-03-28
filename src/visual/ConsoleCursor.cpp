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

#include <wx/wx.h>

#include "visual/ConsoleWin.h"
#include "visual/ConsoleCursor.h"

namespace ice
{
  /*
   *      iceCursor
   */
  iceCursor::iceCursor(int x, int y)
    : wxPoint(x, y)
  {
  }

  iceCursor::iceCursor(const iceCursor& Cursor)
    : wxPoint(Cursor)
  {
  }

  iceCursor iceCursor::operator++()
  {
    x++;
    if (x >= SCREEN_XS)
      {
        x = 0;
        y++;
      }
    return *this;
  }

  iceCursor iceCursor::operator++(int)
  {
    iceCursor OldValue(*this);
    x++;
    if (x >= SCREEN_XS)
      {
        x = 0;
        y++;
      }
    return OldValue;
  }


  iceCursor iceCursor::operator--()
  {
    if (x > 0)
      x--;
    else
      {
        x = SCREEN_XS - 1;
        y--;
      }
    return *this;
  }

  iceCursor iceCursor::operator--(int)
  {
    iceCursor OldValue(*this);
    if (x > 0)
      x--;
    else
      {
        x = SCREEN_XS - 1;
        y--;
      }
    return OldValue;
  }

  iceCursor iceCursor::operator-(unsigned int Steps) const
  {
    iceCursor res = *this;
    unsigned int j;
    for (j = 0; j < Steps; j++) res--;
    return res;
  }

  iceCursor iceCursor::operator+(unsigned int Steps) const
  {
    iceCursor res = *this;
    unsigned int j;
    for (j = 0; j < Steps; j++) res++;
    return res;
  }

  iceCursor iceCursor::operator-=(unsigned int  Steps)
  {
    return this->operator-(Steps);
  }

  bool iceCursor::operator<(const iceCursor& Cursor) const
  {
    // if this cursor lies above the other or at least left
    // from it, we consider it smaller
    return (y < Cursor.y)  || ((y == Cursor.y) && (x < Cursor.x));
  }

  bool iceCursor::operator<=(const iceCursor& Cursor) const
  {
    // if this cursor lies above the other or at least left from
    // it, we consider it smaller
    return (y < Cursor.y || (y == Cursor.y && x <= Cursor.x));
  }

  int iceCursor::operator-(const iceCursor& Other) const
  {
    return x - Other.x + SCREEN_XS * (y - Other.y);
  }

  bool iceCursor::Inside() const
  {
    // x is always normalized to 0<=x<SCREEN_XS
    if (y < 0) return false;
    if (y > SCREEN_YS) return false;
    return true;
  }
}
