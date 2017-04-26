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
    Funktionen fuer geometrische Transformationen                   *
       Suesse 7/93                                                *
*********************************************************************/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "IceException.h"
#include "macro.h"
#include "arith.h"
#include "matrix_function.h"
#include "Contur.h"
#include "contools.h"
#include "numbase.h"
#include "matdef.h"
#include "lineqn.h"

#include "gentrans.h"

namespace ice
{
  /* Initialisieren einer Transformationsmatrix (Einheitsmatrix)*/
  int InitTrans(double Trans[3][3])
  {
    DefMatrix((double*)Trans, 3, 3, UNIT);
    return OK;
  }

  int normalize(double Trans[3][3])
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

  /*******************************************************************/
#define FNAME "ShiftTrans"
  /* Premultiplikation von "Trans" mit Verschiebung (x0,y0)*/
  int ShiftTrans
  (double x0, double y0, double Trans[3][3])
  {
    double m[3][3];

    if (Trans == NULL)
      throw IceException(FNAME, M_WRONG_PTR);

    DefMatrix((double*)m, 3, 3, UNIT);
    m[0][2] = x0;
    m[1][2] = y0;
    MulMatrix((double*)m, (double*)Trans, 3, 3, 3, (double*)Trans);

    /* Normierung nach Summe der Quadrate der unteren Zeile */
    if (normalize(Trans) != OK)
      throw IceException(FNAME, M_NO_PROJ);

    return OK;
  }

#undef FNAME

