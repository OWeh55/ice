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

#include "message.h"
#include "macro.h"

#include "Vector.h"
#include "numbase.h"
#include "defs.h"
#include "numbase.h"
#include "base.h"
#include "ellipse.h"
#include "analygeo.h"

using namespace std;

namespace ice
{
  /*
   * Ellipse
   */
#define FNAME "Ellipse::Ellipse"
  void Ellipse::normalize()
  {
    if (r2 > r)
      {
        double h = r;
        r = r2;
        r2 = h;
        phi += M_PI / 2;
      }

    phi = FMod(phi, M_PI);

    cc = cos(phi);
    ss = sin(phi);
    aa = r * r;
    bb = r2 * r2;
  }

  bool Ellipse::inside_(Point pp) const
  {
    double xr = pp.x - p.x;
    double yr = pp.y - p.y;
    double f = Sqr(xr * cc + yr * ss) / aa +
               Sqr(-xr * ss + yr * cc) / bb - 1;
    return  f <= 0;
  }

  Ellipse::Ellipse(): Circle()
  {
    r2 = 0;
    phi = 0;
    normalize();
  }
  Ellipse::Ellipse(double xp, double yp,
                   double rp, double r2p, double phip) : Circle(xp, yp, rp)
  {
    r2 = r2p;
    phi = phip;
    normalize();
  }

  Ellipse::Ellipse(Point p,
                   double rp, double r2p, double phip) : Circle(p, rp)
  {
    r2 = r2p;
    phi = phip;
    normalize();
  }

  Ellipse::Ellipse(const Vector& v) : Circle(v)
  {
    if (v.Size() < 5)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    r2 = v[3];
    phi = v[4];
    normalize();
  }

  Ellipse::Ellipse(double p[]): Circle(p)
  {
    r2 = p[3];
    phi = p[4];
    normalize();
  }
#undef FNAME

#define FNAME "Ellipse::setR2"
  int Ellipse::setR2(double rv)
  {
    if (rv < 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    r2 = rv;
    normalize();
    return OK;
  }
#undef FNAME

  void Ellipse::setPhi(double rv)
  {
    phi = rv;
    normalize();
  }

  double Ellipse::distance_(Point pp) const
  {
    double par[5]; // old ellipse parameter form
    par[0] = p.x;
    par[1] = p.y;
    par[2] = r;
    par[3] = r2;
    par[4] = phi;

    double op[2];  // old point coordinates
    op[0] = pp.x;
    op[1] = pp.y;
    return DistPointEllipse(op, par);
  }

  /*
   * Ellipsen_segment
   */

  void EllipseSeg::normalize()
  {
    if (r2 > r)
      {
        // alte kleine Halbachse als neue Große Halbachse
        std::swap(r, r2);

        phi += M_PI / 2;
        phi1 -= M_PI / 2;
        phi2 -= M_PI / 2;
      }

    phi = FMod(phi, 2 * M_PI);

    if (phi > M_PI)
      {
        phi -= M_PI;
        phi1 += M_PI;
        phi2 += M_PI;
      }

    phi1 = FMod(phi1, 2 * M_PI);
    phi2 = FMod(phi2, 2 * M_PI);

    if (phi2 < phi1)
      {
        phi2 += 2 * M_PI;
      }

    phi1a = FMod(phi1 + phi, 2 * M_PI);
    phi2a = FMod(phi2 + phi, 2 * M_PI);

    if (phi2a < phi1a)
      {
        phi2a += 2 * M_PI;
      }

    cc = cos(phi);
    ss = sin(phi);
    aa = r * r;
    bb = r2 * r2;
  }

#define FNAME "EllipseSeg::EllipseSeg"
  EllipseSeg::EllipseSeg(): Ellipse() {} ;
  EllipseSeg::EllipseSeg(const EllipseSeg& es): Ellipse(es)
  {
    phi1 = es.phi1;
    phi2 = es.phi2;
  }
  EllipseSeg::EllipseSeg(double xp, double yp,
                         double rp, double r2p, double phi,
                         double phi1p, double phi2p) : Ellipse(xp, yp, rp, r2p, phi)
  {
    phi1 = phi1p;
    phi2 = phi2p;
    normalize();
  }

  EllipseSeg::EllipseSeg(Point p,
                         double rp, double r2p, double phi,
                         double phi1p, double phi2p): Ellipse(p, rp, r2p, phi)
  {
    phi1 = phi1p;
    phi2 = phi2p;
    normalize();
  }

  EllipseSeg::EllipseSeg(const Vector& v) : Ellipse(v)
  {
    if (v.Size() < 7)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return;
      }

    phi1 = v[5];
    phi2 = v[6];
    normalize();
  }

  EllipseSeg::EllipseSeg(double d[]) : Ellipse(d)
  {
    phi1 = d[5];
    phi2 = d[6];
    normalize();
  }

#undef FNAME
  void EllipseSeg::setPhi1(double val)
  {
    phi1 = val;
    normalize();
  }

  void EllipseSeg::setPhi2(double val)
  {
    phi2 = val;
    normalize();
  }

  bool EllipseSeg::inside_(Point pp) const
  {
    double xr = pp.x - p.x;
    double yr = pp.y - p.y;

    double fi = atan2(yr, xr);

    if (fi < phi1a)
      {
        fi += 2 * M_PI;
      }

    if (fi > phi2a)
      {
        return false;
      }

    double f = Sqr(xr * cc + yr * ss) / aa +
               Sqr(-xr * ss + yr * cc) / bb - 1;
    return f <= 0;
  }

}
