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
#ifndef GEO_H
#define GEO_H

#include <vector>
#include "pointlist.h"

#include "Trafo.h"

#include "Matrix.h"
#include "Vector3d.h"

namespace ice
{
  Contur transform(const Trafo& tr, const Contur& c);
  void transformList(const Trafo& tr, Matrix& m);
  void transformList(const Trafo& tr, const Matrix& m1, Matrix& m2);

  Trafo MatchPano(const Matrix& p1, const Matrix& p2, int mode);
  Trafo MatchPano(const Matrix& p1, const Matrix& p2, int mode, const Vector& weight);

  Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                        int mode = TRM_PROJECTIVE);
  Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                        int mode, const Vector& weight);

  Trafo MatchPointlists(const PointList& pl1, const PointList& pl2, int mode);

  Trafo iterateProjective(const Trafo& tr,
                          const std::vector<Point>& p1,
                          const std::vector<Point>& p2);

  Trafo matchPointListsProjective(const std::vector<Point>& pl1,
                                  const std::vector<Point>& pl2,
                                  const std::vector<double>& weight);

  Trafo matchPointListsProjective(const std::vector<Point>& pl1,
                                  const std::vector<Point>& pl2);

  Trafo matchPointListsProjectiveLinear(const std::vector<Point>& pl1,
                                        const std::vector<Point>& pl2,
                                        const std::vector<double>& weight);

  Trafo matchPointListsProjectiveLinear(const std::vector<Point>& pl1,
                                        const std::vector<Point>& pl2);

  Trafo matchPointListsShift(const std::vector<Point>& pl1,
                             const std::vector<Point>& pl2,
                             const std::vector<double>& weight);

  Trafo matchPointListsShift(const std::vector<Point>& pl1,
                             const std::vector<Point>& pl2);

  Trafo matchPointListsShiftScale(const std::vector<Point>& pl1,
                                  const std::vector<Point>& pl2,
                                  const std::vector<double>& weight);

  Trafo matchPointListsShiftScale(const std::vector<Point>& pl1,
                                  const std::vector<Point>& pl2);

  Trafo matchPointListsRotate(const std::vector<Point>& pl1,
                              const std::vector<Point>& pl2,
                              const std::vector<double>& weight);

  Trafo matchPointListsRotate(const std::vector<Point>& pl1,
                              const std::vector<Point>& pl2);

  Trafo matchPointListsEuclidean(const std::vector<Point>& pl1,
                                 const std::vector<Point>& pl2);

  Trafo matchPointListsEuclidean(const std::vector<Point>& pl1,
                                 const std::vector<Point>& pl2,
                                 const std::vector<double>& weight);

  Trafo matchPointListsSimilarity(const std::vector<Point>& pl1,
                                  const std::vector<Point>& pl2,
                                  const std::vector<double>& weight);

  Trafo matchPointListsSimilarity(const std::vector<Point>& pl1,
                                  const std::vector<Point>& pl2);

  Trafo matchPointListsAffine(const std::vector<Point>& pl1,
                              const std::vector<Point>& pl2,
                              const std::vector<double>& weight);

  Trafo matchPointListsAffine(const std::vector<Point>& pl1,
                              const std::vector<Point>& pl2);

  Trafo matchPointLists(const std::vector<Point>& pl1,
                        const std::vector<Point>& pl2,
                        int mode = TRM_PROJECTIVE);
  Trafo matchPointLists(const std::vector<Point>& pl1,
                        const std::vector<Point>& pl2,
                        int mode, const std::vector<double>& weight);
}
#endif
