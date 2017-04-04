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

#include "IceException.h"
#include "macro.h"
#include "Matrix.h"
#include "MatrixAlgebra.h"

#include "analygeo3d.h"

namespace ice
{
  double Distance(const Vector3d& p1, const Vector3d& p2)
  {
    return p1.Distance(p2);
  }

  Matrix MakeMatrix(const Vector& v, int rows, int cols)
  {
    Matrix res(rows, cols);
    int i = 0;

    for (int j = 0; j < res.rows(); j++)
      for (int k = 0; k < res.cols(); k++)
        {
          res[j][k] = v[i++];
        }

    return res;
  }

  double Distance(const Vector3d& p, const Line3d& l, double& par)
  {
    Vector pv(p);
    Vector p1v(l.Pos());
    Vector dv(l.DP());
    Matrix m = MakeMatrix(dv, 3, 1);
    Vector res = SolveLinEqu(m, pv - p1v);
    par = res[0];
    return ((res[0] * dv + p1v) - pv).Length();
  }

  double Distance(const Vector3d& p, const Line3d& l)
  {
    double par;
    return Distance(p, l, par);
  }

  double Distance(const Line3d& l, const Vector3d& p)
  {
    return Distance(p, l);
  }

  double Distance(const Line3d& l1, const Line3d& l2)
  {
    Vector p1v(l1.P1());
    Vector p2v(l2.P1());
    Vector d1v(l1.DP());
    Vector d2v(l2.DP());
    Matrix m = d1v || (-1 * d2v);
    Vector res;

    try {
      res = SolveLinEqu(m, p2v - p1v);
    }
    catch (IceException&ex)
    {
      // parallele Geraden
      return Distance(l1.P1(), l2);
    }
    return (res[0] * d1v + p1v - res[1] * d2v - p2v).Length();
  }

#define FNAME "Intersection"
  int Intersection(const Line3d& l1, const Line3d& l2,
                   Vector3d& p, double& dist, Vector& res)
  {
    Vector3d p1v(l1.P1());
    Vector3d p2v(l2.P1());
    Vector3d d1v(l1.DP());
    Vector3d d2v(l2.DP());
    Matrix m = d1v || (-d2v) ;

    try {
      res = SolveLinEqu(m, p2v - p1v);
    }
    catch (IceException&ex)
    {
      dist = 1e99;
      res = Vector(0.0, 0.0);
      // parallele Geraden
      return PARALLEL;
    }

    Vector p1(res[0]*d1v + p1v);
    Vector p2(res[1]*d2v + p2v);
    p = Vector3d((p1 + p2) * 0.5);
    dist = ((Vector)p - p1).Length();
    return OK;
  }

  int Intersection(const Line3d& l1, const Line3d& l2,
                   Vector3d& p, double& dist)
  {
    Vector res;
    return Intersection(l1, l2, p, dist, res);
  }

  int Intersection(const Line3d& l1, const Line3d& l2,
                   Vector3d& p)
  {
    double dist;
    return Intersection(l1, l2, p, dist);
  }

  int Intersection(const Line3d& l, const Sphere& s, Vector3d& p1, Vector3d& p2)
  {
    double p;
    double distp = Distance(s.Pos(), l, p);

    // Abstand vom Mittelpunkt, p beschreibt Mittelpunkt-nahesten Punkt
    if (distp > s.R())
      {
        p1 = p2 = l(p);
        return NO_INTERSECTION;
      }

    if (distp == s.R())
      {
        // Tangente
        p1 = p2 = l(p);
        return TANGENT;
      }

    // notwendige "Verschiebung" von p auf der Geraden
    double dp = sqrt(s.R() * s.R() - distp * distp);
    dp = dp / l.DP().Length();
    p1 = l(p - dp);
    p2 = l(p + dp);
    return OK;
  }

#undef FNAME
}
