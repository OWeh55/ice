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
/**************************************
Loesen von linearen Gleichungsystemen
(Genauigkeit double)
**************************************/

#ifndef _EQUSYS_H
#define _EQUSYS_H

namespace ice
{
  typedef double(*FuncD)(double*);
  typedef double(*FuncDV)(double);

  /* Pointer auf Funktionen mit Rueckgabewert "double" */

  int EquSys
  (double* A, double* b, int dim, double* x);
  int EquSysMultiple
  (double A[], double b[], double x[],
   int row, int col, double* pdet, int* prang);
  int OverEquSys
  (double* a, double* b, int row, int col, double* x, double* mse);
  int NonLinEquSys
  (FuncD*, int, int*, double*, double*);
  int NormalEqu(double* a, double* b, int row, int col, double* c, double* d);
}
#endif
