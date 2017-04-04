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

#include "Distortion2.h"

using namespace std;

namespace ice
{
  /*************************************************************
   * Klasse Modell 2
   *************************************************************/

#define FNAME "Distortion2::ffr"
  double Distortion2::ffr(double r) const
  {
    return 1.0 + ((d4 * r + d3) * r + d2) * r * r;
  }
#undef FNAME
#define FNAME "Distortion2::frs"
  double Distortion2::frs(double r) const
  {
    double r2 = r * r;
    double r3 = r2 * r;
    double r4 = r2 * r2;
    return r  = 1.0 + 3.0 * d2 * r2 + 4.0 * d3 * r3 + 5.0 * d4 * r4;
  }
#undef FNAME

#define FNAME "Distortion2::Distortion2"
  // constructor
  Distortion2::Distortion2(): DistortionR()
  {
    this->d2 = 0.0;
    this->d3 = 0.0;
    this->d4 = 0.0;
  }

  // constructor
  Distortion2::Distortion2(double x0, double y0, double d2, double d3, double d4): DistortionR(x0, y0)
  {
    this->d2 = d2;
    this->d3 = d3;
    this->d4 = d4;
  }

  Distortion2::Distortion2(double x0p, double y0p, double d2p, double d3p, double d4p,
                           int dimx, int dimy):
    DistortionR(x0p, y0p, dimx, dimy)
  {
    int maxdim = dimx > dimy ? dimx : dimy;
    d2 = d2p / maxdim / maxdim;
    d3 = d3p / maxdim / maxdim / maxdim;
    d4 = d4p / maxdim / maxdim / maxdim / maxdim;
  }

  Distortion2& Distortion2::operator =(const Distortion2& d)
  {
    x0 = d.x0;
    y0 = d.y0;
    d2 = d.d2;
    d3 = d.d3;
    d4 = d.d4;
    return *this;
  }

#if 0
  static int DistFunc2(const Vector& para, Vector& result)
  {
    int i, n, r;
    int punkte = (result.Size() - 2) / 2;
    double xo, yo, xm, ym;
    double xi, yi, zh;
    double xv, yv;

    Distortion2 dist(para[3], para[4], para[0], para[1], para[2]);

    double m11 = para[5];
    double m12 = para[6];
    double m13 = para[7];
    double m21 = para[8];
    double m22 = para[9];
    double m23 = para[10];
    double m31 = para[11];
    double m32 = para[12];

    for (i = 0, n = 13, r = 0; i < punkte; i++, n += 4, r += 2)
      {
        xo = para[n];
        yo = para[n + 1]; // original-punkte
        xm = para[n + 2];
        ym = para[n + 3]; // beobachtete Punkte
        zh = m31 * xo + m32 * yo + 1.0;
        xi = (m11 * xo + m12 * yo + m13) / zh;
        yi = (m21 * xo + m22 * yo + m23) / zh;

        dist.Distort(xi, yi, xv, yv);

        result[r] = xv - xm;
        result[r + 1] = yv - ym;
      }

    result[r] = (para[3] - para[n]) * dist.EXZENTRIZITAETSSTRAFE;
    result[r + 1] = (para[4] - para[n + 1]) * dist.EXZENTRIZITAETSSTRAFE;

    return 0;
  }

  static int DistFunc_Mod2(const Vector& para, Vector& result)
  {
    int i, n, r;
    int punkte = (result.Size() - 2) / 2;
    double xo, yo, xm, ym;
    double xi, yi, zh;
    double X0, Y0, D2, D3, D4;
    double abstand, faktor;
    double xh, yh;
    Distortion2 dist(para[3], para[4], para[0], para[1], para[2]);
    X0 = dist.X0();
    Y0 = dist.Y0();
    D2 = dist.D2();
    D3 = dist.D3();
    D4 = dist.D4();
    double m11 = para[5];
    double m12 = para[6];
    double m13 = para[7];
    double m21 = para[8];
    double m22 = para[9];
    double m23 = para[10];
    double m31 = para[11];
    double m32 = para[12];

    for (i = 0, n = 13, r = 0; i < punkte; i++, n += 4, r += 2)
      {
        xo = para[n];
        yo = para[n + 1]; // original-punkte
        xm = para[n + 2];
        ym = para[n + 3]; // beobachtete Punkte

        zh = m31 * xo + m32 * yo + 1.0;
        xi = (m11 * xo + m12 * yo + m13) / zh;
        yi = (m21 * xo + m22 * yo + m23) / zh;

        abstand = (xi - X0) * (xi - X0) + (yi - Y0) * (yi - Y0);
        faktor = 1.0 + D2 * abstand + D3 * abstand * sqrt(abstand) + D4 * abstand * abstand;
        xh = (xm - X0) / faktor + X0;
        yh = (ym - Y0) / faktor + Y0;

        result[r] = xi - xh;
        result[r + 1] = yi - yh;
      }

    result[r] = (para[3] - para[n]) * dist.EXZENTRIZITAETSSTRAFE;
    result[r + 1] = (para[4] - para[n + 1]) * dist.EXZENTRIZITAETSSTRAFE;
    return 0;
  }

