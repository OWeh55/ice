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
#include <stdio.h>
#include <iostream>

#include "IceException.h"
#include "fit.h"

#include "simplex.h"
#include "simplex7.h"
#include "simplex_glbl.h"
#include "simplex4.h"

#ifdef DEBUG
#include "screen.h"
#include "xio.h"
#endif


// Aus 2 gegebenen Listen wird eine Transformation
// mittels linearer Optimierung berechnet
namespace ice
{

#define FNAME "MatchPointListsLinOpt"
  int MatchPointlistsLinOpt(PointList pl1, PointList pl2, double tr[][3],
                            int mode, double limit)
  {
    int code = 0;

    if (pl1 == NULL || pl2 == NULL)
      {
        throw IceException(FNAME, M_WRONG_POINTLIST, WRONG_PARAM);
      }

    if (pl1->lng != pl2->lng)
      {
        throw IceException(FNAME, M_DIFFERENT_LISTSIZE, WRONG_PARAM);
      }

    if (limit < 0.0)
      {
        throw IceException(FNAME, M_WRONG_VAL, WRONG_PARAM);
      }

    if (pl1->lng > MAXMATCHPOINTSLINOPT)
      {
        throw IceException(FNAME, M_TOO_MUCH_POINTS, WRONG_PARAM);
      }

#include "simplex_init.inc"

    switch (mode)
      {
      case TRM_AFFINE:
        code = FitAffineLinOpt(pl1, pl2, tr, limit);
        break;
      case TRM_SHIFT:
        code = FitShiftLinOpt(pl1, pl2, tr, limit);
        break;
      case TRM_SIMILARITY_NOR:
        code = FitEuklidLinOpt(pl1, pl2, tr, limit);
        break;
      case TRM_PROJECTIVE:
        code = FitProjectiveLinOpt(pl1, pl2, tr, limit);
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE, WRONG_PARAM);
      }

#include "simplex_free.inc"
    return code;
  }
