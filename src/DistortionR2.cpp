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

#include <fstream>
#include <sstream>

#include "strtool.h"
#include "Trafo.h"
#include "geo.h"
#include "lmdif.h"
#include "macro.h"
#include "geo.h"
//#include "bairstow.h"

#include "DistortionR2.h"

using namespace std;

namespace ice
{
#define FNAME "DistortionR::Distort"
  int DistortionR2::distort(double x, double y, double& xd, double& yd) const
  {
    double dx = x - x0;
    double dy = y - y0;
    double r2 = dx * dx + dy * dy;
    double rfak = ffr2(r2);
    xd = dx * rfak + x0;
    yd = dy * rfak + y0;
    return OK;
  }
#undef FNAME

#define FNAME "DistortionR::rectify"
  int DistortionR2::rectify(double x, double y, double& xd, double& yd) const
  {
    double xr = x - x0;
    double yr = y - y0;
    double distorted_radius2 = xr * xr + yr * yr;
    double distorted_radius  = sqrt(distorted_radius2);

    if (distorted_radius != 0)
      {
        double rectified_radius = distorted_radius;
        double afr = 1.0;
        int ct = 0;

        while ((fabs(afr) > 1e-9) // Abweichung zu groß -> weiter
               && (ct < 999))     // Notbremse
          {
            afr = fr(rectified_radius) - distorted_radius; // current f(rectified_radius) - r
            double fs = frs(rectified_radius);   // f'(rectified_radius)

            if (fs != 0.0)
              {
                rectified_radius -= afr / fs;  // Newton
              }
            else
              {
                rectified_radius -= 1.0;  // do something to leave this point
              }

            ct++;
          }

        //  cout << ct << " " ;
        double fak = rectified_radius / distorted_radius;
        xd = xr * fak + x0;
        yd = yr * fak + y0;
      }
    else
      {
        xd = x;
        yd = y;
      }

    return OK;
  }
#undef FNAME
}
