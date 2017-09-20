/*
 * ICE - C++ - Library for image processing 
 *
 * Copyright (C) 1992..2016 FSU Jena, Digital Image Processing Group
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
#include "defs.h"
#include "IceException.h"
#include "LMSolver.h"
#include "Camera.h"

#include "calibc.h"

using namespace std;

namespace ice {
  class CalibFunctor:public LMFunctor
  {
  public:
    CalibFunctor(const std::vector<Vector3d>& xyz,
		 const std::vector<Point>& uv):xyz(xyz),uv(uv)
    {
    }
    int getDimension() const
    {
      return uv.size()*2;
    }
    
    int operator()(const vector<double>& p, vector<double>& result) const
    {
      
    }

  private:
    const std::vector<Vector3d>& xyz;
    const std::vector<Point>& uv;
  };

#define FNAME "calibrate"
  void calibrate(Camera& cam, 
		 const std::vector<Vector3d>& xyz, 
		 const std::vector<Point>& uv, 
		 int mode)
  {
    CalibFunctor cf(xyz,uv);
    
  }
#undef FNAME
} // namespace ice
