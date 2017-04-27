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

#include <float.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "root.h"
#include "momente.h"
#include "gentrans.h"
#include "matdef.h"
#include "lineqn.h"
#include "analygeo.h"
#include "trans.h"
#include "matrix_function.h"
#include "lmdif.h"

#include "affinfit.h"

namespace ice
{
  static int LM_polygon(const Vector& corners, Vector& result);

  /****************************************************************************/
  /* Berechnung des Abstandes zweier Polygone, indem zu Punkten des ersten    */
  /* Polygon der Abstand zum naechstliegenden Punkt des anderen Polygons      */
  /* bestimmt und der Mittelwert gebildet wird.                               */
  /****************************************************************************/
#define FNAME "DistPolygons"
  double DistPolygons(PointList pl1, PointList pl2, double tr[3][3])
  {
    double pm[2], pc1[2], pc2[2], pf[2], d, dmin, dmean, h;
    int i, j, k, diff, cnt = 20;

    if (pl1 == NULL || pl2 == NULL)
      throw IceException(FNAME, M_WRONG_PARAM);

    /* Abstand der Stuetzstellen */
    diff = pl1->lng / cnt;

    if (diff == 0)
      {
        diff = 1;
      }

    /* affine Transformation */
    if (tr[2][0] == 0 && tr[2][1] == 0)
      {
        for (k = 0, dmean = 0; k < pl1->lng; k += diff)
          {
            /* Punkt transformieren */
            pm[0] = tr[0][0] * pl1->xptr[k] + tr[0][1] * pl1->yptr[k] + tr[0][2];
            pm[1] = tr[1][0] * pl1->xptr[k] + tr[1][1] * pl1->yptr[k] + tr[1][2];
            pc1[0] = pl2->xptr[0];
            pc1[1] = pl2->yptr[1];

            /* jeweils naechstliegenden Punkt suchen */
            for (i = 0, dmin = 1e32, j = 1; i < pl2->lng; i++, j++)
              {
                if (j == pl2->lng)
                  {
                    j = 0;
                  }

                pc2[0] = pl2->xptr[j];
                pc2[1] = pl2->yptr[j];
                d = ClosestPointLineSeg(pm, pc1, pc2, pf);

                if (d < dmin)
                  {
                    dmin = d;
                  }

                pc1[0] = pc2[0];
                pc1[1] = pc2[1];
              }

            dmean += dmin;
          }
      }
    /* projektive Transformation */
    else
      {
        for (k = 0, dmean = 0; k < pl1->lng; k += diff)
          {
            /* Punkt transformieren */
            h = 1 / tr[2][0] * pl1->xptr[k] + tr[2][1] * pl1->yptr[k] + tr[2][2];
            pm[0] = tr[0][0] * pl1->xptr[k] + tr[0][1] + pl1->yptr[k] + tr[0][2] * h;
            pm[1] = tr[1][0] * pl1->xptr[k] + tr[1][1] * pl1->yptr[k] + tr[1][2] * h;
            pc1[0] = pl2->xptr[0];
            pc1[1] = pl2->yptr[1];

            /* jeweils naechstliegenden Punkt suchen */
            for (i = 0, dmin = DBL_MAX, j = 1; i < pl2->lng; i++, j++)
              {
                if (j == pl2->lng)
                  {
                    j = 0;
                  }

                pc2[0] = pl2->xptr[j];
                pc2[1] = pl2->yptr[j];
                d = ClosestPointLineSeg(pm, pc1, pc2, pf);

                if (d < dmin)
                  {
                    dmin = d;
                  }

                pc1[0] = pc2[0];
                pc1[1] = pc2[1];
              }

            dmean += dmin;
          }
      }

    return (dmean / cnt);
  }
#undef FNAME
  /*
  * Es wird eine affine Transformation zwischen zwei Polygonen mit Hilfe
  * von Flaechenmomenten und darauf basierenden Standardlagen angepasst
  */
#define FNAME "AffinFitPolygons"
  double AffinFitPolygons(PointList pl1, PointList pl2, double tr[3][3])
  {
    double m1[15], m2[15], mn1[15], mn2[15], mx[15], s1, s2, smin;
    double tr1[3][3], tr2[3][3], trx[8][3][3], tra[8][3][3];
    double pc0[2];
    int i, imin, j, k;

    if (pl1 == NULL || pl2 == NULL)
      throw IceException(FNAME, M_WRONG_PARAM);

    MomentPolygon(pl1, m1, pc0);
    MomentPolygon(pl2, m2, pc0);

    PosSign(m1);
    PosSign(m2);

    /* Flaeche darf nicht verschwinden */
    if (m1[0] < EPSILON || m2[0] < EPSILON)
      throw IceException(FNAME, M_NUM_INSTABILITY);

    /*Normierung der Momente auf vergleichbare Groessen */
    TranslateMoments(m1, -m1[1] / m1[0], -m1[2] / m1[0], mx);
    NormalizeMomentsArea(mx, mx);

    /* optimale Methode auswaehlen */
    for (i = 6, s1 = 0; i < 10; i++)
      {
        s1 += mx[i] * mx[i];
      }

    for (i = 10, s2 = 0; i < 15; i++)
      {
        s2 += mx[i] * mx[i];
      }

    s1 /= 4;
    s2 /= 5;

    if (s1 > s2)
      {
        /* 3. Momente groesser als 4. Momente --> Polynommethode */
        PolyNormMoments(m1, mn1, tr1);
        PolyNormMoments(m2, mn2, tr2);
      }
    else
      {
        /* Iterationsmethode ist besser */
        AffinIterateMoments(m1, mn1, tr1);
        AffinIterateMoments(m2, mn2, tr2);

        /* Sonderbehandlung fuer Dreiecke */
        if (fabs(mn1[6] + mn1[8]) + fabs(mn1[7] + mn1[9]) < 1e-5)
          {
            /*Dreieck --> Polynommethode*/
            PolyNormMoments(m1, mn1, tr1);
            PolyNormMoments(m2, mn2, tr2);
          }
      }

    InvertTrans(tr2);

    /* alle verbleibenden Moeglichkeiten testen */
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++)
        {
          trx[0][i][j] = tr2[i][j];
        }

