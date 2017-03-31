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
/*
 *  Projektive und Affine Transformation von Bildern
 *
 *  M.Schubert 6.93
 *  Ortmann 10/99
 */

#include <stdlib.h>
#include <stddef.h>  /* notwendig fuer Unix !!! */

#include <math.h>

#include "message.h"
#include "macro.h"
#include "based.h"
#include "numbase.h"
#include "arith.h"
#include "arith.h"
#include "matrix_function.h"
#include "Contur.h"
#include "contools.h"

#include "trans.h"

namespace ice
{
  /******* nicht oeffentliche Funktionen****************/
  int TrLine(const Image& img1, const Image& img2, int y2, double p[3][2], double* nx, double mx);

  int  TrLineI(const Image& img1, const Image& img2, int y2, double p[3][2], double* nx, double mx);

  int InvPrjTr(double t1[3][3], double t2[3][3]);

  int Invers(int x, int y, int n1, int n2,
             double a, double b, double c, double d, double* xx, double* yy);


  /*** oeffentliche Funktionen ****************************************/

#define FNAME "TransPoint"
  /* #define DEBUG */
  double* TransPoint(double p1[2], double t[3][3], double p2[2])
  {
    double ph[3];
    double* ptr;

    if ((p1 == NULL) || (t == NULL))
      {
        Message(FNAME, M_WRONG_PTR, WRONG_POINTER);
        return NULL;
      }

    ph[0] = p1[0];
    ph[1] = p1[1];
    ph[2] = 1;
    MulMatrix((double*)t, (double*)ph, 3, 3, 1, (double*)ph);

    if (fabs(ph[2]) < 1e-10)
      {
        Message(FNAME, M_WRONG_TRANS, WRONG_TRANS);
        return NULL;
      }

    if (p2 == NULL)
      {
        ptr = (double*)malloc(2 * sizeof(double));
        ptr[0] = ph[0] / ph[2];
        ptr[1] = ph[1] / ph[2];
        SetOk();
        return ptr;
      }

    p2[0] = ph[0] / ph[2];
    p2[1] = ph[1] / ph[2];
    SetOk();
    return p2;
  }
#undef FNAME
#undef DEBUG
  /*******************************************************************/
#define FNAME "TransContur"
  /* #define DEBUG */
  Contur TransContur(const Contur& c1, double t[3][3])
  {
    Contur c2;
    double ds[2], dd[2];
    int i;
    int x, y;
    double eps = 1e-15;
    int rc;

    if (t == NULL)
      {
        Message(FNAME, M_WRONG_PTR, WRONG_POINTER);
        return c2;
      }

    if (IsMatrixRegular((double*)t, 3, &eps) != true)
      {
        Message(FNAME, M_NO_REGULAR, WRONG_TRANS);
        return c2;
      }

    ds[0] = c1.StartX();
    ds[1] = c1.StartY();
    OffMessage();
    TransPoint(ds, t, dd);
    rc = GetError();
    OnMessage();

    if (rc != OK)
      {
        Message(FNAME, M_INFINIT_POINT, INFINIT_POINT);
        return c2;
      }

    c2.SetStart(RoundInt(dd[0]), RoundInt(dd[1]));

    for (i = 1; i <= c1.Number(); i++)
      {
        c1.getPoint(i, x, y);
        ds[0] = x;
        ds[1] = y;
        OffMessage();
        TransPoint(ds, t, dd);
        rc = GetError();
        OnMessage();

        if (rc != OK)
          {
            if (rc == WRONG_TRANS)
              {
                Message(FNAME, M_INFINIT_POINT, INFINIT_POINT);
                return c2;
              }
            else
              {
                Message(FNAME, M_0, INTERN_ERROR);
                return c2;
              }
          }

        c2.Add(RoundInt(dd[0]), RoundInt(dd[1]));
      }

    return c2;
  }
#undef FNAME
#undef DEBUG

