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
  Trafo::Trafo()
  {
    dimSource = 2;
    dimTarget = 2;
    m.init(3, 3);
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
  }

  Trafo::Trafo(const Trafo& tr)
  {
    dimSource = tr.dimSource;
    dimTarget = tr.dimTarget;
    m = tr.m;
  }

  Trafo::Trafo(const Matrix& mp)
  {
    dimSource = mp.cols() - 1;
    dimTarget = mp.rows() - 1;

    m.init(dimTarget + 1, dimSource + 1);

    for (int i = 0; i < mp.rows(); i++)
      for (int j = 0; j < mp.cols(); j++)
        {
          m[i][j] = mp[i][j];
        }
    normalize();
  }

  Trafo::Trafo(const matrix<double>& mp)
  {
    dimSource = mp.cols() - 1;
    dimTarget = mp.rows() - 1;
    m = mp;
    normalize();
  }

  Trafo::Trafo(int dimSourcep, int dimTargetp)
  {
    if ((dimSourcep <= 0) || (dimTargetp < 0))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

    dimSource = dimSourcep;
    dimTarget = dimTargetp;
    Init();
  }
#undef FNAME
//-----------------------------------------
  void Trafo::Init()
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
    dimSource = mat.cols() - 1;
    dimTarget = mat.rows() - 1;
    m.init(dimTarget + 1, dimSource + 1);

    for (int i = 0; i < mat.rows(); i++)
      for (int j = 0; j < mat.cols(); j++)
        {
          m[i][j] = mat[i][j];
        }

    IF_FAILED(normalize())
    {
      throw IceException(FNAME, M_0, ERROR);
    }
    return *this;
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

  Matrix Trafo::Tmatrix() const
  {
    Matrix res(dimTarget + 1, dimSource + 1);

    for (int i = 0; i <= dimTarget; i++)
      for (int k = 0; k <= dimSource; k++)
        {
          res[i][k] = m[i][k];
        }

    return res;
  }

  Trafo::~Trafo()
  {
  }
//-------------------------------------------
#define FNAME "Trafo:normalize"
  int Trafo::normalize()
  {
    //    double sum=0.0;
    //    for (int i=0;i<=dimSource;i++)
    //      sum+=Sqr(m[dimTarget][i]);
    //    if (sum==0)
    if (m[dimTarget][dimSource] == 0.0)
      {
        throw IceException(FNAME, M_WRONG_TRAFO, WRONG_PARAM);
        return ERROR;
      }

    //    m *= 1/sqrt(sum);
    m *= 1 / m[dimTarget][dimSource];
    return OK;
  }
