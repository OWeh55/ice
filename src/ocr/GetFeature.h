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

/**
* @file GetFeature.h
* @brief abstract getfeature
* @author Dr. Herbert Suesse, Marc Rossner, Erik Rodner, Bjoern Froehlich
* @date 2007-07-17
*/
#ifndef GETFEATUREINCLUDE
#define GETFEATUREINCLUDE

#include <iostream>

#include "base.h"
#include "Contur.h"
#include "Vector.h"

namespace ice
{

  /** Diese Klasse ist eine Abstrakte Schnittstelle
      um den Merkmalsvektor zu extrahieren
  */
  class GetFeature
  {

  protected:
    /** benutzt Funktion quantil_border oder nicht */
    bool quantil_border;

  public:
    /** Standard Konstruktor */
    GetFeature()
    {
      quantil_border = true;
    };

    GetFeature(bool qb)
    {
      quantil_border = qb;
    };

    /** virtueller Destruktor */
    virtual ~ GetFeature()
    {
    }

    /** liefert Merkmalsvektor in feature zurück */
    virtual int get_feature(const Image& pic,
                            const Contur& c,
                            Vector& feature,
                            Window& win) const = 0;

    /** liefert Merkmalsvektor in feature* zurück
     * @param pic Zu untersuchendes Rechteck
     * @param feature[] Array mit den zu bestimmenden Merkmalen
     * @return immer 0
     */
    virtual int get_feature(const Image& pic, Vector& feature) const = 0;

    virtual int getDimension() const = 0;

  };

}
#endif
