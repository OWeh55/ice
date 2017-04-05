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
 * Wolfgang Ortmann, 97, 98
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "fourier.h"

#ifndef NOFFTW3

#include <fftw3.h>

using namespace std;

namespace ice
{
  static fftw_complex* in = NULL;
  static fftw_complex* out = NULL;
  static int fftw_size = 0;
  static fftw_plan fftw_p;
  static bool isforward = false;
  static double fftnorm = 0.0;

#define FNAME "FourierD"
  int FourierD(const double* srcre, const double* srcim,
               int n, int option,
               double* re, double* im)
  {
    if (re == NULL || im == NULL || srcre == NULL || srcim == NULL)
      throw IceException(FNAME, M_WRONG_PTR);

    if (option != NORMAL && option != INVERS)
      throw IceException(FNAME, M_WRONG_PARAM);

    bool forward = option == NORMAL;

    if (n < 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    if ((n != fftw_size) || (forward != isforward))
      {
        // altes array freigeben
        if (in != NULL)
          {
            fftw_free(in);
          }

        if (out != NULL)
          {
            fftw_free(out);
          }

        // plan freigeben
        fftw_destroy_plan(fftw_p);
        // neues array anlegen
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
        isforward = forward;
        fftw_size = n;
        fftnorm = 1.0 / sqrt((double)n);
        fftw_p = fftw_plan_dft_1d(n, in, out,
                                  forward ? FFTW_FORWARD : FFTW_BACKWARD,
                                  FFTW_ESTIMATE);
      }

    // copy data

    for (int i = 0; i < n; i++)
      {
        in[i][0] = srcre[i];
        in[i][1] = srcim[i];
      }

    fftw_execute(fftw_p);

    // renorm and copy data
    for (int i = 0; i < n; i++)
      {
        re[i] = out[i][0] * fftnorm;
        im[i] = out[i][1] * fftnorm;
      }

    return OK;
  }

  int Fourier(const vector<double>& srcre, const vector<double>& srcim,
              vector<double>& re, vector<double>& im,
              int option)
  {
    int dim = srcre.size();

    if (dim < 1 || dim != (int)srcim.size())
      throw IceException(FNAME, M_WRONG_PARAM);

    if (option != NORMAL && option != INVERS)
      throw IceException(FNAME, M_WRONG_PARAM);

    bool forward = option == NORMAL;

    if ((dim != fftw_size) || (forward != isforward))
      {
        // altes array freigeben
        if (in != NULL)
          {
            fftw_free(in);
          }
        if (out != NULL)
          {
            fftw_free(out);
          }

        // plan freigeben
        fftw_destroy_plan(fftw_p);
        // neues array anlegen
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * dim);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * dim);
        isforward = forward;
        fftw_size = dim;
        fftnorm = 1.0 / sqrt((double)dim);
        fftw_p = fftw_plan_dft_1d(dim, in, out,
                                  forward ? FFTW_FORWARD : FFTW_BACKWARD,
                                  FFTW_ESTIMATE);
      }

    // copy data

    for (int i = 0; i < dim; i++)
      {
        in[i][0] = srcre[i];
        in[i][1] = srcim[i];
      }

    fftw_execute(fftw_p);

    // renorm and copy data
    for (int i = 0; i < dim; i++)
      {
        re[i] = out[i][0] * fftnorm;
        im[i] = out[i][1] * fftnorm;
      }

    return OK;
  }

  int Fourier(vector<double>& re, vector<double>& im,
              int option)
  {
    int dim = re.size();

    if (dim < 1 || dim != (int)im.size())
      throw IceException(FNAME, M_WRONG_PARAM);

    if (option != NORMAL && option != INVERS)
      throw IceException(FNAME, M_WRONG_PARAM);

    bool forward = option == NORMAL;

    if ((dim != fftw_size) || (forward != isforward))
      {
        // altes array freigeben
        if (in != NULL)
          {
            fftw_free(in);
          }
        if (out != NULL)
          {
            fftw_free(out);
          }

        // plan freigeben
        fftw_destroy_plan(fftw_p);
        // neues array anlegen
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * dim);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * dim);
        isforward = forward;
        fftw_size = dim;
        fftnorm = 1.0 / sqrt((double)dim);
        fftw_p = fftw_plan_dft_1d(dim, in, out,
                                  forward ? FFTW_FORWARD : FFTW_BACKWARD,
                                  FFTW_ESTIMATE);
      }

    // copy data

    for (int i = 0; i < dim; i++)
      {
        in[i][0] = re[i];
        in[i][1] = im[i];
      }

    fftw_execute(fftw_p);

    // renorm and copy data
    for (int i = 0; i < dim; i++)
      {
        re[i] = out[i][0] * fftnorm;
        im[i] = out[i][1] * fftnorm;
      }

    return OK;
  }
#undef FNAME
}
#endif // if FFTW3

