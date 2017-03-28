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

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "Vector.h"
#include "Matrix.h"
#include "base.h"

#include "fitfn.h"

namespace ice
{
#define FNAME "Funtion2dWithFitting::Fit"
  int Function2dWithFitting::Fit(const Matrix& m)
  {
    int samples = m.rows();
    int i;

    if (m.cols() < 3)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    FitInit();

    for (i = 0; i < samples; i++)
      FitVal(m[i][0], m[i][1], m[i][2]);

    FitFinish();
    return OK;
  }

  int Function2dWithFitting::Fit(const Image& img)
  {
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return WRONG_PARAM;
      }

    FitInit();
    for (int y = 0; y < img.ysize; ++y)
      for (int x = 0; x < img.xsize; ++x)
        {
          FitVal(x, y, GetVal(img, x, y));
        }
    FitFinish();
    return OK;
  }

  int Function2dWithFitting::Fit(ImageD img)
  {
    if (!IsImgD(img))
      {
        Message(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return WRONG_PARAM;
      }

    FitInit();
    for (int y = 0; y < img.ysize; ++y)
      for (int x = 0; x < img.xsize; ++x)
        {
          FitVal(x, y, GetValD(img, x, y));
        }
    FitFinish();
    return OK;
  }
#undef FNAME
//========================================================================
#define FNAME "Constant2d::Constant2d"
  Constant2d::Constant2d(const Vector& v): Function2dWithFittingG(1)
  {
    if (v.Size() < 1)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    co[0] = v[0];
  }
#undef FNAME

  void Constant2d::FitVal(double x, double y, double g, double w)
  {
    Vector p(1);
    p[0] = 1;
    Function2dWithFittingG::FitVal(p, g, w);
  }
#define FNAME "Constant2d::operator()"
  double Constant2d::operator()(double x, double y) const
  {
    return co[0];
  }
#undef FNAME
  int Constant2d::getCoefficient(double& pa) const
  {
    pa = co[0];
    return OK;
  }

//============================================================================
#ifndef OLD
#define FNAME "Polynom2d1o::Polynom2d1o"
  Polynom2d1o::Polynom2d1o(const Vector& v): Function2dWithFittingG(3)
  {
    if (v.Size() < 3)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    co[0] = v[0];
    co[1] = v[1];
    co[2] = v[2];
  }
#undef FNAME

  void Polynom2d1o::FitVal(double x, double y, double g, double w)
  {
    Vector p(3);
    p[0] = 1;
    p[1] = x;
    p[2] = y;
    Function2dWithFittingG::FitVal(p, g, w);
  }
#define FNAME "Polynom2d1o::operator()"
  double Polynom2d1o::operator()(double x, double y) const
  {
    return co[0] + co[1] * x + co[2] * y;
  }
#undef FNAME
#define FNAME "Polynom2d1o::Coeff"
  int Polynom2d1o::getCoefficient(double& pa, double& pb, double& pc) const
  {
    pa = co[0];
    pb = co[1];
    pc = co[2];
    return OK;
  }
#undef FNAME
//============================================
#define FNAME "Polynom2d2o::Polynom2d2o"
  Polynom2d2o::Polynom2d2o(const Vector& v): Function2dWithFittingG(6)
  {
    int i;

    if (v.Size() < 6)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    for (i = 0; i < 6; i++) co[i] = v[i];
  }
#undef FNAME

  void Polynom2d2o::FitVal(double x, double y, double g, double w)
  {
    Vector p(6);
    p.at(0) = 1;
    p.at(1) = x;
    p.at(2) = y;
    p.at(3) = x * x;
    p.at(4) = x * y;
    p.at(5) = y * y;
    Function2dWithFittingG::FitVal(p, g, w);
  }

#define FNAME "Polynom2d2o::operator()"
  double Polynom2d2o::operator()(double x, double y) const
  {
    double x2, y2;
    x2 = x * x;
    y2 = y * y;
    return co.at(3) * x2 + co.at(4) * x * y + co.at(5) * y2 + co.at(1) * x + co.at(2) * y + co.at(0);
  }
#undef FNAME
#define FNAME "Polynom2d2o::Coeff"
  int Polynom2d2o::getCoefficient(double& pa, double& pb, double& pc,
                                  double& pd, double& pe, double& pf) const
  {
    pa = co[0];
    pb = co[1];
    pc = co[2];
    pd = co[3];
    pe = co[4];
    pf = co[5];
    return OK;
  }
#undef FNAME
#else
  void Polynom2d2o::fitinit()
  {
    s = 0;
    sx = 0;
    sy = 0;
    sx2 = 0;
    sxy = 0;
    sy2 = 0;
    sx3 = 0;
    sx2y = 0;
    sxy2 = 0;
    sy3 = 0;
    sx4 = 0;
    sx3y = 0;
    sx2y2 = 0;
    sxy3 = 0;
    sy4 = 0;
    sg = 0;
    sxg = 0;
    syg = 0;
    sx2g = 0;
    sxyg = 0;
    sy2g = 0;
    sg2 = 0;
  }

  void Polynom2d2o::init()
  {
    c_valid = true;
    fitinit();
  }

  void Polynom2d2o::init(double pa, double pb, double pc, double pd, double pe, double pf)
  {
    init();
    a = pa;
    b = pb;
    c = pc;
    d = pd;
    e = pe;
    f = pf;
  }

#define FNAME "Polynom2d2o::Polynom2d2o"
  Polynom2d2o::Polynom2d2o()
  {
    init();
  }
  Polynom2d2o::Polynom2d2o(double pa, double pb, double pc, double pd, double pe, double pf)
  {
    init(pa, pb, pc, pd, pe, pf);
  }

  Polynom2d2o::Polynom2d2o(Vector v)
  {
    if (v.Size() < 6)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    init(v[0], v[1], v[2], v[3], v[4], v[5]);
  }

  Polynom2d2o::Polynom2d2o(const Polynom2d2o& f)
  {
    init(f.a, f.b, f.c, f.d, f.e, f.f);
  }
#undef FNAME

  void Polynom2d2o::FitInit()
  {
    c_valid = false;
    fitinit();
  }

  void Polynom2d2o::FitVal(double x, double y, double g, double w)
  {
    double x2, x3;
    double y2, y3;
    x2 = x * x;
    x3 = x2 * x;
    y2 = y * y;
    y3 = y2 * y;
    s += w;
    sx += x * w;
    sy += y * w;
    sx2 += x2 * w;
    sxy += x * y * w;
    sy2 += y2 * w;
    sx3 += x3 * w;
    sx2y += x2 * y * w;
    sxy2 += x * y2 * w;
    sy3 += y3 * w;
    sx4 += x2 * x2 * w;
    sx3y += x3 * y * w;
    sx2y2 += x2 * y2 * w;
    sxy3 += x * y3 * w;
    sy4 += y2 * y2 * w;
    sg += g * w;
    sxg += x * g * w;
    syg += y * g * w;
    sx2g += x2 * g * w;
    sxyg += x * y * g * w;
    sy2g += y2 * g * w;
    sg2 += g * g * w;
    c_valid = false;
  }
#define FNAME "Polynom2d2o::operator()"
  double Polynom2d2o::operator()(double x, double y) const
  {
    double x2, y2;

    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return 0;
        }
      }

    x2 = x * x;
    y2 = y * y;
    return a + b * x + c * y + d * x2 + e * x * y + f * y2;
  }
