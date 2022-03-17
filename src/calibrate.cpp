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
#include "defs.h"
#include "IceException.h"
#include "LMSolver.h"
#include "Camera.h"
#include "vectortools.h"
#include "calibc.h"

using namespace std;

namespace ice
{
  class CalibFunctor: public LMFunctor
  {
  public:
    CalibFunctor(Camera& cam,
                 const std::vector<Vector3d>& xyz,
                 const std::vector<Point>& uv):
      cam(cam), xyz(xyz), uv(uv), nPoints(uv.size())
    {
      if (xyz.size() != nPoints)
        throw IceException("CalibFunctor", M_DIFFERENT_LISTSIZE);
    }

    int getDimension() const
    {
      return nPoints * 2;
    }

    int operator()(const vector<double>& p, vector<double>& result) const
    {
      cam.set(p);
      int idx = 0;
      for (int i = 0; i < nPoints; i++)
        {
          Point p2 = cam.transform(xyz[i]);
          Point diff = p2 - uv[i];
          result[idx++] = diff.x;
          result[idx++] = diff.y;
        }
      return 0;
    }

  private:
    Camera& cam;
    const std::vector<Vector3d>& xyz;
    const std::vector<Point>& uv;
    int nPoints;
  };

#define FNAME "calibrate"
  void calibrate(Camera& cam,
                 const std::vector<Vector3d>& xyz,
                 const std::vector<Point>& uv,
                 int mode)
  {
    CalibFunctor cf(cam, xyz, uv);
    vector<double> cameraParameter = cam.makeVectorDouble();
    LMSolver lm(cf);
    lm.solve(cameraParameter);
  }
#undef FNAME
} // namespace ice
