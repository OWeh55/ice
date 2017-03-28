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

#include <fstream>
#include <sstream>

#include "strtool.h"
#include "Trafo.h"
#include "geo.h"
#include "lmdif.h"
#include "macro.h"
#include "geo.h"
#include "bairstow.h"

#include "DistortionR.h"

using namespace std;

namespace ice
{
  /*
   *  DistortionR
   */

#define FNAME "DistortionR::Distort"
  int DistortionR::Distort(double x, double y, double& xd, double& yd) const
  {
    double dx = x - x0;
    double dy = y - y0;
    double r = sqrt(dx * dx + dy * dy);
    double rfak = ffr(r);
    xd = dx * rfak + x0;
    yd = dy * rfak + y0;
    return OK;
  }
#undef FNAME
#define FNAME "DistortionR::Rect"
  int DistortionR::Rect(double x, double y, double& xd, double& yd) const
  {
    double xr = x - x0;
    double yr = y - y0;
    double r = sqrt(xr * xr + yr * yr);

    if (r != 0)
      {
        double rs = r;
        double afr = 1.0;
        int ct = 0;

        while ((fabs(afr) > 1e-9) // Abweichung zu groß -> weiter
               && (ct < 999))    // Notbremse
          {
            afr = fr(rs) - r; // current f(rs) - r
            double fs = frs(rs); // f'(rs)

            if (fs != 0.0)
              rs -= afr / fs;    // Newton
            else
              rs -= 1.0; // weiss nichts besseres

            ct++;
          }

        //  cout << ct << " " ;
        double fak = rs / r;
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
