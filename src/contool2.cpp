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
//   Funktionen mit Konturen
//
//   Ortmann 10/99, 8/02
//   m. schubert 11/93
//   neubauer 3/93
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <iostream>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "based.h"

#include "freeman.h"
#include "conturfunctions.h"
#include "contfeat.h"
#include "numbase.h"
#include "gentrans.h"
#include "analygeo.h"
#include "trans.h"
#include "contools.h"

//
namespace ice
{
#define FNAME "CircleContur"
  Contur CircleContur(double* par)
  {
    int x, y, pm[2], s, r;
    Contur c;
    char* data;
    int i, j;

    pm[0] = RoundInt(par[0]);
    pm[1] = RoundInt(par[1]);
    r = RoundInt(par[2]);

    data = (char*)malloc(8 * r * sizeof(char));

    if (data == NULL)
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return c;
      }

    c.SetStart(pm[0] + r, pm[1]);

    // Bresenham im 1.Quadranten
    i = 0;
    s = 0;
    x = r;

    for (y = 0; y < r; y++)                     // 1. Quadrant
      {
        s += 4 * y + 2;

        if (s > 1)
          {
            s += -4 * x + 2;
            x--;
            data[i++] = 3;
            c.Add(3);

            while (s > 1)
              {
                s += -4 * x + 2;
                x--;
                data[i++] = 4;
                c.Add(4);
              }
          }
        else
          {
            data[i++] = 2;
            c.Add(2);
          }
      }

    // weitere Quadranten durch Kopie und Spiegelung

    for (j = 1; j <= i; j++)                   // 2. Quadrant
      {
        c.Add(8 - data[i - j]);
      }

    for (j = 0; j < i; j++)                    // 3. Quadrant
      {
        c.Add((data[j] + 4) % 8);
      }

    for (j = 1; j <= i; j++)                   // 4. Quadrant
      {
        c.Add(((8 - data[i - j]) + 4) % 8);
      }

    return c;
  }
#undef FNAME
  /*******************************************************************/
  typedef struct
  {
    double xm, ym;
    double rr;
    double phi_a, phi_e;
  } CSparam;

  int cs_cls(int x, int y, void* param)
  {
    CSparam* e = (CSparam*)param;
    double phi, f;
    phi = atan2(y - e->ym, x - e->xm);

    if (phi < e->phi_a)
      {
        phi += 2 * M_PI;
      }

    if (phi > e->phi_e)
      {
        return -4;
      }

    f = Sqr(x - e->xm) + Sqr(y - e->ym) - e->rr;

    if (f <= 0)
      {
        return 0;
      }

    return -1;
  }

#define FNAME "CircleSegmentContur"
  Contur CircleSegmentContur(double* par)
  {
    Contur c;
    CSparam e;
    int ps[2], i;
    int xf, yf;

    if (fabs(par[3] - par[4]) > 2 * M_PI)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return c;
      }

    e.xm = par[0];
    e.ym = par[1];
    e.rr = par[2] * par[2];
    e.phi_a = par[3];
    e.phi_e = par[4];

    while (e.phi_a < -M_PI)
      {
        e.phi_a += 2 * M_PI;
        e.phi_e += 2 * M_PI;
      }

    while (e.phi_a > M_PI)
      {
        e.phi_a -= 2 * M_PI;
        e.phi_e -= 2 * M_PI;
      }

    if (e.phi_a > e.phi_e)
      {
        e.phi_e += 2 * M_PI;
      }

    /*Startpunkt bestimmen*/
    ps[0] = RoundInt(par[2] * cos(e.phi_a) + par[0]);
    ps[1] = RoundInt(par[2] * sin(e.phi_a) + par[1]);

    if ((e.rr < 2) || ((e.phi_e - e.phi_a)*par[2] < 2))
      {
        // entartet zu einem Punkt
        c.SetStart(ps[0], ps[1]);
        return c;
      }

    if (cs_cls(ps[0], ps[1], &e) < 0)
      // Untergrund-Punkt: in der Nachbarschaft nach Objektpunkt suchen!
      {
        for (i = 0; i < 8; i++)
          {
            Freeman(i).move(ps[0], ps[1], xf, yf);

            if (cs_cls(xf, yf, &e) == 0)
              {
                break;
              }
          }

        Freeman(i).move(ps[0], ps[1]);
      }

    IF_FAILED(c = GetContur(ps, cs_cls, &e, 0))
    {
      Message(FNAME, M_0, ERROR);
      return c;
    }
    return c;
  }
