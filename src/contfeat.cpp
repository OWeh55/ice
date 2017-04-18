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
/*************************************************************************/
/* Routinen zur Konturbearbeitung                                        */
/*************************************************************************/

#include <iostream>

#include <math.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "conturfunctions.h"
#include "contools.h"
#include "numbase.h"
#include "momente.h"

#include "contfeat.h"

namespace ice
{
  /************************************************/
#define FNAME "MarkContur"
  /* #define DEBUG */
  int MarkContur(const Contur& c, int val, const Image& img)
  {
    int xx, yy;
    int i;

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((val < 0) || val > img.maxval)
      throw IceException(FNAME, M_WRONG_VAL);

    xx = c.StartX();
    yy = c.StartY();

    img.setPixelClipped(xx, yy, val);

    for (i = 0; i < c.Number(); i++)
      {
        c.getDirection(i).move(xx, yy);

        img.setPixelClipped(xx, yy, val);
      }

    return OK;
  }
#undef FNAME
  /*********************************************************/
#define FNAME "FillRegion"
  int FillRegion(const Contur& c, int val, const Image& img)
  {
    int i, x;

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((val < 0) || val > img.maxval)
      throw IceException(FNAME, M_WRONG_VAL);

    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    IMatrix segm = ConturSegmentlist(c);

    for (i = 0; i < segm.rows(); i++)
      for (x = segm[i][1]; x <= segm[i][2]; x++)
        {
          img.setPixelClipped(x, segm[i][0], val);
        }

    return OK;
  }
#undef FNAME
  /*********************************************************/
#define FNAME "RegionPointList"
  PointList RegionPointList(const Contur& c)
  {
    int i, j;
    double* x, *y, *w;

    PointList pl;

    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    IMatrix segm = ConturSegmentlist(c);
    IMatrix points(0, 2);

    for (i = 0; i < segm.rows(); i++)
      for (j = segm[i][1]; j < segm[i][2]; j++)
        {
          points = points && IVector(j, segm[i][0]);
        }

    pl = (PointList)malloc(sizeof(struct PointList_));

    if (pl == NULL)
      throw IceException(FNAME, M_NO_MEM);

    pl->xptr = (double*)malloc(points.rows() * sizeof(double));
    pl->yptr = (double*)malloc(points.rows() * sizeof(double));
    pl->wptr = (double*)malloc(points.rows() * sizeof(double));
    pl->lng = points.rows();

    if ((pl->xptr == NULL) || (pl->yptr == NULL) || (pl->wptr == NULL))
      {
        free(pl);
        throw IceException(FNAME, M_NO_MEM);
      }

    x = pl->xptr;
    y = pl->yptr;
    w = pl->wptr;

    for (i = 0; i < points.rows(); i++)
      {
        x[i] = points[i][0];
        y[i] = points[i][1];
        w[i] = 1.0;
      }

    return pl;
  }
#undef FNAME
#define FNAME "RegionPointlist"
  IMatrix RegionPointlist(const Contur& c)
  {
    int i, j;
    IMatrix points(0, 2);

    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    IMatrix segm = ConturSegmentlist(c);

    for (i = 0; i < segm.rows(); i++)
      for (j = segm[i][1]; j < segm[i][2]; j++)
        {
          points.Append(IVector(j, segm[i][0]));
        }

    return points;
  }
#undef FNAME
  /*********************************************************/
#define FNAME "RoughnessContur"
  double RoughnessContur(const Contur& c, int s1)
  {
    //int fx[8]={1,1,0,-1,-1,-1,0,1};
    //    int fy[8]={0,1,1,1,0,-1,-1,-1};

    IPoint p = c.Start();
    IPoint p1 = p;

    double l1 = 0, l2 = 0;
    int i, z1;

    if (c.Number() < (2 * s1))
      {
        return 0.0;
      }

    z1 = s1;

    for (i = 0; i < c.Number(); i++)
      {
        c.getDirection(i).move(p);

        if ((c.DirCode(i) & 1) == 0)
          {
            l2++;
          }
        else
          {
            l2 += M_SQRT2;
          }

        z1--;

        if (z1 == 0)
          {
            z1 = s1;
            l1 += sqrt(Sqr(p.x - p1.x) + Sqr(p.y - p1.y));
            p1 = p;
          }
      }

    if (z1 < s1)
      {
        l1 += sqrt(Sqr(p.x - p1.x) + Sqr(p.y - p1.y));
      }

    return l2 / l1;
  }
#undef FNAME
  /*********************************************************/
#define FNAME "MomentRegion"
  int MomentRegion(const Contur& c, double mm[15], double s[2])
  {

    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    /* Erstellen der Segmentliste */
    IMatrix segm = ConturSegmentlist(c);

    for (int i = 0; i < 15; i++)
      {
        mm[i] = 0;
      }

    for (int i = 0; i < segm.rows(); i++)
      {
        double y = segm[i][0];;

        double y2 = y * y;
        double y3 = y2 * y;
        double y4 = y2 * y2;

        double m = segm[i][1];
        double m2 = m * m;
        double m3 = m2 * m;
        double m4 = m3 * m;
        double m5 = m4 * m;

        double n = segm[i][2];
        double n2 = n * n;
        double n3 = n2 * n;
        double n4 = n3 * n;
        double n5 = n4 * n;
#if 0
        double x0, x1, x2, x3, x4;
        x0 = n - m + 1;
        x1 = (1 - m + n) * (m + n) / 2;
        x2 = (-m + 3 * m2 - 2 * m3 + n + 3 * n2 + 2 * n3) / 6;
        x3 = (-m2 + 2 * m3 - m4 + n2 + 2 * n3 +  n4) / 4;
        x4 = m / 30 - m3 / 3 + m4 / 2 - m5 / 5 - n / 30 + n3 / 3 + n4 / 2 + n5 / 5;

        mm[i00] += x0;
        mm[i10] += x1;
        mm[i01] += x0 * y;
        mm[i02] += x0 * y2;
        mm[i03] += x0 * y3;
        mm[i04] += x0 * y4;
        mm[i11] += x1 * y;
        mm[i12] += x1 * y2;
        mm[i13] += x1 * y3;
        mm[i20] += x2;
        mm[i21] += x2 * y;
        mm[i22] += x2 * y2;
        mm[i30] += x3;
        mm[i31] += x3 * y;
        mm[i40] += x4;
#else
        // neue berechnung "quasi-flächenmomente"

        // double Y0 = 1;
        // double Y1 = y;

        double Y2 = y2 + 1.0 / 12.0;
        double Y3 = y3 + y / 4.0;
        double Y4 = y4 + 0.5 * y2 + 1.0 / 16.0 / 5.0;

        double X0 = n - m + 1;
        double X1 = 0.5 * (n + m) * (n - m + 1);
        double X2 = (4 * (n3 - m3) + 6 * (n2 + m2) + 3 * (n - m) + 1) / 12.0;
        double X3 = (2 * (n4 - m4) + 4 * (n3 + m3) + 3 * (n2 - m2) + m + n) / 8.0;
        double X4 = (16 * (n5 - m5) + 40 * (n4 + m4) + 40 * (n3 - m3) + 20 * (n2 + m2)
                     + 5 * (n - m) + 1) / 80.0;

        mm[i00] += X0 /* * Y0*/;

        mm[i10] += X1 /* * Y0*/;
        mm[i01] += X0 * y /* == Y1*/;

        mm[i20] += X2 /* *Y0 */;
        mm[i11] += X1 * y /* ==Y1 */;
        mm[i02] += X0 * Y2;

        mm[i30] += X3 /* *Y0 */;
        mm[i21] += X2 * y /* ==Y1 */;
        mm[i12] += X1 * Y2;
        mm[i03] += X0 * Y3;

        mm[i40] += X4  /* *Y0 */;
        mm[i31] += X3 * y /* ==Y1 */;
        mm[i22] += X2 * Y2;
        mm[i13] += X1 * Y3;
        mm[i04] += X0 * Y4;

#endif
      }

    if (fabs(mm[0]) > 1e-20)
      {
        s[0] = mm[1] / mm[0];
        s[1] = mm[2] / mm[0];
      }

    return OK;
  }

