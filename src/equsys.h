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
  (double* a, double* b, int row , int col, double* x, double* mse);
  int NonLinEquSys
  (FuncD*, int, int*, double*, double*);
  int NormalEqu(double* a, double* b, int row, int col, double* c, double* d);
}
#endif
