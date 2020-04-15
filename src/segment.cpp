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
/* ***************************************************************************

   segfit1.c
   fit for lines
   (c) by FeB                                           nbe@mipool.uni-jena.de
   21.1.97

   Release: 03.06.1997 ttbb - Veraenderung der Auswahl der Kreissegmentrichtung
   (in Funktion determine_circle)

   - Neue Funktion DetermineSegment (macht die interne
   Funktion determine_segment oeffentlich und ent-
   scheidet nicht nach dem linaren Rosin-Signifikanz-
   Mass Segmentlaenge/max_Abstand, sondern nach dem
   absoluten Mass max_Abstand)

   - Neue Funktion SegmentPointList

   Release: 10.06.1997 ttbb  - compute_st_fi verbessert

   - Neuer Modus SPL_BIDIRECT fuer "CSegment"

   ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "IceException.h"
#include "defs.h"
#include "macro.h"

#include "fit.h"
#include "numbase.h"
#include "analygeo.h"
#include "contools.h"

#include "segment.h"

namespace ice
{
  /* ***************************************************************************

     segfit.h
     Fittingroutinen för SegmentPoinList (segment.h)
     (c) by FeB                                           nbe@mipool.uni-jena.de
     14.1.97

     ***************************************************************************/

  /* ***************************************************************************
     segfit1.c
     determine_line - berechnet Parameter eines Liniensegmentes för die Punkte
                      der Punkliste pl zwischen den Positionen pos_st & pos_fi */

  void determine_line(int pos_st, int pos_fi, PointList pl,
                      double* p, double* phi,
                      double* ratio, double* max_dev, int* pos_break);

  /* ***************************************************************************
     segfit2.c
     determine_circle - berechnet Parameter eines Kreissegmentes för die Punkte
                       der Punkliste pl zwischen den Positionen pos_st & pos_fi*/

  void determine_circle(int pos_st, int pos_fi, PointList pl,
                        double* radius, double* x_cent, double* y_cent,
                        double* psi1, double* psi2,
                        double* ratio, double* max_dev, int* pos_break);

  /* ***************************************************************************
     segfit3.c
     determine_ellipse - berechnet Parameter eines Ellipsensegmentes för die
                         Punkte der Punkliste pl zwischen den Positionen pos_st
                         und  pos_fi                                           */

  void determine_ellipse(int pos_st, int pos_fi, PointList pl,
                         double* major_axis, double* minor_axis,
                         double* x_cent, double* y_cent,
                         double* psi1, double* psi2, double* w,
                         double* ratio, double* max_dev, int* pos_break);

  /* end of headerfile "segfit.h" */

#define ELLIPSES 0
#define PARABOLA 1
#define HYPERBOLA 2

#define SMALL 1                                               /* type of arc */
#define BIG 2
#define CLOCKWISE 1                 /* direction of arc from start to finish */
#define ANTICLOCKWISE 2

#define LOWE_STOP 1
#define BLATT_STOP 2
#define REPLACED 3

#define MIN_DEV 0.1
#define MIN_LNG 6

  /* ***************************************************************************
     get_ratio -  Ermittelt Verhaeltnis  max. Abweichung / Laenge              */

  double
  get_ratio(double pa[2], double pb[2], double dev)
  {
    double lng;

    lng = sqrt(Sqr(pa[0] - pb[0]) + Sqr(pa[1] - pb[1]));

    if (lng == 0)
      {
        return (1e+37);
      }

    return (dev / lng);
  }

  /* ***************************************************************************
     compute_lbreak_pos -                                                      */

  int
  compute_lbreak_pos(double* p0, double* p1, int pa, int pb, PointList pl,
                     double* max_dev)
  {
    int i, pos_max;
    double dev, pi[2], pf[2];

    pos_max = 0;
    *max_dev = 0;

    for (i = pa; i <= pb; i++)
      {
        pi[0] = pl->xptr[i];
        pi[1] = pl->yptr[i];

        /*    dev = DistPointLine (pi, p, phi); */

        dev = ClosestPointLineSeg(pi, p0, p1, pf);

        dev = fabs(dev);

        if (dev > *max_dev)
          {
            *max_dev = dev;
            pos_max = i;
          }
      }

    return (pos_max);
  }

  /* ***************************************************************************
     determine_line - berechnet Parameter eines Liniensegmentes för die Punkte
                      der Punkliste pl zwischen den Positionen pos_st & pos_fi */
  void
  determine_line(int pos_st, int pos_fi, PointList pl,
                 double* p, double* phi,
                 double* ratio, double* max_dev, int* pos_break)
  {
    double p0[2], p1[2];
    int code;

    p0[0] = pl->xptr[pos_st];
    p0[1] = pl->yptr[pos_st];
    p1[0] = pl->xptr[pos_fi];
    p1[1] = pl->yptr[pos_fi];

    if ((p0[0] == p1[0]) && (p0[1] == p1[1]))      /* Punktliste geschlossen ? */
      {
        p1[0] = pl->xptr[pos_fi - 1];
        p1[1] = pl->yptr[pos_fi - 1];
      }

    code =  ConvPointHesse(p0, p1, p, phi);

    if (code != OK)
      {
        *pos_break = ((pos_st + pos_fi) / 2);
        *ratio = DBL_MAX;
        *max_dev = DBL_MAX;
        return;
      }

    *pos_break =  compute_lbreak_pos(p0, p1, pos_st, pos_fi, pl, max_dev);
    *ratio = get_ratio(p0, p1, *max_dev);

    if ((pos_fi - pos_st) < 2)
      {
        *ratio = DBL_MAX;  /* soll schlecht bewertet werden */
      }
  }

  /* ***************************************************************************

     segfit2.c
     fit for arcs
     (c) by FeB                                           nbe@mipool.uni-jena.de
     21.1.97

     ***************************************************************************/

  /* ***************************************************************************
     angle -                                                                   */

  double angle_a(double x1, double y1, double x2, double y2)
  {
    double w;

    w = atan2(y2 - y1, x2 - x1);
    w = fmod(w, M_PI * 2);

    if (w < 0)
      {
        w = w + M_PI * 2;
      }

    return (w);
  }

  /* ***************************************************************************
     compute_abreak_pos -                                                      */

  int
  compute_abreak_pos(int pa, int pb, PointList pl, double x_cent, double y_cent,
                     double radius, double* max_dev)
  {
    int i, pos_max;
    double dev;

    pos_max = 0;
    *max_dev = 0;

    for (i = pa; i < pb; i++)
      {
        dev = sqrt(Sqr(pl->xptr[i] - x_cent) + Sqr(pl->yptr[i] - y_cent)) - radius;
        dev = fabs(dev);

        if (dev > *max_dev)
          {
            *max_dev = dev;
            pos_max = i;
          }
      }

    return (pos_max);
  }

  /* ***************************************************************************
     compute_algt -                                                            */

  double
  compute_algt(PointList npl, int arc_size, double radius)
  {
    double t1, t2;
    double chord, angle;

    if (radius == 0)
      {
        return (0);
      }

    t1 = npl->xptr[0] - npl->xptr[npl->lng - 1];
    t1 = t1 * t1;
    t2 = npl->yptr[0] - npl->yptr[npl->lng - 1];
    t2 = t2 * t2;
    chord = sqrt(t1 + t2);
    t1 = (chord / 2.0) / radius;

    if (t1 > 1.0)
      {
        t1 = 1;
      }

    angle = asin(t1) * 2;

    if (arc_size == BIG)
      {
        angle = 2 * M_PI - angle;
      }

    return (angle * radius);
  }

  /* ***************************************************************************
     compute_error -                                                           */

  double
  compute_error(int y_val, PointList npl, double* radius)
  {
    int i;
    double error, t;

    error = 0;
    *radius = sqrt((y_val * y_val) + (npl->xptr[0] * npl->xptr[0]));

    for (i = 1; i < npl->lng - 1; i++)
      {
        t = *radius - sqrt((npl->xptr[i] * npl->xptr[i]) +
                           (y_val - npl->yptr[i]) * (y_val - npl->yptr[i]));

        if (t < 0)
          {
            t = -t;
          }

        error = error + t;
      }

    return (error);
  }

  /* ***************************************************************************
     gradient -                                                                */

  double
  gradient(int pos, int direction, PointList npl)
  {
    double error1, error2, temp;

    error1 = compute_error(pos - direction, npl, &temp);
    error2 = compute_error(pos + direction, npl, &temp);
    return (error1 - error2);
  }

  /* ***************************************************************************
     search -                                                                  */

  void
  search(int start_y, int finish_y, PointList npl, int direction, double* b_r,
         int* b_y)
  {
    int diff, middle;

    diff = abs(finish_y - start_y);

    if (diff > 1)
      {
        middle = (start_y + finish_y) / 2;

        if (gradient(middle, direction, npl) <= 0)
          {
            search(start_y, middle, npl, direction, b_r, b_y);
          }
        else
          {
            search(middle, finish_y, npl, direction, b_r, b_y);
          }
      }
    else
      {
        *b_y = start_y;
        compute_error(*b_y, npl, b_r);
      }
  }