  static void CalcDist2(const Matrix& mark, const Matrix& orig, Trafo& tr,
                        double& x0, double& y0, double& d2, double& d3, double& d4,
                        const Vector& ImageCenter)
  {
    int i, n, rc;

    Vector parameterVector(3 + 2 + 8 + 4 * mark.rows() + 2);
    IVector ovar(0);

    parameterVector[0] = 0;
    ovar = ovar && 0; // Startwert d2
    parameterVector[1] = 0;
    ovar = ovar && 1; // Startwert d3
    parameterVector[2] = 0;
    ovar = ovar && 2; // Startwert d4
    parameterVector[3] = ImageCenter[0];
    ovar = ovar && 3; // Startwert
    parameterVector[4] = ImageCenter[1];
    ovar = ovar && 4;

    Matrix tm = tr.Tmatrix();

    parameterVector[5] = tm[0][0] / tm[2][2];
    ovar = ovar && 5; // Startwerte projektive Trafo
    parameterVector[6] = tm[0][1] / tm[2][2];
    ovar = ovar && 6;
    parameterVector[7] = tm[0][2] / tm[2][2];
    ovar = ovar && 7;
    parameterVector[8] = tm[1][0] / tm[2][2];
    ovar = ovar && 8;
    parameterVector[9] = tm[1][1] / tm[2][2];
    ovar = ovar && 9;
    parameterVector[10] = tm[1][2] / tm[2][2];
    ovar = ovar && 10;
    parameterVector[11] = tm[2][0] / tm[2][2];
    ovar = ovar && 11;
    parameterVector[12] = tm[2][1] / tm[2][2];
    ovar = ovar && 12;

    for (i = 0, n = 13; i < mark.rows(); i++, n += 4)
      {
        parameterVector[n] = orig[i][0];
        parameterVector[n + 1] = orig[i][1];
        parameterVector[n + 2] = mark[i][0];
        parameterVector[n + 3] = mark[i][1];
      }

    // Bildmittelpunkt
    parameterVector[n] = ImageCenter[0];
    parameterVector[n + 1] = ImageCenter[1];
    int inumber = 100000;

    rc = LMDif(parameterVector, ovar, DistFunc2, mark.rows() * 2 + 2, inumber);

    if (rc > 4)
      {
        throw IceException(FNAME, M_NUM_INSTABILITY, ERROR);
      }
    else
      {
        inumber = 100000;
        rc = LMDif(parameterVector, ovar, DistFunc_Mod2, mark.rows() * 2 + 2, inumber);

        if (rc > 4)
          {
            throw IceException(FNAME, M_NUM_INSTABILITY, ERROR);
          }
        else
          {
            // store calculated parameters of distortion
            d2 = parameterVector[0];
            d3 = parameterVector[1];
            d4 = parameterVector[2];
            x0 = parameterVector[3];
            y0 = parameterVector[4];
            // Printf("Verzeichnung: k2:%g k3:%g k4:%g x0:%g y0:%g\n",d2,d3,d4,x0,y0);
            // .. and projective transform
            tm[0][0] = parameterVector[5];
            tm[0][1] = parameterVector[6];
            tm[0][2] = parameterVector[7];
            tm[1][0] = parameterVector[8];
            tm[1][1] = parameterVector[9];
            tm[1][2] = parameterVector[10];
            tm[2][0] = parameterVector[11];
            tm[2][1] = parameterVector[12];
            tm[2][2] = 1.0;
            tr = Trafo(tm);
          }
      }
  }
#endif

  Distortion2::Distortion2(const Matrix& mark, const Matrix& orig,
                           Trafo& tr, const Vector& ImageCenter)
  {
    Calc(mark, orig, tr, ImageCenter[0], ImageCenter[1]);
  }

  Distortion2::Distortion2(const Matrix& mark, const Matrix& orig,
                           Trafo& tr)
  {
    Calc(mark, orig, tr);
  }


  Distortion2::Distortion2(const Matrix& mark, const Matrix& orig, const Vector& ImageCenter)
  {
    Calc(mark, orig, ImageCenter[0], ImageCenter[1]);
  }

  Distortion2::Distortion2(const Matrix& mark, const Matrix& orig)
  {
    Calc(mark, orig);
  }
#undef FNAME

#define FNAME "Distortion2::toString"
  string Distortion2::toString() const
  {
    ostringstream os;
    os << "x0: " << x0 << " y0: " << y0 << " d2: " << d2 << " d3: " << d3 << " d4: " << d4 ;
    return os.str();
  }
#undef FNAME

#define FNAME "Distortion2::Set"
  void Distortion2::Set(const string& parastr)
  {
    istringstream is(parastr);

    if (! ReadPara(is, "x0", x0) || ! ReadPara(is, "y0", y0) ||
        ! ReadPara(is, "d2", d2) || ! ReadPara(is, "d3", d3) || ! ReadPara(is, "d4", d4))
      {
        throw IceException(FNAME, M_WRONG_FORMAT, WRONG_PARAM);
      }
  }
#undef FNAME

  Vector Distortion2::MakeVector() const
  {
    return Vector(x0, y0, d2, d4, d3);
  }

#define FNAME "Distortion2::Set"
  void Distortion2::Set(const Vector& v)
  {
    if (v.size() != 5)
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
      }

    x0 = v[0];
    y0 = v[1];
    d2 = v[2];
    d3 = v[3];
    d4 = v[4];
  }
#undef FNAME
}
