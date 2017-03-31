/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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

#include "fit.h"
#include "fit_pointlist.h"
#include "analygeo.h"
#include "vectorarrayfunctions.h"

namespace ice
{
#define FNAME "FitLine"
  int FitLine(PointList pl,
              int ad1, int ad2, int step,
              double* par, double* maxdis, int* maxadr)
  {
    Matrix m;
    RETURN_ERROR_IF_FAILED(PointList2Matrix(pl, ad1, ad2, m));
    RETURN_ERROR_IF_FAILED(FitLine(m, par, step));

    double cc = cos(par[1]);
    double ss = sin(par[1]);

    *maxdis = 0.0;
    *maxadr = 0;

    int i = ad1;

    do
      {
        double d = fabs(pl->xptr[i] * cc +
                        pl->yptr[i] * ss - par[0]);

        if (d > *maxdis)
          {
            *maxdis = d;
            *maxadr = i;
          }

        i = (i + 1) % pl->lng;

      }
    while (i != ad2);

    if (ss == 0.0)
      {
        return 1;
      }

    return OK;
  }
#undef FNAME
#define FNAME "FitLineSegment"
  Segment FitLineSegment(PointList pl, int ad1, int ad2, int step,
                         double* dmax, int* adr)
  {
    double p, phi, ss, cc, h, lmin, lmax, xm, ym;
    double par[2];
    int ad;
    double* xptr, *yptr;
    Segment sl;

    if ((ad2 - ad1) < 2)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return nullptr;
      }

    sl = (Segment)malloc(sizeof(struct Segment_));

    if (sl == nullptr)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return nullptr;
      }

    sl->prev = nullptr;
    sl->next = nullptr;

    IF_FAILED(FitLine(pl, ad1, ad2, step, par, dmax, adr))
    {
      free(sl);
      throw IceException(FNAME, "Kein Segment angepasst", WRONG_PARAM);
    }

    p = par[0];
    phi = par[1];
    cc = cos(phi);
    ss = sin(phi);
    xptr = pl->xptr + ad1;
    yptr = pl->yptr + ad1;
    ad = 0;
    lmin = 100000.0;
    lmax = -100000.0;

    for (ad = ad1; ad <= ad2; ad++)
      {
        h = *yptr * cc - *xptr * ss;

        if (h < lmin)
          {
            lmin = h;  /*minimaler Fusspunkt*/
          }

        if (h > lmax)
          {
            lmax = h;  /*maximaler Fusspunkt*/
          }

        xptr++;
        yptr++;
      }

    xm = cc * p;
    ym = ss * p;
    sl->typ = 1;                  /*Geradensegment*/
    sl->p0[0] = xm - lmin * ss;
    sl->p0[1] = ym + lmin * cc;
    sl->p1[0] = xm - lmax * ss;
    sl->p1[1] = ym + lmax * cc;
    sl->par[0] = p;
    sl->par[1] = phi;
    return sl;
  }
#undef FNAME

#define FNAME "FitCircle"
  int FitCircle(PointList pl, int ad1, int ad2, int step,
                double* par, double* maxdis, int* maxadr)
  /* Zielfunktion: A(xx+yy)+Bx+Cy+D=0 */
  {
    Matrix m;
    RETURN_ERROR_IF_FAILED(PointList2Matrix(pl, ad1, ad2, m));
    RETURN_ERROR_IF_FAILED(FitCircle(m, par, step));

    *maxdis = -HUGE_VAL;

    for (int i = ad1; i != ad2; i = (i + 1) % pl->lng)
      {
        double d[2];
        d[0] = pl->xptr[i];
        d[1] = pl->yptr[i];

        double h = DistPointCircle(d, par);

        if (h > *maxdis)
          {
            *maxdis = h;
            *maxadr = i;
          }
      }

    return OK;
  }
#undef FNAME
  /************************************************************************/