    /*Drehungen um 90 Grad*/
    for (i = 0, j = 1; j < 4; i++, j++)
      {
        for (k = 0; k < 3; k++)
          {
            trx[j][k][0] = trx[i][k][1];
            trx[j][k][1] = -trx[i][k][0];
            trx[j][k][2] = trx[i][k][2];
          }
      }

    /*Spiegelung an der y-Achse*/
    for (i = 0; i < 3; i++)
      {
        trx[4][i][0] = -tr2[i][0];
        trx[4][i][1] = tr2[i][1];
        trx[4][i][2] = tr2[i][2];
      }

    /*Drehungen um 90 Grad*/
    for (i = 4, j = 5; j < 8; i++, j++)
      {
        for (k = 0; k < 3; k++)
          {
            trx[j][k][0] = trx[i][k][1];
            trx[j][k][1] = -trx[i][k][0];
            trx[j][k][2] = trx[i][k][2];
          }
      }

    /* Transformation Objekt1 - Objekt2 bestimmen und Abstandsmass fuer
       Punktlisten berechnen */

    smin = DBL_MAX;
    imin = 0;

    for (i = 0; i < 8; i++)
      {
        MulMatrix((double*)trx[i], (double*)tr1, 3, 3, 3, (double*)tra[i]);
        s1 = DistPolygons(pl1, pl2, tra[i]);

        if (s1 < smin)
          {
            smin = s1;
            imin = i;
          }
      }

    /* Rueckgabe der Transformation mit minimalem Abstand */
    for (i = 0; i < 3; i++)
      for (j = 0; j < 3; j++)
        {
          tr[i][j] = tra[imin][i][j];
        }

    return smin;
  }
#undef FNAME

  /***************************************************************************/
  /* Es wird an das Objekt mit den Momenten m ein Dreieck mit Hilfe          */
  /* von Flaechenmomenten und darauf basierenden Standardlagen angepasst     */
  /***************************************************************************/
#define FNAME "FitTriangle"
  int FitTriangleMoments(const double mp[15], double p[3][2])
  {
    double tr[3][3], mh[15];
    double mx[15];

    double d[3][2] =
    {
      { -1.3642616, 0.7876568},
      {  0.0000000, -1.5753136},
      {  1.3642616, 0.7876568}
    };

    int i;
    PosSign(mp, mx);

    if (mx[0] < EPSILON)
      throw IceException(FNAME, M_WRONG_OBJECT);

    PolyNormMoments(mx, mh, tr);
    InvertTrans(tr);

    for (i = 0; i < 3; i++)
      {
        p[i][0] = d[i][0] * tr[0][0] + d[i][1] * tr[0][1] + tr[0][2];
        p[i][1] = d[i][0] * tr[1][0] + d[i][1] * tr[1][1] + tr[1][2];
      }

    return OK;
  }
#undef FNAME

#define FNAME "FitParallelogramMoments"
  /**********************************************************************/
  int FitParallelogramMoments(const double mp[], double p[][2])
  {
    double guete;
    return FitParallelogramMoments(mp, p, guete);
  }

#undef FNAME

#define FNAME "FitParallelogramMoments"
  /**********************************************************************/
  int FitParallelogramMoments(const double mp[], double p[][2],
                              double& guetemass)
  {
    /*************************************************************/
    /* Fitting of a Parallelogram to an object                  */
    /* Affine Transformation using Iteration method             */
    /* Input: m[] 15 area moments of the object             */
    /* Output: p[][2] four corner points of the parallelogram   */
    /*************************************************************/
    double mx[15], maf[15];
    double h1;
    double tr[3][3], p1[2], p2[2];
    double mhor40, mhor22, mhor04;
    double mdia40, mdia22, mdia04;
    double a1, a2;

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      throw IceException(FNAME, M_WRONG_OBJECT);

    /*******************************************************/
    mhor40 = mhor04 = 0.519615;
    mhor22 = 0.288675;
    mdia40 = mdia04 = 0.692820;
    mdia22 = 0.115470;

    AffinIterateMoments(mx, maf, tr);
    InvertTrans(tr);

    a1  = (maf[10] - mhor40) * (maf[10] - mhor40);
    a1 += (maf[12] - mhor22) * (maf[12] - mhor22);
    a1 += (maf[14] - mhor04) * (maf[14] - mhor04);

    a2  = (maf[10] - mdia40) * (maf[10] - mdia40);
    a2 += (maf[12] - mdia22) * (maf[12] - mdia22);
    a2 += (maf[14] - mdia04) * (maf[14] - mdia04);

    if (a2 < a1)
      /* Standardlage= ein um 45 Grad gedrehtes Quadrat */
      {
        h1 = pow(3.0, 1.0 / 4.0);
        /***************************/
        p1[0] = 0;
        p1[1] = h1;
        TransPoint(p1, tr, p2);
        p[0][0] = p2[0];
        p[0][1] = p2[1];
        /***************************/
        p1[0] = h1;
        p1[1] = 0;
        TransPoint(p1, tr, p2);
        p[1][0] = p2[0];
        p[1][1] = p2[1];
        /*****************************/
        p1[0] = 0;
        p1[1] = -h1;
        TransPoint(p1, tr, p2);
        p[2][0] = p2[0];
        p[2][1] = p2[1];
        /*****************************/
        p1[0] = -h1;
        p1[1] = 0;
        TransPoint(p1, tr, p2);
        p[3][0] = p2[0];
        p[3][1] = p2[1];

        a2 += maf[6] * maf[6] + maf[7] * maf[7] + maf[8] * maf[8] +
              maf[9] * maf[9] + maf[11] * maf[11] + maf[13] * maf[13];

        guetemass = a2;
      }
    else
      /* Standardlage= Achsenparalleles Quadrat               */
      {
        h1 = pow(3.0 / 4.0, 1.0 / 4.0);
        /***************************/
        p1[0] = h1;
        p1[1] = h1;
        TransPoint(p1, tr, p2);
        p[0][0] = p2[0];
        p[0][1] = p2[1];
        /***************************/
        p1[0] = h1;
        p1[1] = -h1;
        TransPoint(p1, tr, p2);
        p[1][0] = p2[0];
        p[1][1] = p2[1];
        /*****************************/
        p1[0] = -h1;
        p1[1] = -h1;
        TransPoint(p1, tr, p2);
        p[2][0] = p2[0];
        p[2][1] = p2[1];
        /*****************************/
        p1[0] = -h1;
        p1[1] = h1;
        TransPoint(p1, tr, p2);
        p[3][0] = p2[0];
        p[3][1] = p2[1];

        a1 += maf[6] * maf[6] + maf[7] * maf[7] + maf[8] * maf[8] +
              maf[9] * maf[9] + maf[11] * maf[11] + maf[13] * maf[13];

        guetemass = a1;
      }

    return OK;
  }
