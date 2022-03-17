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
#include "macro.h"
#include "momente.h"
#include "lists.h"
#include "contfeat.h"
#include "region.h"
#include "momente.h"
#include "moments.h"
#include "affinfit.h"

namespace ice
{
#define FNAME "Moments::Moments"
  Moments::Moments(double m[15])
  {
    for (int i = 0; i < 15; i++)
      {
        mom[i] = m[i];
      }
  }

  Moments::Moments(const Region& r)
  {
    init();
    r.calcMoments(*this);
  }

  Moments::Moments(const Contur& c)
  {
    double s[2];
    MomentRegion(c, mom, s);
  }

  Moments::Moments(const Matrix& p)
  {
    double s[2];
    MomentPolygon(p, mom, s);
  }

  Moments::Moments(const std::vector<Point>& pl)
  {
    double s[2];
    MomentPolygon(pl, mom, s);
  }

  Moments::Moments(PointList p)
  {
    double s[2];
    MomentPolygon(p, mom, s);
  }

  Moments::Moments(PointList p, int a1, int a2)
  {
    double s[2];
    PointListMoment(p, a1, a2, mom, s);
  }

#undef FNAME
  Moments Moments::NormalizeSign() const
  {
    Moments res;

    if (mom[0] < 0)
      {
        for (int i = 0; i < 15; i++)
          {
            res.mom[i] = -mom[i];
          }
      }
    else
      {
        for (int i = 0; i < 15; i++)
          {
            res.mom[i] = mom[i];
          }
      }

    return res;
  }

#define FNAME "Moments::Center"
  Point Moments::Center() const
  {
    if (mom[i00] == 0.0)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    return Point(mom[i10] / mom[i00], mom[i01] / mom[i00]);
  }
#undef FNAME
#define FNAME "Moments::operator ()"
  double& Moments::operator()(int xi, int yi)
  {
    int s = xi + yi;
    const int os[] = {0, 1, 3, 6, 10};

    if ((xi < 0) || (yi < 0) || (s > 4))
      throw IceException(FNAME, M_WRONG_INDEX);

    return mom[os[s] + yi];
  }

  const double& Moments::operator()(int xi, int yi) const
  {
    int s = xi + yi;
    const int os[] = {0, 1, 3, 6, 10};

    if ((xi < 0) || (yi < 0) || (s > 4))
      throw IceException(FNAME, M_WRONG_INDEX);

    return mom[os[s] + yi];
  }
#undef FNAME
#define FNAME "Moments::Add"
  void Moments::add(const Moments& m)
  {
    for (int i = 0; i < 15; i++)
      mom[i] += m.mom[i];
  }

  void Moments::add(double m[15])
  {
    for (int i = 0; i < 15; i++)
      mom[i] += m[i];
  }

  void Moments::add(Point p, double val)
  {
    double x1 = p.x;
    double x2 = x1 * x1;
    double x3 = x2 * x1;
    double x4 = x2 * x2;

    double y1 = p.y;
    double y2 = y1 * y1;
    double y3 = y2 * y1;
    double y4 = y2 * y2;

    mom[i00] += val;

    mom[i10] += val * x1;
    mom[i01] += val * y1;

    mom[i20] += val * x2;
    mom[i11] += val * x1 * y1;
    mom[i02] += val *      y2;

    mom[i30] += val * x3;
    mom[i21] += val * x2 * y1;
    mom[i12] += val * x1 * y2;
    mom[i03] += val *      y3;

    mom[i40] += val * x4;
    mom[i31] += val * x3 * y1;
    mom[i22] += val * x2 * y2;
    mom[i13] += val * x1 * y3;
    mom[i04] += val *      y4;
  }

