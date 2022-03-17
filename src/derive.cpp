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

/*******************************************************************
  derive.c
Partielle Ableitungen von mehrdimensionalen Funktionen:
  DeriveFunc
  Derive2Func
Numerische Integration eindimensionaler Funktionen:
*******************************************************************/

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "equsys.h"
#include "numbase.h"

#include "derive.h"

namespace ice
{
  /*******************************************************************
    Numerische Berechnung der partiellen Ableitung
  *******************************************************************/
  int DeriveFunc(FuncD func, int dim, int i,
                 double* x, double h, int mode, double* val)
  /*
    Es wird die "i"-te partielle Ableitung einer "dim"-dimensionalen Funktion
    ("func" - Pointer auf Funktion) an der stelle "x[]". Partielle Ableitung wird
    auf "val" bereitgestellt.
    Der Parameter "mode" charakterisiert die Wahl der Stüzstellen die im Abstand von
    "h" gewählt werden:
    mode=5 - Stützstellen:  0, 1
    mode=4 - Stützstellen:  0, 1, 2
    mode=3 - Stützstellen:  0, 1, 2, 3
    mode=2 - Stützstellen: -1, 0, 1
    mode=1 - Stützstellen: -1, 0, 1, 2
    mode=0 - Stützstellen: -2,-1, 0, 1, 2
  */
  {
    double* hptrx0;

    /*--- Parametertest ----------------------------------------------*/
    if (dim < 1)
      {
        throw IceException("DeriveFunc", M_WRONG_DIMENSION);
      }

    if (i > dim || i < 1)
      {
        throw IceException("DeriveFunc", M_WRONG_INDEX);
      }

    if (h <= 0.0)
      {
        throw IceException("DeriveFunc", M_WRONG_PARAMETER);
      }

    if (mode > 5 || mode < 0)
      {
        throw IceException("DeriveFunc", M_WRONG_MODE);
      }

    /*----------------------------------------------------------------*/
    hptrx0 = (double*)malloc(dim * sizeof(double));
    *val = 0;

    switch (mode)
      {
      case 0: /* (-2,-1,0,1,2) */
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val -= func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += 8.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= h;
        *val -= 8.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= 2 * h;
        *val += func(hptrx0);

        *val /= 12 * h;
        break;

      case 1: /* (-1,0,1,2) */
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val -= func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += 6.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= 3.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= h;
        *val -= 2.0 * func(hptrx0);
        *val /= 6 * h;
        break;
      case 2: /* (-1,0,1) */
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= h;
        *val -= func(hptrx0);

        *val /= 2 * h;
        break;
      case 3: /* (0,1,2,3) */
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 3 * h;
        *val += 2.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val -= 9.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += 18.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= 11.0 * func(hptrx0);

        *val /= 6 * h;
        break;
      case 4: /* (0,1,2) */
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val -= func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += 4.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= 3.0 * func(hptrx0);
        *val /= 2 * h;
        break;
      case 5: /* (0,1) */
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += func(hptrx0);
        /*
          printf("------\n");
          WriteVecRn("hptrx0",hptrx0,1);
          printf("deriv: %f\n",*val);
          getch();
        */

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= func(hptrx0);
        *val /= h;
        /*
          printf("------\n");
          WriteVecRn("hptrx0",hptrx0,1);
          printf("deriv: %f\n",*val);
          getch();
        */
        break;
      }

    free(hptrx0);
    return OK;
  }

  /*******************************************************************/
  /*         Numerische Berechnung der Differenzenquotienten         */
  /*         2. Ordnung                              */
  /*                        huefner 1/94                             */
  /*******************************************************************/

  int Derive2Func(FuncD func, int dim, int i, double* x, double h, int mode, double* val)

  /*
    Es wird die "2"-te partielle Ableitung einer "dim"-dimensionalen Funktion
    ("func" - Pointer auf Funktion) an der stelle "x[]".
    Partielle Ableitung wird auf "val" bereitgestellt.
    Der Parameter "mode" charakterisiert die Wahl der Stützstellen, die im
    Abstand von "h" gewählt werden:
    mode=4 - Stützstellen:  0, 1, 2
    mode=3 - Stützstellen:  0, 1, 2, 3
    mode=2 - Stützstellen: -1, 0, 1
    mode=1 - Stützstellen: -1, 0, 1, 2, 3
    mode=0 - Stützstellen: -2,-1, 0, 1, 2
  */

