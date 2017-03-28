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
//  rooteig.c
//Eigenwerte:
//  EigenVal
//  TestEigenVal
//

#include <math.h>
#include <stdlib.h>

#include "message.h"
#include "macro.h"
#include "matrix_function.h"
#include "vectorarrayfunctions.h"

//
namespace ice
{
#define FNAME "EigenVal"
#define DEBUG  0

  int EigenVal(double* a, int dim, double* eval, double* evec)
  {
    double* am, *x0, *xt, *xh;
    double* h1, *h2, *h3;
    double* zplus;
    double* zminus;
    double* vec1;

    int i, i1, i2, i3;
    double rayl, rel, h;
    double eps = 1e-15, eps1 = 1e-15, eps2 = 1e-30;
    int flag;
    double l, l1, l2;
    int anz = 10000, anz1 = 50;

    am = (double*)malloc(dim * dim * sizeof(double));
    x0 = (double*)malloc(dim * sizeof(double));
    xt = (double*)malloc(dim * sizeof(double));
    h1 = (double*)malloc(dim * sizeof(double));
    h2 = (double*)malloc(dim * sizeof(double));
    h3 = (double*)malloc(dim * sizeof(double));
    zplus = (double*)malloc(dim * sizeof(double));
    zminus = (double*)malloc(dim * sizeof(double));
    xh = (double*)malloc(dim * sizeof(double));
    vec1 = (double*)malloc(dim * sizeof(double));

#if DEBUG
    printf("dim %d\n", dim);
    PrintMatrix("Eigenval-> a:", a, dim, dim);
    getchar();
#endif

    TranspMatrix(a, dim, dim, am);
    AddMatrix(a, (double*)am, dim, dim, am);
    ScaleMatrix(am, dim, dim, 0.5, am);

    /* Iteration nach Mieses */
    for (i1 = 0; i1 < dim; i1++)
      {
        i2 = 0;
        flag = 0;

        // Berechnung des nächsten Anfangsvektors
        while (flag == 0 && i2 < dim)
          {
            for (i3 = 0; i3 < dim; i3++) x0[i3] = 0;

            x0[i2] = 1;
            MulMatrix((double*)am, x0, dim, dim, 1, xt);

            if ((l = LengthVecRn(xt, dim)) > eps1) flag++;

            i2++;
          }

#if DEBUG
        PrintVecRn("Naechster Startvektor", xt, dim);
        getchar();
#endif

        if (flag == 0)
          {
            free(am);
            free(x0);
            free(xt);
            free(xh);
            free(h1);
            free(h2);
            free(h3);
            free(zplus);
            free(zminus);
            free(vec1);
            Message(FNAME, M_NO_REGULAR, NO_REGULAR);
            return NO_REGULAR;
          }

        NormVecRn(xt, dim, xt);
        rel = 1;
        i2 = 0;

        while (rel > eps && i2 < anz)
          {
            if (i2 > anz1)
              {
                rayl = 0;

                for (i3 = 0; i3 < dim; i3++)
                  rayl += h1[i3] * xh[i3];

                ScaleVecRn(xh, dim, rayl, h2);
                SubVecRn(h1, h2, dim, h3);
                rel = LengthVecRn(h3, dim) / LengthVecRn(h1, dim);
              }

            MoveVecRn(xt, dim, xh);
            MulMatrix(am, xt, dim, dim, 1, h1);
            MoveVecRn(h1, dim, xt);
            NormVecRn(xt, dim, xt);
#if DEBUG
            printf("i1: %d i2: %d\n", i1, i2);
            PrintVecRn("xt", xt, dim);
            printf("rel: %g \n ", rel);
            printf("----------------\n");
            getchar();
#endif
            i2++;
          }

        SubVecRn(xt, xh, dim, zminus);
        AddVecRn(xt, xh, dim, zplus);
        l = LengthVecRn(h1, dim);

        if (l < eps2)
          {
            free(am);
            free(x0);
            free(xt);
            free(xh);
            free(h1);
            free(h2);
            free(h3);
            free(zplus);
            free(zminus);
            free(vec1);
#if DEBUG
            printf("l %f\n", l);
            PrintVecRn("h1", h1, dim);
            getchar();
#endif
            Message(FNAME, M_NO_REGULAR, NO_REGULAR);
            return NO_REGULAR;
          }

        l1 = LengthVecRn(zplus, dim);
        l2 = LengthVecRn(zminus, dim);

        if (l1 > l2)
          {
            eval[i1] = l;
            NormVecRn(zplus, dim, zplus);
            MoveVecRn(zplus, dim, evec + i1 * dim);
          }
        else
          {
            eval[i1] = -l;
            NormVecRn(zminus, dim, zminus);
            MoveVecRn(zminus, dim, evec + i1 * dim);
          }

#if DEBUG
        PrintMatrix("evec", evec, dim, dim);
        printf("i1: %d eval[i1] %g \n ", i1, eval[i1]);
        getchar();
#endif

        /* Generierung der naechsten Iterationsmatrix */
        for (i2 = 0; i2 < dim; i2++)
          for (i3 = 0; i3 < dim; i3++)
            *(am + i2 * dim + i3) -= eval[i1] * xt[i2] * xt[i3];

#if DEBUG
        PrintMatrix("Naechste Iterationsmatrix", am, dim, dim);
        getchar();
#endif
      }

    /* Ordnen der Eigenwerte und Eigenvektoren */
    flag = 1;

    while (flag != 0)
      {
        flag = 0;

        for (i = 0; i < dim - 1; i++)
          if (eval[i] < eval[i + 1])
            {
              flag++;
              h = eval[i];
              eval[i] = eval[i + 1];
              eval[i + 1] = h;
              MoveVecRn((double*)(evec + i * dim), dim, vec1);
              MoveVecRn((double*)(evec + (i + 1)*dim), dim, (double*)(evec + i * dim));
              MoveVecRn(vec1, dim, (double*)(evec + (i + 1)*dim));
            }
      }

    free(am);
    free(x0);
    free(xt);
    free(xh);
    free(h1);
    free(h2);
    free(h3);
    free(zplus);
    free(zminus);
    free(vec1);
    return OK;
  }
#undef DEBUG
#undef FNAME

  /*******************************************************************/
#define DEBUG
  int TestEigenVal
  (double* a, double* eval, double* evec, int dim, double* tvec, double* norm)
  {
    double val;
    int i, j;
    double* ah;
    ah = (double*)malloc(dim * dim * sizeof(double));

    for (i = 0; i < dim; i++)
      {
        MoveMatrix(a, dim, dim, ah);

        for (j = 0; j < dim; j++)
          ah[j * dim + j] -= eval[i];

        MulMatrix
        ((double*)ah, (double*)evec + i * dim, dim, dim, 1, (double*)tvec + i * dim);
#if defined DEBUG
        PrintVecRn("tvec", (double*)tvec + i * dim, dim);
        getchar();
#endif
      }

    NormMatrix((double*)tvec, dim, dim, DEFAULT, &val);

    if (val > *norm)
      {
        *norm = val;
        free(ah);
        return false;
      }

    *norm = val;
    free(ah);
    return true;
  }
#undef DEBUG
}
