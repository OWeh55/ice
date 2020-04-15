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
#include <math.h>

#include "IceException.h"
#include "analygeo.h"
#include "macro.h"

#include "simplex.h"
#include "simplex8.h"
#include "simplex_glbl.h"
#include "simplex4.h"

namespace ice
{
#define FNAME "FitLineLinOpt"
  int FitLineLinOpt(PointList pl, int a1, int a2, int step, double par[], double limit)
  {
    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    if (a1 < 0 || a2 > pl->lng - 1)
      throw IceException(FNAME, M_WRONG_VALUE);

    if (step < 0)
      throw IceException(FNAME, M_WRONG_VALUE);

    if (pl->lng < 2)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // eine Gerade angepaßt
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
    double zf1, zf2;
    double* gew;          /* Vektor der Gewichte der Defekte */

    int N;                /* ANZAHL DER PUNKTE */
    int code;             /* Rueckkehrcode aus Simplexmethode */
    int NN;
    int i, j, k, l;
    double k_a, k_b, k_c;
    PointList plist;

    N = 0;

    for (i = a1; i <= a2; i = i + step)
      {
        N++;
      }

    plist = NewPointList(N);
    k = 0;

    for (i = a1; i <= a2; i = i + step)
      {
        plist->xptr[k] = pl->xptr[i];
        plist->yptr[k] = pl->yptr[i];
        plist->wptr[k] = pl->wptr[i];
        k++;
      }

    NN = N + 3;
    m = N + 3;
    n = 6 + N;
    nz = 6 * N + N + 8 + 6 + N + 4;
    ia = new int[nz + N + 3];
    ka = new int[n + N + 3 + 1];
    a = new double[nz + N + 3];

    for (i = 0; i < nz + N + 3; ++i)
      {
        ia[i] = 0;
        a[i] = 0.0;
      }

    for (i = 0; i < n + N + 3 + 1; ++i)
      {
        ka[i] = 0;
      }

    b = new double[m];

    for (i = 0; i < m; ++i)
      {
        b[i] = 0.0;
      }

    c = new double[n + N + 3];
    x = new double[n + N + 3];
    y = new double[n + N + 3];
    w = new double[n + N + 3];
    z = new double[n + N + 3];

    for (i = 0; i < n + N + 3; ++i)
      {
        c[i] = 0.0;
        x[i] = 0.0;
        y[i] = 0.0;
        w[i] = 0.0;
        z[i] = 0.0;
      }

    gew = new double[N + 3];

    //************** ******************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl entnommen
    for (i = 0; i < N; ++i)
      {
        gew[i] = plist->wptr[i];
      }

    gew[N] = 0.0;
    gew[N + 1] = 0.0;
    gew[N + 2] = 0.0;

    // ******************************
    // Füllen der Zielfunktion
    for (i = 0; i < 6; ++i)
      {
        c[i] = 0.0;
      }

    for (i = 6; i < n; ++i)
      {
        c[i] = -gew[i - 6];
      }

    // *****************************
    // Füllen der rechten Seite
    for (i = 0; i < m - 1; i++)
      {
        b[i] = 0.0;
      }

    b[m - 3] = limit;
    b[m - 2] = 1.0;
    b[m - 1] = -1.0;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 7; ++i)
      {
        ka[i] = i * NN;
      }

    for (i = 7; i < 7 + N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }

    // *************************
    // Speziell: Füllen von ia
    for (j = 0; j < 6; ++j)
      {
        k = 0;

        for (i = 0; i < m; i++)
          {
            ia[j * m + i] = k;
            k++;
          }
      }

    l = 6 * m;
    k = 0;

    for (i = l; i < l + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 3;
        ++k;
      }

    // ***************************

    // Speziell: Füllen von a

    for (i = 0; i < N; ++i)
      {
        a[i] = plist->xptr[i];
      }

    a[NN - 1] = -1.0;
    a[NN - 2] = 1.0;
    a[NN - 3] = 1.0;
    k = 0;

    for (i = NN; i < 2 * NN - 3; ++i)
      {
        a[i] = -plist->xptr[k];
        ++k;
      }

    a[2 * NN - 1] = +1.0;
    a[2 * NN - 2] = -1.0;
    a[2 * NN - 3] = 1.0;
    k = 0;

    for (i = 2 * NN; i < 3 * NN - 3; ++i)
      {
        a[i] = plist->yptr[k];
        ++k;
      }

