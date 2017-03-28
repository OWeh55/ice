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
#ifndef BOXES_H
#define BOXES_H

#include <string>

using namespace std;

namespace ice
{

#define B_CENTER 0
#define B_LEFT 1
#define B_RIGHT 2
#define B_TOP 4
#define B_BOTTOM 8

#define STRLEN 300

  int Box(int x1, int y1, int x2, int y2, bool frame = true);
  int SetBox(int xs, int ys, int where);
  int MessageBox(const char* s, int where = 0);
  int MessageBox(const string& s, int where = 0);
  int Ask(const char* p, const char* yn);
  int Ask(const string& p, const string& yn = "YN");
  int Enter(const char* s);
  int Enter(const string& s);

}
#endif