  /***************************************************************************/
#define FNAME "TransImg"
  int TransImg(const Image& imgss, double t[3][3], int mode, const Image& imgd)
  /* imgs   Quellbild
     imgd   Zielbild
     t    Transformationsmatrix
      (für die Transformation eines Punktes
      vom Quellbild ins Zielbild!!!)

     Projektionsgleichungen:
     xd=[tm(0,0)xs+tm(0,1)ys+tm(0,2)]/[tm(2,0)xs+tm(2,1)ys+tm(2,2)]
     yd=[tm(1,0)xs+tm(1,1)ys+tm(1,2)]/[tm(2,0)xs+tm(2,1)ys+tm(2,2)]
  */
  {
    double tm[3][3];
    double p[3][2], pf1[3][3][2], pf2[3][3][2];
    double* nx, *ny;
    int i, j, y, sx, sy;
    double h;
    double mx1, my1, my2[3];
    int has_temp = false;

    if (!IsImg(imgss) || !IsImg(imgd))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Image imgs;

    if (imgd == imgs)
      {
        /*temporaeres Quellbild*/
        imgs = NewImg(imgd, true);

        if (!IsImg(imgs))
          {
            Message(FNAME, M_0, ERROR);
            return ERROR;
          }

        has_temp = true;
      }
    else
      {
        imgs = imgss;
      }

    for (i = 0; i < 3; i++)
      {
        pf2[0][i][0] = 0;          /*Bezugspunkte im Zielbild*/
        pf2[1][i][0] = imgd.xsize / 2;
        pf2[2][i][0] = imgd.xsize;
        pf2[i][0][1] = 0;
        pf2[i][1][1] = imgd.ysize / 2;
        pf2[i][2][1] = imgd.ysize;
      }

    InvPrjTr(t, tm);                     /*Projektion invertieren*/

    for (i = 0; i < 3; i++)              /*Transformation d. Bezugspunkte*/
      for (j = 0; j < 3; j++)
        {
          h = tm[2][2] + tm[2][0] * pf2[i][j][0] + tm[2][1] * pf2[i][j][1];
          pf1[i][j][0] = (tm[0][2] + tm[0][0] * pf2[i][j][0] + tm[0][1] * pf2[i][j][1]) / h;
          pf1[i][j][1] = (tm[1][2] + tm[1][0] * pf2[i][j][0] + tm[1][1] * pf2[i][j][1]) / h;
        }

    sx = imgd.xsize;
    sy = imgd.ysize;
    nx = (double*)malloc(sx * sizeof(double));
    ny = (double*)malloc(sy * sizeof(double));

    for (i = 0; i < sx; i++)   /*look-up-tables f. Zeilen bzw Spalten*/
      {
        nx[i] = (double)i / (double)(sx - i);
      }

    for (i = 0; i < sy; i++)
      {
        ny[i] = (double)i / (double)(sy - i);
      }

    mx1 = (pf2[2][0][0] - pf2[1][0][0]) / (pf2[1][0][0] - pf2[0][0][0]);
    my1 = (pf2[0][2][1] - pf2[0][1][1]) / (pf2[0][1][1] - pf2[0][0][1]);

    for (i = 0; i < 3; i++)
      {
        my2[i] = sqrt((Sqr(pf1[i][1][0] - pf1[i][0][0]) + Sqr(pf1[i][1][1] - pf1[i][0][1])) / (Sqr(pf1[i][2][0] - pf1[i][1][0]) + Sqr(pf1[i][2][1] - pf1[i][1][1])));
      }

    for (i = 0; i < 3; i++)
      for (j = 0; j < 2; j++)
        {
          p[i][j] = pf1[i][0][j];  /*Bezugspunkte erste Zeile*/
        }

    if (mode == 0)
      {
        TrLine(imgs, imgd, 0, p, nx, mx1);
      }
    else
      {
        TrLineI(imgs, imgd, 0, p, nx, mx1);
      }

    for (y = 1; y < sy; y++)
      {
        for (i = 0; i < 3; i++)
          {
            h = my1 * my2[i] * ny[y];
            p[i][0] = pf1[i][0][0] + h * (pf1[i][2][0] - pf1[i][0][0]) / (1 + h);
            p[i][1] = pf1[i][0][1] + h * (pf1[i][2][1] - pf1[i][0][1]) / (1 + h);
          }

        if (mode == 0)
          {
            TrLine(imgs, imgd, y, p, nx, mx1);
          }
        else
          {
            TrLineI(imgs, imgd, y, p, nx, mx1);
          }
      }

    for (i = 0; i < 3; i++)
      for (j = 0; j < 2; j++)
        {
          p[i][j] = pf1[i][2][j];  /*Bezugspunkte letzte Zeile*/
        }

    if (mode == 0)
      {
        TrLine(imgs, imgd, sy, p, nx, mx1);
      }
    else
      {
        TrLineI(imgs, imgd, sy, p, nx, mx1);
      }

    free(nx);
    free(ny);

    if (has_temp)
      {
        FreeImg(imgs);
      }

    return OK;
  }
#undef FNAME


  /* Nichtöffentliche Funktionen */