#undef FNAME
#define FNAME "Trafo::Shift"
  int Trafo::Shift(double x, double y)
  {
    if (dimTarget != 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    matrix<double> h(3, 3, 1);
    h[0][2] = x;
    h[1][2] = y;
    m = h * m;
    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }

  int Trafo::Shift(double x, double y, double z)
  {
    if (dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    matrix<double> h(4, 4, 1);
    h[0][3] = x;
    h[1][3] = y;
    h[2][3] = z;

    m = h * m;

    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }

  int Trafo::Shift(Vector3d s)
  {
    int ret;
    RETURN_ERROR_IF_FAILED(ret = Shift(s.x, s.y, s.z));
    return ret;
  }

  int Trafo::Shift(const Vector& s)
  {
    int i;

    if (dimTarget != s.Size())
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // generate translation matrix
    matrix<double> h(dimTarget + 1, dimTarget + 1, 1);

    for (i = 0; i < dimTarget; i++)
      {
        h[i][dimTarget] = s[i];
      }

    m = h * m;

    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME
#define FNAME "Trafo::Flip"
  int Trafo::Flip(int achse)
  {
    int i;

    if (dimTarget - 1 < achse)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (i = 0; i < dimSource; i++)
      {
        m[achse][i] = -m[achse][i];
      }

    return OK;
  }
#undef FNAME
#define FNAME "Trafo::Rotate"
  int Trafo::Rotate(double x0, double y0, double phi)
  {
    // 2D-rotation um einen Punkt (x0,y0)
    if (dimTarget != 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Shift(-x0, -y0);

    double si = sin(phi);
    double co = cos(phi);

    matrix<double> h(3, 3, 1);
    h[0][0] = co;
    h[0][1] = -si;
    h[1][0] = si;
    h[1][1] = co;
    m = h * m;
    Shift(x0, y0);
    return OK;
  }

  int Trafo::Rotate(Vector3d p, Vector3d dir, double phi)
  {
    // 3D-Rotation um einen durch Punkt und Richtung gegebenen Strahl
    if (dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Shift(-p);

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
    Shift(p);
    return OK;
  }
#undef FNAME

#define FNAME "Trafo::RotateX"
  int Trafo::RotateX(double phi)
// Rotation um X-Achse für 3D/3D-Trafo (Euler)
  {
    if (dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    double sind = sin(phi);
    double cosd = cos(phi);

    matrix<double> h(4, 4, 1);
    h[1][1] = cosd;
    h[1][2] = -sind;
    h[2][1] = sind;
    h[2][2] = cosd;
    m = h * m;
    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME

#define FNAME "Trafo::RotateY"
  int Trafo::RotateY(double phi)
// Rotation um Y-Achse für 3D/3D-Trafo (Euler)
  {
    if (dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    double sind = sin(phi);
    double cosd = cos(phi);

    matrix<double> h(4, 4, 1);

    h[0][0] = cosd;
    h[0][2] = sind;
    h[2][0] = -sind;
    h[2][2] = cosd;

    m = h * m;
    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME

#define FNAME "Trafo::RotateZ"
  int Trafo::RotateZ(double phi)
// Rotation um Z-Achse für 3D/3D-Trafo (Euler)
  {
    if (dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    double sind = sin(phi);
    double cosd = cos(phi);

    matrix<double> h(4, 4, 1);
    h[0][0] = cosd;
    h[0][1] = -sind;
    h[1][0] = sind;
    h[1][1] = cosd;

    m = h * m;
    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME

  int Trafo::Move(double dx, double dy, double dz,
                  double alpha, double beta, double gamma)
  {
    Shift(dx, dy, dz);

    RotateZ(alpha);
    RotateY(beta);
    RotateX(gamma);
    return OK;
  }

  int Trafo::Move(Vector3d d,
                  double alpha, double beta, double gamma)
  {
    Shift(d);

    RotateZ(alpha);
    RotateY(beta);
    RotateX(gamma);
    return OK;
  }

#define FNAME "Trafo::Projective"
  int Trafo::Projective()
  // Reduktion der Dimension um 1 durch projektive abbildung in Richtung Z-Achse
  // Projektionszentrum (0,0,0)
  // Projektionsebene (x,y,1)
  {
    if (dimTarget < 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME

#define FNAME "Trafo::ShearY"
  int Trafo::ShearY(double dyx)
  {
    // Scherung in Y-Richtung (in Ebene!)
    if (dimTarget != 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    matrix<double> h(3, 3, 1);
    h[1][0] = dyx;
    m = h * m;
    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME

#define FNAME "Trafo::ShearX"
  int Trafo::ShearX(double dxy)
  {
    // Scherung in X-Richtung (in Ebene!)
    if (dimTarget != 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    matrix<double> h(3, 3, 1);

    h[0][1] = dxy;
    m = h * m;

    RETURN_ERROR_IF_FAILED(normalize());
    return OK;
  }
#undef FNAME
#define FNAME "Trafo::Scale"
  int Trafo::Scale(double x0, double y0, double f)
  {
    RETURN_ERROR_IF_FAILED(Scale(x0, y0, f, f))
    return OK;
  }

  int Trafo::Scale(double x0, double y0, double fx, double fy)
  {
    int i;

    if (dimTarget != 2)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Shift(-x0, -y0);

    for (i = 0; i <= dimSource; i++)
      {
        m[0][i] = fx * m[0][i];
        m[1][i] = fy * m[1][i];
      }

    Shift(x0, y0);
    return OK;
  }

  int Trafo::Scale(Vector3d center, double fx, double fy, double fz)
  {
    int i;

    if (dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Shift(-center);

    for (i = 0; i <= dimSource; i++)
      {
        m[0][i] = fx * m[0][i];
        m[1][i] = fy * m[1][i];
        m[2][i] = fz * m[2][i];
      }

    Shift(center);
    return OK;
  }

  int Trafo::Scale(Vector3d center, double f)
  {
    RETURN_ERROR_IF_FAILED(Scale(center, f, f, f))
    return OK;
  }

  int Trafo::Scale(const Vector& center, const Vector& f)
  {
    int i;

    if ((dimTarget != f.Size()) || (dimTarget != center.Size()))
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Shift(-center);

    matrix<double> h(dimTarget + 1, dimTarget + 1, 1);

    for (i = 0; i < dimTarget; i++)
      {
        h[i][i] = f[i];
      }

    h[dimTarget][dimTarget] = 1.0;

    m = h * m;

    Shift(center);
    return OK;
  }

  int Trafo::Scale(const Vector& center, double f)
  {
    Vector h(center.Size());
    h.Set(f);
    RETURN_ERROR_IF_FAILED(Scale(center, h))
    return OK;
  }
#undef FNAME
#define FNAME "Trafo::append"
  int Trafo::append(const Trafo& t2)
  {
    Trafo res(dimSource, t2.dimTarget);

    if (t2.dimSource != dimTarget)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return ERROR;
      }

    res.m = t2.m * m;
    res.normalize();
    *this = res;
    return OK;
  }
#undef FNAME
#define FNAME "Trafo::prepend"
  int Trafo::prepend(const Trafo& t2)
  {
    Trafo res(dimSource, t2.dimTarget);

    if (t2.dimSource != dimTarget)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return ERROR;
      }

    res.m = m * t2.m;
    res.normalize();
    *this = res;
    return OK;
  }
#undef FNAME

#define FNAME "Trafo::Invert"
  int Trafo::Invert()
  {
    if (dimSource != dimTarget)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    m = ice::Inverse(m);
    normalize();
    return OK;
  }
#undef FNAME
#define FNAME "Trafo::Inverse"
  Trafo Trafo::Inverse() const
  {
    Trafo res(*this);

    if (dimSource != dimTarget)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return res;
      }

    res.Invert();
    return res;
  }
#undef FNAME
#define FNAME "Trafo::operator*"
  Trafo operator *(const Trafo& t1, const Trafo& t2)
  {
    Trafo res(t2.dimSource, t1.dimTarget);

    if (t1.dimSource != t2.dimTarget)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return res;
      }

    res.m = t1.m * t2.m;
    res.normalize();
    return res;
  }

  Vector operator *(const Trafo& t, const Vector& v)
  {
    Vector res(t.dimTarget);

    if (t.dimSource != v.Size())
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return res;
      }

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
    Vector3d res;

    if (t.dimSource != 3 || t.dimTarget != 3)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return res;
      }

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
#define FNAME "Transform"
  int Transform(const Trafo& tr, double& x, double& y)
  {
    double hf, xt, yt;

    if ((tr.dimSource != 2) || (tr.dimTarget != 2))
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    //  Vector res=tr*Vector(x,y);
    //  x=res[0]; y=res[1];
    hf = x * tr.m[2][0] + y * tr.m[2][1] + tr.m[2][2];
    xt = (x * tr.m[0][0] + y * tr.m[0][1] + tr.m[0][2]) / hf;
    yt = (x * tr.m[1][0] + y * tr.m[1][1] + tr.m[1][2]) / hf;
    x = xt;
    y = yt;
    return OK;
  }

  int Transform(const Trafo& tr, Point& p)
  {
    return Transform(tr, p.x, p.y, p.x, p.y);
  }

  int Transform(const Trafo& tr, std::vector<Point>& pl)
  {
    int res = OK;
    for (unsigned int i = 0; i < pl.size() && res == OK; i++)
      {
        res = Transform(tr, pl[i]);
      }
    return OK;
  }

  int Transform(const Trafo& tr, const std::vector<Point>& pl1, std::vector<Point>& pl2)
  {
    int res = OK;
    pl2.resize(pl1.size());
    for (unsigned int i = 0; i < pl1.size() && res == OK; i++)
      {
        res = Transform(tr, pl1[i], pl2[i]);
      }
    return OK;
  }

  int Transform(const Trafo& tr, Point p1, Point& p2)
  {
    return Transform(tr, p1.x, p1.y, p2.x, p2.y);
  }

  int Transform(const Trafo& tr, double& x, double& y, double& z)
  {
    double hf, xt, yt, zt;

    if ((tr.dimSource != 3) || (tr.dimTarget != 3))
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    //    Vector res=tr*Vector(x,y,z);
    //    x=res[0]; y=res[1]; z=res[2];
    hf = x * tr.m[3][0] + y * tr.m[3][1] + z * tr.m[3][2] + tr.m[3][3];
    xt = (x * tr.m[0][0] + y * tr.m[0][1] + z * tr.m[0][2] + tr.m[0][3]) / hf;
    yt = (x * tr.m[1][0] + y * tr.m[1][1] + z * tr.m[1][2] + tr.m[1][3]) / hf;
    zt = (x * tr.m[2][0] + y * tr.m[2][1] + z * tr.m[2][2] + tr.m[2][3]) / hf;
    x = xt;
    y = yt;
    z = zt;
    return OK;
  }

  int Transform(const Trafo& tr,
                double x, double y,
                double& xt, double& yt)
  {
    xt = x;
    yt = y;
    return Transform(tr, xt, yt);
  }

  int TransformAndRound(const Trafo& tr,
                        int& xt, int& yt)
  {
    double xtf, ytf;
    RETURN_ERROR_IF_FAILED(Transform(tr, xt, yt, xtf, ytf));
    xt = RoundInt(xtf);
    yt = RoundInt(ytf);
    return OK;
  }

  int TransformAndRound(const Trafo& tr, int x, int y, int& xt, int& yt)
  {
    double xs, ys;
    RETURN_ERROR_IF_FAILED(Transform(tr, x, y, xs, ys));
    xt = RoundInt(xs);
    yt = RoundInt(ys);
    return OK;
  }

  int Transform(const Trafo& tr,
                double x, double y, double z,
                double& xt, double& yt, double& zt)
  {
    xt = x;
    yt = y;
    zt = z;
    RETURN_ERROR_IF_FAILED(Transform(tr, xt, yt, zt));
    return OK;
  }

  int Transform(const Trafo& tr,
                double x, double y, double z,
                double& xt, double& yt)
  {
    double hf;

    if ((tr.dimSource != 3) || (tr.dimTarget != 2))
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    //    Vector res=tr*Vector(x,y,z);
    //    xt=res[0]; yt=res[1];
    hf = x * tr.m[2][0] + y * tr.m[2][1] + z * tr.m[2][2] + tr.m[2][3];
    xt = (x * tr.m[0][0] + y * tr.m[0][1] + z * tr.m[0][2] + tr.m[0][3]) / hf;
    yt = (x * tr.m[1][0] + y * tr.m[1][1] + z * tr.m[1][2] + tr.m[1][3]) / hf;
    return OK;
  }

#undef FNAME
}
