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
/***************************************************************
  Header zu readwrit.h
****************************************************************/

#ifndef _READWRIT_H /* verhindern mehrfacher Verarbeitung in einem */
#define _READWRIT_H /* Programm */

//using std::min;
//using std::max;

namespace ice
{
  /* Funktionen fuer Ein/Ausgabe von Strukturen */

  /* Ausgabe eines "double"-Arrays als File */
  int WriteArrayD(double* ptrd, int number, char name[]);

  /* Lesen eines "double-Arrays" von ext. Datentraeger */
  int ReadArrayD(char name[], int number, double* array);
}

#endif  /* IFNDEF _READWRITE */