    a[3 * NN - 1] = -1.0;
    a[3 * NN - 2] = +1.0;
    a[3 * NN - 3] = 1.0;
    k = 0;

    for (i = 3 * NN; i < 4 * NN - 3; ++i)
      {
        a[i] = -plist->yptr[k];
        ++k;
      }

    a[4 * NN - 1] = +1.0;
    a[4 * NN - 2] = -1.0;
    a[4 * NN - 3] = 1.0;
    k = 0;

    for (i = 4 * NN; i < 5 * NN - 3; ++i)
      {
        a[i] = +1.0;
        ++k;
      }

    a[5 * NN - 1] = 0.0;
    a[5 * NN - 2] = 0.0;
    a[5 * NN - 3] = 1.0;
    k = 0;

    for (i = 5 * NN; i < 6 * NN - 3; ++i)
      {
        a[i] = -1.0;
        ++k;
      }

    a[6 * NN - 1] = 0.0;
    a[6 * NN - 2] = 0.0;
    a[6 * NN - 3] = 1.0;
    l = 6 * NN;

    for (i = l; i < l + 2 * N; i = i + 2)
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

#include "simplex_init.inc"

    code = simplex_method_modified(
             m, /* number of constraints */
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
             &zf1,           /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
#include "simplex_free.inc"
        delete [] ia;
        delete [] ka;
        delete [] a;
        delete [] b;
        delete [] c;
        delete [] x;
        delete [] y;
        delete [] w;
        delete [] z;
        delete [] gew;
        return code;
      }

#include "simplex_free.inc"

    k_a = x[0] - x[1];
    k_b = x[2] - x[3];
    k_c = x[4] - x[5];
    par[0] = -k_c / sqrt(k_a * k_a + k_b * k_b);
    par[1] = atan2(k_b, k_a);

    // Jetzt andere Geradennormierung a-c=1

    a[NN - 1] = -1.0;
    a[NN - 2] = 1.0;
    a[2 * NN - 1] = +1.0;
    a[2 * NN - 2] = -1.0;
    a[3 * NN - 1] = +1.0;
    a[3 * NN - 2] = -1.0;
    a[4 * NN - 1] = -1.0;
    a[4 * NN - 2] = +1.0;

#include "simplex_init.inc"

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
             &zf2,           /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
#include "simplex_free.inc"
        delete [] ia;
        delete [] ka;
        delete [] a;
        delete [] b;
        delete [] c;
        delete [] x;
        delete [] y;
        delete [] w;
        delete [] z;
        delete [] gew;
        return code;
      }

#include "simplex_free.inc"

    if (zf2 > zf1)
      {
        k_a = x[0] - x[1];
        k_b = x[2] - x[3];
        k_c = x[4] - x[5];
        par[0] = -k_c / sqrt(k_a * k_a + k_b * k_b);
        par[1] = atan2(k_b, k_a);
      }

    delete [] ia;
    delete [] ka;
    delete [] a;
    delete [] b;
    delete [] c;
    delete [] x;
    delete [] y;
    delete [] w;
    delete [] z;
    delete [] gew;

    FreePointList(plist);
    return code;
  }

  int FitLineLinOpt(const Matrix& m, double& p, double& phi, double limit)
  {
    if ((m.cols() < 2) || (m.rows() < 2))
      throw IceException(FNAME, M_WRONG_POINTLIST);

    PointList pl = Matrix2PointList(m);
    double par[2];

    try
      {
        FitLineLinOpt(pl, 0, m.rows() - 1, 1, par, limit);
      }
    catch (IceException& ex)
      {
        FreePointList(pl);
        throw IceException(ex, FNAME);
      }

    p = par[0];
    phi = par[1];
    FreePointList(pl);
    return OK;
  }

  LineSeg FitLineLinOpt(const std::vector<Point>& pl, int step, double limit)
  {
    int nPoints = pl.size();

    if (nPoints < 2)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    PointList ppl = NewPointList(nPoints);

    for (int i = 0; i < nPoints; i++)
      {
        PutPoint(ppl, i, pl[i].x, pl[i].y, 1.0);
      }

    double par[2];

    try
      {
        FitLineLinOpt(ppl, 0, pl.size() - 1, step, par, limit);
      }
    catch (IceException& ex)
      {
        FreePointList(ppl);
        throw IceException(ex, FNAME);
      }
    LineSeg res(par[0], par[1]);
    FreePointList(ppl);
    return res;
  }