#undef FNAME

#define FNAME "FitParallelogramMoments"
  int FitParallelogramMoments(const double m[], Matrix& mat)
  {
    double corner[4][2];
    RETURN_ERROR_IF_FAILED(FitParallelogramMoments(m, corner));
    mat = Matrix(4, 2);
    mat[0] = Vector(corner[0][0], corner[0][1]);
    mat[1] = Vector(corner[1][0], corner[1][1]);
    mat[2] = Vector(corner[2][0], corner[2][1]);
    mat[3] = Vector(corner[3][0], corner[3][1]);
    return OK;
  }
#undef FNAME

#define FNAME "FitParallelogramMoments"
  int FitParallelogramMoments(const double m[], Matrix& mat, double& guetemass)
  {
    double corner[4][2];
    RETURN_ERROR_IF_FAILED(FitParallelogramMoments(m, corner, guetemass));
    mat = Matrix(4, 2);
    mat[0] = Vector(corner[0][0], corner[0][1]);
    mat[1] = Vector(corner[1][0], corner[1][1]);
    mat[2] = Vector(corner[2][0], corner[2][1]);
    mat[3] = Vector(corner[3][0], corner[3][1]);
    return OK;
  }
#undef FNAME

#define FNAME "FitRectangleMoments"
  int FitRectangleMoments(const double mp[], double p[][2])
  {
    double guete;
    return FitRectangleMoments(mp, p, guete);
  }
#undef FNAME

