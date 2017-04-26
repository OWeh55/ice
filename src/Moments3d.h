/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef ICE_MOMENTS_3D_H
#define ICE_MOMENTS_3D_H

#include <array>
#include <vector>
#include "Vector3d.h"

namespace ice
{
  class Moments3d
  {
  public:
    const static int vectorSize = 20;

    const static int ind000 = 0;
    const static int ind100 = 1;
    const static int ind010 = 2;
    const static int ind001 = 3;
    const static int ind011 = 4;
    const static int ind101 = 5;
    const static int ind110 = 6;
    const static int ind200 = 7;
    const static int ind020 = 8;
    const static int ind002 = 9;
    const static int ind111 = 10;
    const static int ind012 = 11;
    const static int ind021 = 12;
    const static int ind102 = 13;
    const static int ind201 = 14;
    const static int ind120 = 15;
    const static int ind210 = 16;
    const static int ind300 = 17;
    const static int ind030 = 18;
    const static int ind003 = 19;

    Moments3d()
    {
      for (auto& m : moments)
        {
          m = 0.0;
        }
    }

    explicit Moments3d(const std::vector<Vector3d>& pointList);
    explicit Moments3d(const std::vector<Vector3d>& pointList,
                       const std::vector<std::vector<int>>& triangleList);
    void addPoint(const Vector3d& p);
    void addTriangle(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3);

    void calcInvariants(std::vector< double >& invariants, bool scale = false) const;
  private:
    std::array<double, vectorSize> moments;
  };
}
#endif
