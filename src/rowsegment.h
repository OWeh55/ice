/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2013 FSU Jena, Digital Image Processing Group
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
#ifndef ROW_SEGMENT_H
#define ROW_SEGMENT_H
// class RowSegment
// class to store a Segment of a row in an image
// used to export the segments from an Region

namespace ice
{
  class RowSegment
  {
  public:
    RowSegment(): y(0), x1(0), x2(-1) {}
    RowSegment(int x1, int x2, int y): y(y), x1(x1), x2(x2) {}
    int y;
    int x1, x2;
  };
}
#endif