  IPoint ICentroid(const Contur& c)
  {
    int i;
    int x0, x1, y;
    int m10 = 0, m01 = 0, m00 = 0;

    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    /* Erstellen der Segmentliste */
    IMatrix segm = ConturSegmentlist(c);

    for (i = 0; i < segm.rows(); i++)
      {
        y = segm[i][0];;

        int m = segm[i][1];
        int n = segm[i][2];

        x0 = n - m + 1;
        x1 = (1 - m + n) * (m + n) / 2;

        m00 += x0;
        m10 += x1;
        m01 += x0 * y;
      }

    if (m00 == 0)
      {
        return IPoint();
      }

    return IPoint(m10 / m00, m01 / m00);
  }

  Point Centroid(const Contur& c)
  {
    int i;
    int x0, x1, y;
    int m10 = 0, m01 = 0, m00 = 0;

    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    /* Erstellen der Segmentliste */
    IMatrix segm = ConturSegmentlist(c);

    for (i = 0; i < segm.rows(); i++)
      {
        y = segm[i][0];;

        int m = segm[i][1];
        int n = segm[i][2];

        x0 = n - m + 1;
        x1 = (1 - m + n) * (m + n) / 2;

        m00 += x0;
        m10 += x1;
        m01 += x0 * y;
      }

    if (m00 == 0)
      {
        return Point();
      }

    return Point((double)m10 / (double)m00, (double)m01 / (double)m00);
  }

#undef FNAME
  /*********************************************************/
#define FNAME "FeatureContur"
// Berechnung von Flaeche, Konkavitaet, Formfaktor,
// Konturlaenge aus einer Kontur
  int FeatureContur(const Contur& c, double& length, double& area,
                    double& form, double& conv)
  {
    if (!c.isClosed())
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    if (c.Number() > 0)
      {
        int x = c.StartX();
        int y = c.StartY();

        int nGeradeSchritte = 0;
        int nSchraegeSchritte = 0;

        int ar = 0;

        int xmin = x;
        int xmax = xmin;

        int ymin = y;
        int ymax = ymin;

        int pmin = x + y;
        int pmax = pmin;

        int mmax = x - y;
        int mmin = mmax;

        for (int i = 0; i < c.Number(); i++)
          {
            Freeman rcode = c.getDirection(i);
            Freeman(rcode).move(x, y);

            UpdateLimits(xmin, xmax, x);
            UpdateLimits(ymin, ymax, y);
            UpdateLimits(pmin, pmax, x + y);
            UpdateLimits(mmin, mmax, x - y);

            if ((rcode.Int() & 1) == 0)
              {
                nGeradeSchritte++;
              }
            else
              {
                nSchraegeSchritte++;
              }

            switch (rcode.Int())
              {
              case 0:
                ar -= y;
                break;
              case 1:
                ar += x - y;
                break;
              case 2:
                ar += x;
                break;
              case 3:
                ar += x + y;
                break;
              case 4:
                ar += y;
                break;
              case 5:
                ar -= x - y;
                break;
              case 6:
                ar -= x;
                break;
              case 7:
                ar -= x + y;
                break;
              }
          }

        // Anwendung der Pick'schen Formel
        area = (ar + c.Number()) / 2.0;

        if (c.isHole())
          {
            area -= 1.0;
            length = c.Length() + 4 - 4 * M_SQRT2;
          }
        else
          {
            area += 1.0;
            length = c.Length() + 4;
          }

        conv = (double)(3 * nGeradeSchritte + 4 * nSchraegeSchritte) /
               (double)(2 * (xmax - xmin + ymax - ymin + pmax - pmin + mmax - mmin));

      }
    else
      {
        area = 1.0;
        length = 4.0;
        conv = 1.0;
      }

    form = length * length / area / 4.0 / M_PI;

    return OK;
  }
#undef FNAME
}
