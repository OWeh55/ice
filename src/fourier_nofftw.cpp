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
/* Fourier- und Hartley-Transformation (eindimensional)
 * Wolfgang Ortmann  1997, 1998, 2013
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "darith.h"

#include "fourier.h"

using namespace std;

namespace ice
{

#ifdef NOFFTW3

  /* Nichtoeffentliche Prototypen */
  int FFT1D(double* fr, double* fi, int n, int back);
  int FT1D(double* fr, double* fi, int n, int back);

#define STATICSINCOSTABSIZE 4096

  static int SinCosTabSize = 0;
  static int TabInvers = false;
  static double SinTab[STATICSINCOSTABSIZE];
  static double CosTab[STATICSINCOSTABSIZE];
  typedef double* dptr;

  int makesincostab(int size, dptr& sin, dptr& cos, int invers);
  int freesincostab(int size, dptr& sin, dptr& cos);

#define FNAME "MakeSinCosTab"
  int makesincostab(int size, dptr& sint, dptr& cost, int invers)
// creates Sinus / Cosinus tables if neccessary
// returns pointer to tables
// creates "inverse" table for inverse transform if (invers)
  {
    const double pi2 = M_PI * 2;
    int i;

    if ((size == SinCosTabSize) && (invers == TabInvers))
      {
        sint = SinTab;
        cost = CosTab;
        return OK;
      }

    if (size <= STATICSINCOSTABSIZE)
      {
        sint = SinTab;
        cost = CosTab;
        SinCosTabSize = size;
        TabInvers = invers;
      }
    else
      {
        sint = new double[size];

        if (sint == NULL)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return NO_MEM;
          }

        cost = new double[size];

        if (cost == NULL)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return NO_MEM;
          }
      }

    double n = size;

    for (i = 0; i < size; ++i)
      {
        cost[i] = cos((double)i * pi2 / n);

        if (!invers)
          {
            sint[i] = sin((double)i * pi2 / n);
          }
        else
          {
            sint[i] = -sin((double)i * pi2 / n);
          }
      }

    return OK;
  }
#undef FNAME

  int freesincostab(int size, dptr& sint, dptr& cost)
  {
    if (size > STATICSINCOSTABSIZE)
      {
        delete [] sint;
        delete [] cost;
      }

    return OK;
  }

// Fourier
  int FFT1D(double* fr, double* fi, int n, int back)
  /*
    transforms a complex field with real part fr and
    imaginary part fi with length n
    Fast algorithm for n equal to 2^i
  */
  {
    double ur, ui, wr, wi, tr, ti, b;
    const double pi = M_PI;
    int nv2, j, i, k, l, le, le1, ip, ln;
    wi = 0;
    ln = 0;

    for (i = 1; i < n; i = i + i)
      {
        ln++;
      }

    for (nv2 = n / 2, i = j = 1; i < n; ++i)
      {
        if (i < j)
          {
            tr = fr[j - 1];
            ti = fi[j - 1];
            fr[j - 1] = fr[i - 1];
            fi[j - 1] = fi[i - 1];
            fr[i - 1] = tr;
            fi[i - 1] = ti;
          }

        k = nv2;

        while (k < j)
          {
            j = j - k;
            k = k / 2;
          }

        j = j + k;
      }

    le = 1;

    for (l = 1; l <= ln; l++)
      {
        le = le * 2;
        le1 = le / 2;
        ur = 1.0;
        ui = 0.0;
        wr = cos(pi / (double)le1);

        if (!back)
          {
            wi = -sin(pi / (double)le1);
          }
        else
          {
            wi = sin(pi / (double)le1);
          }

        for (j = 1; j <= le1; j++)
          {
            for (i = j; i <= n; i += le)
              {
                ip = i + le1;
                tr = fr[ip - 1] * ur - fi[ip - 1] * ui;
                ti = fr[ip - 1] * ui + fi[ip - 1] * ur;
                fr[ip - 1] = fr[i - 1] - tr;
                fi[ip - 1] = fi[i - 1] - ti;
                fr[i - 1] += tr;
                fi[i - 1] += ti;
              }

            tr = ur * wr - ui * wi;
            ti = ur * wi + ui * wr;
            ur = tr;
            ui = ti;
          }
      }

    b = sqrt((double)n);

    for (i = 0; i < n; i++)
      {
        fr[i] /= b;
        fi[i] /= b;
      }

    return OK;
  }