  /* Bildzeile transformieren ***********************************************/
  int  TrLine(const Image& img1, const Image& img2, int y2, double p[3][2], double* nx, double mx)
  /*
    img1,img2                      Quell- u. Zielbild
    y2                             Bildzeile Zielb.
    p[3][2]                        Referenzpunkte
    *nx                            look-up-table
    mx                             cons. Faktor
  */
  {
    int x2, x1, y1;
    double m, h, hx, hy;
    m = mx * sqrt((Sqr(p[1][0] - p[0][0]) + Sqr(p[1][1] - p[0][1])) / (Sqr(p[2][0] - p[1][0]) + Sqr(p[2][1] - p[1][1])));
    hx = m * (p[2][0] - p[0][0]);
    hy = (p[2][1] - p[0][1]) / (p[2][0] - p[0][0]);
    OffMessage();
    PutVal(img2, 0, y2, GetVal(img1, (int)(p[0][0] + 0.5), (int)(p[0][1] + 0.5)));

    for (x2 = 1; x2 < img2.xsize - 1; x2++)
      {
        h = hx * nx[x2] / (1 + m * nx[x2]);
        x1 = (int)(p[0][0] + h + 0.5);
        y1 = (int)(p[0][1] + h * hy + 0.5);
        PutVal(img2, x2, y2, GetVal(img1, x1, y1));
      }

    PutVal(img2, img2.xsize - 1, y2, GetVal(img1, (int)(p[2][0] + 0.5), (int)(p[2][1] + 0.5)));
    SetOk();
    OnMessage();
    return 0;
  }
  /************************************************/
  int  TrLineI(const Image& img1, const Image& img2, int y2, double p[3][2], double* nx, double mx)
  /*
    img1,img2                      Quell- u. Zielbild
    y2                             Bildzeile Zielb.
    p[3][2]                        Referenzpunkte
    *nx                            look-up-table
    mx                             cons. Faktor
  */
  {
    int x2;
    double x1, y1;
    double m, h, hx, hy;
    m = mx * sqrt((Sqr(p[1][0] - p[0][0]) + Sqr(p[1][1] - p[0][1])) / (Sqr(p[2][0] - p[1][0]) + Sqr(p[2][1] - p[1][1])));
    hx = m * (p[2][0] - p[0][0]);
    hy = (p[2][1] - p[0][1]) / (p[2][0] - p[0][0]);
    OffMessage();
    PutVal(img2, 0, y2, (int)(0.5 + GetInterpolVal(img1, p[0][0], p[0][1])));

    for (x2 = 1; x2 < img2.xsize - 1; x2++)
      {
        h = hx * nx[x2] / (1 + m * nx[x2]);
        x1 = p[0][0] + h;
        y1 = p[0][1] + h * hy;
        PutVal(img2, x2, y2, (int)(0.5 + GetInterpolVal(img1, x1, y1)));
      }

    PutVal(img2, img2.xsize - 1, y2, (int)(0.5 + GetInterpolVal(img1, p[2][0], p[2][1])));
    SetOk();
    OnMessage();
    return 0;
  }
  /*************************************************/
  int InvPrjTr(double t1[3][3], double t2[3][3])
  {
    t2[0][0] = t1[2][2] * t1[1][1] - t1[2][1] * t1[1][2];
    t2[0][1] = t1[2][1] * t1[0][2] - t1[2][2] * t1[0][1];
    t2[0][2] = t1[0][1] * t1[1][2] - t1[0][2] * t1[1][1];
    t2[1][0] = t1[2][0] * t1[1][2] - t1[2][2] * t1[1][0];
    t2[1][1] = t1[2][2] * t1[0][0] - t1[2][0] * t1[0][2];
    t2[1][2] = t1[0][2] * t1[1][0] - t1[0][0] * t1[1][2];
    t2[2][0] = t1[2][1] * t1[1][0] - t1[2][0] * t1[1][1];
    t2[2][1] = t1[2][0] * t1[0][1] - t1[2][1] * t1[0][0];
    t2[2][2] = t1[0][0] * t1[1][1] - t1[0][1] * t1[1][0];
    return OK;
  }
#undef FNAME

  /***************************************************************************/
#define FNAME "TransAffineImg"

  static int normalize(double Trans[3][3])
  {
    const double eps = 1e-30;
    double fac = Sqr(Trans[2][0]) + Sqr(Trans[2][1]) + Sqr(Trans[2][2]);

    if (fac < eps)
      {
        return ERROR;
      }

    ScaleMatrix((double*)Trans, 3, 3, 1 / sqrt(fac), (double*)Trans);
    return OK;
  }