  void Moments::add(int y, int px1, int px2)
  {
    // Zeilen-Segment zu Momenten hinzufügen

    double y2 = y * y;
    double y3 = y2 * y;
    double y4 = y2 * y2;

    double m = px1;
    double m2 = m * m;
    double m3 = m2 * m;
    double m4 = m2 * m2;
    double m5 = m3 * m2;

    double n = px2;
    double n2 = n * n;
    double n3 = n2 * n;
    double n4 = n2 * n2;
    double n5 = n3 * n2;

    // y components [y-0.5..y+0.5]
    // double Y0 = 1;
    // double Y1 = y;
    double Y2 = y2 + 1.0 / 12.0;
    double Y3 = y3 + y / 4.0;
    double Y4 = y4 + 0.5 * y2 + 1.0 / 16.0 / 5.0;

    // x components [px1-0.5 ... px2+0.5]
    double X0 = n - m + 1;
    double X1 = 0.5 * (n + m) * (n - m + 1);
    double X2 = (4 * (n3 - m3) + 6 * (n2 + m2) + 3 * (n - m) + 1) / 12.0;
    double X3 = (2 * (n4 - m4) + 4 * (n3 + m3) + 3 * (n2 - m2) + m + n) / 8.0;
    double X4 = (16 * (n5 - m5) + 40 * (n4 + m4) + 40 * (n3 - m3) + 20 * (n2 + m2)
                 + 5 * (n - m) + 1) / 80.0;

    mom[i00] += X0 /* * Y0*/;

    mom[i10] += X1 /* * Y0*/;
    mom[i01] += X0 * y /* == Y1*/;

    mom[i20] += X2 /* *Y0 */;
    mom[i11] += X1 * y /* ==Y1 */;
    mom[i02] += X0 * Y2;

    mom[i30] += X3 /* *Y0 */;
    mom[i21] += X2 * y /* ==Y1 */;
    mom[i12] += X1 * Y2;
    mom[i03] += X0 * Y3;

    mom[i40] += X4  /* *Y0 */;
    mom[i31] += X3 * y /* ==Y1 */;
    mom[i22] += X2 * Y2;
    mom[i13] += X1 * Y3;
    mom[i04] += X0 * Y4;
  }

  void Moments::add(const Region& reg)
  {
    reg.calcMoments(*this);
  }

  void Moments::add(const Contur& c)
  {
    double s[2];
    double m[15];
    MomentRegion(c, m, s);
    add(m);
  }

  void Moments::add(const Matrix& p)
  {
    double s[2];
    double m[15];
    MomentPolygon(p, m, s);
    add(m);
  }

  void Moments::add(const std::vector<Point>& pl)
  {
    double s[2];
    double m[15];
    MomentPolygon(pl, m, s);
    add(m);
  }

  void Moments::add(PointList p)
  {
    double s[2];
    double m[15];
    MomentPolygon(p, m, s);
    add(m);
  }

