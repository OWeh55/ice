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

/*
 * Util.h
 * Funktion: Hilfsroutinen fuer diverse Zwecke
 * Datentyp TextEntry - veraltet, TextList nutzen !!
 * Datentyp TextList = deque<string>
 *
 * Hilfsfunktion MulDiv
 *
 * Wertebereichsbeschränkung von Zahlen (limited())
 */
#ifndef _TEXTLIST_H   /* verhindern mehrfacher Verarbeitung in einem */
#define _TEXTLIST_H   /* Programm */

#include <vector>

using namespace std;

namespace ice
{

  /* Textlisten mit stl */

  typedef std::vector<string> TextList;
  typedef TextList::iterator TextListIter;
  typedef TextList::const_iterator TextListConstIter;

}

#endif /* ifndef _TEXTLIST_H */
