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


/**
* @file GetFeatureFD.h
* @brief Merkmalsextrahierung mit Fourierdeskriptoren und speziellen Normalisierungsverfahren
* @author Bjoern Froehlich
* @date 2007-02-20
*/
#ifndef GETFEATUREFDINCLUDE
#define GETFEATUREFDINCLUDE

#include <iostream>
#include "ocr/GetFeatureStandard.h"

namespace ice
{

  class GetFeatureFD: public GetFeatureStandard
  {
  protected:
    int fd;

  public:
    /** Standard Konstruktor */
    GetFeatureFD(): GetFeatureStandard(6, 10, true)
    {
      fd = 18;
    };

    /** empfohlener Konstruktor */
    GetFeatureFD(int g1, int g2, int f, bool qb = true):
      GetFeatureStandard(g1, g2, qb)
    {
      fd = f;
    };

    /** liefert Merkmalsvektor in feature[] zurück */
    int get_feature(const Image& pic,
                    const Contur& c, Vector& feature,
                    Window& win) const;

    virtual int getDimension() const
    {
      return gitter1 * gitter2 + (2 * fd - 1);
    }
  };

}

#endif
