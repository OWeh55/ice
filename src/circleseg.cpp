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
#include <math.h>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "numbase.h"
#include "base.h"
#include "lineseg.h"
#include "paint.h"

#include "circleseg.h"

namespace ice
{
  /*
   * ------- CircleSeg ---------------
   */
#define FNAME "CircleSeg::CircleSeg"
  void CircleSeg::normalize()
  {
    phi1 = FMod(phi1, 2 * M_PI);
    phi2 = FMod(phi2, 2 * M_PI);

    if (phi2 < phi1)
      {
        phi2 += 2 * M_PI;
      }
  }

  CircleSeg::CircleSeg(const Vector& v) : Circle(v)
  {
    if (v.Size() < 5)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    phi1 = v[3];
    phi2 = v[4];
    normalize();
  }
#undef FNAME

  bool CircleSeg::inside_(Point pp) const
  {
    if (Circle::inside_(pp))
      {
        double fi = FMod(atan2(pp.y - p.y, pp.x - p.x), 2 * M_PI);
        return (phi_inside(fi));
      }

    return false;
  }

  void CircleSeg::setPhi1(double phi1p)
  {
    phi1 = phi1p;
    normalize();
  }

  void CircleSeg::setPhi2(double phi2p)
  {
    phi2 = phi2p;
    normalize();
  }

  double CircleSeg::distance_(Point pp) const
  {
    double d;

    double res = LineSeg(p, Point(p.x + r * cos(phi1), p.y + r * sin(phi1))).distance_(pp);

    if ((d = LineSeg(p, Point(p.x + r * cos(phi2), p.y + r * sin(phi2))).distance_(pp)) < res)
      {
        res = d;
      }

    Point delta = pp - p;
    double fi = FMod(delta.phi(), 2 * M_PI);

    if (phi_inside(fi))
      if ((d = Circle::distance_(pp)) < res)
        {
          res = d;
        }

    return res;
  }
}
