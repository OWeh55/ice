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
/********************************************************************
Analytische Geometrie
  2D-Geometrie:
    ConvCartesPolar
    ConvPolarCartes
    ConvPointHesse
    RotTransPoint
    DistPointLine
    IntersecLine
    TangentsCircle
    AreaTriangle
  3D-Geometrie:
    ConvR3PointHesse
    DistR3PointLine
    DistR3PointPlane
*********************************************************************/
#include <float.h>

#include "defs.h"
#include "message.h"

#include "numbase.h"
#include "Point.h"
#include "fit.h"
#include "vectorarrayfunctions.h"
#include "matrix_function.h"
#include "root.h"

#include "analygeo.h"

namespace ice
{
  /*** Nichtoeffentliche Funktionen ***/
  double edAbstand(double psi, double ae, double be, double xq, double yq);
  double ellipseDist(double p[2], double ae, double be,
                     double m[2], double phi, double n[2]);

  /*******************************************************************/
  void ConvCartesPolar(double p[2], double* rad, double* arcus)
  {
    *rad = sqrt(Sqr(p[0]) + Sqr(p[1]));
    *arcus = atan2(p[1], p[0]);
  }

  void ConvCartesPolar(double x, double y, double& rad, double& arcus)
  {
    rad = sqrt(Sqr(x) + Sqr(y));
    arcus = atan2(y, x);
  }

  /*******************************************************************/
  void ConvPolarCartes(double rad, double arcus, double p[2])
  {
    p[0] = cos(arcus) * rad;
    p[1] = sin(arcus) * rad;
  }

  void ConvPolarCartes(double rad, double arcus, double& x, double& y)
  {
    x = cos(arcus) * rad;
    y = sin(arcus) * rad;
  }

  /*******************************************************************/
#define FNAME "ConvPointHesse"
  int ConvPointHesse(double p1[2], double p2[2], double* p, double* phi)
  {
    double dy = p2[1] - p1[1];
    double dx = p2[0] - p1[0];

    double hs = sqrt(dx * dx + dy * dy);

    if (hs < EPSILON)
      {
        Message(FNAME, M_POINT_IDENTIC, POINT_IDENTIC);
        return (POINT_IDENTIC);
      }

    *p = (p1[0] * dy - p1[1] * dx) / hs;

    if (*p < 0)
      {
        dx = -dx;
        dy = -dy;
        *p = -*p;
      }

    *phi = atan2(-dx, dy);
    return OK;
  }

  Vector ConvPointHesse(const Vector& p1, const Vector& p2)
  {
    Vector res(2);
    double dy = p2[1] - p1[1];
    double dx = p2[0] - p1[0];

    double hs = sqrt(dx * dx + dy * dy);

    if (hs < EPSILON)
      {
        Message(FNAME, M_POINT_IDENTIC, POINT_IDENTIC);
        return res;
      }

    double p = (p1[0] * dy - p1[1] * dx) / hs;

    if (p < 0)
      {
        dx = -dx;
        dy = -dy;
        p = -p;
      }

    res[0] = p;
    res[1] = atan2(-dx, dy);
    return res;
  }

#undef FNAME
  /*******************************************************************/
  void RotTransPoint(double p1[2], double centre[2], double arcus, double p2[2])
  {
    if (centre == NULL)
      {
        p2[0] = cos(arcus) * (p1[0]) - sin(arcus) * (p1[1]);
        p2[1] = sin(arcus) * (p1[0]) + cos(arcus) * (p1[1]);
      }
    else
      {
        p2[0] = cos(arcus) * (p1[0] - centre[0]) - sin(arcus) * (p1[1] - centre[1]) + centre[0];
        p2[1] = sin(arcus) * (p1[0] - centre[0]) + cos(arcus) * (p1[1] - centre[1]) + centre[1];
      }
  }
  /*******************************************************************/
  double DistPointPoint(double p1[2], double p2[2])
  {
    double dx = p1[0] - p2[0];
    double dy = p1[1] - p2[1];
    return sqrt(dx * dx + dy * dy);
  }