#undef FNAME

// **************************************************************************
// **************************************************************************
//
// Fitting eines Kreises mittels Linearer Optimierung
//
// **************************************************************************
// **************************************************************************

#define FNAME "FitCircleLinOpt"

  int FitCircleLinOpt(PointList pl, int a1, int a2, int step, double par[], double limit)
  {

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    if (a1 < 0 || a2 > pl->lng - 1)
      throw IceException(FNAME, M_WRONG_VALUE);

    if (step < 0)
      throw IceException(FNAME, M_WRONG_VALUE);

    if (pl->lng < 3)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // ein Kreis angepaßt
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
    int i, j, k, l;
    double k_a, k_b, k_c, k_d;
    PointList plist;

    N = 0;

    for (i = a1; i <= a2; i = i + step)
      {
        N++;
      }

    plist = NewPointList(N);
    k = 0;

    for (i = a1; i <= a2; i = i + step)
      {
        plist->xptr[k] = pl->xptr[i];
        plist->yptr[k] = pl->yptr[i];
        plist->wptr[k] = pl->wptr[i];
        k++;
      }

    NN = N + 3;
    m = N + 3;
    n = 8 + N;
    nz = 8 * N + N + 8 + N + 4;
    ia = new int[nz + N + 3];
    ka = new int[n + N + 3 + 1];
    a = new double[nz + N + 3];

    for (i = 0; i < nz + N + 3; ++i)
      {
        ia[i] = 0;
        a[i] = 0.0;
      }

    for (i = 0; i < n + N + 3 + 1; ++i)
      {
        ka[i] = 0;
      }

    b = new double[m];

    for (i = 0; i < m; ++i)
      {
        b[i] = 0.0;
      }

    c = new double[n + N + 3];
    x = new double[n + N + 3];
    y = new double[n + N + 3];
    w = new double[n + N + 3];
    z = new double[n + N + 3];

    for (i = 0; i < n + N + 3; ++i)
      {
        c[i] = 0.0;
        x[i] = 0.0;
        y[i] = 0.0;
        w[i] = 0.0;
        z[i] = 0.0;
      }

    gew = new double[N + 3];

    //************** ******************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl entnommen
    for (i = 0; i < N; ++i)
      {
        gew[i] = plist->wptr[i];
      }

    gew[N] = 0.0;
    gew[N + 1] = 0.0;
    gew[N + 2] = 0.0;

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
    for (i = 0; i < m - 1; i++)
      {
        b[i] = 0.0;
      }

    b[m - 3] = limit;
    b[m - 2] = 1.0;
    b[m - 1] = -1.0;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 3; ++i)
      {
        ka[i] = i * NN;
      }

    for (i = 3; i < 9; ++i)
      {
        ka[i] = ka[i - 1] + (N + 1);
      }

    for (i = 9; i < 9 + N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }

    // *************************
    // Speziell: Füllen von ia
    for (j = 0; j < 2; ++j)
      {
        k = 0;

        for (i = 0; i < m; i++)
          {
            ia[j * m + i] = k;
            k++;
          }
      }

    l = 2 * m;

    for (j = 0; j < 6; ++j)
      {
        k = 0;

        for (i = 0; i < (N + 1); i++)
          {
            ia[l + j * (N + 1) + i] = k;
            k++;
          }
      }

    l = l + 6 * (N + 1);
    k = 0;

    for (i = l; i < l + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 3;
        ++k;
      }

    // ***************************

    // Speziell: Füllen von a

    for (i = 0; i < N; ++i)
      {
        a[i] = (plist->xptr[i]) * (plist->xptr[i]) + (plist->yptr[i]) * (plist->yptr[i]);
      }

    a[NN - 1] = -1.0;
    a[NN - 2] = 1.0;
    a[NN - 3] = 1.0;
    k = 0;

    for (i = NN; i < 2 * NN - 3; ++i)
      {
        a[i] = -((plist->xptr[k]) * (plist->xptr[k]) + (plist->yptr[k]) * (plist->yptr[k]));
        ++k;
      }

    a[2 * NN - 1] = +1.0;
    a[2 * NN - 2] = -1.0;
    a[2 * NN - 3] = 1.0;
    l = 2 * NN;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = plist->xptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -plist->xptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = plist->yptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -plist->yptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = 1.0;
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -1.0;
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;

    for (i = l; i < l + 2 * N; i = i + 2)
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

