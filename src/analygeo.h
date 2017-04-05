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
#ifndef _ANALYGEO_H
#define _ANALYGEO_H

#include <stdio.h>

#include "pointlist.h"

#include "Vector.h"
#include "Matrix.h"
#include "Point.h"
#include "Window.h"

namespace ice
{
  /*** 2D ************************************************************/
  void ConvCartesPolar(double p[2], double* rad, double* arcus);
  void ConvCartesPolar(double x, double y, double& rad, double& arcus);

  template<typename T>
  void ConvCartesPolar(pointT<T> p, double& rad, double& arcus)
  {
    rad = sqrt(Sqr(p.x) + Sqr(p.y));
    arcus = atan2(p.y, p.x);
  }

  void ConvPolarCartes(double rad, double arcus, double koord[2]);
  void ConvPolarCartes(double rad, double arcus, double& x, double& y);

  template<typename T>
  void ConvPolarCartes(double rad, double arcus, pointT<T>& p)
  {
    p.x = (T)(cos(arcus) * rad);
    p.y = (T)(sin(arcus) * rad);
  }

  int ConvPointHesse(double p1[2], double p2[2], double* p, double* phi);
  Vector ConvPointHesse(const Vector& p1, const Vector& p2);

#define FNAME "ConvPointHesse"
  template<typename T>
  int ConvPointHesse(pointT<T> p1, pointT<T> p2, double& p, double& phi)
  {
    double dx = p2.x - p1.x; // d2
    double dy = p2.y - p1.y; // d1

    double dd = sqrt(dx * dx + dy * dy);

    if (dd < EPSILON)
      {
        throw IceException(FNAME, M_POINT_IDENTIC, POINT_IDENTIC);
      }

    p = (p1.x * dy - p1.y * dx) / dd;

    if (p < 0)
      {
        p = -p;
        dx = -dx;
        dy = -dy;
      }

    phi = atan2(-dx, dy);
    return OK;
  }
#undef FNAME

  void RotTransPoint(double p1[2], double centre[2], double arcus, double p2[2]);

  double DistPointPoint(double p1[2], double p2[2]);
  double DistPointLine(double v[2], double p, double phi);
  double DistPointCircle(double p[2], double* par);
  double DistPointEllipse(double p[2], double* par, double* koord = nullptr);

  int IntersecLine(double p1, double ph1, double p2, double phi2, double* ip);
  int TangentCircle(double p[2], double* par, double pt1[2], double pt2[2]);

  double AreaPolygon(PointList pl);
  double AreaPolygon(const Matrix& pl);

  int FeatureQuadrFunc(double par[6], double feature[5], int* type);
  int ParamQuadrFunc(double feature[5], int type, double par[6]);

  double ClosestPointLineSeg(double p[2], double ls0[2],
                             double ls1[2], double pf[2]);

  /*** 3D ************************************************************/
  int ConvR3PointHesse(double p1[3], double p2[3], double p3[3],
                       double* p, double n[3]);
  double DistR3PointLine(double p[3], double displ[3], double dir[3]);
  double DistR3PointPlane(double v[3], double p, double n[3]);
}
#endif