#define FNAME "FitRectangleMoments"
  int FitRectangleMoments(const double mp[], double p[][2], double& guetemass)
  {
    /************************************************************/
    /* Fitting eines Rechteckes an ein Objekt                    */
    /* Verschiebung, Rotation, isotrope Skalierung               */
    /* Canonical frame wird anistrop skaliert und Ã¼ber dieses    */
    /* VerhÃ¤ltnis optimiert, da fÃ¼r Rechtecke keine Gruppe       */
    /* existiert                                                 */
    /* Input: m[] 15 area moments of the object                  */
    /* Output: p[][2] 4 corner points of the rectangle           */
    /*************************************************************/
    double sx, sy, b, b1_opt, b_opt, s[2];
    double min1, sum1;
    double quot;
    double mx[15];
    double mtrans[15];
    double mrot[15];
    double mtrop[15], mp_h[15];//, mp_opt_n[15];
    double mp_h_n[15];
    //    double sinphi, cosphi, tanphi, phi;
    double alpha, beta;
    double phi_korr = 0.0;
    double tr[3][3], tr_r[3][3], p1[2], p2[2];
    PointList pl;

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      throw IceException(FNAME, M_WRONG_OBJECT);

    sx = mx[1] / mx[0];
    sy = mx[2] / mx[0];

    TranslateMoments(mx, -sx, -sy, mtrans);

    // mtrans sind nun die zentralen Momente
    double phi = 0.0;
    if (fabs(mtrans[11]) <= 1.0E-13 ||
        fabs(mtrans[13]) <= 1.0E-13 ||
        fabs(mtrans[4]) < 1.0e-13)
      {
        phi = 0;
      }
    else
      {
        b = (mtrans[5] - mtrans[3]) / mtrans[4];
        double tanphi = -b / 2.0 + sqrt(b * b / 4.0 + 1.0);
        phi = atan(tanphi);
      }

    RotateMoments(mtrans, cos(phi), sin(phi), mrot);

    // Rotationsnormierung mit m11=0

    alpha = 1.0 / sqrt(mrot[0]);
    ScaleMoments(mrot, alpha, alpha, mtrop);
    // Skalierung auf m00=1

    quot = (mtrop[3] - mtrop[5]) * (mtrop[3] - mtrop[5]) + 4 * mtrop[4] * mtrop[4];
    quot /= ((mtrop[3] + mtrop[5]) * (mtrop[3] + mtrop[5]));

    // 0<=quot<=1 ist mass, ob quadratisches oder lÃ¤ngliches Objekt

    //    int k = 0;
    double min_m40_m04 = 1.0e+38;
    double max_m40_m04 = -1.0e+38;
    double sum;
    double phi1;
    double m_opt[15];

    for (int i = -450; i < 460; ++i)
      {
        phi1 = double(i) / 10.0 / 180.0 * M_PI;

        RotateMoments(mtrop, cos(phi1), sin(phi1), m_opt);
        sum = fabs(m_opt[10]) + fabs(m_opt[14]);

        if (sum < min_m40_m04)
          {
            min_m40_m04 = sum;
            phi_korr = phi1;
            // k = i;
          }

        if (sum > max_m40_m04)
          {
            max_m40_m04 = sum;
          }
      }

    // Winkel fÃ¼r Normierung m40+m04 = min wurde berechnet
    // quot<0.1: Dann wird damit normiert
    // quot>00.1: Dann bleibt die Normierung m11=0

    if (quot < 0.1)
      {
        RotateMoments(mtrop, cos(phi_korr), sin(phi_korr), mtrop);
      }
    else
      {
        phi_korr = 0.0;
      }

    /************************************************************************/
    /* Auf mtrop stehen die Momente des Objektes in Standardlage            */
    /* m00=1, m10=m01=0,m11=0 bzw. m40+m04 = minimum                        */
    /************************************************************************/
    /* Nun soll daran ein Rechteck in Standardlage angepasst werden         */
    /* WÃ¤hlen ein Rechteck in achsenparalleler Lage mit SeitenlÃ¤ngen 1 und b*/
    /* Dazu sind fÃ¼r ein b die normalisierten Momente zu berechnen          */
    /* und das b mit dem besten Fit an die normalisierte Lage des           */
    /* Objektes zu bestimmen                                                */
    /************************************************************************/
    //diskret = 'h';
    b = 0.0;
    min1 = 1.0E20;
    pl = NewPointList(4);
    b1_opt = 1.0;

    do
      {
        if (b > 1.5)
          {
            b += 0.01;
          }
        else
          {
            b += 0.001;
          }

        pl->xptr[0] = b / 2.0;
        pl->yptr[0] = 0.5;
        pl->xptr[1] = -b / 2.0;
        pl->yptr[1] = 0.5;
        pl->xptr[2] = -b / 2.0;
        pl->yptr[2] = -0.5;
        pl->xptr[3] = b / 2.0;
        pl->yptr[3] = -0.5;
        MomentPolygon(pl, mp_h, s);
        beta = 1.0 / sqrt(b);
        ScaleMoments(mp_h, beta, beta, mp_h_n);
        sum1 = 0.0;

        for (int i = 3; i < 15; ++i)
          {
            sum1 += (mtrop[i] - mp_h_n[i]) * (mtrop[i] - mp_h_n[i]);
          }

        sum1 = sqrt(sum1);

        if (sum1 < min1)
          {
            min1 = sum1;
            b1_opt = b;
            /*
                  for (i = 0; i < 15; ++i)
              mp_opt_n[i] = mp_h_n[i];
            */
          }
      }
    while (b <= 100.0);

    //Printf("GÃ¼te %f\n",min1);
    guetemass = min1;
    b_opt = b1_opt;

    /********************************************************************/
    InitTrans(tr);
    ShiftTrans(-sx, -sy, tr);
    RotTrans(0, 0, phi, tr);
    ScaleTrans(0, 0, alpha, alpha, tr);
    RotTrans(0, 0, phi_korr, tr);
    InvertTrans(tr);
    beta = 1.0 / sqrt(b_opt);
    InitTrans(tr_r);
    ScaleTrans(0, 0, beta, beta, tr_r);
    /****************************************************************************/
    p1[0] = b_opt / 2.0;
    p1[1] = 0.5;
    TransPoint(p1, tr_r, p2);
    TransPoint(p2, tr, p1);
    p[0][0] = p1[0];
    p[0][1] = p1[1];
    /***************************/
    p1[0] = -b_opt / 2.0;
    p1[1] = 0.5;
    TransPoint(p1, tr_r, p2);
    TransPoint(p2, tr, p1);
    p[1][0] = p1[0];
    p[1][1] = p1[1];
    /*****************************/
    p1[0] = -b_opt / 2.0;
    p1[1] = -0.5;
    TransPoint(p1, tr_r, p2);
    TransPoint(p2, tr, p1);
    p[2][0] = p1[0];
    p[2][1] = p1[1];
    /*****************************/
    p1[0] = b_opt / 2.0;
    p1[1] = -0.5;
    TransPoint(p1, tr_r, p2);
    TransPoint(p2, tr, p1);
    p[3][0] = p1[0];
    p[3][1] = p1[1];
    FreePointList(pl);
    return OK;
  }
#undef FNAME

#define FNAME "FitEquilateraltriangleMoments"
  int FitEquilateraltriangleMoments(const double mp[], double p[][2])
  {
    double guete;
    return FitEquilateraltriangleMoments(mp, p, guete);
  }
#undef FNAME

