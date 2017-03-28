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

#include "message.h"

#include "simplex.h"
#include "simplex8.h"
#include "simplex_glbl.h"
#include "simplex4.h"

/* Globale Variablen */
// ******************************************************
// Globale bez. File simplex1.cpp und Funktion Nt_times_y
namespace ice
{

  double* Nt_times_y_a = NULL;
  int*  Nt_times_y_tag = NULL;
  int* Nt_times_y_link = NULL;
  int  Nt_times_y_currtag = 1;

// *****************************************************
// Globale bez. File simplex4.cpp - sind dort bez. des Files global

  TNODE* si4_root = NULL;
// ******************************************************
// Globale bezüglich des Files simplex3.cpp
// Bezüglich der Funktion btsolve
  double* btsolve_y = NULL;
  int*  btsolve_tag = NULL;
  int  btsolve_currtag = 1;
// **********************
// Bezüglich der Funktion bsolve
  double* bsolve_y = NULL;
  int*  bsolve_tag = NULL;
  int  bsolve_currtag = 1;
// **********************
// In der Funktion Gauss_Eta
  double* Gauss_Eta_a = NULL;
  int*  Gauss_Eta_tag = NULL;
  int* Gauss_Eta_link = NULL;
  int  Gauss_Eta_currtag = 1;
// In der Funktion Gauss_Eta_T
  double* Gauss_Eta_T_a = NULL;
  int*  Gauss_Eta_T_tag = NULL;
  int  Gauss_Eta_T_currtag = 1;
// Globale bez. des Files simplex3.cpp

  int*  si3_E_d = NULL; /* Eta file; new column location */
  double*  si3_E = NULL; /* Eta file - values (sparse matrix) */
  int* si3_iE = NULL;  /* Eta file - row indices */
  int* si3_kE = NULL;  /* Eta file - column start positions */
  int si3_e_iter = 0;  /* number of iterations since last refactorization */
  int si3_enz = 0;

  int si3_rank;
  int*  si3_kL = NULL, *si3_iL = NULL,
        *si3_kLt = NULL, *si3_iLt = NULL,
         *si3_kU = NULL, *si3_iU = NULL,
          *si3_kUt = NULL, *si3_iUt = NULL;
  int* si3_colperm = NULL, *si3_icolperm = NULL;
  int* si3_rowperm = NULL, *si3_irowperm = NULL;
  double* si3_L = NULL, *si3_Lt = NULL, *si3_U = NULL, *si3_Ut = NULL, *si3_diagU = NULL;

  double si3_cumtime = 0.0;
  double si3_ocumtime = 0.0;

//#define DEBUG

#define FNAME "LinearOptimization"
  int LinearOptimization(int rows, int columns,
                         double** matrix, double* r_side, double* cost_function,
                         double* solution, double& costs)
  {
    if ((rows <= 0) || (columns <= 0))
      {
        Message(FNAME, M_WRONG_VAL, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (matrix == NULL || r_side == NULL || cost_function == NULL || solution == NULL)
      {
        Message(FNAME, M_WRONG_PTR, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
    double zf;            /* Optimal cost value   */

    int code;             /* returncode from simplex_method   */
    int i, j, k, anz;

    m = rows;
    n = columns;
    nz = 0;

    for (i = 0; i < m; ++i)
      for (j = 0; j < n; ++j)
        {
          if (matrix[i][j] != 0.0) nz++;
        }

    ia = new int[nz + m];
    ka = new int[n + m + 1];
    a = new double[nz + m];

    for (i = 0; i < nz + m; ++i)
      {
        ia[i] = 0;
        a[i] = 0.0;
      }

    for (i = 0; i < n + m + 1; ++i)
      ka[i] = 0;

    b = new double[m];

    for (i = 0; i < m; ++i)
      b[i] = 0.0;

    c = new double[n + m];
    x = new double[n + m];
    y = new double[n + m];
    w = new double[n + m];
    z = new double[n + m];

    for (i = 0; i < n + m; ++i)
      {
        c[i] = 0.0;
        x[i] = 0.0;
        y[i] = 0.0;
        w[i] = 0.0;
        z[i] = 0.0;
      }

    // ***********************************************
    // Füllen Zielfunktion

    for (i = 0; i < n; ++i)
      c[i] = -cost_function[i];

    // Füllen Rechte Seite

    for (i = 0; i < m; ++i)
      b[i] = r_side[i];

    // Füllen der Matrixdaten

    ka[0] = 0;

    for (j = 1; j < n + 1; ++j)
      {
        anz = 0;

        for (i = 0; i < m; ++i)
          {
            if (matrix[i][j - 1] != 0) anz++;
          }

        ka[j] = ka[j - 1] + anz;
      }

    for (i = 0; i < n; ++i)
      {

        anz = 0;

        for (k = 0; k < m; ++k)
          {
            if (matrix[k][i] != 0)
              {
                ia[ka[i] + anz] = k;
                ++anz;
              }
          }

      }

    for (i = 0; i < n; ++i)
      {

        anz = 0;

        for (k = 0; k < m; ++k)
          {
            if (matrix[k][i] != 0)
              {
                a[ka[i] + anz] = matrix[k][i];
                ++anz;
              }
          }

      }

#ifdef DEBUG
    // Anzeige der Eingabedaten
    Printf("Zielfunktion \n");

    for (i = 0; i < n; ++i)
      Printf("%f  ", c[i]);

    GetChar();
    Printf("\n Rechte Seite \n");

    for (i = 0; i < m; ++i)
      Printf("%f  ", b[i]);

    GetChar();
    Printf("Matrix: \n\n");
    Printf("Indexarray ka\n");

    for (i = 0; i < n + 1; ++i)
      Printf("%d  ", ka[i]);

    Printf("\n");
    GetChar();
    Printf("Indexarray ia\n");

    for (i = 0; i < nz; ++i)
      Printf("%d  ", ia[i]);

    Printf("\n");
    GetChar();
    Printf("Datenfeld a\n");

    for (i = 0; i < nz ; ++i)
      Printf("%f  ", a[i]);

    Printf("\n");
    GetChar();

#endif

#include "simplex_init.inc"

    double* gew = new double[m];

    for (int i = 0; i < m; i++)
      gew[i] = 1.0;

    code = simplex_method_modified(
             m, /* number of constraints */
             n, /* number of variables */
             nz,/* number of nonzeros in sparse constraint matrix */
             ia,/* array row indices */
             ka,/* array of indices into ia and a */
             a, /* array of nonzeros in the constraint matrix */
             b, /* right-hand side */
             c, /* objective coefficients */
             f, /* objective function shift */
             x, /* primal solution (output) */
             y, /* dual solution (output) */
             w, /* primal slacks (output) */
             z, /* dual slacks (output) */
             &zf,
             gew
           );


    for (i = 0; i < n; ++i)
      solution[i] = x[i];

    costs = zf;

#include"simplex_free.inc"

    delete [] gew;
    delete [] ia;
    delete [] ka;
    delete []  a;
    delete []  b;
    delete []  c;
    delete []  x;
    delete []  y;
    delete []  w;
    delete []  z;

    return code;
  }
#undef FNAME
}
