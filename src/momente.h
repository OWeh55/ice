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
/*
 * Berechnung und Transformation von Momenten
 * Berechnung von Invarianten
 *
 *          M.Schubert   3/1995
 *          W.Ortmann   11/2001,09/2005,02/2009,09/2012
 *          H.Suesse    01/2009
 */
#ifndef _MOMENTE_H
#define _MOMENTE_H

#include <vector>
#include "pointlist.h"
#include "Point.h"
#include "Contur.h"
#include "Trafo.h"

namespace ice
{
// calculate moments
  int MomentPolygon(const Matrix& p, double m[15], double center[2]);
  int MomentPolygon(PointList p, double m[15], double center[2]);
  int MomentPolygon(const std::vector<Point>& pl, double m[15], double center[2]);

  int PointListMoment(PointList pl, int a1, int a2, double m[15], double center[2]);

// transform moments
  int TranslateMoments(const double m1[15], double x, double y, double m2[15]);
  int XShearMoments(const double m[15], double a, double ms[15]);
  int YShearMoments(const double m1[15], double b, double m2[15]);
  int ScaleMoments(const double m1[15], double a, double b, double m2[15]);
  int RotateMoments(const double m1[15], double c, double s, double m2[15]);
  int RotateMoments(const double m1[15], double fi, double m2[15]);
  int AffinTransMoments(const double m1[15], double tr[3][3], double m2[15]);

// normalize moments
  int NormalizeMomentsTranslation(const double m[15], double msk[15],
                                  double& xs, double& ys);
  int NormalizeMomentsXShearing(const double m[15], double msk[15],
                                double& beta);
  int NormalizeMomentsYShearing(const double m[15], double msk[15],
                                double& beta);
  int NormalizeMomentsScaling(const double m[15], double msk[15],
                              double& alpha, double& gamma);
  int NormalizeMomentsRotation(const double m[15], double msk[15],
                               double& cosinus, double& sinus);

  int AffinNormMoments(const double m[15], double maf[15], double atr[3][3]);
  int PolyNormMoments(const double m1[15], double m2[15], double atr[3][3]);
  int AffinIterateMoments(const double m[15], double maf[15], double atr[3][3]);

//
  int CalcCentralMoments(const double m[15], double mc[15]);

  int NormalizeMomentsArea(const double m1[15], double m2[15]);
  int NormalizeMomentsArea(const double m1[15], double m2[15], double& alpha);

// invariants from moments
  int AffinAlgebraicInvar(const double m[15], double flu[4]);
  int HuInvariants(const double ms[15], double hu[7]);
  int AffinHuInvar(const double m[15], double hu[7], bool trans = true);

// fitting with moments
  double AffinFitMoments(const double m1[15], const double m2[15],
                         double tr[3][3]);
  double AffinFitMoments(const double m1[15], const double m2[15],
                         Trafo& tr);

  double DiffMoments(const double m1[15], const double m2[15]);
  double DiffMoments2(const double m1[15], const double m2[15]);

  void CopyMoments(const double ms[15], double md[15]);
  int PosSign(const double ms[15], double md[15]);
  int PosSign(double ms[15]);

  // Abkürzungen für die Indizes der Momente
#define i00 (0)
#define i10 (1)
#define i01 (2)
#define i20 (3)
#define i11 (4)
#define i02 (5)
#define i30 (6)
#define i21 (7)
#define i12 (8)
#define i03 (9)
#define i40 (10)
#define i31 (11)
#define i22 (12)
#define i13 (13)
#define i04 (14)

}
#endif
