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
#ifndef _SIMPLEX8_H_
#define _SIMPLEX8_H_
namespace ice
{
#if 0
  int LinearOptimization(int m, int n,
                         double** matrix, double* r_side, double* cost_function,
                         double* solution, double& costs);
#endif
  int simplex_method(
    int m,  /* number of constraints */
    int n,  /* number of variables */
    int nz, /* number of nonzeros in sparse constraint matrix */
    int* ia,  /* array row indices */
    int* ka,  /* array of indices into ia and a */
    double* a,  /* array of nonzeros in the constraint matrix */
    double* b,  /* right-hand side */
    double* c,   /* objective coefficients */
    double  f,  /* objective function shift */
    double* x,  /* primal solution (output) */
    double* y,  /* dual solution (output) */
    double* w,  /* primal slacks (output) */
    double* z,  /* dual slacks (output) */
    double* zf   /* Optimal value of the objective function */
  );

  int simplex_method_modified(
    int m,  /* number of constraints */
    int n,  /* number of variables */
    int nz, /* number of nonzeros in sparse constraint matrix */
    int* ia,  /* array row indices */
    int* ka,  /* array of indices into ia and a */
    double* a,  /* array of nonzeros in the constraint matrix */
    double* b,  /* right-hand side */
    double* c,  /* objective coefficients */
    double  f,  /* objective function shift */
    double* x,  /* primal solution (output) */
    double* y,  /* dual solution (output) */
    double* w,  /* primal slacks (output) */
    double* z,  /* dual slacks (output) */
    double* zf, /* Optimal value of the objective function */
    double* gew /* Weights for Schlupfvariable */
  );

}
#endif