#undef FNAME

  int FitAffineLinOpt(PointList pl1, PointList pl2,
                      double tr[][3], double limit)
  {
    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // eine affine Transformation berechnet
    // Die Gewichte werden aus der Punktliste pl2 entnommen

    int m;    /* number of constraints */
    int n;    /* number of variables */
    int nz;   /* number of nonzeros in sparse constraint matrix */
    int* ia;    /* array row indices */
    int* ka;    /* array of indices into ia and a */
    double* a;    /* array of nonzeros in the constraint matrix */
    double* b;    /* right-hand side */
    double* c;            /* objective coefficients */
    double  f = 0.0;  /* objective function shift */
    double* x;    /* primal solution (output) */
    double* y;    /* dual solution (output) */
    double* w;    /* primal slacks (output) */
    double* z;    /* dual slacks (output) */
    double zf;
    double* gew;          /* Vektor der Gewichte der Defekte */

    int N;                /* ANZAHL DER PUNKTE */
    int code;             /* Rueckkehrcode aus Simplexmethode */
    int NN;
    int i, j, k;
    double a11, a12, a10, a21, a22, a20;

    N = pl1->lng;
    NN = N + 1;
    m = 2 * N + 1;
    n = 12 + 2 * N;
    nz = 14 * N + n;
    ia = new int[16 * N + n + 1];
    ka = new int[n + 2 * N + 1 + 1];
    a = new double[nz + 2 * N + 1];
    b = new double[m];
    c = new double[n + 2 * N + 1];
    x = new double[n + 2 * N + 1];
    y = new double[n + 2 * N + 1];
    w = new double[n + 2 * N + 1];
    z = new double[n + 2 * N + 1];
    gew = new double[2 * N + 1];

    //********************************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl2 entnommen
    for (i = 0; i < N; ++i)
      {
        gew[2 * i] = pl2->wptr[i];
        gew[2 * i + 1] = pl2->wptr[i];
      }

    gew[2 * N] = 0.0;


    // ******************************
    // Füllen der Zielfunktion
    for (i = 0; i < 12; ++i)
      {
        c[i] = 0.0;
      }

    for (i = 12; i < n; ++i)
      {
        c[i] = -gew[i - 12];
      }

    // *****************************
    // Füllen der rechten Seite
    i = 0;

    for (k = 0; k < m - 1; k = k + 2)
      {
        b[k] = pl2->xptr[i];
        b[k + 1] = pl2->yptr[i];
        ++i;
      }

    b[m - 1] = limit;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // ****************************
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 13; ++i)
      {
        ka[i] = i * (N + 1);
      }

    for (i = 13; i < 13 + 2 * N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }


    // Speziell: Füllen von ia
    for (j = 0; j < 6; ++j)
      {
        k = 0;

        for (i = 0; i < (N + 1); i++)
          {
            ia[j * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[j * (N + 1) + N] = m - 1;
      }

    for (j = 6; j < 12; ++j)
      {
        k = 1;

        for (i = 0; i < (N + 1); i++)
          {
            ia[j * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[j * (N + 1) + N] = m - 1;
      }

    k = 0;

    for (i = 12 * (N + 1); i < 12 * (N + 1) + 2 * N + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 1;
        ++k;
      }

    // Speziell: Füllen von a

    for (i = 0; i < NN - 1; ++i)
      {
        a[i] = pl1->xptr[i];
      }

    a[NN - 1] = 1.0;
    k = 0;

    for (i = NN; i < 2 * NN - 1; ++i)
      {
        a[i] = -pl1->xptr[k];
        ++k;
      }

    a[2 * NN - 1] = 1.0;
    k = 0;

    for (i = 2 * NN; i < 3 * NN - 1; ++i)
      {
        a[i] = pl1->yptr[k];
        ++k;
      }

    a[3 * NN - 1] = 1.0;
    k = 0;

    for (i = 3 * NN; i < 4 * NN - 1; ++i)
      {
        a[i] = -pl1->yptr[k];
        ++k;
      }

    a[4 * NN - 1] = 1.0;

    for (i = 4 * NN; i < 5 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = 5 * NN; i < 6 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[6 * NN - 1] = 1.0;

    k = 0;

    for (i = 6 * NN; i < 7 * NN - 1; ++i)
      {
        a[i] = pl1->xptr[k];
        ++k;
      }

    a[7 * NN - 1] = 1.0;
    k = 0;

    for (i = 7 * NN; i < 8 * NN - 1; ++i)
      {
        a[i] = -pl1->xptr[k];
        ++k;
      }

    a[8 * NN - 1] = 1.0;
    k = 0;

    for (i = 8 * NN; i < 9 * NN - 1; ++i)
      {
        a[i] = pl1->yptr[k];
        ++k;
      }

    a[9 * NN - 1] = 1.0;
    k = 0;

    for (i = 9 * NN; i < 10 * NN - 1; ++i)
      {
        a[i] = -pl1->yptr[k];
        ++k;
      }

    a[10 * NN - 1] = 1.0;

    for (i = 10 * NN; i < 11 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = 11 * NN; i < 12 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[12 * NN - 1] = 1.0;

    for (i = 12 * NN; i < 12 * NN + 4 * N; i = i + 2)
      {
        a[i] = -1.0;
        a[i + 1] = 1.0;
      }

    // ************************************

#ifdef DEBUG

    /* some variable names have changed, commenting out */


    // Anzeige der Eingabedaten
    Printf("Zielfunktion \n");

    for (i = 0; i < n; ++i)
      {
        Printf("%f  ", c[i]);
      }

    GetChar();
    Printf("\n Rechte Seite \n");

    for (i = 0; i < m; ++i)
      {
        Printf("%f  ", b[i]);
      }

    GetChar();
    Printf("Matrix: \n\n");
    Printf("Indexarray ka\n");

    for (i = 0; i < n + 1; ++i)
      {
        Printf("%d  ", ka[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Indexarray ia\n");

    for (i = 0; i < nz; ++i)
      {
        Printf("%d  ", ia[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Datenfeld a\n");

    for (i = 0; i < (12 * NN + 4 * N); ++i)
      {
        Printf("%f  ", a[i]);
      }

    Printf("\n");
    GetChar();


#endif

    code = simplex_method_modified(
             m,     /* number of constraints */
             n,     /* number of variables */
             nz,    /* number of nonzeros in sparse constraint matrix */
             ia,    /* array row indices */
             ka,    /* array of indices into ia and a */
             a,     /* array of nonzeros in the constraint matrix */
             b,     /* right-hand side */
             c,     /* objective coefficients */
             f,     /* objective function shift */
             x,     /* primal solution (output) */
             y,     /* dual solution (output) */
             w,     /* primal slacks (output) */
             z,     /* dual slacks (output) */
             &zf,   /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
        delete [] ia;
        delete [] ka;
        delete []  a;
        delete []  b;
        delete []  c;
        delete []  x;
        delete []  y;
        delete []  w;
        delete []  z;
        delete []  gew;
        return code;
      }

    a11 = x[0] - x[1];
    a12 = x[2] - x[3];
    a10 = x[4] - x[5];
    a21 = x[6] - x[7];
    a22 = x[8] - x[9];
    a20 = x[10] - x[11];

    tr[0][0] = a11;
    tr[0][1] = a12;
    tr[0][2] = a10;
    tr[1][0] = a21;
    tr[1][1] = a22;
    tr[1][2] = a20;
    tr[2][0] = 0.0;
    tr[2][1] = 0.0;
    tr[2][2] = 1.0;


    delete [] ia;
    delete [] ka;
    delete []  a;
    delete []  b;
    delete []  c;
    delete []  x;
    delete []  y;
    delete []  w;
    delete []  z;
    delete []  gew;

    return 0;

  }

// ********************************************************************
// ********************************************************************

  int FitShiftLinOpt(PointList pl1, PointList pl2, double tr[][3], double limit)
  {


    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // eine Translation berechnet
    // Die Gewichte werden aus der Punktliste pl2 entnommen

    int m;    /* number of constraints */
    int n;    /* number of variables */
    int nz;   /* number of nonzeros in sparse constraint matrix */
    int* ia;    /* array row indices */
    int* ka;    /* array of indices into ia and a */
    double* a;    /* array of nonzeros in the constraint matrix */
    double* b;    /* right-hand side */
    double* c;            /* objective coefficients */
    double  f = 0.0;  /* objective function shift */
    double* x;    /* primal solution (output) */
    double* y;    /* dual solution (output) */
    double* w;    /* primal slacks (output) */
    double* z;    /* dual slacks (output) */
    double zf;
    double* gew;          /* Vektor der Gewichte der Defekte */

    int N;                /* ANZAHL DER PUNKTE */
    int code;             /* Rueckkehrcode aus Simplexmethode */
    int NN;
    int i, j, k;
    double a10, a20;

    N = pl1->lng;
    NN = N + 1;
    m = 2 * N + 1;
    n = 12 + 2 * N;
    nz = 14 * N + n;
    ia = new int[16 * N + n + 1];
    ka = new int[n + 2 * N + 1 + 1];
    a = new double[nz + 2 * N + 1];
    b = new double[m];
    c = new double[n + 2 * N + 1];
    x = new double[n + 2 * N + 1];
    y = new double[n + 2 * N + 1];
    w = new double[n + 2 * N + 1];
    z = new double[n + 2 * N + 1];
    gew = new double[2 * N + 1];

    //********************************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl2 entnommen
    for (i = 0; i < N; ++i)
      {
        gew[2 * i] = pl2->wptr[i];
        gew[2 * i + 1] = pl2->wptr[i];
      }

    gew[2 * N] = 0.0;


    // ******************************
    // Füllen der Zielfunktion
    for (i = 0; i < 12; ++i)
      {
        c[i] = 0.0;
      }

    for (i = 12; i < n; ++i)
      {
        c[i] = -gew[i - 12];
      }

    // *****************************
    // Füllen der rechten Seite
    i = 0;

    for (k = 0; k < m - 1; k = k + 2)
      {
        b[k] = pl2->xptr[i] - pl1->xptr[i];
        b[k + 1] = pl2->yptr[i] - pl1->yptr[i];
        ++i;
      }

    b[m - 1] = limit;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // ****************************
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 13; ++i)
      {
        ka[i] = i * (N + 1);
      }

    for (i = 13; i < 13 + 2 * N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }


    // Speziell: Füllen von ia
    for (j = 0; j < 6; ++j)
      {
        k = 0;

        for (i = 0; i < (N + 1); i++)
          {
            ia[j * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[j * (N + 1) + N] = m - 1;
      }

    for (j = 6; j < 12; ++j)
      {
        k = 1;

        for (i = 0; i < (N + 1); i++)
          {
            ia[j * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[j * (N + 1) + N] = m - 1;
      }

    k = 0;

    for (i = 12 * (N + 1); i < 12 * (N + 1) + 2 * N + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 1;
        ++k;
      }

    // Speziell: Füllen von a

    for (i = 0; i < NN - 1; ++i)
      {
        a[i] = 0.0;
      }

    a[NN - 1] = 1.0;
    k = 0;

    for (i = NN; i < 2 * NN - 1; ++i)
      {
        a[i] = 0.0;
      }

    a[2 * NN - 1] = 1.0;
    k = 0;

    for (i = 2 * NN; i < 3 * NN - 1; ++i)
      {
        a[i] = 0.0;
        ++k;
      }

    a[3 * NN - 1] = 1.0;
    k = 0;

    for (i = 3 * NN; i < 4 * NN - 1; ++i)
      {
        a[i] = 0.0;
        ++k;
      }

    a[4 * NN - 1] = 1.0;

    for (i = 4 * NN; i < 5 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = 5 * NN; i < 6 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[6 * NN - 1] = 1.0;

    k = 0;

    for (i = 6 * NN; i < 7 * NN - 1; ++i)
      {
        a[i] = 0.0;
        ++k;
      }

    a[7 * NN - 1] = 1.0;
    k = 0;

    for (i = 7 * NN; i < 8 * NN - 1; ++i)
      {
        a[i] = 0.0;
        ++k;
      }

    a[8 * NN - 1] = 1.0;
    k = 0;

    for (i = 8 * NN; i < 9 * NN - 1; ++i)
      {
        a[i] = 0.0;
        ++k;
      }

    a[9 * NN - 1] = 1.0;
    k = 0;

    for (i = 9 * NN; i < 10 * NN - 1; ++i)
      {
        a[i] = 0.0;
        ++k;
      }

    a[10 * NN - 1] = 1.0;

    for (i = 10 * NN; i < 11 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = 11 * NN; i < 12 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[12 * NN - 1] = 1.0;

    for (i = 12 * NN; i < 12 * NN + 4 * N; i = i + 2)
      {
        a[i] = -1.0;
        a[i + 1] = 1.0;
      }

    // ************************************



#ifdef DEBUG


    // Anzeige der Eingabedaten
    Printf("Zielfunktion \n");

    for (i = 0; i < n; ++i)
      {
        Printf("%f  ", c[i]);
      }

    GetChar();
    Printf("\n Rechte Seite \n");

    for (i = 0; i < m; ++i)
      {
        Printf("%f  ", b[i]);
      }

    GetChar();
    Printf("Matrix: \n\n");
    Printf("Indexarray ka\n");

    for (i = 0; i < n + 1; ++i)
      {
        Printf("%d  ", ka[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Indexarray ia\n");

    for (i = 0; i < nz; ++i)
      {
        Printf("%d  ", ia[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Datenfeld a\n");

    for (i = 0; i < (12 * NN + 4 * N); ++i)
      {
        Printf("%f  ", a[i]);
      }

    Printf("\n");
    GetChar();


#endif

    code = simplex_method_modified(
             m,   /* number of constraints */
             n,   /* number of variables */
             nz,    /* number of nonzeros in sparse constraint matrix */
             ia,    /* array row indices */
             ka,    /* array of indices into ia and a */
             a,   /* array of nonzeros in the constraint matrix */
             b,     /* right-hand side */
             c,              /* objective coefficients */
             f,     /* objective function shift */
             x,     /* primal solution (output) */
             y,   /* dual solution (output) */
             w,     /* primal slacks (output) */
             z,     /* dual slacks (output) */
             &zf,            /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
        delete [] ia;
        delete [] ka;
        delete []  a;
        delete []  b;
        delete []  c;
        delete []  x;
        delete []  y;
        delete []  w;
        delete []  z;
        delete []  gew;
        return code;
      }


    a10 = x[4] - x[5];

    a20 = x[10] - x[11];

    tr[0][0] = 1.0;
    tr[0][1] = 0.0;
    tr[0][2] = a10;
    tr[1][0] = 0.0;
    tr[1][1] = 1.0;
    tr[1][2] = a20;
    tr[2][0] = 0.0;
    tr[2][1] = 0.0;
    tr[2][2] = 1.0;


    delete [] ia;
    delete [] ka;
    delete []  a;
    delete []  b;
    delete []  c;
    delete []  x;
    delete []  y;
    delete []  w;
    delete []  z;
    delete []  gew;

    return 0;

  }

// ********************************************************************
// ********************************************************************
  int FitEuklidLinOpt(PointList pl1, PointList pl2,
                      double tr[][3], double limit)
  {

    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // eine Euklidische Transformation berechnet
    // Die Gewichte werden aus der Punktliste pl2 entnommen

    int m;    /* number of constraints */
    int n;    /* number of variables */
    int nz;   /* number of nonzeros in sparse constraint matrix */
    int* ia;    /* array row indices */
    int* ka;    /* array of indices into ia and a */
    double* a;    /* array of nonzeros in the constraint matrix */
    double* b;    /* right-hand side */
    double* c;            /* objective coefficients */
    double  f = 0.0;  /* objective function shift */
    double* x;    /* primal solution (output) */
    double* y;    /* dual solution (output) */
    double* w;    /* primal slacks (output) */
    double* z;    /* dual slacks (output) */
    double zf;
    double* gew;          /* Vektor der Gewichte der Defekte */

    int N;                /* ANZAHL DER PUNKTE */
    int code;             /* Rueckkehrcode aus Simplexmethode */
    int NN;
    int N2_1;
    int i, j, k, l;
    double a11, a12, a10, a21, a22, a20;

    N = pl1->lng;
    NN = N + 1;
    N2_1 = 2 * N + 1;
    m = 2 * N + 1;
    n = 8 + 2 * N;
    nz = 14 * N + n;
    ia = new int[nz + 2 * N + 1];
    ka = new int[n + 2 * N + 1 + 1];
    a = new double[nz + 2 * N + 1];
    b = new double[m];
    c = new double[n + 2 * N + 1];
    x = new double[n + 2 * N + 1];
    y = new double[n + 2 * N + 1];
    w = new double[n + 2 * N + 1];
    z = new double[n + 2 * N + 1];
    gew = new double[2 * N + 1];

    //********************************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl2 entnommen
    for (i = 0; i < N; ++i)
      {
        gew[2 * i] = pl2->wptr[i];
        gew[2 * i + 1] = pl2->wptr[i];
      }

    gew[2 * N] = 0.0;


    // ******************************
    // Füllen der Zielfunktion
    for (i = 0; i < 8; ++i)
      {
        c[i] = 0.0;
      }

    for (i = 8; i < n; ++i)
      {
        c[i] = -gew[i - 8];
      }

    // *****************************
    // Füllen der rechten Seite
    i = 0;

    for (k = 0; k < m - 1; k = k + 2)
      {
        b[k] = pl2->xptr[i];
        b[k + 1] = pl2->yptr[i];
        ++i;
      }

    b[m - 1] = limit;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // ****************************
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 5; ++i)
      {
        ka[i] = i * (2 * N + 1);
      }

    for (i = 5; i < 9; ++i)
      {
        ka[i] = ka[i - 1] + NN;
      }

    for (i = 9; i < 9 + 2 * N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }

    // **********************

    // Speziell: Füllen von ia
    for (j = 0; j < 4; ++j)
      {
        k = 0;

        for (i = 0; i < (2 * N + 1); i++)
          {
            ia[j * (2 * N + 1) + i] = k;
            k++;
          }

        ia[j * (2 * N + 1) + 2 * N] = m - 1;
      }

    l = 4 * (2 * N + 1);

    for (j = 4; j < 6; ++j)
      {
        k = 0;

        for (i = 0; i < (N + 1); i++)
          {
            ia[l + (j - 4) * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[l + (j - 4) * (N + 1) + N] = m - 1;
      }

    l = l + 2 * (N + 1);

    for (j = 6; j < 8; ++j)
      {
        k = 1;

        for (i = 0; i < (N + 1); i++)
          {
            ia[l + (j - 6) * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[l + (j - 6) * (N + 1) + N] = m - 1;
      }

    l = l + 2 * (N + 1);
    k = 0;

    for (i = l; i < l + 2 * N + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 1;
        ++k;
      }

    // Speziell: Füllen von a
    k = 0;

    for (i = 0; i < N2_1 - 1; i = i + 2)
      {
        a[i] = pl1->xptr[k];
        a[i + 1] = pl1->yptr[k];
        k++;
      }

    a[N2_1 - 1] = 1.0;
    k = 0;

    for (i = N2_1; i < 2 * N2_1 - 1; i = i + 2)
      {
        a[i] = -pl1->xptr[k];
        a[i + 1] = -pl1->yptr[k];
        ++k;
      }

    a[2 * N2_1 - 1] = 1.0;
    k = 0;

    for (i = 2 * N2_1; i < 3 * N2_1 - 1; i = i + 2)
      {
        a[i] = -pl1->yptr[k];
        a[i + 1] = +pl1->xptr[k];
        ++k;
      }

    a[3 * N2_1 - 1] = 1.0;
    k = 0;

    for (i = 3 * N2_1; i < 4 * N2_1 - 1; i = i + 2)
      {
        a[i] = +pl1->yptr[k];
        a[i + 1] = -pl1->xptr[k];
        ++k;
      }

    a[4 * N2_1 - 1] = 1.0;
    l = 4 * N2_1;

    for (i = l; i < l + NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = l + NN; i < l + 2 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[l + 2 * NN - 1] = 1.0;

    for (i = l + 2 * NN; i < l + 3 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = l + 3 * NN; i < l + 4 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[l + 4 * NN - 1] = 1.0;


    for (i = l + 4 * NN; i < l + 4 * NN + 4 * N; i = i + 2)
      {
        a[i] = -1.0;
        a[i + 1] = 1.0;
      }

    // ************************************

#ifdef DEBUG


    // Anzeige der Eingabedaten
    Printf("Zielfunktion \n");

    for (i = 0; i < n; ++i)
      {
        Printf("%f  ", c[i]);
      }

    GetChar();
    Printf("\n Rechte Seite \n");

    for (i = 0; i < m; ++i)
      {
        Printf("%f  ", b[i]);
      }

    GetChar();
    Printf("Matrix: \n\n");
    Printf("Indexarray ka\n");

    for (i = 0; i < n + 1; ++i)
      {
        Printf("%d  ", ka[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Indexarray ia\n");

    for (i = 0; i < nz; ++i)
      {
        Printf("%d  ", ia[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Datenfeld a\n");

    for (i = 0; i < nz ; ++i)
      {
        Printf("%f  ", a[i]);
      }

    Printf("\n");
    GetChar();


#endif

    code = simplex_method_modified(
             m,   /* number of constraints */
             n,   /* number of variables */
             nz,    /* number of nonzeros in sparse constraint matrix */
             ia,    /* array row indices */
             ka,    /* array of indices into ia and a */
             a,   /* array of nonzeros in the constraint matrix */
             b,     /* right-hand side */
             c,              /* objective coefficients */
             f,     /* objective function shift */
             x,     /* primal solution (output) */
             y,   /* dual solution (output) */
             w,     /* primal slacks (output) */
             z,     /* dual slacks (output) */
             &zf,            /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
        delete [] ia;
        delete [] ka;
        delete []  a;
        delete []  b;
        delete []  c;
        delete []  x;
        delete []  y;
        delete []  w;
        delete []  z;
        delete []  gew;
        return code;
      }

    a11 = x[0] - x[1];
    a12 = -(x[2] - x[3]);
    a10 = x[4] - x[5];
    a21 = -a12;
    a22 = a11;
    a20 = x[6] - x[7];

    tr[0][0] = a11;
    tr[0][1] = a12;
    tr[0][2] = a10;
    tr[1][0] = a21;
    tr[1][1] = a22;
    tr[1][2] = a20;
    tr[2][0] = 0.0;
    tr[2][1] = 0.0;
    tr[2][2] = 1.0;


    delete [] ia;
    delete [] ka;
    delete []  a;
    delete []  b;
    delete []  c;
    delete []  x;
    delete []  y;
    delete []  w;
    delete []  z;
    delete []  gew;

    return 0;
  }


  int FitProjectiveLinOpt(PointList pl1, PointList pl2,
                          double tr[][3], double limit)
  {
    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // eine projective Transformation berechnet
    // Die Gewichte werden aus der Punktliste pl2 entnommen

    int m;    /* number of constraints */
    int n;    /* number of variables */
    int nz;   /* number of nonzeros in sparse constraint matrix */
    int* ia;    /* array row indices */
    int* ka;    /* array of indices into ia and a */
    double* a;    /* array of nonzeros in the constraint matrix */
    double* b;    /* right-hand side */
    double* c;            /* objective coefficients */
    double  f = 0.0;  /* objective function shift */
    double* x;    /* primal solution (output) */
    double* y;    /* dual solution (output) */
    double* w;    /* primal slacks (output) */
    double* z;    /* dual slacks (output) */
    double zf;
    double* gew;          /* Vektor der Gewichte der Defekte */

    int N;                /* ANZAHL DER PUNKTE */
    int code;             /* Rueckkehrcode aus Simplexmethode */
    int NN;
    int N2_1;
    int i, j, k, l;
    double a11, a12, a10, a21, a22, a20, a31, a32;

    N = pl1->lng;
    NN = N + 1;
    N2_1 = 2 * N + 1;
    m = 2 * N + 1;
    n = 16 + 2 * N;
    nz = 22 * N + n;
    ia = new int[nz + 2 * N + 1];
    ka = new int[n + 2 * N + 1 + 1];
    a = new double[nz + 2 * N + 1];
    b = new double[m];
    c = new double[n + 2 * N + 1];
    x = new double[n + 2 * N + 1];
    y = new double[n + 2 * N + 1];
    w = new double[n + 2 * N + 1];
    z = new double[n + 2 * N + 1];
    gew = new double[2 * N + 1];

    //********************************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl2 entnommen
    for (i = 0; i < N; ++i)
      {
        gew[2 * i] = pl2->wptr[i];
        gew[2 * i + 1] = pl2->wptr[i];
      }

    gew[2 * N] = 0.0;


    // ******************************
    // Füllen der Zielfunktion
    for (i = 0; i < 16; ++i)
      {
        c[i] = 0.0;
      }

    for (i = 16; i < n; ++i)
      {
        c[i] = -gew[i - 16];
      }

    // *****************************
    // Füllen der rechten Seite
    i = 0;

    for (k = 0; k < m - 1; k = k + 2)
      {
        b[k] = pl2->xptr[i];
        b[k + 1] = pl2->yptr[i];
        ++i;
      }

    b[m - 1] = limit;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // ****************************
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 13; ++i)
      {
        ka[i] = i * (N + 1);
      }

    for (i = 13; i < 17; ++i)
      {
        ka[i] = ka[i - 1] + N2_1;
      }

    for (i = 17; i < 17 + 2 * N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }


    // Speziell: Füllen von ia
    for (j = 0; j < 6; ++j)
      {
        k = 0;

        for (i = 0; i < (N + 1); i++)
          {
            ia[j * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[j * (N + 1) + N] = m - 1;
      }

    for (j = 6; j < 12; ++j)
      {
        k = 1;

        for (i = 0; i < (N + 1); i++)
          {
            ia[j * (N + 1) + i] = k;
            k = k + 2;
          }

        ia[j * (N + 1) + N] = m - 1;
      }

    l = 12 * NN;

    for (j = 12; j < 16; ++j)
      {
        k = 0;

        for (i = 0; i < N2_1; ++i)
          {
            ia[l + (j - 12)*N2_1 + i] = k;
            ++k;
          }

        ia[l + (j - 12)*N2_1 + N2_1 - 1] = m - 1;
      }

    l = l + 4 * N2_1;
    k = 0;

    for (i = l; i < l + 2 * N + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 1;
        ++k;
      }

    // Speziell: Füllen von a *****************************

    for (i = 0; i < NN - 1; ++i)
      {
        a[i] = pl1->xptr[i];
      }

    a[NN - 1] = 1.0;
    k = 0;

    for (i = NN; i < 2 * NN - 1; ++i)
      {
        a[i] = -pl1->xptr[k];
        ++k;
      }

    a[2 * NN - 1] = 1.0;
    k = 0;

    for (i = 2 * NN; i < 3 * NN - 1; ++i)
      {
        a[i] = pl1->yptr[k];
        ++k;
      }

    a[3 * NN - 1] = 1.0;
    k = 0;

    for (i = 3 * NN; i < 4 * NN - 1; ++i)
      {
        a[i] = -pl1->yptr[k];
        ++k;
      }

    a[4 * NN - 1] = 1.0;

    for (i = 4 * NN; i < 5 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = 5 * NN; i < 6 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[6 * NN - 1] = 1.0;

    k = 0;

    for (i = 6 * NN; i < 7 * NN - 1; ++i)
      {
        a[i] = pl1->xptr[k];
        ++k;
      }

    a[7 * NN - 1] = 1.0;
    k = 0;

    for (i = 7 * NN; i < 8 * NN - 1; ++i)
      {
        a[i] = -pl1->xptr[k];
        ++k;
      }

    a[8 * NN - 1] = 1.0;
    k = 0;

    for (i = 8 * NN; i < 9 * NN - 1; ++i)
      {
        a[i] = pl1->yptr[k];
        ++k;
      }

    a[9 * NN - 1] = 1.0;
    k = 0;

    for (i = 9 * NN; i < 10 * NN - 1; ++i)
      {
        a[i] = -pl1->yptr[k];
        ++k;
      }

    a[10 * NN - 1] = 1.0;

    for (i = 10 * NN; i < 11 * NN; ++i)
      {
        a[i] = 1.0;
      }

    for (i = 11 * NN; i < 12 * NN - 1; ++i)
      {
        a[i] = -1.0;
      }

    a[12 * NN - 1] = 1.0;
    l = 12 * NN;
    k = 0;

    for (i = l; i < l + N2_1; i = i + 2)
      {
        a[i] = -pl2->xptr[k] * pl1->xptr[k];
        a[i + 1] = -pl2->yptr[k] * pl1->xptr[k];
        ++k;
      }

    a[l + N2_1 - 1] = 1.0;
    l = l + N2_1;
    k = 0;

    for (i = l; i < l + N2_1; i = i + 2)
      {
        a[i] = +pl2->xptr[k] * pl1->xptr[k];
        a[i + 1] = +pl2->yptr[k] * pl1->xptr[k];
        ++k;
      }

    a[l + N2_1 - 1] = 1.0;
    l = l + N2_1;
    k = 0;

    for (i = l; i < l + N2_1; i = i + 2)
      {
        a[i] = -pl2->xptr[k] * pl1->yptr[k];
        a[i + 1] = -pl2->yptr[k] * pl1->yptr[k];
        ++k;
      }

    a[l + N2_1 - 1] = 1.0;
    l = l + N2_1;
    k = 0;

    for (i = l; i < l + N2_1; i = i + 2)
      {
        a[i] = +pl2->xptr[k] * pl1->yptr[k];
        a[i + 1] = +pl2->yptr[k] * pl1->yptr[k];
        ++k;
      }

    a[l + N2_1 - 1] = 1.0;
    l = l + N2_1;

    for (i = l; i < l + 4 * N; i = i + 2)
      {
        a[i] = -1.0;
        a[i + 1] = 1.0;
      }

    // ************************************



#ifdef DEBUG


    // Anzeige der Eingabedaten
    Printf("Zielfunktion \n");

    for (i = 0; i < n; ++i)
      {
        Printf("%f  ", c[i]);
      }

    GetChar();
    Printf("\n Rechte Seite \n");

    for (i = 0; i < m; ++i)
      {
        Printf("%f  ", b[i]);
      }

    GetChar();
    Printf("Matrix: \n\n");
    Printf("Indexarray ka\n");

    for (i = 0; i < n + 1; ++i)
      {
        Printf("%d  ", ka[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Indexarray ia\n");

    for (i = 0; i < nz; ++i)
      {
        Printf("%d  ", ia[i]);
      }

    Printf("\n");
    GetChar();
    Printf("Datenfeld a\n");

    for (i = 0; i < (nz); ++i)
      {
        Printf("%f  ", a[i]);
      }

    Printf("\n");
    GetChar();


#endif

    code = simplex_method_modified(
             m,   /* number of constraints */
             n,   /* number of variables */
             nz,    /* number of nonzeros in sparse constraint matrix */
             ia,    /* array row indices */
             ka,    /* array of indices into ia and a */
             a,   /* array of nonzeros in the constraint matrix */
             b,     /* right-hand side */
             c,              /* objective coefficients */
             f,     /* objective function shift */
             x,     /* primal solution (output) */
             y,   /* dual solution (output) */
             w,     /* primal slacks (output) */
             z,     /* dual slacks (output) */
             &zf,            /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
        delete [] ia;
        delete [] ka;
        delete []  a;
        delete []  b;
        delete []  c;
        delete []  x;
        delete []  y;
        delete []  w;
        delete []  z;
        delete []  gew;
        return code;
      }

    a11 = x[0] - x[1];
    a12 = x[2] - x[3];
    a10 = x[4] - x[5];
    a21 = x[6] - x[7];
    a22 = x[8] - x[9];
    a20 = x[10] - x[11];
    a31 = x[12] - x[13];
    a32 = x[14] - x[15];

    tr[0][0] = a11;
    tr[0][1] = a12;
    tr[0][2] = a10;
    tr[1][0] = a21;
    tr[1][1] = a22;
    tr[1][2] = a20;
    tr[2][0] = a31;
    tr[2][1] = a32;
    tr[2][2] = 1.0;


    delete [] ia;
    delete [] ka;
    delete []  a;
    delete []  b;
    delete []  c;
    delete []  x;
    delete []  y;
    delete []  w;
    delete []  z;
    delete []  gew;

    return 0;

  }
}