  /*******************************************************************/
#define FNAME "RotTrans"
  /* Premultiplikation von "Trans" mit Rotation um Punkt (x0,y0) */
  int RotTrans
  (double x0, double y0, double phi, double Trans[3][3])
  {
    double rot[3][3];

    if (Trans == NULL)
      throw IceException(FNAME, M_WRONG_PTR);

    InitTrans(rot);
    rot[0][0] = cos(phi);
    rot[0][1] = -sin(phi);
    rot[1][0] = sin(phi);
    rot[1][1] = cos(phi);

    rot[0][2] = -cos(phi) * x0 + sin(phi) * y0 + x0;
    rot[1][2] = -sin(phi) * x0 - cos(phi) * y0 + y0;

    MulMatrix((double*)rot, (double*)Trans, 3, 3, 3, (double*)Trans);

    /* Normierung nach Summe der Quadrate der unteren Zeile */
    if (normalize(Trans) != OK)
      throw IceException(FNAME, M_NO_PROJ);

    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ScaleTrans"
  /* Premultiplikation von "Trans" mit Skalierung um Punkt (x0,y0) */
  int ScaleTrans
  (double x0, double y0, double facx, double facy, double Trans[3][3])
  {
    double scal[3][3];

    if (Trans == NULL)
      throw IceException(FNAME, M_WRONG_PTR);

    InitTrans(scal);
    scal[0][0] = facx;
    scal[0][2] = -facx * x0 + x0;
    scal[1][1] = facy;
    scal[1][2] = -facy * y0 + y0;
    MulMatrix((double*)scal, (double*)Trans, 3, 3, 3, (double*)Trans);

    /* Normierung nach Summe der Quadrate der unteren Zeile */
    if (normalize(Trans) != OK)
      throw IceException(FNAME, M_NO_PROJ);

    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "AffinTrans"
  /* Premultiplikation von "Trans" mit affiner Transformation */
  int AffinTrans(double a11, double a12, double a13,
                 double a21, double a22, double a23, double Trans[3][3])
  {
    double affin[3][3];
    double eps = 1e-10;
    double fac;

    if (Trans == NULL)
      throw IceException(FNAME, M_WRONG_PTR);

    InitTrans(affin);
    affin[0][0] = a11;
    affin[0][1] = a12;
    affin[0][2] = a13;
    affin[1][0] = a21;
    affin[1][1] = a22;
    affin[1][2] = a23;
    MulMatrix((double*)affin, (double*)Trans, 3, 3, 3, (double*)Trans);

    /* Normierung nach Summe der Quadrate der unteren Zeile */
    if (normalize(Trans) != OK)
      throw IceException(FNAME, M_NO_PROJ);

    fac = Sqr(Trans[2][0]) + Sqr(Trans[2][1]) + Sqr(Trans[2][2]);

    if (fac < eps)
      throw IceException(FNAME, M_NO_PROJ);

    ScaleMatrix((double*)Trans, 3, 3, fac, (double*)Trans);
    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "ProjectivTrans"
  /* Premultiplikation von "Trans" mit projektiver Transformation */
  int ProjectivTrans(double a11, double a12, double a13,
                     double a21, double a22, double a23,
                     double a31, double a32, double a33, double Trans[3][3])
  {
    double proj[3][3];
    double eps = 1e-10;
    double fac;

    if (Trans == NULL)
      throw IceException(FNAME, M_WRONG_PTR);

    InitTrans(proj);
    proj[0][0] = a11;
    proj[0][1] = a12;
    proj[0][2] = a13;
    proj[1][0] = a21;
    proj[1][1] = a22;
    proj[1][2] = a23;
    proj[2][0] = a31;
    proj[2][1] = a32;
    proj[2][2] = a33;
    MulMatrix((double*)proj, (double*)Trans, 3, 3, 3, (double*)Trans);

    /* Normierung nach Summe der Quadrate der unteren Zeile */
    if (normalize(Trans) != OK)
      throw IceException(FNAME, M_NO_PROJ);

    fac = Sqr(Trans[2][0]) + Sqr(Trans[2][1]) + Sqr(Trans[2][2]);

    if (fac < eps)
      throw IceException(FNAME, M_NO_PROJ);

    ScaleMatrix((double*)Trans, 3, 3, fac, (double*)Trans);
    return OK;
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "InvertTrans"
  /* Invertieren einer Transformationsmatrix */
  int InvertTrans(double Trans[3][3])
  {
    double m[3][3];
    double eps = 1e-10;
    double fac;

    MoveMatrix((double*)Trans, 3, 3, (double*)m);
    InvertMatrix((double*)m, 3, (double*)Trans);

    /* Normierung nach Summe der Quadrate der unteren Zeile */
    if (normalize(Trans) != OK)
      throw IceException(FNAME, M_NO_PROJ);

    fac = Sqr(Trans[2][0]) + Sqr(Trans[2][1]) + Sqr(Trans[2][2]);

    if (fac < eps)
      throw IceException(FNAME, M_NO_PROJ);

    ScaleMatrix((double*)Trans, 3, 3, 1 / fac, (double*)Trans);
    return OK;
  }
#undef FNAME
  /*****************************************************/
#define FNAME "FitProjectiveTrans"
  int FitProjectiveTrans(PointList pl1, PointList pl2, double tr[3][3])
  {
    double* ih, *ip, w;
    MatrixStruct A;
    int i, rc;

    if (pl1->lng < 4)
      throw IceException(FNAME, M_MISS_P);

    if (pl2->lng < pl1->lng)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    A = NewMatrix(MAT_DOUBLE, pl1->lng * 2, 8);

    if (A == NULL)
      throw IceException(FNAME, M_NO_MEM);

    ih = ip = (double*)malloc(pl1->lng * 2 * sizeof(double));

    if (ih == NULL)
      {
        free(A);
        throw IceException(FNAME, M_NO_MEM);
      }

    for (i = 0; i < pl1->lng; i++)
      {
        w = pl2->wptr[i];
        A->data[2 * i][0] = pl1->xptr[i] * w;
        A->data[2 * i][1] = pl1->yptr[i] * w;
        A->data[2 * i][2] = w;
        A->data[2 * i][3] = 0           ;
        A->data[2 * i][4] = 0           ;
        A->data[2 * i][5] = 0;
        A->data[2 * i][6] = -pl1->xptr[i] * pl2->xptr[i] * w;
        A->data[2 * i][7] = -pl1->yptr[i] * pl2->xptr[i] * w;
        *(ip++) = pl2->xptr[i] * w;
        A->data[2 * i + 1][0] = 0           ;
        A->data[2 * i + 1][1] = 0           ;
        A->data[2 * i + 1][2] = 0;
        A->data[2 * i + 1][3] = pl1->xptr[i] * w;
        A->data[2 * i + 1][4] = pl1->yptr[i] * w;
        A->data[2 * i + 1][5] = w;
        A->data[2 * i + 1][6] = -pl1->xptr[i] * pl2->yptr[i] * w;
        A->data[2 * i + 1][7] = -pl1->yptr[i] * pl2->yptr[i] * w;
        *(ip++) = pl2->yptr[i] * w;
      }

    rc = NormalEquationSys(A, ih, (double*)tr);

    FreeMatrix(A);
    free(ih);
    tr[2][2] = 1;

    if (rc != OK)
      throw IceException(FNAME, M_0);

    return (OK);
  }
#undef FNAME
  /*****************************************************/
#define FNAME "FitAffineTrans"
  int FitAffineTrans(PointList pl1, PointList pl2, double tr[3][3])
  {
    double* ih, *ip, w;
    MatrixStruct A;
    int i, rc;

    if (pl1->lng < 3)
      throw IceException(FNAME, M_MISS_P);

    if (pl2->lng < pl1->lng)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    A = NewMatrix(MAT_DOUBLE, pl1->lng * 2, 6);

    if (A == NULL)
      throw IceException(FNAME, M_NO_MEM);

    ih = ip = (double*)malloc(pl1->lng * 2 * sizeof(double));

    if (ih == NULL)
      {
        FreeMatrix(A);
        throw IceException(FNAME, M_NO_MEM);
      }

    if (A == NULL || ih == NULL)
      {
        return (ERROR);
      }

    for (i = 0; i < pl1->lng; i++)
      {
        w = pl2->wptr[i];
        A->data[2 * i  ][0] = pl1->xptr[i] * w;
        A->data[2 * i  ][1] = pl1->yptr[i] * w;
        A->data[2 * i  ][2] = w;
        A->data[2 * i  ][3] = 0             ;
        A->data[2 * i  ][4] = 0             ;
        A->data[2 * i  ][5] = 0;
        *(ip++) = pl2->xptr[i] * w;
        A->data[2 * i + 1][0] = 0             ;
        A->data[2 * i + 1][1] = 0             ;
        A->data[2 * i + 1][2] = 0;
        A->data[2 * i + 1][3] = pl1->xptr[i] * w;
        A->data[2 * i + 1][4] = pl1->yptr[i] * w;
        A->data[2 * i + 1][5] = w;
        *(ip++) = pl2->yptr[i] * w;
      }

    rc = NormalEquationSys(A, ih, (double*)tr);

    FreeMatrix(A);
    free(ih);
    tr[2][0] = 0;
    tr[2][1] = 0;
    tr[2][2] = 1;

    if (rc != OK)
      throw IceException(FNAME, M_0);

    return (OK);
  }
#undef FNAME
}