#define FNAME "FT1D"
//------------------------------------------------
  int FT1D(double* fr, double* fi, int n, int back)
  /*
    transforms a complex field with real part fr and
    imageinary part fi with length n
    Normal algorithm for n _not_ equal to potences of 2
  */
  {
    double b, *wr, *wi, *tr, *ti;
    int i, j;
    int index;

    if (makesincostab(n, wi, wr, back) != OK)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    tr = new double[n];

    if (tr == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    ti = new double[n];

    if (ti == NULL)
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return NO_MEM;
      }

    for (i = 0; i < n; ++i)
      {
        tr[i] = ti[i] = 0.0;
        index = 0;

        for (j = 0; j < n; ++j)
          {
            tr[i] += fr[j] * wr[index] + fi[j] * wi[index];
            ti[i] += -fr[j] * wi[index] + fi[j] * wr[index];
            index = (index + i) % n;
          }
      }

    b = sqrt((double)n);

    for (i = 0; i < n; ++i)
      {
        fr[i] = tr[i] / b;
        fi[i] = ti[i] / b;
      }

    delete [] tr;
    delete [] ti;
    freesincostab(n, wi, wr);
    return OK;
  }
#undef FNAME
  /*-----------------------------------------------------------*/
#define FNAME "FourierD"
  int FourierD(const double* srcre, const double* srcim,
               int n, int option,
               double* re, double* im)
  {
    /*
      transforms a complex field with real part fr and
      imageinary part fi with length n
      control part with parameter checking and selection of
      the suitable algorithm
    */

    //    int zp[12]={2,4,8,16,32,64,128,256,512,1024,2048,4096};

    int i, use_fast;
    int inverse;

    if (option != NORMAL && option != INVERS)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (n < 1)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (re == NULL || im == NULL || srcre == NULL || srcim == NULL)
      {
        throw IceException(FNAME, M_WRONG_PTR, WRONG_POINTER);
        return WRONG_POINTER;
      }

    inverse = (option != NORMAL);

    if (re != srcre)
      for (i = 0; i < n; i++)
        {
          re[i] = srcre[i];
        }

    if (im != srcim)
      for (i = 0; i < n; i++)
        {
          im[i] = srcim[i];
        }

    use_fast = (n & (n - 1)) == 0;

    if (use_fast)
      {
        return FFT1D(re, im, n, inverse);
      }
    else
      {
        return FT1D(re, im, n, inverse);
      }
  }

  int Fourier(vector<double>& dstre, vector<double>& dstim,
              int option)
  {
    /*
      transforms a complex field with real part srcre and
      imageinary part srcim
      here: with parameter checking and selection of
      the suitable algorithm
    */

    bool use_fast;
    int inverse;

    if (option != NORMAL && option != INVERS)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    unsigned int dim = dstre.size();

    if (dim < 1 || dim != dstim.size())
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    inverse = (option != NORMAL);

    use_fast = (dim & (dim - 1)) == 0;

    if (use_fast)
      {
        return FFT1D(dstre.data(), dstim.data(), dim, inverse);
      }
    else
      {
        return FT1D(dstre.data(), dstim.data(), dim, inverse);
      }
  }

  int Fourier(const vector<double>& srcre, const vector<double>& srcim,
              vector<double>& dstre, vector<double>& dstim,
              int option)
  {
    /*
      transforms a complex field with real part srcre and
      imageinary part srcim
      here: with parameter checking and selection of
      the suitable algorithm
    */

    //    int zp[12]={2,4,8,16,32,64,128,256,512,1024,2048,4096};

    bool use_fast;
    int inverse;

    if (option != NORMAL && option != INVERS)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    unsigned int dim = srcre.size();

    if (dim < 1 || dim != srcim.size())
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    dstre = srcre;
    dstim = srcim;

    inverse = (option != NORMAL);

    use_fast = (dim & (dim - 1)) == 0;

    if (use_fast)
      {
        return FFT1D(dstre.data(), dstim.data(), dim, inverse);
      }
    else
      {
        return FT1D(dstre.data(), dstim.data(), dim, inverse);
      }
  }
#endif
#undef FNAME
}
