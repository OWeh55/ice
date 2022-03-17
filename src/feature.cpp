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
/********************************************************************
    Funktionen zur Berechnung von Merkmalen
       Suesse 7/93:     ImageD MomentImg()
       Schubert 4/96:   InvarFivePoint2d(),InvarFivePoint3d()
*********************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stddef.h>  /* notwendig fuer Unix !!! */

#include "IceException.h"
#include "defs.h"
#include "macro.h"
#include "contools.h"
#include "contfeat.h"
#include "numbase.h"
#include "vectorarrayfunctions.h"

#include "feature.h"

namespace ice
{
#define EPS 1.0e-5

#define FNAME "InvarFivePoint2d"
  int InvarFivePoint2d(double p[5][2], double* i1, double* i2)
  {
    double m310, m320, m312, m410, m420, m412;
    m310 = (p[3][0] - p[1][0]) * (p[1][1] - p[0][1]) - (p[1][0] - p[0][0]) * (p[3][1] - p[1][1]);
    m320 = (p[3][0] - p[2][0]) * (p[2][1] - p[0][1]) - (p[2][0] - p[0][0]) * (p[3][1] - p[2][1]);
    m312 = (p[3][0] - p[1][0]) * (p[1][1] - p[2][1]) - (p[1][0] - p[2][0]) * (p[3][1] - p[1][1]);
    m410 = (p[4][0] - p[1][0]) * (p[1][1] - p[0][1]) - (p[1][0] - p[0][0]) * (p[4][1] - p[1][1]);
    m420 = (p[4][0] - p[2][0]) * (p[2][1] - p[0][1]) - (p[2][0] - p[0][0]) * (p[4][1] - p[2][1]);
    m412 = (p[4][0] - p[1][0]) * (p[1][1] - p[2][1]) - (p[1][0] - p[2][0]) * (p[4][1] - p[1][1]);

    if ((fabs(m310) < EPS) || (fabs(m320) < EPS) || (fabs(m312) < EPS) ||
        (fabs(m410) < EPS) || (fabs(m420) < EPS) || (fabs(m412) < EPS))
      {
        throw IceException(FNAME, M_NO_SOLUTION);
        *i1 = *i2 = 0;
        return (NO_SOLUTION);
      }

    *i1 = m320 * m410 / (m310 * m420);
    *i2 = m310 * m412 / (m312 * m410);
    return (OK);
  }
#undef FNAME
  /************************************************/
#define FNAME "InvarFivePoint3d"
  int InvarFivePoint3d(double p[5][3], double* i1, double* i2)
  {
    double m310, m320, m312, m410, m420, m412;
    double n[3];
    double v1[3], v2[3], v3[3];
    SubVec(p[3], p[1], v1);
    SubVec(p[1], p[0], v2);
    CrossProdVec(v1, v2, n);
    m310 = LengthVec(n);
    SubVec(p[3], p[2], v1);
    SubVec(p[2], p[0], v2);
    CrossProdVec(v1, v2, v3);

    if (ScalProdVec(v3, n) > 0)
      {
        m320 = LengthVec(v3);
      }
    else
      {
        m320 = -LengthVec(v3);
      }

    SubVec(p[3], p[1], v1);
    SubVec(p[1], p[2], v2);
    CrossProdVec(v1, v2, v3);

    if (ScalProdVec(v3, n) > 0)
      {
        m312 = LengthVec(v3);
      }
    else
      {
        m312 = -LengthVec(v3);
      }

    SubVec(p[4], p[1], v1);
    SubVec(p[1], p[0], v2);
    CrossProdVec(v1, v2, v3);

    if (ScalProdVec(v3, n) > 0)
      {
        m410 = LengthVec(v3);
      }
    else
      {
        m410 = -LengthVec(v3);
      }

    SubVec(p[4], p[2], v1);
    SubVec(p[2], p[0], v2);
    CrossProdVec(v1, v2, v3);

    if (ScalProdVec(v3, n) > 0)
      {
        m420 = LengthVec(v3);
      }
    else
      {
        m420 = -LengthVec(v3);
      }

    SubVec(p[4], p[1], v1);
    SubVec(p[1], p[2], v2);
    CrossProdVec(v1, v2, v3);

    if (ScalProdVec(v3, n) > 0)
      {
        m412 = LengthVec(v3);
      }
    else
      {
        m412 = -LengthVec(v3);
      }

    if ((fabs(m310) < EPS) || (fabs(m320) < EPS) || (fabs(m312) < EPS) ||
        (fabs(m410) < EPS) || (fabs(m420) < EPS) || (fabs(m412) < EPS))
      {
        throw IceException(FNAME, M_NO_SOLUTION);
        *i1 = *i2 = 0;
        return (NO_SOLUTION);
      }

    *i1 = m320 * m410 / (m310 * m420);
    *i2 = m310 * m412 / (m312 * m410);
    return (OK);
  }
#undef FNAME

  void mmask(int p, int n, double* mask)
  /* Eindimensionale Momentenmaske der Ordnung p und Ausdehnung -n bis +n  */
  {
    int i, j;
    double prod;

    for (i = -n; i <= n; i++)
      {
        prod = 1.0;

        for (j = 1; j <= p; j++)
          {
            prod *= (double) i;
          }

        mask[i + n] = prod;
      }
  }

  void mmask(int p, int n, double* mask);
#define FNAME "MomentImg"
  /*********************************************************************/
  ImageD MomentImg(Image quelle, int p, int q, int n, ImageD ziel)
  {
    double mask[1024];
    int nx, ny;
    int i, j, k;
    double sum;
    ImageD pic;

    if ((!IsImg(quelle)) || (!IsImgD(ziel)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (p < 0 || q < 0 || n <= 0 || ((n & 1) == 0))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    n = n / 2; // Umgebungsgröße -> Abstandswert
    nx = quelle.xsize;
    ny = quelle.ysize;
    pic = NewImgD(nx, ny, -1., +1.);
    mmask(p, n, mask);  /* Momentenmaske berechnen  */
    /* Momentenbild fuer x-Richtung */
    for (i = 0; i <= ny - 1; i++)
      {
        for (j = n; j <= nx - 1 - n; j++)
          {
            sum = 0;

            for (k = -n; k <= n; k++)
              {
                sum = sum + GetVal(quelle, j + k, i) * mask[k + n];
              }

            PutValD(pic, j, i, sum);
          }
      }

    mmask(q, n, mask);  /* Momentenmaske berechnen */
    /* Momentenbild fuer y-Richtung  */
    for (i = n; i <= nx - 1 - n; i++)
      {
        for (j = n; j <= ny - 1 - n; j++)
          {
            sum = 0;

            for (k = -n; k <= n; k++)
              {
                sum = sum + GetValD(pic, i, j + k) * mask[k + n];
              }

            PutValD(ziel, i, j, sum);
          }
      }
    return ziel;
  }
#undef FNAME
} // namespace ice
