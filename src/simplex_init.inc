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
// Initialisierung aller globalen Variablen
// ********************
Nt_times_y_a = nullptr;
Nt_times_y_tag = nullptr;
Nt_times_y_link = nullptr;
Nt_times_y_currtag = 1;
// ********************

si4_root = nullptr;
// *******************

btsolve_y = nullptr;
btsolve_tag = nullptr;
btsolve_currtag = 1;
// *******************

bsolve_y = nullptr;
bsolve_tag = nullptr;
bsolve_currtag = 1;
// *********************
Gauss_Eta_a = nullptr;
Gauss_Eta_tag = nullptr;
Gauss_Eta_link = nullptr;
Gauss_Eta_currtag = 1;
// ********************
Gauss_Eta_T_a = nullptr;
Gauss_Eta_T_tag = nullptr;
Gauss_Eta_T_currtag = 1;
// ********************
si3_E_d = nullptr;
si3_E = nullptr;  /* Eta file - values (sparse matrix) */
si3_iE = nullptr;  /* Eta file - row indices */
si3_kE = nullptr;  /* Eta file - column start positions */
si3_e_iter = 0; /* number of iterations since last refactorization */
si3_enz = 0;

si3_kL = nullptr;
si3_iL = nullptr;
si3_kLt = nullptr;
si3_iLt = nullptr;
si3_kU = nullptr;
si3_iU = nullptr;
si3_kUt = nullptr;
si3_iUt = nullptr;
si3_colperm = nullptr;
si3_icolperm = nullptr;
si3_rowperm = nullptr;
si3_irowperm = nullptr;
si3_L = nullptr;
si3_Lt = nullptr;
si3_U = nullptr;
si3_Ut = nullptr;
si3_diagU = nullptr;

si3_cumtime = 0.0;
si3_ocumtime = 0.0;