  void Moments::add(PointList pl, int a1, int a2)
  {
    double s[2];
    double m[15];
    PointListMoment(pl, a1, a2, m, s);
    add(m);
  }
#undef FNAME

#define FNAME "Moments::CentralMoments"
  Moments Moments::CentralMoments() const
  {
    try
      {
        double res[15];
        CalcCentralMoments(mom, res);
        return Moments(res);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Moments::XShear"
  Moments Moments::XShear(double a) const
  {
    double res[15];
    XShearMoments(mom, a, res);
    return Moments(res);
  }
#undef FNAME
#define FNAME "Moments::YShear"
  Moments Moments::YShear(double a) const
  {
    double res[15];
    YShearMoments(mom, a, res);
    return Moments(res);
  }
#undef FNAME
#define FNAME "Moments::Scale"
  Moments Moments::Scale(double a) const
  {
    return Scale(a, a);
  }

  Moments Moments::Scale(double a, double b) const
  {
    double res[15];
    ScaleMoments(mom, a, b, res);
    return Moments(res);
  }
#undef FNAME
#define FNAME "Moments::Rotate"
  Moments Moments::Rotate(double fi) const
  {
    double res[15];
    RotateMoments(mom, fi, res);
    return Moments(res);
  }
#undef FNAME
#define FNAME "Moments::Translate"
  Moments Moments::Translate(double dx, double dy) const
  {
    double res[15];
    TranslateMoments(mom, dx, dy, res);
    return Moments(res);
  }
#undef FNAME
#define FNAME "Moments::Diff"
  double Moments::Diff(const Moments& m) const
  {
    double d = 0.0;
    for (int i = 0; i < 15; i++)
      {
        double dd = mom[i] - m.mom[i];
        d += dd * dd;
      }

    return d;
  }
#undef FNAME
#define FNAME "Moments:AffineAlgebraicInvariants"
  Vector Moments::AffineAlgebraicInvariants() const
  {
    Vector res(4);
    double flu[4];
    AffinAlgebraicInvar(mom, flu);
    for (int i = 0; i < 4; i++)
      {
        res[i] = flu[i];
      }

    return res;
  }
#undef FNAME
#define FNAME "Moments:HuInvariants"
  Vector Moments::HuInvariants() const
  {
    Vector res(7);
    double hu[7];
    ice::HuInvariants(mom, hu);

    for (int i = 0; i < 7; i++)
      {
        res[i] = hu[i];
      }

    return res;
  }
#undef FNAME
#define FNAME "Moments:AffineHuInvariants"
  Vector Moments::AffineHuInvariants(bool trans) const
  {
    Vector res(7);
    double hu[7];
    AffinHuInvar(mom, hu, trans);
    for (int i = 0; i < 7; i++)
      {
        res[i] = hu[i];
      }

    return res;
  }
#undef FNAME
#define FNAME "Moments::NormalizeTranslation"
  Moments Moments::NormalizeTranslation() const
  {
    double x, y;
    return NormalizeTranslation(x, y);
  }

  Moments Moments::NormalizeTranslation(double& x, double& y) const
  {
    try
      {
        double momres[15];
        NormalizeMomentsTranslation(mom, momres, x, y);
        return Moments(momres);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Moments::NormalizeXShearing"
  Moments Moments::NormalizeXShearing() const
  {
    double beta;
    return NormalizeXShearing(beta);
  }

  Moments Moments::NormalizeXShearing(double& beta) const
  {
    double momres[15];
    NormalizeMomentsXShearing(mom, momres, beta);
    return Moments(momres);
  }
#undef FNAME
#define FNAME "Moments::NormalizeYShearing"
  Moments Moments::NormalizeYShearing() const
  {
    double beta;
    return NormalizeYShearing(beta);
  }

  Moments Moments::NormalizeYShearing(double& beta) const
  {
    double momres[15];
    NormalizeMomentsYShearing(mom, momres, beta);
    return Moments(momres);
  }
#undef FNAME
#define FNAME "Moments::NormalizeScaling"
  Moments Moments::NormalizeScaling(scalemode mode) const
  {
    //    std::cout << "***************x"<<std::endl;
    double alpha, beta;

    switch (mode)
      {
      case isotropic:
        return NormalizeScaling(alpha);
      case anisotropic:
        return NormalizeScaling(alpha, beta);
      default:
        throw IceException(FNAME, M_WRONG_INDEX);
      }
  }

  Moments Moments::NormalizeScaling(double& alpha) const
  {
    try
      {
        double momres[15];
        NormalizeMomentsArea(mom, momres, alpha);
        return Moments(momres);
      }
    RETHROW;
  }

  Moments Moments::NormalizeScaling(double& alpha, double& beta) const
  {
    try
      {
        double momres[15];
        NormalizeMomentsScaling(mom, momres, alpha, beta);
        return Moments(momres);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Moments::NormalizeRotation"
  Moments Moments::NormalizeRotation() const
  {
    double phi;
    return NormalizeRotation(phi);
  }

  Moments Moments::NormalizeRotation(double& phi) const
  {
    try
      {
        double momres[15];
        double c, s;
        NormalizeMomentsRotation(mom, momres, c, s);
        if (c == 1.0 && s == 1.0) // rotation is undetermined
          phi = 0;
        else
          phi = atan2(s, c);
        return Moments(momres);
      }
    RETHROW;
  }

  Moments Moments::NormalizeRotation(double& c, double& s) const
  {
    try
      {
        double momres[15];
        NormalizeMomentsRotation(mom, momres, c, s);
        return Moments(momres);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Moments::Normalize"
  Moments Moments::Normalize(Trafo& tr, nmode mode) const
  {
    double trd[3][3];
    double resd[15];

    switch (mode)
      {
      case standard:
        AffinNormMoments(mom, resd, trd);
        break;
      case polynom:
        PolyNormMoments(mom, resd, trd);
        break;
      case iteration:
        AffinIterateMoments(mom, resd, trd);
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }

    Matrix trm(3, 3);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        {
          trm[i][j] = trd[i][j];
        }

    tr = Trafo(trm);
    return Moments(resd);
  }

  Moments Moments::Normalize(nmode mode) const
  {
    Trafo tr;
    return Normalize(tr, mode);
  }
#undef FNAME
#define FNAME "Moments::AffineTransform"
  Moments Moments::AffineTransform(const Trafo& tr) const
  {
    if ((tr.DimSource() != 2) || (tr.DimTarget() != 2))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    Matrix trm(tr.getMatrix());

    if (fabs(trm[2][0]) + fabs(trm[2][1]) > EPSILON)
      throw IceException(FNAME, M_TRAFO_NOTAFFINE);

    double trd[3][3];

    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        {
          trd[i][j] = trm[i][j];
        }

    double resd[15];
    AffinTransMoments(mom, trd, resd);
    return Moments(resd);
  }
#undef FNAME
}