  int InvAffTr(double t1[3][3], double t2[3][3])
  {
    t2[0][0] = t1[1][1];
    t2[0][1] = -t1[0][1];
    t2[0][2] = t1[0][1] * t1[1][2] - t1[0][2] * t1[1][1];
    t2[1][0] = -t1[1][0];
    t2[1][1] = t1[0][0];
    t2[1][2] = t1[0][2] * t1[1][0] - t1[0][0] * t1[1][2];
    t2[2][0] = 0;
    t2[2][1] = 0;
    t2[2][2] = t1[0][0] * t1[1][1] - t1[0][1] * t1[1][0];
    normalize(t2);
    return OK;
  }

  int TransAffineImg(const Image& imgss, double t[3][3], int mode, const Image& imgd)
  /* imgs   Quellbild
     imgd   Zielbild
     t    Transformationsmatrix
     (für die Transformation eines Punktes
     vom Quellbild ins Zielbild!!!)

     Projektionsgleichungen:
     xd=[tm(0,0)xs+tm(0,1)ys+tm(0,2)]/[tm(2,0)xs+tm(2,1)ys+tm(2,2)]
     yd=[tm(1,0)xs+tm(1,1)ys+tm(1,2)]/[tm(2,0)xs+tm(2,1)ys+tm(2,2)]
  */
  {
    const double eps = 1e-30;
    double tm[3][3];
    double tm02, tm12;
    double tx, ty;
    double* nxx, *nxy, *nyx, *nyy;
    int x, y, sx, sy;
    double ox, oy;
    int has_temp = false;


    normalize(t);

    if ((Abs(t[2][0]) > eps) || (Abs(t[2][1]) > eps) || (Abs(t[2][2] - 1.0) > eps))
      {
        // not an affine transform
        Message(FNAME, M_WRONG_TRANS, WRONG_TRANS);
        return WRONG_TRANS;
      }


    if (!IsImg(imgd) || !IsImg(imgss))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Image imgs;

    if (imgd == imgss)
      {
        /*temporaeres Quellbild*/
        imgs = NewImg(imgd, true);

        if (!IsImg(imgs))
          {
            Message(FNAME, M_0, ERROR);
            return ERROR;
          }

        has_temp = true;
      }
    else
      {
        imgs = imgss;
      }

    //      for (y=0;y<3;y++)
    //        {
    //        for (x=0;x<3;x++) Printf("%5.2f ",t[y][x]);
    //        Printf("\n");
    //    }
    //    Printf("\n");
    InvAffTr(t, tm);                     /*Projektion invertieren*/

    //      for (y=0;y<3;y++)
    //        {
    //        for (x=0;x<3;x++) Printf("%5.2f ",tm[y][x]);
    //        Printf("\n");
    //        }
    //    Printf("\n");

    sx = imgd.xsize;
    sy = imgd.ysize;

    nxx = new double[imgd.xsize]; // Lookup-Tabellen für t[i][j]*x
    nyx = new double[imgd.xsize];
    nxy = new double[imgd.ysize];
    nyy = new double[imgd.ysize];

    for (x = 0; x < sx; x++)   //look-up-tables f. Zeilen bzw Spalten
      {
        nxx[x] = tm[0][0] * x;
        nyx[x] = tm[1][0] * x;
      }

    for (y = 0; y < sy; y++)
      {
        nxy[y] = tm[0][1] * y;
        nyy[y] = tm[1][1] * y;
      }

    tm02 = tm[0][2];
    tm12 = tm[1][2];

    if (mode == INTERPOL)
      {
        for (y = 0; y < sy; y++)
          {
            tx = nxy[y] + tm02;
            ty = nyy[y] + tm12;

            for (x = 0; x < sx; x++)
              {
                ox = nxx[x] + tx;
                oy = nyx[x] + ty;
                PutVal(imgd, x, y, RoundInt(GetInterpolVal(imgs, ox, oy)));
              }
          }
      }
    else
      {
        tm02 += 0.5;
        tm12 += 0.5; // für Rundung !!

        for (y = 0; y < sy; y++)
          {
            tx = nxy[y] + tm02;
            ty = nyy[y] + tm12;

            for (x = 0; x < sx; x++)
              {
                ox = nxx[x] + tx;
                oy = nyx[x] + ty;
                PutVal(imgd, x, y, GetValClipped(imgs, (int)(ox), (int)(oy)));
              }
          }
      }

    delete [] nxx;
    delete [] nyx;
    delete [] nxy;
    delete [] nyy;

    if (has_temp)
      {
        FreeImg(imgs);
      }

    return OK;
  }
#undef FNAME
}