#define FNAME "FitEquilateraltriangleMoments"
  int FitEquilateraltriangleMoments(const double mp[], double p[][2], double& guetemass)
  {
    /************************************************************/
    /* Fitting eines gleichseitigen Dreiecks an ein Objekt                    */
    /* Verschiebung, Rotation, isotrope Skalierung               */
    /* Es gibt vier verschiedene Rotationslagen, eine           */
    /* muß durch Zusatzmomente eindeutig ausgewaehlt werden   */
    /* Input: m[] 15 area moments of the object                  */
    /* Output: p[][2] 3 corner points of the equilateral triangle           */
    /*************************************************************/
    double sx, sy;
    double mx[15];
    double mtrans[15];
    double mrot[15];
    double mtrop[15];
    //    double sinphi, cosphi, tanphi, phi;
    double alpha;
    double tr[3][3], p1[2], p2[2];
    //    int isx, isy;

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      throw IceException(FNAME, M_WRONG_OBJECT);

    //
    // *****************************************************
    // Canonical Frame fuer das equialateral triangle
    double cx1 = 0.0;
    double cy1 = (2.0 / 3.0) * sqrt(3.0 / sqrt(3.0));
    double cx2 = -1.0 / sqrt(sqrt(3.0));
    double cy2 = -1.0 / sqrt(3.0 * sqrt(3.0));
    double cx3 = 1.0 / sqrt(sqrt(3.0));
    double cy3 = -1.0 / sqrt(3.0 * sqrt(3.0));
    // End canonical frame
    double m_can[15];
    double centroid_can[2];

    PointList pl = NewPointList(3);
    PutPoint(pl, 0, cx1, cy1);
    PutPoint(pl, 1, cx2, cy2);
    PutPoint(pl, 2, cx3, cy3);
    MomentPolygon(pl, m_can, centroid_can);
    //  for (int i=0;i<15;i++)
    //  Printf(" %lf \n",m_can[i]);
    // Das waren die 3 Eckpunkte des canonical frame
    //
    sx = mx[1] / mx[0];
    sy = mx[2] / mx[0];
    //isx = (int)sx;
    //isy = (int)sy;
    TranslateMoments(mx, -sx, -sy, mtrans);

    double phi = 0;

    // mtrans sind nun die zentralen Momente
    // **********************************************************************
    if (fabs(mtrans[11]) <= 1.0E-13 ||
        fabs(mtrans[13]) <= 1.0E-13 ||
        fabs(mtrans[4]) < 1.0e-13)
      {
        phi = 0;
      }
    else
      {
        //b=(mtrans[5]-mtrans[3])/mtrans[4];
        //tanphi=-b/2.0+sqrt(b*b/4.0+1.0);
        double tanphi = (2.0 * mtrans[4]) / (mtrans[5] - mtrans[3]);
        phi = atan(tanphi) / 2.0;
        //norm=sqrt(tanphi*tanphi+1.0);
        //sinphi=tanphi/norm;
        //cosphi=1.0/norm;
      }

    RotateMoments(mtrans, cos(phi), sin(phi), mrot);

    alpha = 1.0 / sqrt(mrot[0]);
    ScaleMoments(mrot, alpha, alpha, mtrop);

    // Skalierung auf m00=1
    // *********************************************************************

    for (int i = 0; i < 15; i++)
      {
        mrot[i] = mtrans[i] = mtrop[i];
      }

    double min = +1.0e+10;
    double min_h, phi_opt = 0.0;
    double phi_end = 2 * M_PI / 3.0;
    double phi_h = 0.0;

    do
      {
        RotateMoments(mtrans, cos(phi_h), sin(phi_h), mrot);
        min_h = 0.0;

        for (int i = 0; i < 15; i++)
          {
            min_h = min_h + (mrot[i] - m_can[i]) * (mrot[i] - m_can[i]);
          }

        if (min_h < min)
          {
            min = min_h;
            phi_opt = phi_h;
          }

        //printf("%lf %lf %lf\n",mrot[4],min,phi_opt);
        phi_h = phi_h + 0.01;
      }
    while (phi_h < phi_end);

    guetemass = sqrt(min);

    //printf("%lf \n",sqrt(min));
    //printf("\n");
    //  for (int i=0;i<15;i++)
    //  Printf(" %lf\n",mrot[i]);

    //RotateMoments(mtrans,cos(phi_opt),sin(phi_opt),mrot);
    //for (int i=0;i<15;i++)
    //  Printf(" %lf  %lf\n",m_can[i],mrot[i]);

    // Ab jetzt zurueck-transformieren !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /********************************************************************/
    InitTrans(tr);
    ShiftTrans(-sx, -sy, tr);
    RotTrans(0, 0, phi + phi_opt, tr);
    ScaleTrans(0, 0, alpha, alpha, tr);
    InvertTrans(tr);

    /****************************************************************************/
    p2[0] = cx1;
    p2[1] = cy1;
    TransPoint(p2, tr, p1);
    p[0][0] = p1[0];
    p[0][1] = p1[1];
    /***************************/
    p2[0] = cx2;
    p2[1] = cy2;
    TransPoint(p2, tr, p1);
    p[1][0] = p1[0];
    p[1][1] = p1[1];
    /*****************************/
    p2[0] = cx3;
    p2[1] = cy3;
    TransPoint(p2, tr, p1);
    p[2][0] = p1[0];
    p[2][1] = p1[1];
    /*****************************/
    return OK;
  }
#undef FNAME
#define FNAME "FitIsoscelestriangleMoments"
  int FitIsoscelestriangleMoments(const double mp[], double p[][2])
  {
    double guete;
    return FitIsoscelestriangleMoments(mp, p, guete);
  }
#undef FNAME