  double DistPointLine(double v[2], double p, double phi)
  {
    return cos(phi) * v[0] + sin(phi) * v[1] - p;
  }
  /*******************************************************************/
  double DistPointCircle(double p[2], double* par)
  {
    return fabs(sqrt(Sqr(p[0] - par[0]) + Sqr(p[1] - par[1])) - par[2]);
  }
  /*******************************************************************/
  int IntersecLine(double p1, double phi1, double p2, double phi2,
                   double* ip)
  {
    double c1, s1, c2, s2, det;
    c1 = cos(phi1);
    s1 = sin(phi1);
    c2 = cos(phi2);
    s2 = sin(phi2);
    det = c1 * s2 - s1 * c2;

    if (fabs(det) < 1e-32) return (PARALLEL);

    ip[0] = (p1 * s2 - p2 * s1) / det;
    ip[1] = (p2 * c1 - p1 * c2) / det;
    return OK;
  }
  /*******************************************************************/
  int TangentCircle(double p[2], double* par, double pt1[2], double pt2[2])
  {
    double p0[2] = {0.0, 0.0};
    double ph[2];
    double phi, len;
    ph[0] = p[0] - par[0];
    ph[1] = p[1] - par[1];
    phi = atan2(ph[1], ph[0]);
    len = sqrt(Sqr(ph[0]) + Sqr(ph[1]));
    pt1[0] = pt2[0] = Sqr(par[2]) / len;
    pt1[1] = par[2] * sin(acos(par[2] / len));
    pt2[1] = -pt1[1];
    RotTransPoint(pt1, p0, phi, pt1);
    pt1[0] += par[0];
    pt1[1] += par[1];
    RotTransPoint(pt2, nullptr, phi, pt2);
    pt2[0] += par[0];
    pt2[1] += par[1];
    return OK;
  }
  /*******************************************************************/
#define FNAME "AreaPolygon"
  double AreaPolygon(PointList pl)
  {
    int i;
    double area;
    area = 0;

    for (i = 0; i < pl->lng - 1; i++)
      area += ((pl->xptr)[i] - (pl->xptr)[i + 1]) *
              ((pl->yptr)[i] + (pl->yptr)[i + 1]);

    area += ((pl->xptr)[pl->lng - 1] - (pl->xptr)[0]) *
            ((pl->yptr)[pl->lng - 1] + (pl->yptr)[0]);
    return (area * 0.5);
  }

  double AreaPolygon(const Matrix& pl)
  {
    int i;
    double area = 0;

    if (pl.cols() < 2)
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return 0.0;
      }

    int last = pl.rows() - 1;

    for (i = 0; i < last - 1; i++)
      area += (pl[i][0] - pl[i + 1][0]) * (pl[i][1] + pl[i + 1][1]);

    area += (pl[last][0] - pl[0][0]) *
            (pl[last][1] + pl[0][1]);
    return (area * 0.5);
  }
#undef FNAME
  /*******************************************************************/