#undef FNAME
  /*******************************************************************/
  typedef struct
  {
    double xm, ym;
    double cc, ss, aa, bb;
    double phi_a, phi_e;
  } ESparam;

  int es_cls(int x, int y, void* param)
  {
    ESparam* e;
    double phi, f;
    e = (ESparam*)param;
    phi = atan2(y - e->ym, x - e->xm);

    if (phi < e->phi_a)
      {
        phi += 2 * M_PI;
      }

    if (phi > e->phi_e)
      {
        return -4;
      }

    f = Sqr((x - e->xm) * e->cc + (y - e->ym) * e->ss) / e->aa + \
        Sqr(-(x - e->xm) * e->ss + (y - e->ym) * e->cc) / e->bb - 1;

    if (f <= 0)
      {
        return (0);
      }

    return (-1);
  }

#define FNAME "EllipseSegmentContur"
  Contur EllipseSegmentContur(double* par)
  {
#define XM par[0]
#define YM par[1]
#define RA par[2]
#define RI par[3]
#define PHI par[4]
#define PHI1 par[5]
#define PHI2 par[6]

    Contur c;

    ESparam e;

    int ps[2], i;
    double phi, xx, yy;
    int xf, yf;

    if (fabs(PHI1 - PHI2) > 2 * M_PI ||
        fabs(PHI1 - PHI2)*RA < 1)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return c;
      }

    e.xm = XM;
    e.ym = YM;
    e.cc = cos(PHI);
    e.ss = sin(PHI);
    e.aa = RA * RA;
    e.bb = RI * RI;
    e.phi_a = PHI1 + PHI;
    e.phi_e = PHI2 + PHI;

    while (e.phi_a < -M_PI)
      {
        e.phi_a += 2 * M_PI;
        e.phi_e += 2 * M_PI;
      }

    while (e.phi_a > M_PI)
      {
        e.phi_a -= 2 * M_PI;
        e.phi_e -= 2 * M_PI;
      }

    if (e.phi_a > e.phi_e)
      {
        e.phi_e += 2 * M_PI;
      }

    /*Startpunkt bestimmen*/
    phi = PHI1;
    phi = atan2(RA * sin(phi), RI * cos(phi));
    xx = RA * cos(phi);
    yy = RI * sin(phi);
    ps[0] = RoundInt(xx * cos(PHI) - yy * sin(PHI) + XM);
    ps[1] = RoundInt(xx * sin(PHI) + yy * cos(PHI) + YM);

    if (es_cls(ps[0], ps[1], &e) < 0)
      {
        for (i = 0; i < 8; i++)
          {
            Freeman(i).move(ps[0], ps[1], xf, yf);

            if (es_cls(xf, yf, &e) == 0)
              {
                break;
              }
          }

        Freeman(i).move(ps[0], ps[1]);
      }

    IF_FAILED(c = GetContur(ps, es_cls, &e, 0))
    {
      Message(FNAME, M_0, ERROR);
      return c;
    }
    return c;
  }
#undef FNAME
  /*******************************************************************/
  typedef struct
  {
    double xm, ym;
    double cc, ss, aa, bb;
  } Eparam;

  int e_cls(int x, int y, void* param)
  {
    Eparam* e;
    double f;
    e = (Eparam*)param;
    f = Sqr((x - e->xm) * e->cc + (y - e->ym) * e->ss) / e->aa +  \
        Sqr(-(x - e->xm) * e->ss + (y - e->ym) * e->cc) / e->bb - 1;

    if (f <= 0)
      {
        return 0;
      }

    return -1;
  }

#define FNAME "EllipseContur"
  Contur EllipseContur(double* par)
  {
    Contur c(RoundInt(par[0]), RoundInt(par[1]));

    if ((par[2] < 1.0) && (par[3] < 1.0))
      {
        return c;
      }

    Eparam e;
    int ps[2];
    e.xm = par[0];
    e.ym = par[1];
    e.cc = cos(par[4]);
    e.ss = sin(par[4]);
    e.aa = par[2] * par[2];
    e.bb = par[3] * par[3];
    // std::cout << par[2] << " " << par[3] << std::endl;
    /*Startpunkt bestimmen*/
    ps[0] = (int)par[0];
    ps[1] = (int)par[1];

    while (e_cls(ps[0], ps[1], &e) == 0)
      {
        ps[0]++;
      }

    ps[0]--;

    IF_FAILED(c = GetContur(ps, e_cls, &e, 0))
    {
      Message(FNAME, M_0, ERROR);
      return c;
    }
    return c;
  }
#undef FNAME
}