#include "simplex_init.inc"

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
             &zf,           /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
#include "simplex_free.inc"
        delete [] ia;
        delete [] ka;
        delete [] a;
        delete [] b;
        delete [] c;
        delete [] x;
        delete [] y;
        delete [] w;
        delete [] z;
        delete [] gew;
        return code;
      }

#include"simplex_free.inc"

    k_a = x[0] - x[1];
    k_b = x[2] - x[3];
    k_c = x[4] - x[5];
    k_d = x[6] - x[7];

    par[0] = -k_b / 2.0;
    par[1] = -k_c / 2.0;
    par[2] = sqrt(k_b * k_b / 4.0 + k_c * k_c / 4.0 - k_d);

    delete [] ia;
    delete [] ka;
    delete [] a;
    delete [] b;
    delete [] c;
    delete [] x;
    delete [] y;
    delete [] w;
    delete [] z;
    delete [] gew;

    FreePointList(plist);
    return code;
  }

#undef FNAME

// *********************************************************************************
// *********************************************************************************
//
// Fitting einer Ellipse mittels Linearer Optimierung
//
// *********************************************************************************
// *********************************************************************************

#define FNAME "FitEllipseLinOpt"

  int FitEllipseLinOpt(PointList pl, int a1, int a2, int step, double par[], double limit)
  {

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_POINTLIST);

    if (a1 < 0 || a2 > pl->lng - 1)
      throw IceException(FNAME, M_WRONG_INDEX);

    if (step < 0)
      throw IceException(FNAME, M_WRONG_VALUE);

    if (pl->lng < 5)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    // Bezüglich  der Punktlisten pl1 und pl2
    // wird mittels linearer Optimierung
    // eine Ellipse angepaßt
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
    int i, j, k, l;
    double k_a, k_b, k_c, k_d, k_e, k_f;
    int type;
    double koef[6];
    PointList plist;

    N = 0;

    for (i = a1; i <= a2; i = i + step)
      {
        N++;
      }

    plist = NewPointList(N);
    k = 0;

    for (i = a1; i <= a2; i = i + step)
      {
        plist->xptr[k] = pl->xptr[i];
        plist->yptr[k] = pl->yptr[i];
        plist->wptr[k] = pl->wptr[i];
        k++;
      }

    NN = N + 3;
    m = N + 3;
    n = 12 + N;
    nz = 12 * N + N + 12 + N + 8;
    ia = new int[nz + N + 3];
    ka = new int[n + N + 3 + 1];
    a = new double[nz + N + 3];

    for (i = 0; i < nz + N + 3; ++i)
      {
        ia[i] = 0;
        a[i] = 0.0;
      }

    for (i = 0; i < n + N + 3 + 1; ++i)
      {
        ka[i] = 0;
      }

    b = new double[m];

    for (i = 0; i < m; ++i)
      {
        b[i] = 0.0;
      }

    c = new double[n + N + 3];
    x = new double[n + N + 3];
    y = new double[n + N + 3];
    w = new double[n + N + 3];
    z = new double[n + N + 3];

    for (i = 0; i < n + N + 3; ++i)
      {
        c[i] = 0.0;
        x[i] = 0.0;
        y[i] = 0.0;
        w[i] = 0.0;
        z[i] = 0.0;
      }

    gew = new double[N + 3];

    //************** ******************
    // Füllen der Gewichte
    // Diese werden aus der Punktliste pl entnommen
    for (i = 0; i < N; ++i)
      {
        gew[i] = plist->wptr[i];
      }

    gew[N] = 0.0;
    gew[N + 1] = 0.0;
    gew[N + 2] = 0.0;

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
    for (i = 0; i < m - 1; i++)
      {
        b[i] = 0.0;
      }

    b[m - 3] = limit;
    b[m - 2] = 1.0;
    b[m - 1] = -1.0;

    // ****************************
    // Füllen der Koeffizientenmatrix
    // Speziell: Füllen von ka
    // auf ka stehen die Anzahlen der Elemente ungleich Null pro Spalte
    // es wird aber mit Null begonnen, und es wird pro Spalte die
    // Summe aller Elemente (aller Spalten bis zu dieser Spalte)
    // abgelegt, deshalb der Beginn mit Null
    for (i = 0; i < 5; ++i)
      {
        ka[i] = i * NN;
      }

    for (i = 5; i < 13; ++i)
      {
        ka[i] = ka[i - 1] + (N + 1);
      }

    for (i = 13; i < 13 + N; ++i)
      {
        ka[i] = ka[i - 1] + 2;
      }

    // *************************
    // Speziell: Füllen von ia
    for (j = 0; j < 4; ++j)
      {
        k = 0;

        for (i = 0; i < m; i++)
          {
            ia[j * m + i] = k;
            k++;
          }
      }

    l = 4 * m;

    for (j = 0; j < 8; ++j)
      {
        k = 0;

        for (i = 0; i < (N + 1); i++)
          {
            ia[l + j * (N + 1) + i] = k;
            k++;
          }
      }

    l = l + 8 * (N + 1);
    k = 0;

    for (i = l; i < l + 2 * N; i = i + 2)
      {
        ia[i] = k;
        ia[i + 1] = m - 3;
        ++k;
      }

    // ***************************

    // Speziell: Füllen von a

    for (i = 0; i < N; ++i)
      {
        a[i] = (plist->xptr[i]) * (plist->xptr[i]);
      }

    a[NN - 1] = -1.0;
    a[NN - 2] = 1.0;
    a[NN - 3] = 1.0;
    k = 0;

    for (i = NN; i < 2 * NN - 3; ++i)
      {
        a[i] = -((plist->xptr[k]) * (plist->xptr[k]));
        k++;
      }

    a[2 * NN - 1] = +1.0;
    a[2 * NN - 2] = -1.0;
    a[2 * NN - 3] = 1.0;
    k = 0;

    for (i = 2 * NN; i < 3 * NN - 3; ++i)
      {
        a[i] = +((plist->yptr[k]) * (plist->yptr[k]));
        k++;
      }

    a[3 * NN - 1] = -1.0;
    a[3 * NN - 2] = +1.0;
    a[3 * NN - 3] = 1.0;
    k = 0;

    for (i = 3 * NN; i < 4 * NN - 3; ++i)
      {
        a[i] = -((plist->yptr[k]) * (plist->yptr[k]));
        k++;
      }

    a[4 * NN - 1] = +1.0;
    a[4 * NN - 2] = -1.0;
    a[4 * NN - 3] = 1.0;

    l = 4 * NN;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = (plist->xptr[k]) * (plist->yptr[k]);
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -(plist->xptr[k]) * (plist->yptr[k]);
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = plist->xptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -plist->xptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = plist->yptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -plist->yptr[k];
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = 1.0;
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;
    k = 0;

    for (i = l; i < l + N; ++i)
      {
        a[i] = -1.0;
        ++k;
      }

    a[l + N] = 1.0;
    l = l + N + 1;

    for (i = l; i < l + 2 * N; i = i + 2)
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