#define FNAME "FeatureQuadrFunc"
  /* #define DEBUG */
  int FeatureQuadrFunc(double par[6], double feat[5], int* type)
  {
    double A, A33;
    double Ah, Bh;
    double a, b, c, d, e, f;
    double xm, ym, phi, ha, hb, hc, hd, hf, hha, hhb, hhc, hhd, hhe;
    a = par[0];
    b = par[1];
    c = par[2];
    d = par[3];
    e = par[4];
    f = par[5];
    A =   a * (b * f   - d * d / 4)
          - e / 2 * (e * f / 2 - d * c / 4)
          + c / 2 * (e * d / 4 - b * c / 2);
#ifdef DEBUG
    printf("A %f\n", A);
    getchar();
#endif

    if (fabs(A) < EPSILON)
      {
        *type = DEGENERATE;
        return OK;
      }

    A33 = a * b - e * e / 4;
#ifdef DEBUG
    printf("A33 %f\n", A33);
    getchar();
#endif

    if (fabs(A33) < EPSILON) /* Parabel*/
      {
        if (a < 0)
          {
            a = -a;
            b = -b;
            c = -c;
            d = -d;
            e = -e;
            f = -f;
          }

        /* Bestimmung des Scheitelpunktes */
#ifdef DEBUG
        printf("Beginn Parabel\n");
        getchar();
        printf("a %f b %f c %f d %f e %f f %f\n", a, b, c, d, e, f);
        printf("fabs(2*b*sqrt(b)-e*sqrt(a) %g\n", fabs(2 * b * sqrt(b) - e * sqrt(a)));
#endif

        if (fabs(2 * b * sqrt(b) - e * sqrt(a)) < EPSILON) /* Parabelachse parallel zur y-Achse */
          {
#ifdef DEBUG
            printf("parallel y-Achse\n");
            getchar();
#endif
            xm = (sqrt(a) * c - sqrt(b) * d) / (sqrt(b) * e - 2 * sqrt(a) * a);

            if (fabs(b) < EPSILON)
              ym = -(a * xm * xm + c * xm + f) / (d + e * xm);
            else
              ym = (e * xm + d) / (-2 * b);

            goto label;
          }

        if (fabs(sqrt(b)*e - 2 * a * sqrt(a)) < EPSILON) /* Parabelachse parallel zur x-Achse */
          {
#ifdef DEBUG
            printf("parallel x-Achse\n");
            printf("a %f b %f c %f d %f e %f f %f\n", a, b, c, d, e, f);
            getchar();
#endif
            ym = (sqrt(a) * c - sqrt(b) * d) / (2 * sqrt(b) * b - sqrt(a) * e);

            if (fabs(a) < EPSILON) xm = -(b * ym * ym + d * ym + f) / (c + e * ym);
            else xm = (c + e * ym) / (-2 * a);

            goto label;
          }

        /* Allgemeiner Fall */
#ifdef DEBUG
        printf("Allgemeiner Fall\n");
        getchar();
#endif
        Ah = (2 * a * sqrt(a) - sqrt(b) * e) / (2 * b * sqrt(b) - e * sqrt(a));
        Bh = (sqrt(a) * c - sqrt(b) * d) / (2 * b * sqrt(b) - e * sqrt(a));

        if (fabs(a + b * Ah * Ah + e * Ah) < EPSILON)
          xm = (-b * Bh * Bh - d * Bh - f) / (2 * b * Ah * Bh + c + d * Ah + e * Bh);
        else
          xm = (2 * b * Ah * Bh + c + d * Ah + e * Bh) / (-2 * (a + b * Ah * Ah + e * Ah));

        ym = Ah * xm + Bh;
label:
        feat[0] = xm;
        feat[1] = ym;
#ifdef DEBUG
        printf("xm %f ym %f\n", xm, ym);
        getchar();
#endif

        /* Bestimmung der neuen Koeffizienten nach der Verschiebung */
        hc = 2 * a * xm + c + e * ym;
        hd = 2 * b * ym + d + e * xm;
        phi = atan2(hd, hc);
#ifdef DEBUG
        printf("phi %f\n", phi);
        getchar();
#endif

        hf = a * xm * xm + b * ym * ym + c * xm + d * ym + e * xm * ym + f;
        par[0] = hha = a * Sqr(cos(phi)) + b * Sqr(sin(phi)) + e * sin(phi) * cos(phi);
        par[1] = hhb = a * Sqr(sin(phi)) + b * Sqr(cos(phi)) - e * sin(phi) * cos(phi);
        par[2] = hhc = hc * cos(phi) + hd * sin(phi);
        par[3] = hhd = -hc * sin(phi) + hd * cos(phi);
        par[4] = hhe = 2 * (a * sin(phi) * cos(phi) - b * sin(phi) * cos(phi)) +
                       e * (Sqr(cos(phi)) - Sqr(sin(phi)));
        par[5] = hf;
#ifdef DEBUG
        PrintVecRn("par nach R?cknahme Verdrehung", par, 6);
        getchar();
#endif
        feat[2] = -hhc / (hhb * 2);
#ifdef DEBUG
        printf("p: %f\n", feat[2]);
        getchar();
#endif

        if (feat[2] < 0)
          {
            feat[2] = -feat[2];
            phi += M_PI;
          }

        feat[3] = fmod(phi, 2 * M_PI);

        if (phi < 0) feat[3] += 2 * M_PI;

        *type = PARABEL;
        return OK;
      }
    else /* Ellipse/Hyperbel */
      {
        feat[0] = xm = (e * d / 4 - c * b / 2) / A33;
        feat[1] = ym = (c * e / 4 - a * d / 2) / A33;

#ifdef DEBUG
        printf("ELLIPSE\n");
        printf("xm %f ym %f\n", xm, ym);
        getchar();
#endif

        hf = a * xm * xm + b * ym * ym + c * xm + d * ym + e * xm * ym + f;

        if (fabs(e) < EPSILON && fabs(a - b) < EPSILON) phi = 0;
        else phi = atan2(e, a - b) / 2;

#ifdef DEBUG
        printf("phi %f cos(phi) %f\n", phi, cos(phi));
        PrintVecRn("par", par, 6);
        getchar();
#endif
        ha = (a * Sqr(cos(phi)) + b * Sqr(sin(phi)) + e * sin(phi) * cos(phi)) / (-hf);
        hb = (a * Sqr(sin(phi)) + b * Sqr(cos(phi)) - e * sin(phi) * cos(phi)) / (-hf);
#ifdef DEBUG
        printf("ha %f hb %f\n", ha, hb);
        getchar();
#endif

        if (SignD(ha * hb) < 0) /* Hyperbel */
          {
#ifdef DEBUG
            printf("Hyperbel\n");
            getchar();
#endif
            *type = HYPERBEL;

            if (ha < 0)
              {
                phi += M_PI / 2;
                feat[2] = 1 / sqrt(hb);
                feat[3] = 1 / sqrt(-ha);
              }
            else
              {
                feat[2] = 1 / sqrt(ha);
                feat[3] = 1 / sqrt(-hb);

                if (phi < 0) feat[2] += M_PI;
              }

            feat[4] = phi;

            if (phi > M_PI)
              feat[4] -= M_PI;

            if (phi < 0)
              feat[4] += M_PI;

            return OK;
          }
        else /* Ellipse */
          {
            if (ha < 0)
              {
                *type = DEGENERATE;
                return OK;
              }
            else
              {
#ifdef DEBUG
                printf("Ellipse\n");
                getchar();
#endif
                *type = ELLIPSE;

                if (ha > hb) /* Vertauschen der Achsen */
                  {
                    phi += M_PI / 2;
                    feat[2] = 1 / sqrt(hb);
                    feat[3] = 1 / sqrt(ha);
                  }
                else
                  {
                    feat[2] = 1 / sqrt(ha);
                    feat[3] = 1 / sqrt(hb);
                  }

#ifdef DEBUG
                printf("vor Auswertung phi: %f\n", phi);
                getchar();
#endif
                feat[4] = phi;

                if (phi > M_PI)
                  feat[4] -= M_PI;

                if (phi < 0)
                  feat[4] += M_PI;

                return OK;
              }
          }
      }
  }