  {
    double* hptrx0;

    /*--- Parametertest ----------------------------------------------*/
    if (dim < 1)
      {
        throw IceException("DeriveFunc", M_WRONG_DIMENSION);
      }

    if (i > dim || i < 1)
      {
        throw IceException("DeriveFunc", M_WRONG_INDEX);
      }

    if (h <= 0.0)
      {
        throw IceException("DeriveFunc", M_WRONG_PARAMETER);
      }

    if (mode > 4 || mode < 0)
      {
        throw IceException("DeriveFunc", M_WRONG_MODE);
      }

    /*----------------------------------------------------------------*/

    hptrx0 = (double*)malloc(dim * sizeof(double));
    *val = 0;

    switch (mode)
      {

      case 0: /*(-2,-1,0,1,2) (-y[2]+16*y[1]-30*y[0]-16*y[-1]-y[-2])/(12*h*h)*/
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val -= func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += 16.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= 30.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= h;
        *val += 16.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= 2 * h;
        *val -= func(hptrx0);

        *val /= 12 * h * h;
        break;

      case 1: /* (-1,0,1,2,3) (-y[3]+4*y[2]+6*y[1]-20*y[0]+11*y[-1])/(12*h*h)*/
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 3 * h;
        *val -= func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val += 4.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += 6.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= 20.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= h;
        *val += 11.0 * func(hptrx0);

        *val /= 12 * h * h;
        break;

      case 2: /* (-1,0,1) (y[1]-2*y[0]+y[-1])/(h*h)*/
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val += func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val -= 2 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] -= h;
        *val += func(hptrx0);

        *val /= h * h;
        break;

      case 3: /* (0,1,2,3) (-y[3]+4*y[2]-5*y[1]+2*y[0])/(h*h)*/
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 3 * h;
        *val -= func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val += 4.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val -= 5.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val += 2.0 * func(hptrx0);

        *val /= h * h;
        break;

      case 4: /* (0,1,2) (y[2]-2*y[1]+y[0])/(h*h)*/
        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += 2 * h;
        *val += func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        hptrx0[i - 1] += h;
        *val -= 2.0 * func(hptrx0);

        memcpy(hptrx0, x, dim * sizeof(double));
        *val += func(hptrx0);

        *val /= h * h;
        break;
      }

    free(hptrx0);
    return OK;
  }

  /******************************************************************
           Numerische Berechnung des Integrals nach der
              Romberg-Methode
                          huefner 1/94
  *******************************************************************/
#define FNAME "IntegrateFunc"
  int IntegrateFunc(FuncDV func, double down, double top,
                    double* prec, double* val)
  {

    /*
      Es wird das Integral zwischen a und b mit der Genauigkeit prec von der
      Funktion f(x) ausgerechnet.
      ("func" - Pointer auf Funktion) an der Stelle "x[]".
      Der Wert des Integrals wird auf "val" bereitgestellt.
    */

    //    int    j, k, K, n, N;
    int    N2[11] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
    int    N4[9] = {1, 4, 16, 64, 256, 1024, 4096, 16384, 65536};

    double Sf[10][11];
    double dif = 0;

    int K = 8;
    int N = 10;

    /*--- Parametertest ----------------------------------------------*/
    if (*prec >= 1)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (down > top)
      {
        std::swap(top, down);
      }

    double range = top - down;
    double f2 = 0.5 * (func(down) + func(top));
    Sf[0][0] = f2 * range;

    for (int n = 1; n < N; ++n)
      {
        double f = f2;
        double step = range / N2[n];
        double x = down + step;
        for (int j = 1; j < N2[n]; j++)
          {
            f = f + func(x);
            x += step;
          }

        Sf[0][n] = f * step;
      }

    for (int k = 0; k < K - 1; k++)
      {
        for (int n = k; n < N - 1; n++)
          {
            Sf[k + 1][n + 1] = (N4[k + 1] * Sf[k][n + 1] - Sf[k][n]) / (N4[k + 1] - 1);
            dif = Sf[k + 1][n + 1] - Sf[k][n + 1];

            if (dif < *prec)
              {
                *prec = dif;
                *val = Sf[k + 1][n + 1];
                return OK;
              }
          }
      }

    *prec = dif;
    *val = Sf[K][N];
    return -3;              /* TO_LARGE;*/
  }
}
