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
//
//  root.cpp
//
//  Nullstellen von Polynomen 2,3,und 4. Grades:
//  Root2   Root3   Root4

//#include <math.h>
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <stdlib.h>

#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "analygeo.h"
#include "complex.h"
#include "matrix_function.h"

namespace ice
{
  /* lokale Funktion */
  bool roottest(double, double, double, double, double,
                double&, double&, double&, double&);

  /*******************************************************************/
  int Root2(double p1, double p0, Complex& c1, Complex& c2)
  {
    double dis = (p1 * p1 / 4.0) - p0;

    if (dis > 0)
      {
        double h = sqrt(dis);
        c1 = Complex(-(p1 / 2.0) + h, 0);
        c2 = Complex(-(p1 / 2.0) - h, 0);
      }

    else if (dis < 0)
      {
        double h = sqrt(-dis);
        c1 = Complex(-p1 / 2.0, h);
        c2 = Complex(-p1 / 2.0, -h);
      }
    else
      {
        c1 = Complex(-p1 / 2.0, 0);
        c2 = Complex(-p1 / 2.0, 0);
      }
    return OK;
  }
  /*******************************************************************/
#define FNAME "Root3"

  int Root3(double p2, double p1, double p0, std::vector<Complex>& c)
  {
    c.resize(3);

    const double eps = 1e-10;

    double b2 = -(p2 * p2 / 3.0) + p1;
    double h = -p1 * p2 / 3.0;
    double b3 = (2.0 * p2 * p2 * p2 / 27.0) + p0 + h;
    h = b2 * b2 * b2 / 27.0;
    double dis = (b3 * b3 / 4.0) + h;

    if (dis < 0)
      {
        double b3sign = 1.0;
        if (fabs(b3) > eps)
          {
            b3sign = SignD(b3);
          }

        double r = sqrt(fabs(b2 / 3.0)) * b3sign;
        double x = b3 / 2.0 / r / r / r;
        double y = sqrt(std::max(-x * x + 1.0, 0.0));

        double phi;
        ConvCartesPolar(x, y, h, phi);

        h = p2 / 3.0;
        c[0] = Complex(-2 * r * cos(phi / 3.0) - h, 0.0);
        c[1] = Complex(2 * r * cos((-phi + M_PI) / 3.0) - h, 0.0);
        c[2] = Complex(2 * r * cos((phi + M_PI) / 3.0) - h, 0.0);
      }
    else
      {
        dis = sqrt(dis);
        h = -b3 / 2.0 + dis;
        double u = CubRoot(h);
        h = -b3 / 2.0 - dis;
        double v = CubRoot(h);
        h = p2 / 3.0;
        c[0] = Complex(u + v - h, 0.0);
        c[1] = Complex(-(u + v) / 2.0 - h, (u - v) / 2.0 * sqrt(3.0));
        c[2] = Complex(c[1].real(), -c[1].imag());
      }
    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "Root4"
  int Root4
  (double p3, double p2, double p1, double p0, std::vector<Complex>& cv)
  {
    cv.resize(4);
    const double eps = 1e-10;

    if (fabs(p1) < eps && fabs(p3) < eps)
      /* Sonderfall p2*x*x+p0=0 */
      {
        Complex hc1, hc2;
        Root2(p2, p0, hc1, hc2);

        if (fabs(hc1.imag()) > eps)
          {
            double rad, phi;
            ConvCartesPolar(hc1.real(), hc1.imag(), rad, phi);
            rad = sqrt(rad);
            phi = phi / 2.0;
            double re, im;
            ConvPolarCartes(rad, phi, re, im);
            cv[0] = Complex(re, im);
            cv[1] = Complex(re, -im);
            cv[2] = Complex(-re, im);
            cv[3] = Complex(-re, -im);
          }
        else
          {
            if (hc1.real() < 0)
              {
                cv[0] = Complex(0.0, sqrt(-hc1.real()));
              }
            else
              {
                cv[0] = Complex(sqrt(hc1.real()), 0.0);
              }
            cv[1] = -cv[0];

            if (hc2.real() < 0)
              {
                cv[2] = Complex(0.0, sqrt(-hc2.real()));
              }
            else
              {
                cv[2] = Complex(sqrt(hc2.real()), 0.0);
              }
            cv[3] = -cv[2];
          }
      }
    else
      /* allgemeiner Fall */
      {
        double b2 = p3 * p1 - 4.0 * p0 - p2 * p2 / 3.0;
        double h = 8.0 * p2 * p0 / 3.0 + p3 * p2 * p1 / 3.0 - p1 * p1;
        double b3 = -p3 * p3 * p0 - 2.0 * p2 * p2 * p2 / 27.0 + h;

        std::vector<Complex> ch(3);
        Root3(0.0, b2, b3, ch);

        double a0, a1, b0, b1;
        if (!(roottest(ch[0].real(), p3, p2, p1, p0, a0, a1, b0, b1) ||
              roottest(ch[1].real(), p3, p2, p1, p0, a0, a1, b0, b1) ||
              roottest(ch[2].real(), p3, p2, p1, p0, a0, a1, b0, b1))
           )
          throw IceException(FNAME, M_NUM_INSTABILITY);

        Complex hc1, hc2;
        Root2(a1, a0, hc1, hc2);
        cv[0] = hc1;
        cv[1] = hc2;
        Root2(b1, b0, hc1, hc2);
        cv[2] = hc1;
        cv[3] = hc2;
      }

    return OK;
  }
#undef FNAME

  /***** Nichtoeffentliche Funktionen *****/
  bool roottest(double y,
                double p3, double p2, double p1, double p0,
                double& a0, double& a1,
                double& b0, double& b1)
  {
    double mm = ((p2 / 3.0 + y) * (p2 / 3.0 + y) / 4.0) - p0;

    if (fabs(mm) < 0.0001)
      {
        mm = std::max(mm, 0.0);
      }

    if (mm < 0.0)
      {
        return false;
      }

    double nn = p3 * p3 / 16.0 + y / 4.0 - p2 / 6.0;

    if (fabs(nn) < 0.0001)
      {
        nn = std::max(nn, 0.0);
      }

    if (nn < 0.0)
      {
        return false;
      }

    double mn = (p2 / 3.0 + y) * p3 / 8.0 - p1 / 4.0;

    double m = sqrt(mm);
    double n = sqrt(nn);

    if (mn < 0.0)
      {
        n = -n;
      }

    a1 = p3 / 2.0 + 2.0 * n;
    b1 = p3 / 2.0 - 2.0 * n;
    a0 = p2 / 6.0 + y / 2.0 + m;
    b0 = p2 / 6.0 + y / 2.0 - m;
    return true;
  }
#define FNAME "GammaFunction"
  /*******************************************************************/
  /* Berechnung der Gamma-Funktion mit                               */
  /* a) Reihenentwicklung                                            */
  /* b) Horner-Schema                                                */
  /* c) Rekursionsformel                                             */
  /*******************************************************************/
  double GammaFunction(double x)
  {
    if (x <= 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    double c0 = 1.0;
    double c1 = 1.0    / 12.0;
    double c2 = 1.0    / 288.0;
    double c3 = -139.0 / 51840.0;
    double c4 = -134.0 / 583949.0;
    double c5 = 101.0  / 128820.0;
    double c6 = 1.0    / 14514.0;
    /* Nutzung von G(1+x+6)=(x+6)*(x+5)*(x+4)*(x+3)*(x+2)*(x+1)*x*G(x)   */
    double z = x + 6.0;
    double h1 = pow(z / M_E, z) * sqrt(2 * M_PI * z);
    /* Horner-Schema */
    double h2 = c6 / z + c5;
    h2 = h2 / z + c4;
    h2 = h2 / z + c3;
    h2 = h2 / z + c2;
    h2 = h2 / z + c1;
    h2 = h2 / z + c0;
    double h = (h1 * h2) / (x * (x + 1.0) * (x + 2.0) * (x + 3.0) * (x + 4.0) * (x + 5.0) * (x + 6.0));
    return h;
  }
#undef FNAME
}