#undef FNAME
#undef DEBUG

  /*******************************************************************/
#define FNAME "ParamQuadrFunc"
  /* #define DEBUG */
  int ParamQuadrFunc(double feat[5], int type, double par[6])
  {
    double xm, ym, phi, a, b, p;
    double hc, hd;

    switch (type)
      {
      case ELLIPSE: /* Normallage: xx/aa + yy/bb - 1 = 0 */
        xm = feat[0];
        ym = feat[1];
        phi = feat[4];
        a = feat[2];
        b = feat[3];

        if (a <= 0 || b <= 0)
          {
            Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
            return WRONG_PARAM;
          }

#ifdef DEBUG
        printf("xm: %f ym %f phi %f a %f b %f\n", xm, ym, phi, a, b);
        getchar();
#endif

        par[0] = Sqr(cos(phi) / a) + Sqr(sin(phi) / b);
        par[1] = Sqr(sin(phi) / a) + Sqr(cos(phi) / b);
        par[4] = 2 * sin(phi) * cos(phi) / (a * a) - 2 * sin(phi) * cos(phi) / (b * b);
        par[2] = -2 * par[0] * xm - par[4] * ym;
        par[3] = -2 * par[1] * ym - par[4] * xm;
        par[5] = par[0] * xm * xm + par[1] * ym * ym + par[4] * xm * ym - 1;
#ifdef DEBUG
        PrintVecRn("par in ParamQuadr", par, 6);
        getchar();
#endif
        return OK;

      case HYPERBEL:  /* Normallage: xx/aa - yy/bb - 1 = 0 */
        xm = feat[0];
        ym = feat[1];
        phi = feat[4];
        a = feat[2];
        b = feat[3];

        if (a <= 0 || b <= 0)
          {
            Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
            return WRONG_PARAM;
          }

        par[0] = Sqr(cos(phi) / a) - Sqr(sin(phi) / b);
        par[1] = Sqr(sin(phi) / a) - Sqr(cos(phi) / b);
        par[4] = 2 * sin(phi) * cos(phi) / (a * a) + 2 * sin(phi) * cos(phi) / (b * b);
        par[2] = -2 * par[0] * xm - par[4] * ym;
        par[3] = -2 * par[1] * ym - par[4] * xm;
        par[5] = par[0] * xm * xm + par[1] * ym * ym + par[4] * xm * ym - 1;
        return OK;

      case PARABEL: /* Normallage: yy - 2px = 0 */
        xm = feat[0];
        ym = feat[1];
        phi = feat[3];
        p = feat[2];

        if (p <= 0)
          {
            Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
            return WRONG_PARAM;
          }

        par[0] = Sqr(sin(phi));
        par[1] = Sqr(cos(phi));
        par[4] = -2 * sin(phi) * cos(phi);
        hc = -2 * p * cos(phi);
        hd = -2 * p * sin(phi);
        par[2] = -2 * par[0] * xm - par[4] * ym + hc;
        par[3] = -2 * par[1] * ym - par[4] * xm + hd;
        par[5] = par[0] * xm * xm +
                 par[1] * ym * ym -
                 hc * xm -
                 hd * ym +
                 par[4] * xm * ym;
        return OK;
      default:
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }
  }