#define FNAME "FitCircleSegment"
  /*
    Es werden die Punkte der Punktliste "pl" (von Adresse "adr1" bis
    "adr2") durch einen Kreisbogen approximiert. Bei Angabe der Schritt-
    zahl "step">1 wird eine iterative Approximation (Wichtung der Ausreisser)
    realisiert. Auf "mdist" steht der groesste Abstand aller Listenelemente
    ("madr" Adresse des Listenelementes mit dem groessten Abstand) bereit.
  */

  Segment FitCircleSegment(PointList pl, int adr1, int adr2, int step,
                           double* mdist, int* madr)
  {
    double* ptr;
    double etamin, eta, eta1, eta2, eta10, eta20;
    int i, len;
    double centre[2], rad;
    double par[3];
    Segment segm;
    double eps = 1e-5;
    eta10 = 0;
    eta20 = 0;

    if (pl == nullptr)
      {
        throw IceException(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return nullptr;
      }

    if ((adr1 < 0) || (adr2 < 0) || (adr2 < adr1) || (adr2 > pl->lng - 1))
      {
        throw IceException(FNAME, M_WRONG_INDEX, WRONG_PARAM);
        return nullptr;
      }

    if ((adr2 - adr1) < 6)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return nullptr;
      }

    FitCircle(pl, adr1, adr2, step, par, mdist, madr);

    centre[0] = par[0];
    centre[1] = par[1];
    rad = par[2];

    len = adr2 - adr1 + 1;
    ptr = (double*)malloc(len * sizeof(double));

    if (ptr == nullptr)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return nullptr;
      }

    for (i = adr1; i <= adr2; i++)
      {
        eta = atan2(pl->yptr[i] - centre[1], pl->xptr[i] - centre[0]);
        eta = fmod(eta, M_PI * 2);

        if (eta < 0)
          {
            ptr[i - adr1] = eta + M_PI * 2;
          }
        else
          {
            ptr[i - adr1] = eta;
          }
      }

    SortVectorRn(ptr, len);

    etamin = HUGE_VAL;

    for (i = 0; i < len; i++)
      {
        eta1 = ptr[i];
        eta2 = ptr[(i + len - 1) % len];
        eta = eta2 - eta1;

        if (eta < 0)
          {
            eta += M_PI * 2;
          }

        if ((eta < etamin) && (eta > eps))
          {
            etamin = eta;
            eta10 = eta1;
            eta20 = eta2;
          }
      }

    if ((segm = (Segment)malloc(sizeof(struct Segment_))) == nullptr)
      {
        free(ptr);
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return nullptr;
      }

    segm->typ = 2; /* Kreis */
    segm->p0[0] = rad * cos(eta10);
    segm->p0[1] = rad * sin(eta10);
    segm->p1[0] = rad * cos(eta20);
    segm->p1[1] = rad * sin(eta20);
    segm->par[0] = centre[0];
    segm->par[1] = centre[1];
    segm->par[2] = rad;
    segm->par[3] = eta10;
    segm->par[4] = eta20;
    segm->prev = nullptr;
    segm->next = nullptr;

    free(ptr);
    return segm;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "FitEllipseSegment"
  /*
    Es werden die Punkte der Punktliste "pl" (von Adresse "adr1" bis
    "adr2") durch ein Ellipsenbogen approximiert. Bei Angabe der Schritt-
    zahl "step">1 wird eine iterative Approximation (Wichtung der Ausreisser)
    realisiert. Auf "mdist" steht der groesste Abstand aller Listenelemente
    ("madr" Adresse des Listenelementes mit dem groessten Abstand) bereit.

    rcode:  WRONG_PARAM
    NO_ELLIPSE
    NO_MEM
    INTERN_ERROR
  */
  Segment FitEllipseSegment(PointList pl, int adr1, int adr2, int step,
                            double* mdist, int* madr)
  {
    int code;
    double* ptr;
    double etamin, eta, eta1, eta2, eta10, eta20;
    int i, len;
    double centre[2], a, b, phi, cc, ss;
    double par[5], x, y;
    Segment segm;
    double eps = 1e-5;
    eta10 = 0;
    eta20 = 0;

    if (pl == nullptr)
      {
        throw IceException(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return nullptr;
      }

    if ((adr1 < 0) || (adr2 < 0) || (adr2 < adr1) || (adr2 > pl->lng - 1))
      {
        throw IceException(FNAME, M_WRONG_INDEX, WRONG_PARAM);
        return nullptr;
      }

    if ((adr2 - adr1) < 6)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
      }

    RETURN_ERROR_IF_FAILED(code = FitEllipse(pl, adr1, adr2, step, par, mdist, madr));

    centre[0] = par[0];
    centre[1] = par[1];
    a = par[2];
    b = par[3];
    phi = par[4];

    len = adr2 - adr1 + 1;
    ptr = (double*)malloc(len * sizeof(double));

    for (i = adr1; i <= adr2; i++)
      {
        eta = atan2(pl->yptr[i] - centre[1], pl->xptr[i] - centre[0]);
        eta -= phi;
        eta = fmod(eta, M_PI * 2);

        if (eta < 0)
          {
            ptr[i - adr1] = eta + M_PI * 2;
          }
        else
          {
            ptr[i - adr1] = eta;
          }
      }

    code = SortVectorRn(ptr, len);

    etamin = HUGE_VAL;

    for (i = 0; i < len; i++)
      {
        eta1 = ptr[i];
        eta2 = ptr[(i + len - 1) % len];
        eta = eta2 - eta1;

        if (eta < 0)
          {
            eta += M_PI * 2;
          }

        if ((eta < etamin) && (eta > eps))
          {
            etamin = eta;
            eta10 = eta1;
            eta20 = eta2;
          }
      }

    if ((segm = (Segment)malloc(sizeof(struct Segment_))) == nullptr)
      {
        free(ptr);
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return nullptr;
      }

    segm->typ = 3; /* Ellipse */
    /*Anfangs- und Endpunkt bestimmen*/
    cc = cos(phi);
    ss = sin(phi);
    eta = eta10 - phi;

    if (eta > M_PI * 3 / 2)
      {
        eta = atan(a / b * tan(eta)) + M_PI * 2;
      }
    else if (eta > M_PI / 2)
      {
        eta = atan(a / b * tan(eta)) + M_PI;
      }
    else
      {
        eta = atan(a / b * tan(eta));
      }

    x = a * cos(eta);
    y = b * sin(eta);
    segm->p0[0] = x * cc - y * ss + centre[0];
    segm->p0[1] = x * ss + y * cc + centre[1];
    eta = eta20 - phi;

    if (eta > M_PI * 3 / 2)
      {
        eta = atan(a / b * tan(eta)) + M_PI * 2;
      }
    else if (eta > M_PI / 2)
      {
        eta = atan(a / b * tan(eta)) + M_PI;
      }
    else
      {
        eta = atan(a / b * tan(eta));
      }

    x = a * cos(eta);
    y = b * sin(eta);
    segm->p1[0] = x * cc - y * ss + centre[0];
    segm->p1[1] = x * ss + y * cc + centre[1];
    /*Parameter übertragen*/
    segm->par[0] = centre[0];
    segm->par[1] = centre[1];
    segm->par[2] = a;
    segm->par[3] = b;
    segm->par[4] = phi;
    segm->par[5] = eta10;
    segm->par[6] = eta20;
    segm->prev = nullptr;
    segm->next = nullptr;
    free(ptr);
    return segm;
  }
#undef FNAME
  /*******************************************************************/
#define FNAME "FitEllipse"
  int FitEllipse(PointList pl, int ad1, int ad2, int step,
                 double* par, double* mdist, int* madr, int mode)
  {
    Matrix m;
    RETURN_ERROR_IF_FAILED(PointList2Matrix(pl, ad1, ad2, m));
    RETURN_ERROR_IF_FAILED(FitEllipse(m, par, step));

    if ((mdist != nullptr) && madr != nullptr)
      {
        /* Berechnung des Punktlisteneintrages mit dem groessten Abstand */
        *mdist = -HUGE_VAL;

        for (int i = ad1; i != ad2; i = (i + 1) % pl->lng)
          {
            double d[2];
            d[0] = pl->xptr[i];
            d[1] = pl->yptr[i];
            double h = DistPointEllipse(d, par);

            if (h > *mdist)
              {
                *mdist = h;
                *madr = i;
              }
          }
      }

    return OK;
  }
#undef FNAME
}