#include "simplex_init.inc"

    code = simplex_method_modified(
             m,   /* number of constraints */
             n,   /* number of variables */
             nz,    /* number of nonzeros in sparse constraint matrix */
             ia,    /* array row indices */
             ka,    /* array of indices into ia and a */
             a,   /* array of nonzeros in the constraint matrix */
             b,     /* right-hand side */
             c,          /* objective coefficients */
             f,     /* objective function shift */
             x,     /* primal solution (output) */
             y,   /* dual solution (output) */
             w,     /* primal slacks (output) */
             z,     /* dual slacks (output) */
             &zf,        /* Optimal value of the objective function */
             gew
           );

    if (code != 0)
      {
#include "simplex_free.inc"
        delete [] ia;
        delete [] ka;
        delete [] a;
        delete [] b;
        delete [] c;
        delete [] x;
        delete [] y;
        delete [] w;
        delete [] z;
        delete [] gew;
        return code;
      }

#include "simplex_free.inc"
    k_a = x[0] - x[1];
    k_b = x[2] - x[3];
    k_c = x[4] - x[5];
    k_d = x[6] - x[7];
    k_e = x[8] - x[9];
    k_f = x[10] - x[11];

    koef[0] = k_a;
    koef[1] = k_b;
    koef[2] = k_d;
    koef[3] = k_e;
    koef[4] = k_c;
    koef[5] = k_f;

    FeatureQuadrFunc(koef, par, &type);

    if (type != ELLIPSE)
      {
        code = 10;
      }

    delete [] ia;
    delete [] ka;
    delete [] a;
    delete [] b;
    delete [] c;
    delete [] x;
    delete [] y;
    delete [] w;
    delete [] z;
    delete [] gew;

    FreePointList(plist);
    return code;
  }
#undef FNAME
}
