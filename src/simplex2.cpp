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
#include <stdlib.h>
#include "numbase.h"

#ifdef QuadPrec
#include "Quad.h"
#define double Quad
#else
#define high(x) (x)
#endif


#include "simplex2.h"
#include "simplex4.h"

namespace ice
{
  /*---------------------------------------------------------------+
  |  inner product between n-vectors x and y                      */

  double dotprod(double* x, double* y, int n)
  {
    int i;
    double dotprod = 0.0e0;

    for (i = 0; i < n; i++) dotprod += x[i] * y[i];

    return (dotprod);
  }

  /*---------------------------------------------------------------+
  |  y = basis submatrix of (a,ka,ia) times x                     */

  void bmx(int m, double* a, int* ka, int* ia, int* basis,
           double* x, double* y)
  {
    int i, j, k;

    for (i = 0; i < m; i++) y[i] = 0.0e0;

    for (i = 0; i < m; i++)
      {
        j = basis[i];

        for (k = ka[j]; k < ka[j + 1]; k++)
          y[ia[k]] += a[k] * x[i];
      }
  }

  /*---------------------------------------------------------------+
  |  y = basis submatrix of (a,ka,ia) transpose times x           */

  void btmx(int m, double* a, int* ka, int* ia, int* basis,
            double* x, double* y)
  {
    int i, j, k;

    for (i = 0; i < m; i++) y[i] = 0.0e0;

    for (i = 0; i < m; i++)
      {
        j = basis[i];

        for (k = ka[j]; k < ka[j + 1]; k++)
          y[i] += a[k] * x[ia[k]];
      }
  }

  /*---------------------------------------------------------------+
  |  y = sparse matrix (a,ka,ia) times x                          */

  void smx(int m, int n, double* a, int* ka, int* ia, double* x, double* y)
  {
    int i, j, k;

    for (i = 0; i < m; i++) y[i] = 0.0e0;

    for (j = 0; j < n; j++)
      for (k = ka[j]; k < ka[j + 1]; k++)
        y[ia[k]] += a[k] * x[j];
  }

  /*---------------------------------------------------------------+
  |  (kat,iat,at) = transpose of (ka,ia,a)                        */

  void atnum(int m, int n, int* ka, int* ia, double* a,
             int* kat, int* iat, double* at
            )
  {
    int i, j, k, row, addr;
    int* iwork;

    CALLOC(iwork, m, int);

    for (k = 0; k < ka[n]; k++)
      {
        row = ia[k];
        iwork[row]++;
      }

    kat[0] = 0;

    for (i = 0; i < m; i++)
      {
        kat[i + 1] = kat[i] + iwork[i];
        iwork[i] = 0;
      }

    for (j = 0; j < n; j++)
      {
        for (k = ka[j]; k < ka[j + 1]; k++)
          {
            row = ia[k];
            addr = kat[row] + iwork[row];
            iwork[row]++;
            iat[addr] = j;
            at[addr]  = a[k];
          }
      }

    FREE(iwork);
  }

  /*---------------------------------------------------------------+
  |  compute componentwise maximum of n-vector x                  */

  double maxv(double* x, int n)
  {
    int i;
    double maxv = 0.0e0;

    for (i = 0; i < n; i++) maxv = Max(maxv, Abs(x[i]));

    return (maxv);
  }
}