#define FNAME "FitIsoscelestriangleMoments"
  int FitIsoscelestriangleMoments(const double mp[], double p[][2], double& guetemass)
  {
    /************************************************************/
    /* Fitting eines gleichschenkligen Dreiecks an ein Objekt                    */
    /* Verschiebung, Rotation, anisotrope Skalierung               */
    /* Nach der Rotationsnormierung wird durch anisotrope Skaliereung           */
    /* auf ein gleichseitiges Dreieck normiert m20=m02=0.096225               */
    /* Input: m[] 15 area moments of the object                  */
    /* Output: p[][2] 3 corner points of the equilateral triangle           */
    /*************************************************************/
    double sx, sy;
    double mx[15];
    double mtrans[15];
    double mrot[15];
    double mtrop[15];
    double beta, gamma;
    double tr[3][3], p1[2], p2[2];

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      throw IceException(FNAME, M_WRONG_OBJECT);

    //
    // *****************************************************
    // Canonical Frame fuer das equialateral triangle
    // als Bezug für die gleichschenkligen Dreiecke
    double cx1 = 0.0;
    double cy1 = (2.0 / 3.0) * sqrt(3.0 / sqrt(3.0));
    double cx2 = -1.0 / sqrt(sqrt(3.0));
    double cy2 = -1.0 / sqrt(3.0 * sqrt(3.0));
    double cx3 = 1.0 / sqrt(sqrt(3.0));
    double cy3 = -1.0 / sqrt(3.0 * sqrt(3.0));
    // End canonical frame
    double m_can[15];
    double centroid_can[2];

    PointList pl = NewPointList(3);
    PutPoint(pl, 0, cx1, cy1);
    PutPoint(pl, 1, cx2, cy2);
    PutPoint(pl, 2, cx3, cy3);
    MomentPolygon(pl, m_can, centroid_can);
    // Auf m_can stehen alle Momente des gleichseitigen Dreicks als canonical frame
    //  for (int i=0;i<15;i++)
    //  Printf(" %lf \n",m_can[i]);
    // Das waren die 3 Eckpunkte des canonical frame
    //
    sx = mx[1] / mx[0];
    sy = mx[2] / mx[0];

    TranslateMoments(mx, -sx, -sy, mtrans);

    // mtrans sind nun die zentralen Momente nach der Translationsnormierung  !!!!!!!!
    // **********************************************************************
    // **********************************************************************
    // printf(" %lf %lf %lf\n",mtrans[3],mtrans[4],mtrans[5]);
    double phi = 0;
    if (fabs(mtrans[11]) <= 1.0E-13 ||
        fabs(mtrans[13]) <= 1.0E-13 ||
        fabs(mtrans[4]) < 1.0e-13)
      {
        phi = 0;
      }
    else
      {
        //b=(mtrans[5]-mtrans[3])/mtrans[4];
        //tanphi=-b/2.0+sqrt(b*b/4.0+1.0);
        double tanphi = (2.0 * mtrans[4]) / (mtrans[5] - mtrans[3]);
        phi = atan(tanphi) / 2.0;
        //norm=sqrt(tanphi*tanphi+1.0);
        //sinphi=tanphi/norm;
        //cosphi=1.0/norm;
      }

    // ******************************************************************************
    // Nun Rotationsbestimmung
    double result[4] = {0.0};
    double phi_all[4] = {0.0};
    double beta_min, gamma_min, phi_min = phi;
    // Da der Winkel bis auf 90 Grad, bestimmt ist muss von den 4 Winkeln der beste bestimmt werden
    phi_all[0] = phi;
    RotateMoments(mtrans, cos(phi), sin(phi), mrot);
    beta = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[5] / (mrot[3] * mrot[3] * mrot[3]))));
    gamma = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[3] / (mrot[5] * mrot[5] * mrot[5]))));
    ScaleMoments(mrot, beta, gamma, mtrop);

    for (int i = 0; i < 15; i++)
      {
        result[0] += (mtrop[i] - m_can[i]) * (mtrop[i] - m_can[i]);
      }

    guetemass = result[0];
    beta_min = beta;
    gamma_min = gamma;
    //printf("Normierte Momente des Objektes\n");
    //for (int i=0;i<15;i++)
    //  printf("%lf  %lf \n",mtrop[i],m_can[i]);
    // getchar();fflush(stdin);

    phi_all[1] = phi + M_PI / 2.0;;
    RotateMoments(mtrans, cos(phi_all[1]), sin(phi_all[1]), mrot);
    beta = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[5] / (mrot[3] * mrot[3] * mrot[3]))));
    gamma = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[3] / (mrot[5] * mrot[5] * mrot[5]))));
    ScaleMoments(mrot, beta, gamma, mtrop);

    for (int i = 0; i < 15; i++)
      {
        result[1] += (mtrop[i] - m_can[i]) * (mtrop[i] - m_can[i]);
      }

    if (result[1] < guetemass)
      {
        guetemass = result[1];
        phi_min = phi + M_PI / 2.0;
        beta_min = beta;
        gamma_min = gamma;
      }

    //printf("Normierte Momente des Objektes\n");
    //for (int i=0;i<15;i++)
    //  printf("%lf  %lf \n",mtrop[i],m_can[i]);
    //getchar();fflush(stdin);

    phi_all[2] = phi + M_PI;
    RotateMoments(mtrans, cos(phi_all[2]), sin(phi_all[2]), mrot);
    beta = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[5] / (mrot[3] * mrot[3] * mrot[3]))));
    gamma = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[3] / (mrot[5] * mrot[5] * mrot[5]))));
    ScaleMoments(mrot, beta, gamma, mtrop);

    for (int i = 0; i < 15; i++)
      {
        result[2] += (mtrop[i] - m_can[i]) * (mtrop[i] - m_can[i]);
      }

    if (result[2] < guetemass)
      {
        guetemass = result[2];
        phi_min = phi + M_PI;
        beta_min = beta;
        gamma_min = gamma;
      }

    //printf("Normierte Momente des Objektes\n");
    //for (int i=0;i<15;i++)
    //  printf("%lf  %lf \n",mtrop[i],m_can[i]);
    //getchar();fflush(stdin);

    phi_all[3] = phi + 3.0 * M_PI / 2.0;
    RotateMoments(mtrans, cos(phi_all[3]), sin(phi_all[3]), mrot);
    beta = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[5] / (mrot[3] * mrot[3] * mrot[3]))));
    gamma = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[3] / (mrot[5] * mrot[5] * mrot[5]))));
    ScaleMoments(mrot, beta, gamma, mtrop);

    for (int i = 0; i < 15; i++)
      {
        result[3] += (mtrop[i] - m_can[i]) * (mtrop[i] - m_can[i]);
      }

    if (result[3] < guetemass)
      {
        guetemass = result[3];
        phi_min = phi + 3.0 * M_PI / 2.0;
        beta_min = beta;
        gamma_min = gamma;
      }

    //printf("Normierte Momente des Objektes\n");
    //for (int i=0;i<15;i++)
    //  printf("%lf  %lf \n",mtrop[i],m_can[i]);
    //      getchar();fflush(stdin);

    // *********************************************************************
    // Ende anisotrope Skalierung

    RotateMoments(mtrans, cos(phi_min), sin(phi_min), mrot);
    ScaleMoments(mrot, beta_min, gamma_min, mtrop);
    double rel = beta_min / gamma_min;
    guetemass = sqrt(guetemass);
    //printf(" %lf \n",beta_min/gamma_min);

    if ((rel >= 1.0 && rel < 1.4) || (rel <= 1.0 && rel > 0.6))
      {
        //printf("ja\n");
        phi_min = phi;
        double guete = 0.0;
        RotateMoments(mtrans, cos(phi), sin(phi), mrot);
        beta = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[5] / (mrot[3] * mrot[3] * mrot[3]))));
        gamma = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[3] / (mrot[5] * mrot[5] * mrot[5]))));
        ScaleMoments(mrot, beta, gamma, mtrop);

        for (int i = 0; i < 15; i++)
          {
            guete += (mtrop[i] - m_can[i]) * (mtrop[i] - m_can[i]);
          }

        guetemass = guete;
        beta_min = beta;
        gamma_min = gamma;
        double phi_end = phi + 2 * M_PI / 3.0;

        while (phi < phi_end)
          {

            phi = phi + 0.01;
            RotateMoments(mtrans, cos(phi), sin(phi), mrot);
            beta = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[5] / (mrot[3] * mrot[3] * mrot[3]))));
            gamma = sqrt(sqrt(sqrt(3.0) / 18.0)) * sqrt(sqrt(sqrt(mrot[3] / (mrot[5] * mrot[5] * mrot[5]))));
            ScaleMoments(mrot, beta, gamma, mtrop);
            guete = 0.0;

            for (int i = 0; i < 15; i++)
              {
                guete += (mtrop[i] - m_can[i]) * (mtrop[i] - m_can[i]);
              }

            if (guete < guetemass)
              {
                guetemass = guete;
                phi_min = phi;
                beta_min = beta;
                gamma_min = gamma;
              }
          }

        guetemass = sqrt(guetemass);
      }

    //printf("Normierte Momente des Objektes\n");
    //RotateMoments(mtrans,cos(phi_min),sin(phi_min),mrot);
    //beta=sqrt(sqrt(sqrt(3.0)/18.0))* sqrt(sqrt(sqrt(mrot[5]/(mrot[3]*mrot[3]*mrot[3]))));
    //gamma=sqrt(sqrt(sqrt(3.0)/18.0))* sqrt(sqrt(sqrt(mrot[3]/(mrot[5]*mrot[5]*mrot[5]))));
    //ScaleMoments(mrot,beta,gamma,mtrop);

    //for (int i=0;i<15;i++)
    //  printf("%lf  %lf \n",mtrop[i],m_can[i]);
    //getchar();fflush(stdin);

    // Ab jetzt zurueck-transformieren !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /********************************************************************/
    InitTrans(tr);
    ShiftTrans(-sx, -sy, tr);
    RotTrans(0, 0, phi_min, tr);
    ScaleTrans(0, 0, beta_min, gamma_min, tr);
    InvertTrans(tr);

    /****************************************************************************/
    p2[0] = cx1;
    p2[1] = cy1;
    TransPoint(p2, tr, p1);
    p[0][0] = p1[0];
    p[0][1] = p1[1];
    /***************************/
    p2[0] = cx2;
    p2[1] = cy2;
    TransPoint(p2, tr, p1);
    p[1][0] = p1[0];
    p[1][1] = p1[1];
    /*****************************/
    p2[0] = cx3;
    p2[1] = cy3;
    TransPoint(p2, tr, p1);
    p[2][0] = p1[0];
    p[2][1] = p1[1];
    /*****************************/
    return OK;
  }
