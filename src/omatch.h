/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002-1014 FSU Jena, Digital Image Processing Group
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
#ifndef OMATCH_H
#define OMATCH_H

#include <vector>
#include <utility>

#include "geo.h"
#include "assignment.h"
#include "vectordistance.h"
#include "ObjectMatcher.h"

namespace ice
{
#define MM_LINEAR 1
#define MM_SQUARE 2

  template<typename Tobj1, typename Tobj2>
  Trafo MatchObject(const Tobj1& obj1, const Tobj2& obj2,
                    int tmode = TRM_AFFINE)
  {
    ObjectMatcher om(tmode);
    om.setObject(0, obj1);
    om.setObject(1, obj2);
    return om.getTrafo();
  }

  int ObjectPointFeatures(const std::vector<Point>& pl,
                          matrix<double>& feat,
                          int tmode = TRM_AFFINE);

}
#endif
