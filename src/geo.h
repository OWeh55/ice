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
#ifndef GEO_H
#define GEO_H

#include <vector>
#include "pointlist.h"

#include "Trafo.h"

#include "Matrix.h"
#include "Vector3d.h"

namespace ice
{
  Contur Transform(const Trafo& tr, const Contur& c);
  void TransformList(const Trafo& tr, Matrix& m);
  void TransformList(const Trafo& tr, const Matrix& m1, Matrix& m2);

  Trafo MatchPano(const Matrix& p1, const Matrix& p2, int mode);
  Trafo MatchPano(const Matrix& p1, const Matrix& p2, int mode, const Vector& weight);

  Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                        int mode = TRM_PROJECTIVE);
  Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                        int mode, const Vector& weight);

  Trafo MatchPointlists(const PointList& pl1, const PointList& pl2, int mode);

  Trafo MatchPointlists(const std::vector<Point>& pl1,
                        const std::vector<Point>& pl2,
                        int mode = TRM_PROJECTIVE);
  Trafo MatchPointlists(const std::vector<Point>& pl1,
                        const std::vector<Point>& pl2,
                        int mode, const std::vector<double>& weight);
}
#endif