#undef FNAME

#define FNAME "FitPolygonMoments"
  int FitPolygonMoments(int n, const double mp[15],
                        double pstart[][2], double p[][2])
  {
    double guete;
    return FitPolygonMoments(n, mp, pstart, p, guete);
  }
#undef FNAME

#define FNAME "FitPolygonMoments"
  /**********************************************************************/
  int FitPolygonMoments(int n, const double mp[15],
                        double pstart[][2], double p[][2],
                        double& guetemass)
  {
    /*
     * Fitting of a Polygon 3 <= n <= 7 to an object
     * Affine Transformation using Iteration method
     * Input: mp[]     15 area moments of the object
     * Input: pstart  3 <= n <=7  corner points as starting solution for the polygon
     * Output: p[][2] n corner points of the polygon
     */

    double mx[15], maf[15];
    double tr[3][3], tr_hin[3][3], p1[2], p2[2];

    int inumber;

    Vector corners(2 * n + 15); // parameter vector for lmdif

    double po[2], pz[2];

    PosSign(mp, mx);

    if (mx[0] <= 0.0)
      throw IceException(FNAME, M_WRONG_OBJECT);

    if (n < 3 || n > 7)
      throw IceException(FNAME, M_WRONG_PARAM);

    /*******************************************************/

    AffinIterateMoments(mx, maf, tr);

    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        {
          tr_hin[i][j] = tr[i][j];
        }

    InvertTrans(tr);

    // construct vector for lmdif

    int j = 0;

    for (int i = 0; i < n; i++)
      {
        po[0] = pstart[i][0];
        po[1] = pstart[i][1];
        TransPoint(po, tr_hin, pz);
        corners[j++] = pz[0];
        corners[j++] = pz[1];
      }

    for (int i = 0; i < 15; i++)
      {
        corners[j++] = maf[i];
      }

    int rc = LMDif(corners, 2 * n, LM_polygon, 15, inumber, 10000);

    if (rc > 3)
      {
        throw IceException(FNAME, "LMDif " + LMDifMessage(rc));
      }

    for (int i = 0; i < n; i++)
      {
        p1[0] = corners[2 * i];
        p1[1] = corners[2 * i + 1];
        TransPoint(p1, tr, p2);
        p[i][0] = p2[0];
        p[i][1] = p2[1];
      }

    Vector result(15);
    LM_polygon(corners, result);
    guetemass = result.Length();
    return OK;
  }
#undef FNAME

  int LM_polygon(const Vector& corners_and_moments, Vector& result)
  {
    double momente[15];
    int points = (corners_and_moments.Size() - 15) / 2;
    double centroid[2];

    Matrix m_corner(points, 2);

    int k = 0;

    for (int i = 0; i < points; i++)
      {
        m_corner[i][0] = corners_and_moments[k++];
        m_corner[i][1] = corners_and_moments[k++];
      }

    MomentPolygon(m_corner, momente, centroid);

    for (int i = 0; i < 15; i++)
      {
        result[i] = corners_and_moments[k++] - momente[i];
      }

    return 0;
  }