#undef FNAME
#define FNAME "Polynom2d2o::coeff"
  Vector Polynom2d2o::Coeff() const
  {
    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return Vector(0);
        }
      }

    return Vector(a, b, c, d, e, f);
  }

  int Polynom2d2o::Coeff(double& pa, double& pb, double& pc,
                         double& pd, double& pe, double& pf) const
  {
    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return ERROR;
        }
      }

    pa = a;
    pb = b;
    pc = c;
    pd = d;
    pe = e;
    pf = f;
    return OK;
  }
#undef FNAME
#define FNAME "Polynom2d2o::Variance"
  double Polynom2d2o::Variance() const
  {
    double var;

    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return ERROR;
        }
      }

    var = a * a * s + 2 * a * b * sx + 2 * a * c * sy;
    var += (2 * a * d + b * b) * sx2 + (2 * a * e + 2 * b * c) * sxy + (2 * a * f + c * c) * sy2;
    var += 2 * b * d * sx3 + 2 * (b * e + c * d) * sx2y + 2 * (b * f + c * e) * sxy2 + 2 * c * f * sy3;
    var += d * d * sx4 + 2 * d * e * sx3y + (2 * d * f + e * e) * sx2y2 + 2 * e * f * sxy3 + f * f * sy4;
    var -= 2 * (a * sg + b * sxg + c * syg + d * sx2g + e * sxyg + f * sy2g);
    var += sg2;
    return var / s;
  }