// function only replaced from another position in this file:

  /* ***************************************************************************
     angel-                                                                    */

  double angle(double x1, double y1, double x2, double y2)
  {
    double w;

    w = atan2(y2 - y1, x2 - x1);
    w = fmod(w, M_PI * 2);

    if (w < 0)
      {
        w = w + M_PI * 2;
      }

    return (w);
  }

  /* ***************************************************************************
     compute_st_fi                                                             */

  void compute_st_fi(PointList pl, int st, int fi,
                     double x_cent, double y_cent,
                     double* psi1, double* psi2)
  {
    int i, rechts, links;
    double temp, a1, a2, a_min, a_max, a1t, a2t, da_max, da_min;

    links = 0;
    rechts = 0;

    *psi1 = angle(x_cent, y_cent, pl->xptr[st], pl->yptr[st]);
    a_min = a_max = a2 = *psi1;
    da_max = da_min = 0;

    for (i = st + 1; i <= fi; i++)
      {
        a1t = a1 = a2;
        a2t = a2 = angle(x_cent, y_cent, pl->xptr[i], pl->yptr[i]);

        if (a2t < a1t)
          {
            a2t += (M_PI + M_PI);
          }

        if (a2t - a1t < M_PI)
          {
            links++;
            da_max += a2t - a1t;
            da_min += a2t - a1t;
          }
        else
          {
            a2t = a2;

            if (a1t < a2t)
              {
                a1t += (M_PI + M_PI);
              }

            if (a1t - a2t < M_PI)
              {
                rechts++;
                da_max -= a1t - a2t;
                da_min -= a1t - a2t;
              }
          }

        if (da_max > 0)
          {
            da_max = 0;
            a_max = a2;
          }

        if (da_min < 0)
          {
            da_min = 0;
            a_min = a2;
          }
      }

    *psi2 = a2;

    if (links < rechts)
      {
        temp = *psi1;
        *psi1 = *psi2;
        *psi2 = temp;
      }

    if (*psi1 == *psi2)
      {
        *psi1 = 0;
        *psi2 = 2 * M_PI;
      }

    if (a_min != std::min(*psi1, *psi2) || a_max != std::max(*psi1, *psi2))
      {
        // voellig entarteter Fall
        // negative Werte dienen als Markierung fuer Entartungen
        temp = *psi1;
        *psi1 = -*psi2;
        *psi2 = -temp;
      }

  }

  /* ***************************************************************************
     determine_circle - berechnet Parameter eines Kreissegmentes fuer die Punkte
                       der Punkliste pl zwischen den Positionen pos_st & pos_fi*/
  void determine_circle(int pos_st, int pos_fi, PointList pl,
                        double* radius, double* x_cent, double* y_cent,
                        double* psi1, double* psi2,
                        double* ratio, double* max_dev, int* pos_break)
  {
    int i, direction, start_y, finish_y, best_y, arc_size;
    //  double error;
    double error_pos, error_neg, w_sin, w_cos;
    double xt, yt, x_off, y_off, winkel, sum, temp, arc_lng;
    PointList npl;

    *radius = 0;
    *x_cent = 0;
    *y_cent = 0;
    *psi1   = 0;
    *psi2   = 0;

    npl = NewPointList(pos_fi - pos_st + 1);

    if (npl == nullptr)
      {
        *pos_break = ((pos_st + pos_fi) / 2);
        *ratio = DBL_MAX;
        *max_dev = DBL_MAX;
        return;
      }

    for (i = 0; i < npl->lng; i++)
      {
        npl->xptr[i] = pl->xptr[pos_st + i];
        npl->yptr[i] = pl->yptr[pos_st + i];
        npl->wptr[i] = 0;
      }

    x_off = (pl->xptr[pos_st] + pl->xptr[pos_fi]) / 2;
    y_off = (pl->yptr[pos_st] + pl->yptr[pos_fi]) / 2;
    xt = pl->xptr[pos_fi] - pl->xptr[pos_st];
    yt = pl->yptr[pos_fi] - pl->yptr[pos_st];

    if (xt != 0)
      {
        winkel = -atan(yt / xt);
      }
    else
      {
        winkel = M_PI / 2.0;
      }

    w_sin = sin(winkel);
    w_cos = cos(winkel);

    for (i = 0; i < npl->lng; i++)
      {
        xt = npl->xptr[i] - x_off;
        yt = npl->yptr[i] - y_off;
        npl->xptr[i] = xt * w_cos - yt * w_sin;
        npl->yptr[i] = xt * w_sin + yt * w_cos;
      }

    // error = compute_error(0, npl, radius);
    error_pos = compute_error(-1, npl, radius);
    error_neg = compute_error(1, npl, radius);

    sum = 0;

// endvalue of i incremented by Baumbach

    for (i = 1; i < npl->lng; i++)
      {
        sum = sum + npl->yptr[i];
      }

    temp = error_pos - error_neg;

    if (temp < 0)
      {
        temp = -temp;
      }

    if (temp < 0.0000001)
      {
        if (sum < 0)
          {
            direction = 1;
          }
        else
          {
            direction = -1;
          }
      }
    else
      {
        if (error_pos < error_neg)
          {
            direction = -1;
          }
        else
          {
            direction = 1;
          }
      }

    if (((direction == 1) && (sum < 0)) || ((direction == -1) && (sum > 0)))
      {
        arc_size = SMALL;
        start_y = 0;

        if (direction == 1)
          {
            finish_y = 16384;
          }
        else
          {
            finish_y = -16384;
          }
      }
    else
      {
        arc_size = BIG;
        start_y = 0;
        finish_y = 0;

        for (i = 1; i < npl->lng - 1; i++)
          if (direction == 1)
            {
              if (npl->yptr[i] > finish_y)
                {
                  finish_y = (int)npl->yptr[i];
                }
            }
          else
            {
              if (npl->yptr[i] < finish_y)
                {
                  finish_y = (int)npl->yptr[i];
                }
            }
      }

    search(start_y, finish_y, npl, direction, radius, &best_y);

    // calculation of arc_dir has no sense more:

    /*
    if (arc_size == SMALL)
    {
      if (npl->xptr[0] < npl->xptr[npl->lng-1])
      {
        if (direction == -1)
          arc_dir = CLOCKWISE;
        else
          arc_dir = ANTICLOCKWISE;
      }
      else
      {
        if (direction == -1)
          arc_dir = ANTICLOCKWISE;
        else
          arc_dir = CLOCKWISE;
      }
    }
    else
    {
      if (npl->xptr[0] < npl->xptr[npl->lng-1])
      {
        if (direction == -1)
          arc_dir = ANTICLOCKWISE;
        else
          arc_dir = CLOCKWISE;
      }
      else
      {
        if (direction == -1)
          arc_dir = CLOCKWISE;
        else
          arc_dir = ANTICLOCKWISE;
      }
    }
    */

    *x_cent = 0.0;
    *y_cent = best_y;
    winkel = -winkel;
    w_sin = sin(winkel);
    w_cos = cos(winkel);
    xt = *x_cent;
    yt = *y_cent;
    *x_cent = (xt * w_cos - yt * w_sin) + x_off;
    *y_cent = (xt * w_sin + yt * w_cos) + y_off;

    arc_lng = compute_algt(npl, arc_size, *radius);

    *pos_break = compute_abreak_pos(pos_st, pos_fi, pl,
                                    *x_cent, *y_cent, *radius, max_dev);

    if (arc_lng != 0)
      {
        *ratio = *max_dev / arc_lng;
      }
    else
      {
        *ratio = DBL_MAX;
      }

    if (*radius > 1000)
      {
        *ratio = *ratio * (*radius / 1000);
      }

    // calculation of psi1 & psi2 replaced by the call to compute_st_fi:

    compute_st_fi(pl, pos_st, pos_fi, *x_cent, *y_cent, psi1, psi2);

    if (*psi1 < 0 || *psi2 < 0)
      // entartetes Verhaeltnis Punktliste zu Kreis
      // (neu in compute_st_fi mit Release 10.06.97 implementiert)
      {
        *ratio = DBL_MAX;
      }

    if (*ratio > DBL_MAX / 2 && *psi1 >= 0 && *psi2 >= 0)
      {
        temp = *psi1;
        *psi1 = -*psi2;
        *psi2 = -temp;
      }

    /*

    *psi1 = angle_a (*x_cent, *y_cent, pl->xptr[pos_st], pl->yptr[pos_st]);
    *psi2 = angle_a (*x_cent, *y_cent, pl->xptr[pos_fi], pl->yptr[pos_fi]);

    if ( ((arc_dir == CLOCKWISE) && (arc_size == SMALL)  ) ||
         ((arc_dir == ANTICLOCKWISE) && (arc_size == BIG))   )
    {
      temp = *psi1;
      *psi1 = *psi2;
      *psi2 = temp;
    }
    */

    FreePointList(npl);
  }

  /* ***************************************************************************

     segfit3.c
     fit for ellipses
     (c) by FeB                                           nbe@mipool.uni-jena.de
     21.1.97

     ***************************************************************************/

  /* ***************************************************************************
     distance                                                                  */

  double
  distance(double x1, double y1, double x2, double y2)
  {
    return (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)));
  }

  /* ***************************************************************************
     find_conic_type                                                           */

  int
  find_conic_type(double c1, double c2, double c3)
  {
    double fx;
    int temp = 0;

    fx = c1 * c3 - c2 * c2 / 4;

    if (fx > 0)
      {
        temp = ELLIPSES;
      }

    if (fx < 0)
      {
        temp = HYPERBOLA;
      }

    if (fx == 0)
      {
        temp = PARABOLA;
      }

    return (temp);
  }

  /* ***************************************************************************
     ludcmp                                                                    */

  void
  ludcmp(double a[6][6], int n, int indx[6], double* d, int* ok)
  {
    int i, imax = 0, j, k;
    double big, dum, sum, temp;
    double vv[6];

    *d = 1.0;

    for (i = 1; i <= n; i++)
      {
        big = 0.0;

        for (j = i; j <= n; j++)
          if ((temp = fabs(a[i][j])) > big)
            {
              big = temp;
            }

        if (big == 0.0)
          {
            *ok = false;
            return;
          }

        vv[i] = 1.0 / big;
      }

    for (j = 1; j <= n; j++)
      {
        for (i = 1; i < j; i++)
          {
            sum = a[i][j];

            for (k = 1; k < i; k++)
              {
                sum -= a[i][k] * a[k][j];
              }

            a[i][j] = sum;
          }

        big = 0.0;

        for (i = j; i <= n; i++)
          {
            sum = a[i][j];

            for (k = 1; k < j; k++)
              {
                sum -= a[i][k] * a[k][j];
              }

            a[i][j] = sum;

            if ((dum = vv[i] * fabs(sum)) >= big)
              {
                big = dum;
                imax = i;
              }
          }

        if (j != imax)
          {
            for (k = 1; k <= n; k++)
              {
                dum = a[imax][k];
                a[imax][k] = a[j][k];
                a[j][k] = dum;
              }

            *d = -(*d);
            vv[imax] = vv[j];
          }

        indx[j] = imax;

        if (a[j][j] == 0.0)
          {
            a[j][j] = 1.0e-20;
          }

        if (j != n)
          {
            dum = 1.0 / (a[j][j]);

            for (i = j + 1; i <= n ; i++)
              {
                a[i][j] *= dum;
              }
          }
      }
  }

  /* ***************************************************************************
     lubksb                                                                    */

  void
  lubksb(double a[6][6], int n, int indx[6], double b[6])
  {
    int i, ii = 0, ip, j;
    double sum;

    for (i = 1; i <= n; i++)
      {
        ip = indx[i];
        sum = b[ip];
        b[ip] = b[i];

        if (ii)
          for (j = ii; j <= i - 1; j++)
            {
              sum -= a[i][j] * b[j];
            }
        else if (sum)
          {
            ii = i;
          }

        b[i] = sum;
      }

    for (i = n; i >= 1; i--)
      {
        sum = b[i];

        for (j = i + 1; j <= n; j++)
          {
            sum -= a[i][j] * b[j];
          }

        if (a[i][i] != 0)
          {
            b[i] = sum / a[i][i];
          }
      }
  }

  /* ***************************************************************************
     run_lms_fitting                                                           */

  void
  run_lms_fitting(PointList npl, double m[7], double lambda, int* conic_type,
                  int* ok)
  {
    int j;
    double sx, sx2, sy, sy2, sxy, sx2y, sxy2;
    double sdx2y2xy, sdx2y2x, sdx2y2y, sdx2y2;
    double sy3, sxy3;
    double dx2y2, xy, x, y;
    double x2, y2, y3;
    double m1[6][6];
    double m2[6];
    int indx[6];
    double d;
    double snew1, snew2, snew3;

    sx = sx2 = sy = sy2 = sxy = sx2y = sxy2 = 0;
    sdx2y2xy = sdx2y2x = sdx2y2y = sdx2y2 = 0;
    sy3 = sxy3 = 0;
    snew1 = snew2 = snew3 = 0;

    for (j = 1; j <= npl->lng; j++)
      {
        x = npl->xptr[j - 1];
        y = npl->yptr[j - 1];
        x2 = x * x;
        y2 = y * y;
        y3 = y * y * y;
        xy = x * y;
        dx2y2 = x2 - y2;

        sx += x;
        sx2 += x2;
        sy += y;
        sy2 += y2;
        sy3 += y3;
        sxy += xy;
        sx2y += (x2 * y);
        sxy2 += (x * y2);
        sxy3 += (x * y3);

        sdx2y2xy += (dx2y2 * xy);
        sdx2y2x += (dx2y2 * x);
        sdx2y2y += (dx2y2 * y);
        sdx2y2 += dx2y2;

        snew1 += (dx2y2 * dx2y2) + 4 * npl->lng * lambda;
        snew2 += (x2 * y2) + 4 * npl->lng * lambda;
        snew3 += (dx2y2 * y2) - 2 * npl->lng * lambda;
      }

    m1[1][1] = snew1;
    m1[2][1] = sdx2y2xy;
    m1[3][1] = sdx2y2x;
    m1[4][1] = sdx2y2y;
    m1[5][1] = sdx2y2;
    m1[1][2] = sdx2y2xy;
    m1[2][2] = snew2;
    m1[3][2] = sx2y;
    m1[4][2] = sxy2;
    m1[5][2] = sxy;
    m1[1][3] = sdx2y2x;
    m1[2][3] = sx2y;
    m1[3][3] = sx2;
    m1[4][3] = sxy;
    m1[5][3] = sx;
    m1[1][4] = sdx2y2y;
    m1[2][4] = sxy2;
    m1[3][4] = sxy;
    m1[4][4] = sy2;
    m1[5][4] = sy;
    m1[1][5] = sdx2y2;
    m1[2][5] = sxy;
    m1[3][5] = sx;
    m1[4][5] = sy;
    m1[5][5] = npl->lng;

    m2[1] = -snew3;
    m2[2] = -sxy3;
    m2[3] = -sxy2;
    m2[4] = -sy3;
    m2[5] = -sy2;

    ludcmp(m1, 5, indx, &d, ok);

    if (*ok == false)
      {
        *conic_type = 4;
        return;
      }

    lubksb(m1, 5, indx, m2);
    m[1] = m2[1];
    m[2] = m2[2];
    m[3] = 1 - m2[1];
    m[4] = m2[3];
    m[5] = m2[4];
    m[6] = m2[5];
    *conic_type = find_conic_type(m[1], m[2], m[3]);
  }

  /* ***************************************************************************
     determine_ellipse_lms3                                                    */

  void
  determine_ellipse_lms3(PointList npl, double m[7], int* ok)
  {
    double little_lambda, big_lambda, mid_lambda;
    double lambda;
    int conic_type;

    lambda = 0;
    little_lambda = 0;
    big_lambda = 2;

    run_lms_fitting(npl, m, lambda, &conic_type, ok);

    if (conic_type != ELLIPSES)
      {
        while ((big_lambda - little_lambda) > 0.1)
          {
            mid_lambda = (big_lambda + little_lambda) / 2.0;
            lambda = mid_lambda;
            run_lms_fitting(npl, m, lambda, &conic_type, ok);

            if (conic_type == ELLIPSES)
              {
                big_lambda = mid_lambda;
              }
            else
              {
                little_lambda = mid_lambda;
              }
          }

        lambda = big_lambda;
        run_lms_fitting(npl, m, lambda, &conic_type, ok);
      }
  }

  /* ***************************************************************************
     compute_lgt -                                                             */

  double
  compute_elgt(PointList npl, double major_axis, double minor_axis)
  {
    double s_ang, f_ang, temp;
    double xold, yold, xnew, ynew;
    double step;
    double lgt;

    s_ang = angle(0.0, 0.0,
                  npl->xptr[0] / major_axis, npl->yptr[0] / minor_axis);
    f_ang = angle(0.0, 0.0,
                  npl->xptr[npl->lng - 1] / major_axis,
                  npl->yptr[npl->lng - 1] / minor_axis);

    if (f_ang <= s_ang)
      {
        f_ang += M_PI * 2.0;
      }

    lgt = 0.0;

    xold = major_axis * cos(s_ang);
    yold = minor_axis * sin(s_ang);
    step = (f_ang - s_ang) / 20.0;
    temp = s_ang;

    do
      {
        temp += step;
        xnew = major_axis * cos(temp);
        ynew = minor_axis * sin(temp);
        lgt = lgt + distance(xold, yold, xnew, ynew);
        xold = xnew;
        yold = ynew;
      }
    while (temp <= f_ang);

    return (lgt);
  }

  /* ***************************************************************************
     compute_minimum_diff -                                                   */

  double
  compute_minimum_diff(double x_cir, double y_cir,
                       double x1, double y1, double x2, double y2)
  {
    double angle1;
    double x_off, y_off, cosine, sine;
    double temp;
    double min1, min2;
    double deviation;

    angle1 = angle(x1, y1, x2, y2);
    cosine = cos(-angle1);
    sine = sin(-angle1);
    x_off = x1;
    y_off = y1;

    x1 = 0;
    y1 = 0;
    x2 = x2 - x_off;
    y2 = y2 - y_off;
    x_cir = x_cir - x_off;
    y_cir = y_cir - y_off;

    temp = x2 * cosine - y2 * sine;
    y2 = x2 * sine + y2 * cosine;
    x2 = temp;
    temp = x_cir * cosine - y_cir * sine;
    y_cir = x_cir * sine + y_cir * cosine;
    x_cir = temp;
    min1 = distance(x_cir, y_cir, x1, y1);
    min2 = distance(x_cir, y_cir, x2, y2);

    if (x_cir < x1)
      {
        deviation = min1;
      }
    else
      {
        if (x_cir > x2)
          {
            deviation = min2;
          }
        else
          {
            deviation = fabs(y_cir);
          }
      }

    return (deviation);
  }

  /* ***************************************************************************
     compute_break_pos -                                                       */

  int
  compute_ebreak_pos(PointList npl, double major_axis, double minor_axis,
                     double* max_dev)
  {
    int l2;
    double l1, step;
    double s_ang, f_ang;
    double x_cir, y_cir;
    double deviation;
    double min_dev;
    int min_pos = 0, max_pos = 0;

    *max_dev = 0.0;

    s_ang = angle(0.0, 0.0,
                  npl->xptr[0] / major_axis, npl->yptr[0] / minor_axis);
    f_ang = angle(0.0, 0.0,
                  npl->xptr[npl->lng - 1] / major_axis,
                  npl->yptr[npl->lng - 1] / minor_axis);

    if (f_ang <= s_ang)
      {
        f_ang += M_PI * 2.0;
      }

    step = (f_ang - s_ang) / 10.0;
    l1 = s_ang;

    do
      {
        min_dev = DBL_MAX;
        x_cir = cos(l1) * major_axis;
        y_cir = sin(l1) * minor_axis;
        l2 = 0;

        do
          {
            deviation = compute_minimum_diff(x_cir, y_cir,
                                             npl->xptr[l2], npl->yptr[l2],
                                             npl->xptr[l2 + 1], npl->yptr[l2 + 1]);

            if (deviation < min_dev)
              {
                min_dev = deviation;
                min_pos = l2;
              }

            l2++;
          }
        while (l2 < npl->lng - 1);

        if (min_dev > *max_dev)
          {
            *max_dev = min_dev;
            max_pos = min_pos;
          }

        l1 += step;
      }
    while (l1 <= f_ang);

    return (max_pos);
  }

  /* ***************************************************************************
     determine_parameters                                                      */

  void
  determine_parameters(double m[7], double* major_axis, double* minor_axis,
                       double* rot_angle)
  {
    double a, b, c, d, e, f;
    double ca, sa;
    double t, u, v, w;

    a = m[1];
    b = m[2];
    c = m[3];
    d = m[4];
    e = m[5];
    f = m[6];

    if (a == c)
      {
        *rot_angle = 0;
      }
    else
      {
        *rot_angle = atan(b / (a - c)) / 2.0;
      }

    ca = cos(*rot_angle);
    sa = sin(*rot_angle);
    t = a * (ca * ca) + b * ca * sa + c * (sa * sa);
    u = a * (sa * sa) - b * ca * sa + c * (ca * ca);
    v = d * ca + e * sa;
    w = -d * sa + e * ca;
    *major_axis = sqrt((((v * v) / (4 * t)) + ((w * w) / (4 * u)) - f) / t);
    *minor_axis = sqrt((((v * v) / (4 * t)) + ((w * w) / (4 * u)) - f) / u);
  }

  /* ***************************************************************************
     determine_ellipse - berechnet Parameter eines Ellipsensegmentes för die
                         Punkte der Punkliste pl zwischen den Positionen pos_st
                         und  pos_fi                                           */
  void
  determine_ellipse(int pos_st, int pos_fi, PointList pl,
                    double* major_axis, double* minor_axis,
                    double* x_cent, double* y_cent,
                    double* psi1, double* psi2, double* w,
                    double* ratio, double* max_dev, int* pos_break)
  {
    double m[7];
    double c_ang, xt, yt;
    double sine, cosine;
    double arc_lng;
    double temp;
    double x_off, y_off, x_org, y_org;
    double rot_angle;
    //    double xs, ys, xf, yf;
    double scale;
    int i;
    int ok;
    PointList npl;

    ok = true;
    scale = 0;
    x_org = 0;
    y_org = 0;
    *major_axis = 0;
    *minor_axis = 0;
    *x_cent = 0;
    *y_cent = 0;
    *psi1 = 0;
    *psi2 = 0;
    *w = 0;

    npl = NewPointList(pos_fi - pos_st + 1);

    if (npl == nullptr)
      {
        *pos_break = (pos_st + pos_fi) / 2 ;
        *max_dev = DBL_MAX;
        *ratio = DBL_MAX;
        return;
      }

    for (i = 0; i < npl->lng; i++)
      {
        npl->xptr[i] = pl->xptr[pos_st + i];
        npl->yptr[i] = pl->yptr[pos_st + i];
        npl->wptr[i] = 0;

        if (scale < npl->xptr[i])
          {
            scale = npl->xptr[i];
          }

        if (scale < npl->yptr[i])
          {
            scale = npl->yptr[i];
          }

        x_org = x_org + npl->xptr[i];
        y_org = y_org + npl->yptr[i];
      }

    x_org = x_org / npl->lng;
    y_org = y_org / npl->lng;

    /* x_org, y_org - Mittelwerte / Verschiebung in den Nullpunkt                */

    for (i = 0; i < npl->lng; i++)
      {
        npl->xptr[i] = (npl->xptr[i] - x_org) / scale;
        npl->yptr[i] = (npl->yptr[i] - y_org) / scale;
      }

    determine_ellipse_lms3(npl, m, &ok);

    if (ok == false)
      {
        *max_dev = DBL_MAX;
        *pos_break = (pos_st + pos_fi) / 2 ;
        *ratio = DBL_MAX;
        FreePointList(npl);
        return;
      }

    c_ang = angle(npl->xptr[0], npl->yptr[0],
                  npl->xptr[npl->lng - 1], npl->yptr[npl->lng - 1]);
    sine = sin(-c_ang);
    cosine = cos(-c_ang);

    x_off = npl->xptr[0];
    y_off = npl->yptr[0];

    for (i = 0; i < npl->lng; i++)
      {
        npl->xptr[i] = npl->xptr[i] - x_off;
        npl->yptr[i] = npl->yptr[i] - y_off;
      }

    for (i = 0; i < npl->lng; i++)
      {
        xt = npl->xptr[i];
        yt = npl->yptr[i];
        temp = xt * cosine - yt * sine;
        npl->xptr[i] = temp;
        temp = xt * sine + yt * cosine;
        npl->yptr[i] = temp;
      }

    *x_cent = (-m[4] * m[3] / 2.0 + m[5] * m[2] / 4.0) / (m[1] * m[3] - m[2] * m[2] / 4.0);
    *y_cent = (-m[1] * m[5] / 2.0 + m[4] * m[2] / 4.0) / (m[1] * m[3] - m[2] * m[2] / 4.0);

    determine_parameters(m, major_axis, minor_axis, &rot_angle);

    *major_axis = *major_axis * scale;
    *minor_axis = *minor_axis * scale;

    /* Entartung etwas eingrenzen ! */
    if (*major_axis < 1.0)
      {
        *major_axis = 1.0;
      }

    if (*minor_axis < 1.0)
      {
        *minor_axis = 1.0;
      }

    if (*major_axis > 10000)
      {
        *major_axis = 10000;
      }

    if (*minor_axis > 10000)
      {
        *minor_axis = 10000;
      }

    *x_cent = *x_cent * scale;
    *y_cent = *y_cent * scale;

    *x_cent = *x_cent + x_org;
    *y_cent = *y_cent + y_org;
    /*
    xs = pl->xptr[pos_st];
    ys = pl->yptr[pos_st];
    xf = pl->xptr[pos_fi];
    yf = pl->yptr[pos_fi];
    */
    for (i = 0; i < npl->lng; i++)
      {
        npl->xptr[i] = pl->xptr[pos_st + i];
        npl->yptr[i] = pl->yptr[pos_st + i];
        npl->wptr[i] = 0;
      }

    for (i = 0; i < npl->lng; i++)
      {
        npl->xptr[i] = npl->xptr[i] - *x_cent;
        npl->yptr[i] = npl->yptr[i] - *y_cent;
      }

    sine = sin(-rot_angle);
    cosine = cos(-rot_angle);

    for (i = 0; i < npl->lng; i++)
      {
        xt = npl->xptr[i];
        yt = npl->yptr[i];
        temp = xt * cosine - yt * sine;
        npl->xptr[i] = temp;
        temp = xt * sine + yt * cosine;
        npl->yptr[i] = temp;
      }

    compute_st_fi(pl, pos_st, pos_fi, *x_cent, *y_cent, psi1, psi2);

    arc_lng = compute_elgt(npl, *major_axis, *minor_axis);
    *w = rot_angle;
    *pos_break = compute_ebreak_pos(npl, *major_axis, *minor_axis, max_dev)
                 + pos_st;

    if ((*pos_break <= pos_st + 6) || (*pos_break >= pos_fi - 6))
      {
        *pos_break = (pos_st + pos_fi) / 2 ;
      }

    if (arc_lng == 0)
      {
        *ratio = DBL_MAX;
      }
    else
      {
        *ratio = *max_dev / arc_lng;
      }

    /* Entartung wird bestraft ! */
    if ((std::max((int)*major_axis, (int)*minor_axis) /
         std::min((int)*major_axis, (int)*minor_axis)) > 300)
      {
        *ratio = DBL_MAX;
      }

    if (*psi1 < 0 || *psi2 < 0)
      {
        *ratio = DBL_MAX;
      }

    if (*ratio > DBL_MAX / 2 && *psi1 >= 0 && *psi2 >= 0)
      {
        temp = *psi1;
        *psi1 = -*psi2;
        *psi2 = -temp;
      }

    FreePointList(npl);
  }

  /* ***************************************************************************

     segment.c
     Segmentierung von Punktlisten
     (c) by FeB                                           nbe@mipool.uni-jena.de
     21.1.97

     ***************************************************************************/

  struct Segmentt                          /* mit ICE-Typ Segment vergleichbar */
  {
    double p0[2], p1[2];                                /* Anfangs- & Endpunkt */
    int pa, pb;                                    /* deren Pos. in Punktliste */
    int typ;                               /* Segmenttyp : 1 - Geradensegment  */
    /* 2 - Kreissegment    */
    /* 3 - Ellipsensegment */
    double par[7];                                             /* Segmentwerte */
    /* Geradensegment  : p, phi                        */
    /* Kreissegment    : xm, ym, r, psi1, psi2         */
    /* Ellipsensegment : xm ,ym, a, b, phi, psi1, psi2 */
    double sig;                                             /* Signifikanzwert */
    int flag;                                                   /* Abbruchflag */
    Segmentt* prev, *next;                         /* Listenverkettung */
  };

  /* ***************************************************************************
     determine_segment - öbergibt Linien/Kreis/Ellipsen -  Parameter an das
                         Segment sg, je nach besten Signifikanzwert und mode   */

  void determine_segment(int pos_st, int pos_fi, PointList pl, int mode,
                         Segmentt* sg, double* ratio, double* max_dev, int* pos_break)
  {
    double l_ratio, a_ratio, e_ratio;
    double l_max_dev, a_max_dev, e_max_dev;
    int l_pos_break, a_pos_break, e_pos_break;
    double p, phi;
    double radius, major_axis, minor_axis, x_cent, y_cent, psi1, psi2, w;

    if ((mode == 0) && (pl->lng < 6))
      {
        mode = 1;
      }

    if ((mode == 1) && (pl->lng < 4))
      {
        mode = 2;
      }

    /* Parameter Liniensegment */
    determine_line(pos_st, pos_fi, pl,
                   &p, &phi,
                   &l_ratio, &l_max_dev, &l_pos_break);

    sg->p0[0] = pl->xptr[pos_st];                         /* Parameteröbergabe */
    sg->p0[1] = pl->yptr[pos_st];
    sg->p1[0] = pl->xptr[pos_fi];
    sg->p1[1] = pl->yptr[pos_fi];
    sg->typ = 1;
    sg->par[0] = p;
    sg->par[1] = phi;
    sg->par[2] = 0;
    sg->par[3] = 0;
    sg->par[4] = 0;
    sg->par[5] = 0;
    sg->par[6] = 0;
    sg->sig = l_ratio;
    sg->flag = 0;
    sg->pa = pos_st;
    sg->pb = pos_fi;
    *ratio = l_ratio;
    *max_dev = l_max_dev;
    *pos_break = l_pos_break;

    if (mode < 2)
      {
        /* Parameter Kreissegment */
        determine_circle(pos_st, pos_fi, pl,
                         &radius, &x_cent, &y_cent, &psi1, &psi2,
                         &a_ratio, &a_max_dev, &a_pos_break);

        if (a_ratio < *ratio)                /* Signifikanz des Kreises besser ? */
          {
            sg->typ = 2;                                      /* Parameteröbergabe */
            sg->par[0] = x_cent;
            sg->par[1] = y_cent;
            sg->par[2] = radius;
            sg->par[3] = psi1;
            sg->par[4] = psi2;
            sg->sig = a_ratio;
            *ratio = a_ratio;
            *max_dev = a_max_dev;
            *pos_break = a_pos_break;
          }

        if (mode == 0)
          {
            /* Parameter Ellipsensegment */

            determine_ellipse(pos_st, pos_fi, pl,
                              &major_axis, &minor_axis, &x_cent, &y_cent,
                              &psi1, &psi2, &w,
                              &e_ratio, &e_max_dev, &e_pos_break);

            if (e_ratio < *ratio)              /* Signifikanz der Ellipse besser ? */
              {
                sg->typ = 3;                                    /* Parameteröbergabe */
                sg->par[0] = x_cent;
                sg->par[1] = y_cent;
                sg->par[2] = major_axis;
                sg->par[3] = minor_axis;
                sg->par[4] = w;
                sg->par[5] = psi1;
                sg->par[6] = psi2;
                sg->sig = e_ratio;
                *ratio = e_ratio;
                *max_dev = e_max_dev;
                *pos_break = e_pos_break;
              }
          }
      }
  }

  /* ***************************************************************************
     add_seg - fuegt ein Segment hinten an die Segment-Liste sl an             */

  Segmentt* add_seg(Segmentt* sl, Segmentt* sg)
  {
    if (sl == nullptr)
      {
        /* SList noch leer */
        sl = (Segmentt*)malloc(sizeof(Segmentt));

        if (sl == nullptr)
          {
            return (nullptr);
          }

        sl->prev = nullptr;
        sl->next = nullptr;
      }
    else
      {
        sl->next = (Segmentt*)malloc(sizeof(Segmentt));

        if (sl->next == nullptr)
          {
            return (nullptr);
          }

        sl->next->prev = sl;                              /* (doppelt) Verketten */
        sl->next->next = nullptr;
        sl = sl->next;
      }

    if (sg == nullptr)
      {
        return (nullptr);
      }

    sl->p0[0] = sg->p0[0];
    sl->p0[1] = sg->p0[1];
    sl->p1[0] = sg->p1[0];
    sl->p1[1] = sg->p1[1];
    sl->typ = sg->typ;
    sl->par[0] = sg->par[0];
    sl->par[1] = sg->par[1];
    sl->par[2] = sg->par[2];
    sl->par[3] = sg->par[3];
    sl->par[4] = sg->par[4];
    sl->par[5] = sg->par[5];
    sl->par[6] = sg->par[6];
    sl->sig = sg->sig;
    sl->flag = 0;
    sl->pa = sg->pa;
    sl->pb = sg->pb;
    return (sl);
  }

  /* ***************************************************************************
     all_seg_gr - gibt False zuröck, falls die Anzahl der Punkte in der
                  Punktliste zwischen den Positionen pa und pi bzw. pi und pb
                  zu klein wird, ansonsten True                                */

  int all_seg_gr(int pa, int pb, int pi)
  {
    if (std::min(pi - pa, pb - pi) < MIN_LNG)
      {
        return (false);
      }
    else
      {
        return (true);
      }
  }

  /* ***************************************************************************
     m_tree - Baut rekursiv binà¥ren Baum aus dem Typ Segmentt                  */

  double m_tree(Segmentt* act, int pos_a, int pos_b, PointList pl, int mode)
  {
    int pos_break;
    double max, ratio, sig, sig1, sig2;

    determine_segment(pos_a, pos_b, pl, mode, act, &ratio, &max, &pos_break);
    sig = act->sig;

    if ((max >= MIN_DEV) && (all_seg_gr(pos_a, pos_b, pos_break) == true))
      {
        act->prev = (Segmentt*)malloc(sizeof(Segmentt));
        act->next = (Segmentt*)malloc(sizeof(Segmentt));

        if ((act->prev == nullptr) || (act->next == nullptr))
          {
            return (-1);
          }

        sig1 = m_tree(act->prev, pos_a, pos_break, pl, mode);
        sig2 = m_tree(act->next, pos_break, pos_b, pl, mode);

        if ((sig1 == -1) || (sig2 == -1))
          {
            return (-1);
          }

        if (sig < std::min(sig1, sig2))
          {
            /* setze Flag fuer Abbruch nach Lowe */
            act->flag = LOWE_STOP;
          }
        else
          {
            sig = std::min(sig1, sig2);
          }
      }
    else
      {
        /* Blatt */
        act->flag = BLATT_STOP;
        act->prev = nullptr;
        act->next = nullptr;
      }

    return (sig);
  }

  /* ***************************************************************************
     t_tree - Traversiert Baum und baut dabei Segmentliste sl auf              */

  Segmentt* t_tree(Segmentt* act, Segmentt* sl)
  {
    if (act->flag == BLATT_STOP)                                      /* Blatt */
      {
        sl = add_seg(sl, act);

        if (sl == nullptr)
          {
            return (nullptr);
          }
      }
    else if (act->flag == LOWE_STOP)                                /* Nach Lowe */
      {
        sl = add_seg(sl, act);

        if (sl == nullptr)
          {
            return (nullptr);
          }
      }
    else
      {
        /* rekursiver Abstieg */
        if (act->prev != nullptr)
          {
            sl = t_tree(act->prev, sl);
          }

        if (sl == nullptr)
          {
            return (nullptr);
          }

        if (act->next != nullptr)
          {
            sl = t_tree(act->next, sl);
          }

        if (sl == nullptr)
          {
            return (nullptr);
          }
      }

    return (sl);
  }

  /* ***************************************************************************
     f_tree - gibt Baum wieder frei                                            */

  void f_tree(Segmentt* act)
  {
    if (act->prev != nullptr)
      {
        f_tree(act->prev);
      }

    if (act->next != nullptr)
      {
        f_tree(act->next);
      }

    free(act);
  }

  /* ***************************************************************************
     improve - Datenreduktion auf der Segment-Liste                            */

  Segmentt* improve(Segmentt* sl, PointList pl, int mode)
  {
    Segmentt* sg1, *sg2, *sg3, *act1, *act2, *act3, *tmp;
    double sig[4], dummy1[4], sigp, sigc, sign;
    int dummy2[4], cont, sl_lng, replace, best[4], i, j;

    sl_lng = 1;

    while (sl->prev != nullptr)
      {
        sl = sl->prev;
        sl_lng++;
      }

    sg1 = sl;
    cont = true;

    while ((cont == true) && (sl_lng >= 3))
      {
        cont = false;
        sg1 = sl;                                       /* die ersten 3 Segmente */
        sg2 = sg1->next;
        sg3 = sg2->next;

        while (sg3 != nullptr)
          {
            sigp = sg1->sig;                        /* alte Signifikanzwerte  prev */
            sigc = sg2->sig;                                            /* current */
            sign = sg3->sig;                                               /* next */

            act1 = (Segmentt*)malloc(sizeof(Segmentt));
            act2 = (Segmentt*)malloc(sizeof(Segmentt));
            act3 = (Segmentt*)malloc(sizeof(Segmentt));

            if ((act1 == nullptr) || (act2 == nullptr) || (act3 == nullptr))
              {
                return (nullptr);
              }

            /* neue Signifikanzwerte fuer */
            /* current & prev */
            determine_segment(sg1->pa, sg2->pb, pl, mode,
                              act1, &sig[1], &dummy1[1], &dummy2[1]);
            /* current & next */
            determine_segment(sg2->pa, sg3->pb, pl, mode,
                              act2, &sig[2], &dummy1[2], &dummy2[2]);
            /* current & prev & next */
            determine_segment(sg1->pa, sg3->pb, pl, mode,
                              act3, &sig[3], &dummy1[3], &dummy2[3]);

            best[1] = 1;
            best[2] = 2;
            best[3] = 3;

            for (j = 1; j <= 2; j++)       /* Sortiert nach bestem Signifikanzwert */
              for (i = 1; i <= 2; i++)
                if (sig[i + 1] < sig[i])
                  {
                    sig[0] = sig[i];
                    sig[i] = sig[i + 1];
                    sig[i + 1] = sig[0];
                    best[0] = best[i];
                    best[i] = best[i + 1];
                    best[i + 1] = best[0];
                  }

            i = 0;
            replace = false;

            while ((replace == false) && (i < 3))
              {
                /* probiert der Reihe nach jede Methode aus */
                i++;

                if (best[i] == 1)                                       /* Methode 1 */
                  {
                    if ((sig[i] <= sigp) && (sig[i] <= sigc))
                      {
                        /* reduziere aktuelles & vorhergehendes Segment zu einem */
                        /* là¥«sche sg1 & sg2 , ersetze durch act1 */
                        replace = true;

                        if (sg1->prev != nullptr)
                          {
                            tmp = sg1->prev;
                            tmp->next = act1;
                            act1->prev = tmp;
                            act1->next = sg3;
                            sg3->prev = act1;
                            free(sg1);
                            free(sg2);
                            sg1 = tmp->next;
                          }
                        else
                          {
                            act1->next = sg3;
                            act1->prev = nullptr;
                            sg3->prev = act1;
                            free(sg1);
                            free(sg2);
                            sg1 = act1;
                            sl = act1;
                          }

                        act1 = nullptr;
                        sl_lng--;
                      }
                  }

                if (best[i] == 2)                                       /* Methode 2 */
                  {
                    if ((sig[i] <= sigc) && (sig[i] <= sign))
                      {
                        /* reduziere aktuelles & naechstes Segment zu einem */
                        /* là¥«sche sg2 & sg3 , ersetze durch act2 */
                        replace = true;

                        if (sg3->next != nullptr)
                          {
                            act2->next = sg3->next;
                            act2->next->prev = act2;
                          }
                        else
                          {
                            act2->next = nullptr;
                          }

                        sg1->next = act2;
                        act2->prev = sg1;
                        free(sg2);
                        free(sg3);
                        act2 = nullptr;
                        sl_lng--;
                      }
                  }

                if (best[i] == 3)                                       /* Methode 3 */
                  {
                    if ((sig[i] <= sigp) && (sig[i] <= sigc) && (sig[i] <= sign))
                      {
                        /* reduziere alle 3 Segmente zu einem */
                        /* là¥«sche sg1 & sg2 & sg3, ersetze durch act3 */
                        replace = true;

                        if (sg1->prev != nullptr)
                          {
                            tmp = sg1->prev;
                            tmp->next = act3;
                            act3->prev = tmp;

                            if (sg3->next != nullptr)
                              {
                                act3->next = sg3->next;
                                act3->next->prev = act3;
                              }
                            else
                              {
                                act3->next = nullptr;
                              }
                          }
                        else
                          {
                            act3->prev = nullptr;

                            if (sg3->next != nullptr)
                              {
                                act3->next = sg3->next;
                                act3->next->prev = act3;
                              }
                            else
                              {
                                act3->next = nullptr;
                              }

                            sl = act3;
                          }

                        free(sg1);
                        free(sg2);
                        free(sg3);
                        sg1 = act3;
                        act3 = nullptr;
                        sl_lng = sl_lng - 2;
                      }
                  }
              }

            if (replace == true)
              {
                cont = true;
              }

            if (act1 != nullptr)
              {
                free(act1);
              }

            if (act2 != nullptr)
              {
                free(act2);
              }

            if (act3 != nullptr)
              {
                free(act3);
              }

            sg2 = nullptr;
            sg3 = nullptr;

            if (sg1->next != nullptr)                       /* die naechsten Segmente */
              if (sg1->next->next != nullptr)
                if (sg1->next->next->next != nullptr)
                  {
                    sg1 = sg1->next;
                    sg2 = sg1->next;
                    sg3 = sg2->next;
                  }
          }
      }

    sl = sg1;

    while (sl->next != nullptr)
      {
        sl = sl->next;
      }

    return (sl);
  }

  /* ***************************************************************************
     changesl - wandelt die Segmentliste vom erweiterten Typ Segmentt in eine
                Segmentliste vom Typ Segment um                                */

  Segment changesl(Segmentt* sl)
  {
    Segmentt* tmp;
    Segment segl, seg;

    segl = nullptr;

    while (sl != nullptr)
      {
        seg = (Segment)malloc(sizeof(struct Segment_));         /* neues Segment */

        if (seg == nullptr)
          {
            return (nullptr);
          }

        seg->p0[0]  = sl->p0[0];                            /* Parameteröbergabe */
        seg->p0[1]  = sl->p0[1];
        seg->p1[0]  = sl->p1[0];
        seg->p1[1]  = sl->p1[1];
        seg->typ    = sl->typ;
        seg->par[0] = sl->par[0];
        seg->par[1] = sl->par[1];
        seg->par[2] = sl->par[2];
        seg->par[3] = sl->par[3];
        seg->par[4] = sl->par[4];
        seg->par[5] = sl->par[5];
        seg->par[6] = sl->par[6];
        seg->prev = nullptr;
        seg->next = nullptr;

        if (segl == nullptr)
          {
            segl = seg;  /* neue Segmentliste verketten */
          }
        else
          {
            segl->prev = seg;
            seg->next = segl;
            segl = seg;
          }

        tmp = sl;                                       /* alte Segmente là¥«schen */
        sl = sl->prev;
        free(tmp);
      }

    return (segl);
  }

  /* ***************************************************************************
     SegmentPointList - Segmentiert die Punkliste pl je nach mode in Linien -
                        Kreis & Ellipsensegmente.
                        mode : 0 - Linien & Kreise & Ellipsen (DEFAULT)
                               1 - Linien & Kreise
                               2 - Linien
                        Das zuröckgegebene Segment ist das erste einer
                        doppelt verketten Liste des Typs Segment.
                        Im Fehlerfall wird ein nullptr-Pointer zuröckgegeben.     */