// *****************************************
  int FitSquareMoments(const double mp[], double p[][2])
  {
    double guete;
    return FitSquareMoments(mp, p, guete);
  }
#undef FNAME

#define FNAME "FitSquareMoments"
  int FitSquareMoments(const double mp[], double p[][2], double& guetemass)
  {
    /************************************************************/
    /* Fitting eines Quadrates an ein Objekt                    */
    /* Verschiebung, Rotation, isotrope Skalierung               */
    /* Input: m[] 15 area moments of the object                  */
    /* Output: p[][2] 3 corner points of the equilateral triangle           */
    /*************************************************************/
    double sx, sy;
    double mx[15];
    double mtrans[15];
    double mrot[15];
    double mtrop[15];
    //    double sinphi, cosphi, tanphi, phi;
    double alpha;
    double tr[3][3], p1[2], p2[2];
    //    int isx, isy;

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      throw IceException(FNAME, M_WRONG_OBJECT);

    //
    // *****************************************************
    // Canonical Frame for square
    double cx1 = -1.0 / 2.0;
    double cy1 = +1.0 / 2.0;
    double cx2 = +1.0 / 2.0;
    double cy2 = +1.0 / 2.0;
    double cx3 = +1.0 / 2.0;
    double cy3 = -1.0 / 2.0;
    double cx4 = -1.0 / 2.0;
    double cy4 = -1.0 / 2.0;
    // End canonical frame

    double m_can[15];
    double centroid_can[2];

    PointList pl = NewPointList(4);
    PutPoint(pl, 0, cx1, cy1);
    PutPoint(pl, 1, cx2, cy2);
    PutPoint(pl, 2, cx3, cy3);
    PutPoint(pl, 3, cx4, cy4);
    MomentPolygon(pl, m_can, centroid_can);
    //  for (int i=0;i<15;i++)
    //  Printf(" %lf \n",m_can[i]);
    // Das waren die 3 Eckpunkte des canonical frame
    //
    sx = mx[1] / mx[0];
    sy = mx[2] / mx[0];
    //    isx = (int)sx;
    //    isy = (int)sy;
    TranslateMoments(mx, -sx, -sy, mtrans);

    // mtrans sind nun die zentralen Momente
    // **********************************************************************
    double phi = 0.0;

    if (fabs(mtrans[11]) <= 1.0E-13 ||
        fabs(mtrans[13]) <= 1.0E-13 ||
        fabs(mtrans[4]) < 1.0e-13)
      {
        phi = 0;
      }
    else
      {
        //b=(mtrans[5]-mtrans[3])/mtrans[4];
        //tanphi=-b/2.0+sqrt(b*b/4.0+1.0);
        double tanphi = (2.0 * mtrans[4]) / (mtrans[5] - mtrans[3]);
        phi = atan(tanphi) / 2.0;
        //norm=sqrt(tanphi*tanphi+1.0);
        //sinphi=tanphi/norm;
        //cosphi=1.0/norm;
      }

    RotateMoments(mtrans, cos(phi), sin(phi), mrot);

    alpha = 1.0 / sqrt(mrot[0]);
    ScaleMoments(mrot, alpha, alpha, mtrop);

    // Skalierung auf m00=1
    // *********************************************************************

    for (int i = 0; i < 15; i++)
      {
        mrot[i] = mtrans[i] = mtrop[i];
      }

    double min = +1.0e+10;
    double min_h, phi_opt = 0.0;
    double phi_end = 2 * M_PI / 4.0;
    double phi_h = 0.0;

    do
      {
        RotateMoments(mtrans, cos(phi_h), sin(phi_h), mrot);
        min_h = 0.0;

        for (int i = 0; i < 15; i++)
          {
            min_h = min_h + (mrot[i] - m_can[i]) * (mrot[i] - m_can[i]);
          }

        if (min_h < min)
          {
            min = min_h;
            phi_opt = phi_h;
          }

        //printf("%lf %lf %lf\n",mrot[4],min,phi_opt);
        phi_h = phi_h + 0.01;
      }
    while (phi_h < phi_end);

    guetemass = sqrt(min);

    //printf("%lf \n",sqrt(min));
    //printf("\n");
    //  for (int i=0;i<15;i++)
    //  Printf(" %lf\n",mrot[i]);

    //RotateMoments(mtrans,cos(phi_opt),sin(phi_opt),mrot);
    // for (int i=0;i<15;i++)
    //  printf(" %lf  %lf\n",m_can[i],mrot[i]);

    // Ab jetzt zurueck-transformieren !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /********************************************************************/
    InitTrans(tr);
    ShiftTrans(-sx, -sy, tr);
    RotTrans(0, 0, phi + phi_opt, tr);
    ScaleTrans(0, 0, alpha, alpha, tr);
    InvertTrans(tr);

    /****************************************************************************/
    p2[0] = cx1;
    p2[1] = cy1;
    TransPoint(p2, tr, p1);
    p[0][0] = p1[0];
    p[0][1] = p1[1];
    /***************************/
    p2[0] = cx2;
    p2[1] = cy2;
    TransPoint(p2, tr, p1);
    p[1][0] = p1[0];
    p[1][1] = p1[1];
    /*****************************/
    p2[0] = cx3;
    p2[1] = cy3;
    TransPoint(p2, tr, p1);
    p[2][0] = p1[0];
    p[2][1] = p1[1];
    /*****************************/
    p2[0] = cx4;
    p2[1] = cy4;
    TransPoint(p2, tr, p1);
    p[3][0] = p1[0];
    p[3][1] = p1[1];
    /*****************************/
    return OK;
  }
#undef FNAME
}
