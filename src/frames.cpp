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
/*************************************************************/
/* Funktionen zur Manipulation von homogenen (euklidischen)  */
/* Transformationen                                          */
/*                                                           */
/* (C) m.schubert 1/96                                       */
/*************************************************************/
/*
*/

#include <stdlib.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "matrix_function.h"
#include "vectorarrayfunctions.h"

#include "robmsg.h"
#include "math.h"
#include "paint.h"
#include "matdef.h"
#include "lineqn.h"
#include "Trafo.h"

#include "frames.h"

namespace ice
{
  /*********************************************************/
  double* FTransPoint(double p1[3], Frame* f, double p2[3])
  {
    int i, j;
    double h[3], *p;

    if (p2 == p1)
      {
        p = h;
      }
    else
      {
        p = p2;
      }

    for (i = 0; i < 3; i++)
      {
        for (j = 0, p[i] = f->frame[i][3]; j < 3; j++)
          {
            p[i] += p1[j] * f->frame[i][j];
          }
      }

    if (p == p2)
      {
        return (p2);
      }

    for (i = 0; i < 3; i++)
      {
        p2[i] = p[i];
      }

    return (p2);
  }
  /*********************************************************/
  double* FTransVec(double v1[3], Frame* f, double v2[3])
  {
    double vr[3], *v;
    int i, j;

    if (v2 == v1)
      {
        v = vr;
      }
    else
      {
        v = v2;
      }

    for (i = 0; i < 3; i++)
      for (j = 0, v[i] = 0; j < 3; j++)
        {
          v[i] += f->frame[i][j] * v1[j];
        }

    if (v == v2)
      {
        return (v2);
      }

    for (i = 0; i < 3; i++)
      {
        v2[i] = v[i];
      }

    return (v2);
  }
  /*********************************************************/
  Frame* CatFrames(Frame* f1, Frame* f2, Frame* f3)
  {
    /*f3=f1*f2 (Verkettung der Frames f1 und f2)*/
    int i, j, k;
    Frame fr, *f;

    if (f3 == f1 || f3 == f2)
      {
        f = &fr;
      }
    else
      {
        f = f3;
      }

    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        for (k = 0, f->frame[i][j] = 0; k < 4; k++)
          {
            f->frame[i][j] += f1->frame[i][k] * f2->frame[k][j];
          }

    if (f == f3)
      {
        return (f3);
      }

    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        {
          f3->frame[i][j] = f->frame[i][j];
        }

    return (f3);
  }
  /*********************************************************/
#define FNAME "InvertFrame"
  Frame* InvertFrame(Frame* f1, Frame* f2)
  {
    int i, j;
    MatrixStruct A, B;

    A = NewMatrix(MAT_DOUBLE, 4, 4);
    B = NewMatrix(MAT_DOUBLE, 4, 4);

    if (A == nullptr || B == nullptr)
      throw IceException(FNAME, M_0);

    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        {
          A->data[i][j] = f1->frame[i][j];
        }

    InvertMat(A, B);

    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        {
          f2->frame[i][j] = B->data[i][j];
        }

    FreeMatrix(A);
    FreeMatrix(B);
    return (f2);
  }
#undef FNAME
  /*********************************************************/
  int CopyFrame(Frame* f1, Frame* f2)
  {
    int i, j;

    for (i = 0; i < 4; i++)
      for (j = 0; j < 4; j++)
        {
          f2->frame[i][j] = f1->frame[i][j];
        }

    f2->err = f1->err;
    return (OK);
  }
  /*********************************************************/
#if 0
  Frame* CameraFrame(camera cam, Frame* frm)
  {
    double a[3][3];
    int i, j;
    /*   CrossProdVec(cam->ac[0],cam->ac[1],a[2]); */
    NormVec(cam->ac[0], a[0]);
    /*   NormVec(a[2],a[2]); */
    NormVec(cam->a[2], a[2]);
    CrossProdVec(a[2], a[0], a[1]);

    for (i = 0; i < 3; i++)
      {
        for (j = 0; j < 3; j++)
          {
            frm->frame[i][j] = a[j][i];
          }

        frm->frame[i][3] = cam->c[i];
        frm->frame[3][i] = 0;
      }

    frm->frame[3][3] = 1;
    return (frm);
  }
  /*********************************************************/
#define FNAME "RecalibCamera"
  int RecalibCamera(camera cam, Frame* f)
  {
    int i, j;
    double a[3][4];
    double norm;
    Frame f1;
    /*Kameramatrix mit f(invers) multiplizieren*/
    InvertFrame(f, &f1);
    MulMatrix((double*)cam->a, (double*)f1.frame, 3, 4, 4, (double*)a);

    /*Normierung*/
    if (fabs(a[2][3]) > 1e-5)
      {
        norm = a[2][3];
      }
    else
      {
        for (i = 0, norm = 0; i < 4; i++)
          {
            norm += a[2][i] * a[2][i];
          }

        norm = sqrt(norm);
      }

    for (i = 0; i < 3; i++)
      for (j = 0; j < 4; j++)
        {
          cam->a[i][j] = a[i][j] / norm;
        }

    /*Sehstrahlvektoren transformieren*/
    for (i = 0; i < 3; i++)
      {
        FTransVec(cam->ac[i], f, cam->ac[i]);
      }

    /*Projektionszentrum transformieren*/
    FTransPoint(cam->c, f, cam->c);
    return (OK);
  }
#undef FNAME
  /*********************************************************/
  int CopyPointCorr(PointCorr* pcs, PointCorr* pcd)
  {
    pcd->pw[0] = pcs->pw[0];
    pcd->pw[1] = pcs->pw[1];
    pcd->pw[2] = pcs->pw[2];
    pcd->pc[0] = pcs->pc[0];
    pcd->pc[1] = pcs->pc[1];
    pcd->w = pcs->w;
    pcd->status = pcs->status;
    return (OK);
  }
#endif
  /*********************************************************/
  int CalcPlaneFrame(double* p, int n, Frame* f)
  {
    int i, j;
    double base[4][3];
    double l;
    MoveVec(p, base[3]);
    SubVec(p + 3, base[3], base[0]);
    l = 0;

    for (i = 2; i < n && l < 1e-15; i++)
      {
        SubVec(p + 3 * i, base[3], base[1]);
        CrossProdVec(base[0], base[1], base[2]);
        l = LengthVec(base[2]);
      }

    if ((i == n) && (l < 1e-15))
      {
        return (ERROR);
      }

    NormVec(base[0], base[0]);
    NormVec(base[2], base[2]);
    CrossProdVec(base[2], base[0], base[1]);

    for (i = 0; i < 3; i++)
      {
        for (j = 0; j < 4; j++)
          {
            f->frame[i][j] = base[j][i];
          }

        f->frame[3][i] = 0;
      }

    f->frame[3][3] = 1;
    return (OK);
  }
  /*********************************************************/
#define FNAME "WriteFrame"
  int WriteFrame(Frame* f, char* file)
  {
    int i, j;
    FILE* stream;
    stream = fopen(file, "w");

    if (stream == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    fprintf(stream, "%%P\n");

    for (i = 0; i < 4; i++)
      {
        for (j = 0; j < 4; j++)
          {
            fprintf(stream, "%10.5f  ", f->frame[i][j]);
          }

        fprintf(stream, "\n");
      }

    fclose(stream);
    return (OK);
  }
#undef FNAME
  /*********************************************************/
#define FNAME "ReadFrame"
  int ReadFrame(char* file, Frame* f)
  {
    int i;
    FILE* stream;
    char line[100];
    double* frp;
    stream = fopen(file, "r");

    if (stream == nullptr)
      throw IceException(FNAME, M_FILE_OPEN);

    do
      {
        if (fgets(line, 100, stream) == nullptr)
          throw IceException(FNAME, M_WRONG_FILE);
      }
    while (line[0] != '%' || line[1] != 'P');

    for (i = 0; i < 4; i++)
      {
        if (fgets(line, 100, stream) == nullptr)
          throw IceException(FNAME, M_WRONG_FILE);

        frp = &f->frame[i][0];

        if (sscanf(line, "%le%le%le%le", frp, frp + 1, frp + 2, frp + 3) != 4)
          throw IceException(FNAME, M_WRONG_FILE);
      }

    fclose(stream);
    return (OK);
  }
#undef FNAME
  /*********************************************************/
#define FNAME "MakeFrame"
  Frame* MakeFrame(int mode, double par[6], Frame* frm)
  /*Erzeugung einer Frame-Repraesentation aus einer Euler-Repraesentation*/
  /* mode = 0 : Rcz*Rby*Rax */
  /* mode = 1 : Rax*Rby*Rcz */
  /* mode = 2 : Raz*Rby*Rcx (Roboter manutec r2)*/
  /* mode = 3 : Raz*Rby*Rcz (Roboter stäubli rx90 == yaw/pitch/roll)*/
  {
    double sa, ca, sb, cb, sc, cc;
    int i;
    sa = sin(par[0]);
    ca = cos(par[0]);
    sb = sin(par[1]);
    cb = cos(par[1]);
    sc = sin(par[2]);
    cc = cos(par[2]);

    switch (mode)
      {
      case 0:
        frm->frame[0][0] = cb * cc;
        frm->frame[0][1] = sa * sb * cc - ca * sc;
        frm->frame[0][2] = ca * sb * cc + sa * sc;
        frm->frame[1][0] = cb * sc;
        frm->frame[1][1] = sa * sb * sc + ca * cc;
        frm->frame[1][2] = ca * sb * sc - sa * cc;
        frm->frame[2][0] = -sb;
        frm->frame[2][1] = sa * cb;
        frm->frame[2][2] = ca * cb;
        break;
      case 1:
        frm->frame[0][0] = cb * cc;
        frm->frame[0][1] = -cb * sc;
        frm->frame[0][2] = sb;
        frm->frame[1][0] = ca * sc + sa * sb * cc;
        frm->frame[1][1] = ca * cc - sa * sb * sc;
        frm->frame[1][2] = -sa * cb;
        frm->frame[2][0] = sa * sc - ca * sb * cc;
        frm->frame[2][1] = sa * cc + ca * sb * sc;
        frm->frame[2][2] = ca * cb;
        break;
      case 2:
        frm->frame[0][0] = ca * cb;
        frm->frame[0][1] = ca * sb * sc - sa * cc;
        frm->frame[0][2] = ca * sb * cc + sa * sc;
        frm->frame[1][0] = sa * cb;
        frm->frame[1][1] = sa * sb * sc + ca * cc;
        frm->frame[1][2] = sa * sb * cc - ca * sc;
        frm->frame[2][0] = -sb;
        frm->frame[2][1] = cb * sc;
        frm->frame[2][2] = cb * cc;
        break;
      case 3:
        frm->frame[0][0] = ca * cb * cc - sa * sc;
        frm->frame[0][1] = -ca * cb * sc - sa * cc;
        frm->frame[0][2] = ca * sb;
        frm->frame[1][0] = sa * cb * cc + ca * sc;
        frm->frame[1][1] = -sa * cb * sc + ca * cc;
        frm->frame[1][2] = sa * sb;
        frm->frame[2][0] = -sb * cc;
        frm->frame[2][1] = sb * sc;
        frm->frame[2][2] = cb;
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }

    for (i = 0; i < 3; i++)
      {
        frm->frame[i][3] = par[i + 3];
        frm->frame[3][i] = 0;
      }

    frm->frame[3][3] = 1;
    return (frm);
  }
#undef FNAME
  /*********************************************************/
#define FNAME "MakeEuler"
  int MakeEuler(int mode, Frame* f, double par1[6], double par2[6])
  /*Umwandlung einer Frame-Repraesentation in die zug. Eulerrepraesentation*/
  /*   mode = 0 : Rcz*Rby*Rax */
  /*   mode = 1 : Rax*Rby*Rcz */
  /*   mode = 2 : Raz*Rby*Rcx (Roboter manutec r2) */
  {
    int i;
    double sa, ca;

    switch (mode)
      {
      case 0:
        par1[0] = atan2(f->frame[2][1], f->frame[2][2]);

        if (par1[0] > 0)
          {
            par2[0] = par1[0] - M_PI;
          }
        else
          {
            par2[0] = par1[0] + M_PI;
          }

        sa = sin(par1[0]);

        if (fabs(sa) > 0.5)
          {
            par1[1] = atan2(-f->frame[2][0], f->frame[2][1] / sa);
            par2[1] = atan2(-f->frame[2][0], -f->frame[2][1] / sa);
          }
        else
          {
            ca = cos(par1[0]);
            par1[1] = atan2(-f->frame[2][0], f->frame[2][2] / ca);
            par2[1] = atan2(-f->frame[2][0], -f->frame[2][2] / ca);
          }

        par1[2] = atan2(f->frame[1][0], f->frame[0][0]);

        if (par1[2] > 0)
          {
            par2[2] = par1[2] - M_PI;
          }
        else
          {
            par2[2] = par1[2] + M_PI;
          }

        break;
      case 1:
        par1[2] = atan2(-f->frame[0][1], f->frame[0][0]);

        if (par1[2] > 0)
          {
            par2[2] = par1[2] - M_PI;
          }
        else
          {
            par2[2] = par1[2] + M_PI;
          }

        sa = sin(par1[2]);

        if (fabs(sa) > 0.5)
          {
            par1[1] = atan2(f->frame[0][2], -f->frame[0][1] / sa);
            par2[1] = atan2(f->frame[0][2], f->frame[0][1] / sa);
          }
        else
          {
            ca = cos(par1[2]);
            par1[1] = atan2(f->frame[0][2], f->frame[0][0] / ca);
            par2[1] = atan2(f->frame[0][2], -f->frame[0][0] / ca);
          }

        par1[0] = atan2(-f->frame[1][2], f->frame[2][2]);

        if (par1[0] > 0)
          {
            par2[0] = par1[0] - M_PI;
          }
        else
          {
            par2[0] = par1[0] + M_PI;
          }

        break;
      case 2:
        par1[2] = atan2(f->frame[2][1], f->frame[2][2]);

        if (par1[2] > 0)
          {
            par2[2] = par1[2] - M_PI;
          }
        else
          {
            par2[2] = par1[2] + M_PI;
          }

        sa = sin(par1[2]);

        if (fabs(sa) > 0.5)
          {
            par1[1] = atan2(-f->frame[2][0], f->frame[2][1] / sa);
            par2[1] = atan2(-f->frame[2][0], -f->frame[2][1] / sa);
          }
        else
          {
            ca = cos(par1[2]);
            par1[1] = atan2(-f->frame[2][0], f->frame[2][2] / ca);
            par2[1] = atan2(-f->frame[2][0], -f->frame[2][2] / ca);
          }

        par1[0] = atan2(f->frame[1][0], f->frame[0][0]);

        if (par1[0] > 0)
          {
            par2[0] = par1[0] - M_PI;
          }
        else
          {
            par2[0] = par1[0] + M_PI;
          }

        break;
      case 3:

        if (fabs(f->frame[1][2]) < 1e-10 && fabs(f->frame[0][2]) < 1e-10)
          {
            par1[1] = acos(f->frame[2][2]);
            par2[1] = par1[1];
            par1[0] = atan2(f->frame[1][0], f->frame[1][1]);
            par2[0] = par1[0];
            par1[2] = par2[2] = 0;
          }
        else
          {
            par1[0] = atan2(f->frame[1][2], f->frame[0][2]);

            if (par1[0] > 0)
              {
                par2[0] = par1[0] - M_PI;
              }
            else
              {
                par2[0] = par1[0] + M_PI;
              }

            sa = sin(par1[0]);

            if (fabs(sa) > 0.5)
              {
                par1[1] = atan2(f->frame[1][2] / sa, f->frame[2][2]);
                par2[1] = atan2(-f->frame[1][2] / sa, f->frame[2][2]);
              }
            else
              {
                ca = cos(par1[0]);
                par1[1] = atan2(f->frame[0][2] / ca, f->frame[2][2]);
                par2[1] = atan2(-f->frame[0][2] / ca, f->frame[2][2]);
              }

            par1[2] = atan2(f->frame[2][1], -f->frame[2][0]);

            if (par1[2] > 0)
              {
                par2[2] = par1[2] - M_PI;
              }
            else
              {
                par2[2] = par1[2] + M_PI;
              }
          }

        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }

    for (i = 0; i < 3; i++)
      {
        par1[i + 3] = f->frame[i][3];
        par2[i + 3] = f->frame[i][3];
      }

    return (OK);
  }
#undef FNAME
  /*********************************************************/
#define FNAME "OrthonormalizeFrame"
  Frame* OrthonormalizeFrame(Frame* frm)
  {
    double hv1[3], hv2[3];
    double l;
    l = LengthVec(frm->frame[0]);

    if (l < 1e-15)
      throw IceException(FNAME, RM_NO_BASIS);

    ScaleVec(frm->frame[0], 1 / l, frm->frame[0]);
    ScaleVec(frm->frame[0], ScalProdVec(frm->frame[1], frm->frame[0]), hv1);
    SubVec(frm->frame[1], hv1, frm->frame[1]);
    l = LengthVec(frm->frame[1]);

    if (l < 1e-15)
      throw IceException(FNAME, RM_NO_BASIS);

    ScaleVec(frm->frame[1], 1 / l, frm->frame[1]);
    ScaleVec(frm->frame[0], ScalProdVec(frm->frame[2], frm->frame[0]), hv1);
    ScaleVec(frm->frame[1], ScalProdVec(frm->frame[2], frm->frame[1]), hv2);
    SubVec(frm->frame[2], hv1, frm->frame[2]);
    SubVec(frm->frame[2], hv2, frm->frame[2]);
    l = LengthVec(frm->frame[2]);

    if (l < 1e-15)
      throw IceException(FNAME, RM_NO_BASIS);

    ScaleVec(frm->frame[2], 1 / l, frm->frame[2]);
    return (frm);
  }
#undef FNAME
  /************************************************/
  int MakeAxleFrame(double axle[3], double phi, Frame* f)
  {
    /*Drehung um eine (normierte) Drehachse*/
    double cc, cc1, ss;
    int i;
    cc = cos(phi);
    ss = sin(phi);
    cc1 = 1 - cc;
    f->frame[0][0] = cc + axle[0] * axle[0] * cc1;
    f->frame[0][1] = axle[2] * ss + axle[0] * axle[1] * cc1;
    f->frame[0][2] = -axle[1] * ss + axle[0] * axle[2] * cc1;
    f->frame[1][0] = -axle[2] * ss + axle[0] * axle[1] * cc1;
    f->frame[1][1] = cc + axle[1] * axle[1] * cc1;
    f->frame[1][2] = axle[0] * ss + axle[1] * axle[2] * cc1;
    f->frame[2][0] = axle[1] * ss + axle[0] * axle[2] * cc1;
    f->frame[2][1] = -axle[0] * ss + axle[1] * axle[2] * cc1;
    f->frame[2][2] = cc + axle[2] * axle[2] * cc1;

    for (i = 0; i < 3; i++)
      {
        f->frame[i][3] = 0;
        f->frame[3][i] = 0;
      }

    f->frame[3][3] = 1;
    return (OK);
  }

  int FrameInit(Frame* f)
  {
    int i, j;

    for (i = 0; i < 4; i++)
      {
        for (j = 0; j < 4; j++)
          {
            if (i == j)
              {
                f->frame[i][j] = 1;
              }
            else
              {
                f->frame[i][j] = 0;
              }
          }
      }

    return 0;
  }

  int TrafoToFrame(const Trafo& t, Frame* f)
  {
    int i, j;

    for (i = 0; i < 4; i++)
      {
        for (j = 0; j < 4; j++)
          {
            f->frame[i][j] = t(i, j);
          }
      }

    return 0;
  }

  int FrameToTrafo(Frame* f, Trafo& t)
  {
    int i, j;

    for (i = 0; i < 4; i++)
      {
        for (j = 0; j < 4; j++)
          {
            t(i, j) = f->frame[i][j];
          }
      }

    return 0;
  }
}
