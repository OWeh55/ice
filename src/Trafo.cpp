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

#include <cstdlib>

#include "defs.h"
#include "numbase.h"
#include "macro.h"
#include "IceException.h"

#include "Trafo.h"
#include "Matrix.h"
#include "MatrixAlgebra.h"

//--------------------------------------------
namespace ice
{
#define FNAME "Trafo::Trafo"
  Trafo::Trafo(): m(3, 3, 1), dimSource(2), dimTarget(2)
  {
  }

  Trafo::Trafo(const Trafo& tr): m(tr.m), dimSource(tr.dimSource), dimTarget(tr.dimTarget)
  {
  }

  Trafo::Trafo(const Matrix& mp): m(mp.rows(), mp.cols()), dimSource(mp.cols() - 1), dimTarget(mp.rows() - 1)
  {
    for (int i = 0; i < mp.rows(); i++)
      for (int j = 0; j < mp.cols(); j++)
        {
          m[i][j] = mp[i][j];
        }
    normalize();
  }

  Trafo::Trafo(const matrix<double>& mp): m(mp), dimSource(mp.cols() - 1), dimTarget(mp.rows() - 1)
  {
    normalize();
  }

  Trafo::Trafo(int dimSourcep, int dimTargetp)
  {
    if ((dimSourcep <= 0) || (dimTargetp < 0))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    dimSource = dimSourcep;
    dimTarget = dimTargetp;
    init();
  }
#undef FNAME
//-----------------------------------------
  void Trafo::init()
  {
    m.init(dimTarget + 1, dimSource + 1, 1); // init as identity

    for (int i = 0; i < dimTarget; i++)
      {
        m[i][dimSource] = 0.0;
      }

    for (int j = 0; j < dimSource; j++)
      {
        m[dimTarget][j] = 0.0;
      }

    m[dimTarget][dimSource] = 1.0;
  }

  Trafo& Trafo::operator=(const Trafo& tr)
  {
    dimSource = tr.dimSource;
    dimTarget = tr.dimTarget;
    m = tr.m;
    return *this;
  }

#define FNAME "Trafo::operator="
  Trafo& Trafo::operator=(const Matrix& mat)
  {
    try
      {
        dimSource = mat.cols() - 1;
        dimTarget = mat.rows() - 1;
        m.init(dimTarget + 1, dimSource + 1);

        for (int i = 0; i < mat.rows(); i++)
          for (int j = 0; j < mat.cols(); j++)
            {
              m[i][j] = mat[i][j];
            }

        normalize();
        return *this;
      }
    RETHROW;
  }
#undef FNAME

  int Trafo::DimSource() const
  {
    return dimSource;
  }

  int Trafo::DimTarget() const
  {
    return dimTarget;
  }

  double& Trafo::operator()(int i, int j)
  {
    return m[i][j];
  }

  const double& Trafo::operator()(int i, int j) const
  {
    return m[i][j];
  }

  Trafo::~Trafo()
  {
  }
//-------------------------------------------
#define FNAME "Trafo:normalize"
  void Trafo::normalize()
  {
    if (m[dimTarget][dimSource] == 0.0)
      throw IceException(FNAME, M_WRONG_TRAFO);

    m *= 1 / m[dimTarget][dimSource];
  }
#undef FNAME
#define FNAME "Trafo::shift"
  void Trafo::shift(double x, double y)
  {
    if (dimTarget != 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        matrix<double> h(3, 3, 1);
        h[0][2] = x;
        h[1][2] = y;
        m = h * m;
        normalize();
      }
    RETHROW;
  }