#define FNAME   "SegmentPointList"

  Segment
  SegmentPointList(PointList pl, int mode)
  {
    Segmentt* tree, *sl;
    Segment segl;
    double code;

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    if ((mode < 0) || (mode > 2))
      throw IceException(FNAME, M_WRONG_MODE);

    if (pl->lng < 2)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    sl = nullptr;
    tree = (Segmentt*)malloc(sizeof(Segmentt));                      /* Wurzel */

    if (tree == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    code = m_tree(tree, 0, (pl->lng) - 1, pl, mode);          /* Baum aufbauen */

    if (code == -1)
      throw IceException(FNAME, M_NO_MEM);

    sl = t_tree(tree, sl);        /* Baum traversieren & Segmentliste aufbauen */

    f_tree(tree);                                              /* Baum là¥«schen */
    sl = improve(sl, pl, mode);                              /* Datenreduktion */

    segl = changesl(sl);                         /* in Segmentliste Typ à¥ndern */

    if (segl) while (segl->prev)
        {
          segl = segl->prev;
        }

    return (segl);
  }
#undef FNAME

  double compute_PointEllipseDist(double* pt, double* par)
  {

    double d[2];
    double phi, rad1, rad2;

    d[0] = pt[0] - par[0];
    d[1] = pt[1] - par[1];

    ConvCartesPolar(d, &rad1, &phi);

    if (par[2] > par[3])
      {
        rad2 = par[2] * par[3] / sqrt(Sqr(par[2]) - (Sqr(par[2]) - Sqr(par[3])) * Sqr(cos(phi - par[4])));
      }
    else
      {
        rad2 = par[2] * par[3] / sqrt(Sqr(par[3]) - (Sqr(par[3]) - Sqr(par[2])) * Sqr(cos(phi - par[4] + M_PI / 4)));
      }

    return fabs(rad1 - rad2);
  }

  /* ***************************************************************************
     DetermineSegment - Passt genau ein Segment an den Aussschnitt [pa,pe]
              der Punktliste pl an.

      Als Segmenttyp stehen
      type==1(DS_LINE)     -> Liniensegment
      type==2(DS_CIRCLE)   -> Kreissegment
      type==4(DS_ELLIPSE)  -> Ellipsensegment
      oder eine ODER-Verknuepfung dieser Typen zur Verfgung.
      Bei einer ODER-Verknuepfung wird in Abhaengigkeit vom
      maximalen Abstand der beste Segmenttyp ausgewaehlt.

      Rueckgabe: Segment-Parameter im ICE-Typ Segment
           (bzw. nullptr bei aufgetretenem Fehler)
           Auf max_dev steht der maximale Abstand und
           auf max_adr der zugehoerige Punkt zur Verfuegung.

      Normalerweise sind die Winkelangaben bei Kreis-/Ellipsensegment
      nicht-negative Werte. Sollten die Werte negativ sein, so
      bedeutet das, dass moeglicherweise ein entarteter Fall vorliegt.
      Alle Parameter wurden trotzdem, soweit moeglich, berechnet.
  */

#define FNAME   "DetermineSegment"

  Segment
  DetermineSegment(PointList pl, int pa, int pe,
                   unsigned int type,
                   Segment sg_liste,
                   int* max_adr, double* max_dev)
  {

    Segment sg;

    double l_ratio, a_ratio, e_ratio;
    double l_max_dev, a_max_dev, e_max_dev;
    int l_pos_break, a_pos_break, e_pos_break, ok = true;
    double p, phi;
    double radius, major_axis, minor_axis, x_cent, y_cent, psi1, psi2, w, temp;

    double par[7], ppa[2], ppe[2];

    // Parameterkontrolle

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    if ((type < 1) || (type > 7) || (pa < 0) || (pe >= pl->lng) || (pa > pe))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (pe == pa)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    // Fuer ein Ellipsensegment muessen mindestens 6 Punkte zur
    // Verfuegung stehen

    if (pe - pa < 5)
      {

        if (type == DS_ELLIPSE)
          throw IceException(FNAME, M_TOO_LESS_POINTS);

        type &= ~DS_ELLIPSE;
      }

    // Fuer ein Kreissegment muessen mindestens 4 Punkte zur
    // Verfuegung stehen

    if (pe - pa < 3)
      {

        if (type == DS_CIRCLE)
          throw IceException(FNAME, M_TOO_LESS_POINTS);

        type &= ~DS_CIRCLE;
      }

    // ICE-Segment anfordern

    sg = (Segment)malloc(sizeof(struct Segment_));

    if (sg == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    sg->typ = -1;

    // Veraenderte Kopie der Funktion determine_segment

    if (type & DS_LINE)
      {

        /* Parameter Liniensegment */
        determine_line(pa, pe, pl,
                       &p, &phi,
                       &l_ratio, &l_max_dev, &l_pos_break);

        sg->typ   = 1;
        sg->par[0] = p;
        sg->par[1] = phi;
        sg->par[2] = 0;
        sg->par[3] = 0;
        sg->par[4] = 0;
        sg->par[5] = 0;
        sg->par[6] = 0;
        *max_dev = l_max_dev;
        *max_adr = l_pos_break;
      }

    if (type & DS_CIRCLE)
      {

        /* Parameter Kreissegment */
        determine_circle(pa, pe, pl,
                         &radius, &x_cent, &y_cent, &psi1, &psi2,
                         &a_ratio, &a_max_dev, &a_pos_break);

        par[0] = x_cent;
        par[1] = y_cent;
        par[2] = radius;

        ppa[0] = pl->xptr[pa];
        ppa[1] = pl->yptr[pa];
        ppe[0] = pl->xptr[pe];
        ppe[1] = pl->yptr[pe];

        if (((ok = (DistPointCircle(ppa, par) < 1 && DistPointCircle(ppe, par) < 1)) &&
             a_max_dev < *max_dev && a_ratio < DBL_MAX / 2) ||
            (sg->typ == -1))
          // falls keine Linie angepasst oder Kreis besser
          {
            if (!ok && psi1 >= 0 && psi2 >= 0)
              {
                temp = psi1;
                psi1 = -psi2;
                psi2 = -temp;
              }

            sg->typ = 2;                            /* Parameteruebergabe */
            sg->par[0] = x_cent;
            sg->par[1] = y_cent;
            sg->par[2] = radius;
            sg->par[3] = psi1;
            sg->par[4] = psi2;
            *max_dev = a_max_dev;
            *max_adr = a_pos_break;
          }
      }

    if (type & DS_ELLIPSE)
      {

        /* Parameter Ellipsensegment */
        determine_ellipse(pa, pe, pl,
                          &major_axis, &minor_axis, &x_cent, &y_cent,
                          &psi1, &psi2, &w,
                          &e_ratio, &e_max_dev, &e_pos_break);

        par[0] = x_cent;
        par[1] = y_cent;
        par[2] = major_axis;
        par[3] = minor_axis;
        par[4] = w;
        ppa[0] = pl->xptr[pa];
        ppa[1] = pl->yptr[pa];
        ppe[0] = pl->xptr[pe];
        ppe[1] = pl->yptr[pe];

        if ((e_max_dev < *max_dev &&  e_ratio < DBL_MAX / 2 &&
             (ok = (compute_PointEllipseDist(ppa, par) < 2 && compute_PointEllipseDist(ppe, par) < 2))) ||
            (sg->typ == -1))
          // falls noch keine Linie/Kreis angepasst oder Ellipse besser
          {

            if (!ok && psi1 >= 0 && psi2 >= 0)
              {
                temp = psi1;
                psi1 = -psi2;
                psi2 = -temp;
              }

            sg->typ = 3;                          /* Parameteruebergabe */

            sg->par[0] = x_cent;
            sg->par[1] = y_cent;
            sg->par[2] = major_axis;
            sg->par[3] = minor_axis;
            sg->par[4] = w;
            sg->par[5] = psi1;
            sg->par[6] = psi2;
            *max_dev = e_max_dev;
            *max_adr = e_pos_break;
          }
      }

    sg->p0[0] = pl->xptr[pa];
    sg->p0[1] = pl->yptr[pa];
    sg->p1[0] = pl->xptr[pe];
    sg->p1[1] = pl->yptr[pe];

    if (sg_liste == nullptr)
      {
        sg->next = nullptr;
        sg->prev = nullptr;
      }
    else
      {

        while (sg_liste->next)
          {
            sg_liste = sg_liste->next;
          }

        sg_liste->next = sg;
        sg->next = nullptr;
        sg->prev = sg_liste;
      }

    return (sg);
  }
#undef FNAME

  /* ***************************************************************************
     SegmentPointList - Segmentiert eine Punktliste mit dem
              CSegment-Algortihmus (DIAS)

    Parameterliste/Rueckgabe analog SegmentPointList
    Zusaetzlich kann der maximal zulaessige Abstand vorgegeben werden.
    Eine ODER-Verknuepfung des mode-Wertes mit SPL_NOCLOSE verhindert
    den Test, ob bei geschlossenen Punktlisten Anfangs- und Endsegment
    zu einem Segment zusammengefasst werden koennen.

      Release 10.06.97 - Neuer Modus SPL_BIDIRECT

    Kann mit allem bisherigen ODER-verknuepft werden.
    Mit SPL_BIDIRECT erfolgt im Anschluss eine zweite Segmentierung
    beginnend beim Endpunkt zum Punktlistenanfang. Aus beiden so
    gewonnenen Segmentierungen werden die Segmentenden gemittelt.
  */

#define FNAME   "SegmentPointList"

#define MAXSTEP 7

  Segment
  SegmentPointList(PointList pl, unsigned int mode, double max_dev)
  {

    Segment   sg, sg_temp, sg_ok;

    int       start = 0, ende = 0, ende_ok = 0, max_adr = 0, stop = 0;
    int       first_end = 0, last_start = 0;
    int   i, c, step;
    double    s_max_dev, temp_max_dev;

    PointList pl_temp;

    // Parameterkontrolle

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    if (((mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) > 2) || max_dev <= 0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (pl->lng < 2)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    start = 0;
    sg = nullptr;

    do   // solange durchlaufen, bis gesamte Punktliste abgearbeitet ist
      {

        sg_temp = sg_ok = nullptr;
        ende = start + 1;

        if (ende < pl->lng)
          {

            last_start = start; // last_start enthaelt am Ende die Punktlistennummer des
            // Anfangspunkes des letzten Segmentes

            // Laengstes Liniensegment-Stueck beginnend bei start suchen

            step = MAXSTEP; // Schrittweite, in der die Punktliste durchmustert wird

            do   // solange, bis laengstes Liniensegment beginnend bei start gefunden
              {

                sg_temp = DetermineSegment(pl, start, ende, DS_LINE, nullptr, &max_adr, &s_max_dev);
                // berechne Liniensegment und max. Abstand

                if (sg_temp == nullptr)   // kein Liniensegment gefunden (gibt es im Normalfall nicht)
                  {
                    if (sg)
                      {
                        FreeSegmentList(sg);
                      }

                    if (sg_ok)
                      {
                        FreeSegmentList(sg_ok);
                      }

                    return (nullptr); // Fehlermeldung erfolgte in DetermineSegment
                  }

                if (max_dev >= s_max_dev)   // maximaler Abstand ist okay
                  {
                    if (sg_ok)
                      {
                        FreeSegmentList(sg_ok);  // vorher schon Liniensegment gefunden
                      }

                    sg_ok = sg_temp; // neues (laengeres) Liniensegment merken
                    ende_ok = ende; // zugehoerigen Endpunkt der Punktliste merken

                    if (step == -1)
                      {
                        break;  // Punktliste wurde rueckwaerts durchmustert
                      }

                    if (ende == pl->lng - 1)
                      {
                        break;  // gesamte Punktliste segmentiert
                      }

                    ende = std::min(ende + step, pl->lng - 1);
                  }
                else     // maximaler Abstand war zu gross
                  {
                    FreeSegmentList(sg_temp);
                    step = -1; // grobe Schrittweite beenden und in feinen Schritten rueckwaerts

                    if (ende == ende_ok + 1)
                      {
                        break;  // nichts besseres gefunden
                      }

                    ende--;
                  }

              }
            while (1);   // (Ende durch breaks)

          }

        if ((mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) < 2 && pl->lng - start > 5)   // Kreissegmente erwuenscht und noch genuegend Punkte da
          {
            // Laengstes Kreissegmentstueck suchen (falls laenger als Linie)

            ende = ende_ok + 1; // Kreissegment muss mehr Punkte als gefundenes Liniensegment enthalten

            if (ende - start < 4)
              {
                ende = start + 4;  // Kreissegment muss mindestens 5 Punkte enthalten
              }

            if (ende < pl->lng)
              {

                step = MAXSTEP; // Schrittweite fuer grobe Durchmusterung

                do   // solange, bis Punktliste zu Ende, oder laengstmoegliches Kreissegment gefunden
                  {
                    // berechne Kreissegment und maximalen Abstand
                    try
                      {
                        sg_temp = DetermineSegment(pl, start, ende, DS_CIRCLE, nullptr, &max_adr, &s_max_dev);
                      }
                    catch (IceException& ex)
                      {
                        break;  // Kreissegment konnte nicht berechnet werden
                      }

                    // (bei Fehler wird das Liniensegment genommen [besser als Abbruch?!])

                    if (max_dev >= s_max_dev && sg_temp->par[4] >= 0)   // maximaler Abstand ist okay
                      {
                        FreeSegmentList(sg_ok); // bisher gefundenes Segment loeschen
                        sg_ok = sg_temp; // neues (laengeres) Kreisegment merken
                        ende_ok = ende; // zugehoerigen Endpunkt der Liste merken

                        if (step == -1)
                          {
                            break;  // Punktliste wurde rueckwaerts durchmustert
                          }

                        if (ende == pl->lng - 1)
                          {
                            break;  // gesamte Punktliste segmentiert
                          }

                        ende = std::min(ende + step, pl->lng - 1);
                      }
                    else
                      {
                        FreeSegmentList(sg_temp);
                        step = -1; // grobe Schrittweite beenden, und fein rueckwaerts durchmustern

                        if (ende == ende_ok + 1)
                          {
                            break;  // nichts besseres gefunden
                          }

                        ende--;
                      }

                  }
                while (1);   // (Ende durch breaks)

              }

            if ((mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) == 0 && pl->lng - start > 6)   // Ellipsen erwuenscht und Punktliste genuegend Punkte
              {
                // Laengstes Ellipsensegment suchen

                ende = ende_ok + 1; // Ellipsensegment muss laenger, als bisher gefundenes Segment sein

                if (ende - start < 5)
                  {
                    ende = start + 5;  // Ellipsensegment muss mindestens 6 Punkte enthalten
                  }

                if (ende < pl->lng)
                  {

                    step = MAXSTEP; // Schrittweite fuer grobe Durchmusterung

                    do   // solange, bis Punktliste zuende, oder laengstmoegliches Segment gefunden
                      {

                        sg_temp = DetermineSegment(pl, start, ende, DS_ELLIPSE, nullptr, &max_adr, &s_max_dev);
                        // berechnet Ellipsensegment und max. Abstand

                        if (sg_temp == nullptr)
                          {
                            break;  // Ellipsensegment konnte nicht berechnet werden
                          }

                        if (max_dev >= s_max_dev && sg_temp->par[6] >= 0)   // maximaler Abstand okay
                          {
                            FreeSegmentList(sg_ok); // bisher gefundenes Segment loeschen
                            sg_ok = sg_temp; // neues Segment merken
                            ende_ok = ende; // zugehoerigen letzten Punktlistenpunkt merken

                            if (step == -1)
                              {
                                break;  // Rueckwaertsdurchmusterung beenden
                              }

                            if (ende == pl->lng - 1)
                              {
                                break;  // gesamte Punktliste segmentiert
                              }

                            ende = std::min(ende + step, pl->lng - 1);
                          }
                        else
                          {
                            FreeSegmentList(sg_temp);
                            step = -1; // grobe Schrittweite durch feine Rueckwaertsschritte ersetzen

                            if (ende == ende_ok + 1)
                              {
                                break;  // nichts besseres gefunden
                              }

                            ende--;
                          }

                      }
                    while (1);

                  }

              } // if (Ellipsensegment probieren)

          } // if (Kreissegment probieren)

        if (sg)   // gefundenes Segment an Segmentliste anhaengen
          {

            sg->next = sg_ok;
            sg_ok->prev = sg;
            sg = sg->next;
          }

        else   // gefundenes Segment als erstes Segment in Segmentliste uebernehmen
          {
            sg = sg_ok;
            first_end = ende_ok;
          }

        if (mode & SPL_BIDIRECT && (sg->prev || ende_ok < pl->lng - 1))
          {
            // im Bi-Direktionalen Modus beim ersten Durchlauf
            // in den Segmenten den zugehoerigen Punktlistenteil merken
            sg->p0[0] = start;
            sg->p0[1] = ende_ok;
          }

        start = ende_ok; // aktueller Endpunkt des Segmentes ist neuer Startpunkt

      }
    while (start < pl->lng - 1);

    if (mode & SPL_BIDIRECT && (sg->prev || sg->next))
      {
        // Bidirekt-Modus und mindestens zwei Segmente
        // -> Punktliste umdrehen und nochmal

        start = 0;

        do   // solange durchlaufen, bis gesamte Punktliste abgearbeitet ist
          {

            sg_temp = sg_ok = nullptr;

            ende = pl->lng - (int)(sg->p0[0]);
            stop = pl->lng - 1 - (int)((sg->prev->p0[0] + sg->prev->p0[1]) / 2.0);
            ende_ok = ende - 1;

            if (ende <= stop)
              {

                // Laengstes Liniensegment-Stueck beginnend bei start suchen

                step = MAXSTEP; // Schrittweite, in der die Punktliste durchmustert wird

                do   // solange, bis laengstes Liniensegment beginnend bei start gefunden
                  {

                    sg_temp = DetermineSegment(pl, pl->lng - 1 - ende, pl->lng - 1 - start, DS_LINE, nullptr, &max_adr, &s_max_dev);
                    // berechne Liniensegment und max. Abstand

                    if (sg_temp == nullptr)   // kein Liniensegment gefunden (gibt es im Normalfall nicht)
                      {
                        if (sg)
                          {
                            FreeSegmentList(sg);
                          }

                        return (nullptr); // Fehlermeldung erfolgte in DetermineSegment
                      }

                    if (max_dev >= s_max_dev)   // maximaler Abstand ist okay
                      {
                        ende_ok = ende; // zugehoerigen Endpunkt der Punktliste merken

                        if (step == -1)
                          {
                            break;  // Punktliste wurde rueckwaerts durchmustert
                          }

                        if (ende == stop)
                          {
                            break;  // gesamte Punktliste segmentiert
                          }

                        ende = std::min(ende + step, stop);
                      }
                    else     // maximaler Abstand war zu gross
                      {
                        step = -1; // grobe Schrittweite beenden und in feinen Schritten rueckwaerts

                        if (ende == ende_ok + 1)
                          {
                            break;  // nichts besseres gefunden
                          }

                        ende--;
                      }

                    FreeSegmentList(sg_temp);

                  }
                while (1);   // (Ende durch breaks)

              }

            if ((mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) < 2 && pl->lng - start > 5 && sg->typ > 1)
              // Kreissegmente erwuenscht und noch genuegend Punkte da
              // und Segment an dieser Stelle (aus anderer Richtung war keine Gerade)
              {
                // Laengstes Kreissegmentstueck suchen (falls laenger als Linie)

                ende = ende_ok + 1; // Kreissegment muss mehr Punkte als gefundenes Liniensegment enthalten

                if (ende - start < 4)
                  {
                    ende = start + 4;  // Kreissegment muss mindestens 5 Punkte enthalten
                  }

                if (ende <= stop)
                  {

                    step = MAXSTEP; // Schrittweite fuer grobe Durchmusterung

                    do   // solange, bis Punktliste zu Ende, oder laengstmoegliches Kreissegment gefunden
                      {

                        sg_temp = DetermineSegment(pl, pl->lng - 1 - ende, pl->lng - 1 - start, DS_CIRCLE, nullptr, &max_adr, &s_max_dev);
                        // berechne Kreissegment und maximalen Abstand

                        if (sg_temp == nullptr)
                          {
                            break;  // Kreissegment konnte nicht berechnet werden
                          }

                        // (bei Fehler wird das Liniensegment genommen [besser als Abbruch?!])

                        if (max_dev >= s_max_dev && sg_temp->par[4] >= 0)   // maximaler Abstand ist okay
                          {
                            ende_ok = ende; // zugehoerigen Endpunkt der Liste merken

                            if (step == -1)
                              {
                                break;  // Punktliste wurde rueckwaerts durchmustert
                              }

                            if (ende == stop)
                              {
                                break;  // gesamte Punktliste segmentiert
                              }

                            ende = std::min(ende + step, stop);
                          }
                        else
                          {
                            step = -1; // grobe Schrittweite beenden, und fein rueckwaerts durchmustern

                            if (ende == ende_ok + 1)
                              {
                                break;  // nichts besseres gefunden
                              }

                            ende--;
                          }

                        FreeSegmentList(sg_temp);

                      }
                    while (1);   // (Ende durch breaks)

                  }

                if ((mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) == 0 && pl->lng - start > 6 && sg->typ == 3)
                  // Ellipsen erwuenscht und Punktliste genuegend Punkte
                  // und Segment an dieser Stelle (von anderer Seite) war Ellipse
                  {
                    // Laengstes Ellipsensegment suchen

                    ende = ende_ok + 1; // Ellipsensegment muss laenger, als bisher gefundenes Segment sein

                    if (ende - start < 5)
                      {
                        ende = start + 5;  // Ellipsensegment muss mindestens 6 Punkte enthalten
                      }

                    if (ende <= stop)
                      {

                        step = MAXSTEP; // Schrittweite fuer grobe Durchmusterung

                        do   // solange, bis Punktliste zuende, oder laengstmoegliches Segment gefunden
                          {

                            sg_temp = DetermineSegment(pl, pl->lng - 1 - ende, pl->lng - 1 - start, DS_ELLIPSE, nullptr, &max_adr, &s_max_dev);
                            // berechnet Ellipsensegment und max. Abstand

                            if (sg_temp == nullptr)
                              {
                                break;  // Ellipsensegment konnte nicht berechnet werden
                              }

                            if (max_dev >= s_max_dev && sg_temp->par[6] >= 0)   // maximaler Abstand okay
                              {
                                ende_ok = ende; // zugehoerigen letzten Punktlistenpunkt merken

                                if (step == -1)
                                  {
                                    break;  // Rueckwaertsdurchmusterung beenden
                                  }

                                if (ende == stop)
                                  {
                                    break;  // gesamte Punktliste segmentiert
                                  }

                                ende = std::min(ende + step, stop);
                              }
                            else
                              {
                                step = -1; // grobe Schrittweite durch feine Rueckwaertsschritte ersetzen

                                if (ende == ende_ok + 1)
                                  {
                                    break;  // nichts besseres gefunden
                                  }

                                ende--;
                              }

                            FreeSegmentList(sg_temp);

                          }
                        while (1);

                      }

                  } // if (Ellipsensegment probieren)

              } // if (Kreissegment probieren)

            sg->p1[0] = pl->lng - 1 - ende_ok;

            if (!start)
              {
                sg->p1[1] = pl->lng - 1;
              }
            else
              {
                sg->p1[1] = sg->next->p1[0];
              }

            start = pl->lng - 1 - (int)((sg->p1[0] + sg->p0[0]) / 2.0 + 0.5);
            sg = sg->prev;

          }
        while (sg->prev);

        sg->p1[0] = 0;
        sg->p1[1] = sg->next->p1[0];

        do
          {
            start = (int)((sg->p0[0] + sg->p1[0]) / 2.0 + 0.5);
            ende = (int)((sg->p0[1] + sg->p1[1]) / 2.0 + 0.5);

            if (start != ende)
              {
                sg_ok = DetermineSegment(pl, start, ende, DS_LINE, nullptr, &max_adr, &s_max_dev);

                if (sg_ok == nullptr)
                  {
                    FreeSegmentList(sg);
                    return (nullptr); // Fehler wurde in DetermineSegment gemeldet
                  }

                temp_max_dev = s_max_dev;

                if (s_max_dev > max_dev && ende - start > 4 && (mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) < 2)
                  {
                    sg_temp = sg_ok;

                    sg_ok = DetermineSegment(pl, start, ende, DS_CIRCLE, nullptr, &max_adr, &s_max_dev);

                    if (sg_ok)
                      {
                        if (s_max_dev >= temp_max_dev || sg_ok->par[4] < 0)
                          {
                            FreeSegmentList(sg_ok);
                            sg_ok = sg_temp;
                          }
                        else
                          {
                            FreeSegmentList(sg_temp);
                            temp_max_dev = s_max_dev;
                          }
                      }
                    else
                      {
                        sg_ok = sg_temp;
                      }

                    if (temp_max_dev > max_dev && ende - start > 5 && (mode & ~(SPL_NOCLOSE | SPL_BIDIRECT)) == 0)
                      {
                        sg_temp = sg_ok;
                        sg_ok = DetermineSegment(pl, start, ende, DS_ELLIPSE, nullptr, &max_adr, &s_max_dev);
                        if (sg_ok)
                          {
                            if (s_max_dev >= temp_max_dev || sg_ok->par[6] < 0)
                              {
                                FreeSegmentList(sg_ok);
                                sg_ok = sg_temp;
                              }
                            else
                              {
                                FreeSegmentList(sg_temp);
                              }
                          }
                        else
                          {
                            sg_ok = sg_temp;
                          }
                      }
                  }

                sg_ok->next = sg->next;
                sg_ok->prev = sg->prev;
                last_start = start;

                if (!sg->prev)
                  {
                    first_end = ende;
                  }
                else
                  {
                    sg->prev->next = sg_ok;
                  }

                if (sg->next)
                  {
                    sg->next->prev = sg_ok;
                  }

                free(sg);
                sg = sg_ok;
                sg_temp = sg->next;

                if (sg->next)
                  {
                    sg = sg->next;
                  }

              }
            else     // Segment beseitigen
              {
                if (sg->prev)
                  {
                    sg->prev->next = sg->next;
                  }

                if (sg->next)
                  {
                    sg->next->prev = sg->prev;
                  }

                sg_temp = sg->prev;
                sg_ok = sg->next;
                free(sg);

                if (sg_ok)
                  {
                    sg = sg_ok;
                    sg_temp = (Segment)1;
                  }
                else
                  {
                    sg = sg_temp;
                    sg_temp = (Segment)0;
                  }
              }
          }
        while (sg_temp);
      }

    if ((mode & SPL_NOCLOSE) == 0 &&
        first_end != last_start &&
        sg->prev != nullptr &&
        pl->xptr[0] == pl->xptr[pl->lng - 1] && pl->yptr[0] == pl->yptr[pl->lng - 1])
      {
        // Erstes und letztes Segment bei geschlossener Punktliste
        // eventuell zusammenfassen

        // zugehoerige Punktlistenteile als neue Punktliste zusammenfassen

        pl_temp = NewPointList(first_end + (pl->lng - last_start));

        if (pl_temp == nullptr)
          {
            FreeSegmentList(sg);
            throw IceException(FNAME, M_NO_MEM);
          }

        c = 0;

        for (i = last_start; i < pl->lng; i++)
          {
            PutPoint(pl_temp, c++, pl->xptr[i], pl->yptr[i], 1);
          }

        for (i = 1; i <= first_end; i++)
          {
            PutPoint(pl_temp, c++, pl->xptr[i], pl->yptr[i], 1);
          }

        sg_temp = SegmentPointList(pl_temp, mode | SPL_NOCLOSE, max_dev);
        // SPL_NOCLOSE verhindert hier eine endlose Rekursion

        FreePointList(pl_temp);

        if (sg_temp)
          {
            // erstes Segment der "alten" Segmentierung loeschen
            while (sg->prev)
              {
                sg = sg->prev;
              }

            sg_ok = sg;
            sg = sg->next;
            sg->prev = nullptr;
            free(sg_ok);

            // letztes Segment der "alten" Segmentierung loeschen
            while (sg->next)
              {
                sg = sg->next;
              }

            sg_ok = sg;
            sg = sg->prev;
            free(sg_ok);

            // "neue" Teilsegmentierung anhaengen
            if (sg)
              {
                while (sg_temp->prev)
                  {
                    sg_temp = sg_temp->prev;
                  }

                sg->next = sg_temp;
                sg_temp->prev = sg;
              }
            else
              {
                sg = sg_temp;
              }
          }
      }

    if (sg)
      while (sg->prev)
        {
          sg = sg->prev;
        }

    return sg;
  }

#undef FNAME
}
