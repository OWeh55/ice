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
// Freigabe von Speicherbereichen, deren Adresse als globale Variable
// vereinbart wurden
// ********************

FREE(Nt_times_y_a);
FREE(Nt_times_y_tag);
if (Nt_times_y_link != NULL) Nt_times_y_link--;
FREE(Nt_times_y_link);
// ********************

FREE(si4_root);
// *******************

FREE(btsolve_y);
FREE(btsolve_tag);
// *******************

FREE(bsolve_y);
FREE(bsolve_tag);
// *********************
FREE(Gauss_Eta_a);
FREE(Gauss_Eta_tag);
if (Gauss_Eta_link != NULL) Gauss_Eta_link--;
FREE(Gauss_Eta_link);
// ********************
FREE(Gauss_Eta_T_a);
FREE(Gauss_Eta_T_tag);
// ********************
FREE(si3_E_d);
FREE(si3_E);    /* Eta file - values (sparse matrix) */
FREE(si3_iE);    /* Eta file - row indices */
FREE(si3_kE);    /* Eta file - column start positions */

FREE(si3_kL);
FREE(si3_iL);
FREE(si3_kLt);
FREE(si3_iLt);
FREE(si3_kU);
FREE(si3_iU);
FREE(si3_kUt);
FREE(si3_iUt);
FREE(si3_colperm);
FREE(si3_icolperm);
FREE(si3_rowperm);
FREE(si3_irowperm);
FREE(si3_L);
FREE(si3_Lt);
FREE(si3_U);
FREE(si3_Ut);
FREE(si3_diagU);