#undef FNAME
#undef DEBUG

  /*******************************************************************/
#define FNAME "ConvR3PointHesse"
  /* #define DEBUG */
  int ConvR3PointHesse(double p1[3], double p2[3], double p3[3],
                       double* p, double n[3])
  {
    double a, b, c, d, l;
    a =  p1[1] * (p2[2] - p3[3]) - p1[2] * (p2[1] - p3[1]) + p2[1] * p3[3] - p3[1] * p2[2];
    b = -(p1[0] * (p2[2] - p3[3]) - p1[2] * (p2[0] - p3[0]) + p2[0] * p3[3] - p3[0] * p2[2]);
    c =  p1[0] * (p2[1] - p3[1]) - p1[1] * (p2[0] - p3[0]) + p2[0] * p3[1] - p3[0] * p2[1];

    d = -(p1[0] * (p2[1] * p3[3] - p3[1] * p2[2]) - p1[1] * (p2[0] * p3[3] - p3[0] * p2[2]) +
          p1[2] * (p2[0] * p3[1] - p3[0] * p2[1]));

    l = sqrt(a * a + b * b + c * c);

    if (l < EPSILON)
      {
        Message(FNAME, M_WRONG_POINTS, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (d < 0)
      {
        *p = -d / l;
        n[0] = a / l;
        n[1] = b / l;
        n[2] = c / l;
        return OK;
      }
    else
      {
        *p = d / l;
        n[0] = -a / l;
        n[1] = -b / l;
        n[2] = -c / l;
        return OK;
      }
  }
#undef FNAME

  /*******************************************************************/
  double DistR3PointPlane(double v[3], double p, double n[3])
  {
    return cos(n[0]) * v[0] + cos(n[1]) * v[1] + cos(n[2]) * v[2] - p;
  }

  /*******************************************************************/
  double DistR3PointLine(double p[3], double displ[3], double dir[3])
  {
    return sqrt(Sqr((p[0] - displ[0]) * dir[1] - (p[1] - displ[1]) * dir[0]) +
                Sqr((p[1] - displ[1]) * dir[2] - (p[2] - displ[2]) * dir[1]) +
                Sqr((p[2] - displ[2]) * dir[0] - (p[0] - displ[0]) * dir[2]))
           / Sqr(LengthVec(dir));
  }

  /*******************************************************************/
#define FNAME "DistPointEllipse"
#define DEBUG 0
  double DistPointEllipse(double p[2], double* par, double* koord)
  {
    double m[2], phi, A, B;
    double cphi, sphi, e, xx, yy, x, y, d, cospsi, sinpsi;
    double cospsi_opt = 0.0, sinpsi_opt = 0.0;
    double x_e, y_e, x_e_or, y_e_or;
    double min_dist = 0.0, xs;
    int i;

    m[0] = par[0];
    m[1] = par[1];
    A = par[2];
    B = par[3];
    phi = par[4];

    if ((A <= 0) || (B <= 0))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return 0.0;
      }

#if DEBUG
    printf("DistPointEllipse\n");
    printf("p %f %f\n", p[0], p[1]);
    printf("m %f %f\n", m[0], m[1]);
    printf("phi %f A %f B %f\n", phi, A, B);
    getchar();
#endif

    if (A < B)
      {
        double h = A;
        A = B;
        B = h;
      }

    cphi = cos(phi);
    sphi = sin(phi);

    xx = p[0] - m[0];
    yy = p[1] - m[1];
    x = xx * cphi + yy * sphi;
    y = -xx * sphi + yy * cphi;
    e = x * x / (A * A) + y * y / (B * B);

    if (e == 1.0)
      return (0.0);

    // *******************************************************
    const double eps = 1.0e-5;

    if (fabs(y) > eps)
      {
        std::vector<Complex> c;
        Root4(2.0 * (A * A - B * B + A * x) / (B * y),
              0.0,
              -2.0 * (A * A - B * B - A * x) / (B * y),
              -1.0,
              c);
        min_dist = DBL_MAX;

        for (i = 0; i < 4; ++i)
          {
            if (fabs(c[i].imag()) < eps)
              {
                sinpsi = (2.0 * c[i].real()) / (1.0 + c[i].real() * c[i].real());
                cospsi = (1.0 - c[i].real() * c[i].real()) / (1.0 + c[i].real() * c[i].real());
                d = sqrt((x - A * cospsi) * (x - A * cospsi) + (y - B * sinpsi) * (y - B * sinpsi));

                if (d < min_dist)
                  {
                    min_dist = d;
                    sinpsi_opt = sinpsi;
                    cospsi_opt = cospsi;
                  }
              }
          }
      }
    //
    // Fall mit 4 Loesungen wurde behandelt
    //*
    else
      {
        if (fabs(A - B) < eps)
          {
            if (x >= 0)
              {
                sinpsi_opt = 0.0;
                cospsi_opt = 1.0;
                min_dist = fabs(A - x);
              }
            else
              {
                sinpsi_opt = 0.0;
                cospsi_opt = -1.0;
                min_dist = fabs(A + x);
              }
          }
        //
        // Fall wurde behandelt: Objekt ist Kreis und Punkt auf x-Achse
        //
        else
          {
            xs = (A * A - B * B) / A;

            if (x > xs)
              {
                cospsi_opt = 1.0;
                sinpsi_opt = 0.0;
                min_dist = sqrt((x - A) * (x - A));
              }
            else
              {
                if (x < -xs)
                  {
                    cospsi_opt = -1.0;
                    sinpsi_opt = 0.0;
                    min_dist = sqrt((A + x) * (A + x));
                  }
                else // if (x<=xs && x>=-xs)
                  {
                    cospsi_opt = (A * x) / (A * A - B * B);
                    sinpsi_opt = sqrt(1.0 - cospsi_opt * cospsi_opt);
                    min_dist = sqrt((x - A * cospsi_opt) * (x - A * cospsi_opt) +
                                    (y - B * sinpsi_opt) * (y - B * sinpsi_opt));
                  }
              }
          }
      }

    //----------------------------------------------------------------------
    x_e = A * cospsi_opt;
    y_e = B * sinpsi_opt;
    x_e_or = x_e * cphi - y_e * sphi + m[0];
    y_e_or = x_e * sphi + y_e * cphi + m[1];

    //
    // Auf x_e_or,y_e_or stehen die Koordinaten des Punktes der originalen
    // Ellipse, der zum gegebenen Punkt den minimalen Abstand besitzt !!
    //
    if (koord != nullptr)
      {
        koord[0] = x_e_or;
        koord[1] = y_e_or;
      }

    SetOk();
    return (min_dist);
  }
#undef FNAME
#undef DEBUG

  /*******************************************************************/
#define FNAME "ClosestPointLineSeg"
  double ClosestPointLineSeg(double p[2], double ls0[2], double ls1[2], double pf[2])
  {
    double my, r1[2], r2[2];
    r1[0] = ls1[0] - ls0[0];
    r1[1] = ls1[1] - ls0[1];
    r2[0] = p[0] - ls0[0];
    r2[1] = p[1] - ls0[1];

    /*  my=sqrt(r1[0]*r1[0]+r1[1]*r1[1]);*/
    my = (r1[0] * r1[0] + r1[1] * r1[1]);

    /*Geradensegment entartet?*/
    if (my < 1e-15)
      {
        pf[0] = ls0[0];
        pf[1] = ls0[1];
      }
    else
      {
        my = (r1[0] * r2[0] + r1[1] * r2[1]) / my;

        /*Fusspunkt vor Anfangspunkt?*/
        if (my <= 0)
          {
            pf[0] = ls0[0];
            pf[1] = ls0[1];
          }
        else

          /*Fusspunkt hinter Endpunkt?*/
          if (my >= 1)
            {
              pf[0] = ls1[0];
              pf[1] = ls1[1];
            }
          else
            /*Fusspunkt zwischen Anfangs- und Endpunkt*/
            {
              pf[0] = ls0[0] + my * r1[0];
              pf[1] = ls0[1] + my * r1[1];
            }
      }

    return DistPointPoint(p, pf);
  }
#undef FNAME
}
