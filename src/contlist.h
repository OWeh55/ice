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
#ifndef _CONTLIST_H_
#define _CONTLIST_H_
//
//      Funktionen zur Arbeit mit Konturlisten
//

#include "Contur.h"

namespace ice
{

#define CONTURLISTBLOCKSIZE 10

  class ConturList
  {
  public:
    ConturList();  // Standard Constructor
    ConturList(const ConturList&);  // Copy Constructor
    int add(const Contur& c);
    int sub(int i);
    Contur* getContur(int i);
    ~ConturList();
    const int& number;
    ConturList& operator =(const ConturList& cl);
  protected:
    int conturs;
    int datalen;
    Contur** data;
  };
}
#endif
