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
#ifndef _SIMPLEX_INIT_H_
#define _SIMPLEX_INIT_H_

namespace ice
{
  /* Globale Variablen */
// ******************************************************
// Globale bez. File simplex1.cpp und Funktion Nt_times_y

  extern double* Nt_times_y_a;
  extern int*  Nt_times_y_tag;
  extern int* Nt_times_y_link;
  extern int  Nt_times_y_currtag;

// *****************************************************
// Globale bez. File simplex4.cpp - sind dort bez. des Files global

  typedef struct tnode
  {
    int           data;
    struct tnode* parent;
    struct tnode* left;
    struct tnode* right;
  } TNODE;

  extern TNODE* si4_root;
// ******************************************************
// Globale bezüglich des Files simplex3.cpp
// Bezüglich der Funktion btsolve
  extern double* btsolve_y;
  extern int*  btsolve_tag;
  extern int  btsolve_currtag;
// **********************
// Bezüglich der Funktion bsolve
  extern double* bsolve_y;
  extern int*  bsolve_tag;
  extern int  bsolve_currtag;
// **********************
// In der Funktion Gauss_Eta
  extern double* Gauss_Eta_a;
  extern int*  Gauss_Eta_tag;
  extern int* Gauss_Eta_link;
  extern int  Gauss_Eta_currtag;
// In der Funktion Gauss_Eta_T
  extern double* Gauss_Eta_T_a;
  extern int*  Gauss_Eta_T_tag;
  extern int  Gauss_Eta_T_currtag;
// Globale bez. des Files simplex3.cpp

  extern int*  si3_E_d;  /* Eta file; new column location */
  extern double*  si3_E; /* Eta file - values (sparse matrix) */
  extern int* si3_iE;    /* Eta file - row indices */
  extern int* si3_kE;    /* Eta file - column start positions */
  extern int si3_e_iter ;  /* number of iterations since last refactorization */
  extern int si3_enz;

  extern int si3_rank;
  extern int*  si3_kL, *si3_iL,
         *si3_kLt, *si3_iLt,
         *si3_kU, *si3_iU,
         *si3_kUt, *si3_iUt;
  extern int* si3_colperm, *si3_icolperm;
  extern int* si3_rowperm, *si3_irowperm;
  extern double* si3_L, *si3_Lt, *si3_U, *si3_Ut, *si3_diagU;

  extern double si3_cumtime ;
  extern double si3_ocumtime;
}
#endif