  void Trafo::shift(double x, double y, double z)
  {
    if (dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    try
      {
        matrix<double> h(4, 4, 1);
        h[0][3] = x;
        h[1][3] = y;
        h[2][3] = z;

        m = h * m;

        normalize();
      }
    RETHROW;
  }

  void Trafo::shift(Vector3d s)
  {
    shift(s.x, s.y, s.z);
  }

  void Trafo::shift(const Vector& s)
  {
    if (dimTarget != s.Size())
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        // generate translation matrix
        matrix<double> h(dimTarget + 1, dimTarget + 1, 1);

        for (int i = 0; i < dimTarget; i++)
          {
            h[i][dimTarget] = s[i];
          }

        m = h * m;
        normalize();
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Trafo::flip"
  void Trafo::flip(int achse)
  {
    if (dimTarget - 1 < achse)
      throw IceException(FNAME, M_WRONG_INDEX);

    for (int i = 0; i < dimSource; i++)
      {
        m[achse][i] = -m[achse][i];
      }
  }
#undef FNAME
#define FNAME "Trafo::rotate"
  void Trafo::rotate(double x0, double y0, double phi)
  {
    // 2D-rotation um einen Punkt (x0,y0)
    if (dimTarget != 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    try
      {
        shift(-x0, -y0);

        double si = sin(phi);
        double co = cos(phi);

        matrix<double> h(3, 3, 1);
        h[0][0] = co;
        h[0][1] = -si;
        h[1][0] = si;
        h[1][1] = co;
        m = h * m;

        shift(x0, y0);
      }
    RETHROW;
  }

  void Trafo::rotate(Vector3d p, Vector3d dir, double phi)
  {
    // 3D-Rotation um einen durch Punkt und Richtung gegebenen Strahl
    if (dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    try
      {
        shift(-p);

        double sind = sin(phi);
        double cosd = cos(phi);

        double cosd1 = 1 - cosd;

        double dirx = dir.x / dir.Length();
        double diry = dir.y / dir.Length();
        double dirz = dir.z / dir.Length();

        matrix<double> h(4, 4, 1);
        h[0][0] = dirx * dirx * cosd1 + cosd;
        h[0][1] = dirx * diry * cosd1 + dirz * sind;
        h[0][2] = dirx * dirz * cosd1 - diry * sind;

        h[1][0] = diry * dirx * cosd1 - dirz * sind;
        h[1][1] = diry * diry * cosd1 + cosd;
        h[1][0] = diry * dirz * cosd1 + dirx * sind;

        h[2][0] = dirz * dirx * cosd1 + diry * sind;
        h[2][1] = dirz * diry * cosd1 - dirx * sind;
        h[2][1] = dirz * dirz * cosd1 + cosd;

        m = h * m;
        shift(p);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::rotateX"
  void Trafo::rotateX(double phi)
// Rotation um X-Achse für 3D/3D-Trafo (Euler)
  {
    if (dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        double sind = sin(phi);
        double cosd = cos(phi);

        matrix<double> h(4, 4, 1);
        h[1][1] = cosd;
        h[1][2] = -sind;
        h[2][1] = sind;
        h[2][2] = cosd;
        m = h * m;
        normalize();
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::rotateY"
  void Trafo::rotateY(double phi)
// Rotation um Y-Achse für 3D/3D-Trafo (Euler)
  {
    if (dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        double sind = sin(phi);
        double cosd = cos(phi);

        matrix<double> h(4, 4, 1);

        h[0][0] = cosd;
        h[0][2] = sind;
        h[2][0] = -sind;
        h[2][2] = cosd;

        m = h * m;
        normalize();
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::rotateZ"
  void Trafo::rotateZ(double phi)
// Rotation um Z-Achse für 3D/3D-Trafo (Euler)
  {
    if (dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        double sind = sin(phi);
        double cosd = cos(phi);

        matrix<double> h(4, 4, 1);
        h[0][0] = cosd;
        h[0][1] = -sind;
        h[1][0] = sind;
        h[1][1] = cosd;

        m = h * m;
        normalize();
      }
    RETHROW;
  }
#undef FNAME

  void Trafo::move(double dx, double dy, double dz,
                   double alpha, double beta, double gamma)
  {
    shift(dx, dy, dz);

    rotateZ(alpha);
    rotateY(beta);
    rotateX(gamma);
  }

  void Trafo::move(Vector3d d,
                   double alpha, double beta, double gamma)
  {
    shift(d);

    rotateZ(alpha);
    rotateY(beta);
    rotateX(gamma);
  }

#define FNAME "Trafo::Projective"
  void Trafo::projective()
  // Reduktion der Dimension um 1 durch projektive abbildung in Richtung Z-Achse
  // Projektionszentrum (0,0,0)
  // Projektionsebene (x,y,1)
  {
    if (dimTarget < 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        matrix<double> h(dimTarget, dimTarget + 1);

        for (int i = 0; i < dimTarget; i++)
          for (int j = 0; j < dimTarget + 1; j++)
            {
              if (i == j)
                {
                  h[i][j] = 1.0;
                }
              else
                {
                  h[i][j] = 0.0;
                }
            }

        m = h * m;
        dimTarget--;
        normalize();
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::shearY"
  void Trafo::shearY(double dyx)
  {
    // Scherung in Y-Richtung (in Ebene!)
    if (dimTarget != 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        matrix<double> h(3, 3, 1);
        h[1][0] = dyx;
        m = h * m;
        normalize();
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::ShearX"
  void Trafo::shearX(double dxy)
  {
    // Scherung in X-Richtung (in Ebene!)
    if (dimTarget != 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        matrix<double> h(3, 3, 1);

        h[0][1] = dxy;
        m = h * m;
        normalize();
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::scale"
  void Trafo::scale(double x0, double y0, double f)
  {
    scale(x0, y0, f, f);
  }

  void Trafo::scale(double x0, double y0, double fx, double fy)
  {
    if (dimTarget != 2)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    shift(-x0, -y0);

    for (int i = 0; i <= dimSource; i++)
      {
        m[0][i] = fx * m[0][i];
        m[1][i] = fy * m[1][i];
      }

    shift(x0, y0);
  }

  void Trafo::scale(Vector3d center, double fx, double fy, double fz)
  {
    if (dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    shift(-center);

    for (int i = 0; i <= dimSource; i++)
      {
        m[0][i] = fx * m[0][i];
        m[1][i] = fy * m[1][i];
        m[2][i] = fz * m[2][i];
      }

    shift(center);
  }

  void Trafo::scale(Vector3d center, double f)
  {
    scale(center, f, f, f);
  }

  void Trafo::scale(const Vector& center, const Vector& f)
  {
    if ((dimTarget != f.Size()) || (dimTarget != center.Size()))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    shift(-center);

    matrix<double> h(dimTarget + 1, dimTarget + 1, 1);

    for (int i = 0; i < dimTarget; i++)
      {
        h[i][i] = f[i];
      }

    h[dimTarget][dimTarget] = 1.0;

    m = h * m;

    shift(center);
  }

  void Trafo::scale(const Vector& center, double f)
  {
    Vector h(center.Size());
    h.set(f);
    scale(center, h);
  }
#undef FNAME
#define FNAME "Trafo::append"
  void Trafo::append(const Trafo& t2)
  {
    if (t2.dimSource != dimTarget)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        Trafo res(dimSource, t2.dimTarget);
        res.m = t2.m * m;
        res.normalize();
        *this = res;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Trafo::prepend"
  void Trafo::prepend(const Trafo& t2)
  {
    if (t2.dimSource != dimTarget)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        Trafo res(dimSource, t2.dimTarget);
        res.m = m * t2.m;
        res.normalize();
        *this = res;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Trafo::invert"
  void Trafo::invert()
  {
    if (dimSource != dimTarget)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        m = ice::Inverse(m);
        normalize();
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Trafo::inverse"
  Trafo Trafo::inverse() const
  {
    try
      {
        Trafo res(*this);
        res.invert();
        return res;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "Trafo::operator*"
  Trafo operator *(const Trafo& t1, const Trafo& t2)
  {
    if (t1.dimSource != t2.dimTarget)
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        Trafo res(t2.dimSource, t1.dimTarget);
        res.m = t1.m * t2.m;
        res.normalize();
        return res;
      }
    RETHROW;
  }

  Vector operator *(const Trafo& t, const Vector& v)
  {
    Vector res(t.dimTarget);

    if (t.dimSource != v.Size())
      throw IceException(FNAME, M_WRONG_DIMENSION);

    // tmp = t.m * tmp;

    double norm = 0;

    for (int i = 0; i < t.dimSource; i++)
      {
        norm += t.m[t.dimTarget][i] * v[i];
      }

    norm += t.m[t.dimTarget][t.dimSource];

    for (int i = 0; i < t.dimTarget; i++)
      {
        double sum = 0.0;

        for (int k = 0; k < t.dimSource; k++)
          {
            sum += t.m[i][k] * v[k];
          }

        sum += t.m[i][t.dimSource];

        res[i] = sum / norm;
      }

    return res;
  }

  Vector3d operator *(const Trafo& t, const Vector3d& v)
  {
    if (t.dimSource != 3 || t.dimTarget != 3)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    Vector3d res;
    double norm =  t.m[3][0] * v[0] + t.m[3][1] * v[1] +
                   t.m[3][2] * v[2] + t.m[3][3];

    res.x = (t.m[0][0] * v[0] + t.m[0][1] * v[1] + t.m[0][2] * v[2] + t.m[0][3]) / norm;
    res.y = (t.m[1][0] * v[0] + t.m[1][1] * v[1] + t.m[1][2] * v[2] + t.m[1][3]) / norm;
    res.z = (t.m[2][0] * v[0] + t.m[2][1] * v[1] + t.m[2][2] * v[2] + t.m[2][3]) / norm;
    return res;
  }
#undef FNAME
//-----------------------------
// Several ways to use a Trafo
//-------------------------------
// runtime optimization : explicite formula instead of matrix-Vector-product
#define FNAME "transform"
  void transform(const Trafo& tr, double& x, double& y)
  {
    double hf, xt, yt;

    if ((tr.dimSource != 2) || (tr.dimTarget != 2))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    hf = x * tr.m[2][0] + y * tr.m[2][1] + tr.m[2][2];
    xt = (x * tr.m[0][0] + y * tr.m[0][1] + tr.m[0][2]) / hf;
    yt = (x * tr.m[1][0] + y * tr.m[1][1] + tr.m[1][2]) / hf;

    x = xt;
    y = yt;
  }

  void transform(const Trafo& tr, Point& p)
  {
    transform(tr, p.x, p.y, p.x, p.y);
  }

  void transform(const Trafo& tr, std::vector<Point>& pl)
  {
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        transform(tr, pl[i]);
      }
  }

  void transform(const Trafo& tr,
                 const std::vector<Point>& pl1, std::vector<Point>& pl2)
  {
    pl2.resize(pl1.size());
    for (unsigned int i = 0; i < pl1.size(); i++)
      {
        transform(tr, pl1[i], pl2[i]);
      }
  }

  void transform(const Trafo& tr, Point p1, Point& p2)
  {
    transform(tr, p1.x, p1.y, p2.x, p2.y);
  }

  void transform(const Trafo& tr, double& x, double& y, double& z)
  {
    if ((tr.dimSource != 3) || (tr.dimTarget != 3))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    double hf = x * tr.m[3][0] + y * tr.m[3][1] + z * tr.m[3][2] + tr.m[3][3];
    double xt = (x * tr.m[0][0] + y * tr.m[0][1] + z * tr.m[0][2] + tr.m[0][3]) / hf;
    double yt = (x * tr.m[1][0] + y * tr.m[1][1] + z * tr.m[1][2] + tr.m[1][3]) / hf;
    double zt = (x * tr.m[2][0] + y * tr.m[2][1] + z * tr.m[2][2] + tr.m[2][3]) / hf;
    x = xt;
    y = yt;
    z = zt;
  }

  void transform(const Trafo& tr,
                 double x, double y,
                 double& xt, double& yt)
  {
    xt = x;
    yt = y;
    transform(tr, xt, yt);
  }

  void transformAndRound(const Trafo& tr,
                         int& xt, int& yt)
  {
    double xtf, ytf;
    try
      {
        transform(tr, xt, yt, xtf, ytf);
        xt = RoundInt(xtf);
        yt = RoundInt(ytf);
      }
    RETHROW;
  }

  void transformAndRound(const Trafo& tr, double x, double y, int& xt, int& yt)
  {
    try
      {
        double xs, ys;
        transform(tr, x, y, xs, ys);
        xt = RoundInt(xs);
        yt = RoundInt(ys);
      }
    RETHROW;
  }

  void transform(const Trafo& tr,
                 double x, double y, double z,
                 double& xt, double& yt, double& zt)
  {
    xt = x;
    yt = y;
    zt = z;
    transform(tr, xt, yt, zt);
  }

  void transform(const Trafo& tr,
                 double x, double y, double z,
                 double& xt, double& yt)
  {
    if ((tr.dimSource != 3) || (tr.dimTarget != 2))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    double hf;

    hf = x * tr.m[2][0] + y * tr.m[2][1] + z * tr.m[2][2] + tr.m[2][3];
    xt = (x * tr.m[0][0] + y * tr.m[0][1] + z * tr.m[0][2] + tr.m[0][3]) / hf;
    yt = (x * tr.m[1][0] + y * tr.m[1][1] + z * tr.m[1][2] + tr.m[1][3]) / hf;
  }

#undef FNAME
}