#undef FNAME
#define FNAME "Polynom2d2o::calc_c"
  int Polynom2d2o::calc_c() const
  {
    Matrix A;
    Vector x;
    Vector y;

    if (s < 6)
      {
        Message(FNAME, M_TOO_LESS_VALUES, WRONG_PARAM);
        return WRONG_PARAM;
      }

    A = Vector(sx4, sx3y, sx2y2, sx3, sx2y, sx2) &&
        Vector(sx3y, sx2y2, sxy3, sx2y, sxy2, sxy) &&
        Vector(sx2y2, sxy3, sy4, sxy2, sy3, sy2) &&
        Vector(sx3, sx2y, sxy2, sx2, sxy, sx) &&
        Vector(sx2y, sxy2, sy3, sxy, sy2, sy) &&
        Vector(sx2, sxy, sy2, sx, sy, s);
    y = Vector(sx2g, sxyg, sy2g, sxg, syg, sg);

    x = SolveLinEqu(A, y);

    d = x[0];
    e = x[1];
    f = x[2];
    b = x[3];
    c = x[4];
    a = x[5];
    c_valid = true;
    return OK;
  }
#undef FNAME
//=============================================================================
  void Polynom2d1o::fitinit()
  {
    s = 0;
    sx = 0;
    sy = 0;
    sx2 = 0;
    sxy = 0;
    sy2 = 0;
    sg = 0;
    sxg = 0;
    syg = 0;
    sg2 = 0;
  }

  void Polynom2d1o::init()
  {
    c_valid = true;
    fitinit();
  }

  void Polynom2d1o::init(double pa, double pb, double pc)
  {
    init();
    a = pa;
    b = pb;
    c = pc;
  }

#define FNAME "Polynom2d1o::Polynom2d1o"
  Polynom2d1o::Polynom2d1o()
  {
    init();
  }
  Polynom2d1o::Polynom2d1o(double pa, double pb, double pc)
  {
    init(pa, pb, pc);
  }

  Polynom2d1o::Polynom2d1o(Vector v)
  {
    if (v.Size() < 3)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    init(v[0], v[1], v[2]);
  }

  Polynom2d1o::Polynom2d1o(const Polynom2d1o& f)
  {
    init(f.a, f.b, f.c);
  }
#undef FNAME

  void Polynom2d1o::FitInit()
  {
    c_valid = false;
    fitinit();
  }

  void Polynom2d1o::FitVal(double x, double y, double g, double w)
  {
    double x2, x3;
    double y2, y3;
    x2 = x * x;
    x3 = x2 * x;
    y2 = y * y;
    y3 = y2 * y;
    s += 1.0 * w;
    sx += x * w;
    sy += y * w;
    sx2 += x2 * w;
    sxy += x * y * w;
    sy2 += y2 * w;
    sg += g * w;
    sxg += x * g * w;
    syg += y * g * w;
    sg2 += g * g * w;
    c_valid = false;
  }
#define FNAME "Polynom2d1o::operator()"
  double Polynom2d1o::operator()(double x, double y) const
  {
    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return 0;
        }
      }

    return a + b * x + c * y;
  }
#undef FNAME
#define FNAME "Polynom2d1o::coeff"
  int Polynom2d1o::Coeff(double& pa, double& pb, double& pc) const
  {
    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return ERROR;
        }
      }

    pa = a;
    pb = b;
    pc = c;
    return OK;
  }

  Vector Polynom2d1o::Coeff() const
  {
    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return Vector(0);
        }
      }

    return Vector(a, b, c);
  }
#undef FNAME

#define FNAME "Polynom2d1o::Variance"
  double Polynom2d1o::Variance() const
  {
    double var;

    if (!c_valid)
      {
        IfFailed(calc_c())
        {
          Message(FNAME, M_0, ERROR);
          return ERROR;
        }
      }

    var = a * a * s + 2 * a * b * sx + 2 * a * c * sy + b * b * sx2 + 2 * b * c * sxy + c * c * sy2;
    var -= 2 * (a * sg + b * sxg + c * syg);
    var += sg2;
    return var / s;
  }
#undef FNAME

#define FNAME "Polynom2d1o::calc_c"
  int Polynom2d1o::calc_c() const
  {
    Matrix A;
    Vector x;
    Vector y;

    if (s < 6)
      {
        Message(FNAME, M_TOO_LESS_VALUES, WRONG_PARAM);
        return WRONG_PARAM;
      }

    A = Vector(sx2, sxy, sx) &&
        Vector(sxy, sy2, sy) &&
        Vector(sx, sy, s);

    y = Vector(sxg, syg, sg);

    x = SolveLinEqu(A, y);

    b = x[0];
    c = x[1];
    a = x[2];
    c_valid = true;
    return OK;
  }
#undef FNAME

#endif
}
