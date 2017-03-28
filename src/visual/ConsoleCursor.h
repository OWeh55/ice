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

#ifndef CONSOLE_CURSOR
#define CONSOLE_CURSOR

#include <wx/frame.h>

namespace ice
{
  class iceCursor : public wxPoint
  {
  protected:
//      int subposition;
  public:
    iceCursor(const iceCursor& Cursor);
    iceCursor(int x, int y);

    /** Moving the cursor backwards and forwards (as if the user moves through the window
        the left and right arrow keys).*/

    iceCursor operator++();
    iceCursor operator--();
    iceCursor operator++(int);  // the prefix versions
    iceCursor operator--(int);
    iceCursor operator-=(unsigned int  Steps);
    iceCursor operator-(unsigned int Steps) const;
    iceCursor operator+(unsigned int Steps) const;

    /* Returns the number of Characters between the two cursor positions,
     */
    int operator-(const iceCursor& Other) const;

    /* Compares two cursors: a cursor position ist less than another,
       if the cursor is above or left.*/
    bool operator<(const iceCursor& Other) const;
    bool operator<=(const iceCursor& Other) const;

    bool Inside() const;
  };
}
#endif
