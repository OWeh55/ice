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
* @file GetFeatureHist.h
* @brief Merkmalsextraktion mit Fourierdeskriptoren und Histogrammlinearisierung
* @author Bjoern Froehlich
* @date 2007-02-23
*/
#ifndef GETFEATUREHISTINCLUDE
#define GETFEATUREHISTINCLUDE

#include <iostream>
#include "ocr/GetFeature.h"

namespace ice
{

  class GetFeatureHist: public GetFeature
  {
  protected:
    /** Anzahl Fourierdeskriptoren */
    int fd;

    /** Gitter */
    int gitter1;
    int gitter2;

  public:
    /** Standard Konstruktor */
    GetFeatureHist(): GetFeature()
    {
      fd = 18;
    };

    /** empfohlener Konstruktor */
    GetFeatureHist(int g1, int g2, int f, bool qb = true):
      GetFeature(qb), fd(f), gitter1(g1), gitter2(g2)
    {
    };

    /** liefert Merkmalsvektor in feature zurück */
    int get_feature(const Image& pic,
                    const Contur& c, Vector& feature,
                    Window& w) const;

    virtual int getDimension() const
    {
      return gitter1 * gitter2 + 2 * fd - 1;
    };
  };

}

#endif
