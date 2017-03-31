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
 * Wolfgang Ortmann, 1997, 1998, 2013
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "darith.h"

#include "fourier.h"

using namespace std;

namespace ice
{
  //---------------------------
  //       Hartley transform
  //---------------------------

#define FNAME "HartleyD"
  int HartleyD(const double* src, int n, double* res)
  {
    double* im;
    double* re;
    int i;

    if (n < 2)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (src == NULL || res == NULL)
      {
        Message(FNAME, M_WRONG_PTR, WRONG_POINTER);
        return WRONG_POINTER;
      }

    if ((n & 1) == 0)
      {
        // n is even
        // effective algorithm
        // Split vector into to parts (odd and even)

        int n2 = n / 2;
        im = new double[n2];
        re = new double[n2];

        int hn = 0;

        for (i = 0; i < n; i += 2)
          {
            re[hn] = src[i];
            im[hn] = src[i + 1];
            hn++;
          }

        // combined FFT even=real odd=imaginary
        FourierD(re, im, n2, NORMAL, re, im);

        const double dfi = 2 * M_PI / n;
        const double w22 = 1 / (2 * sqrt(2.0));
        double dcos = cos(dfi);
        double dsin = sin(-dfi);
        double sinus = 0;
        double cosinus = 1;
        double c1;
        double rv, iv, rn, in, v1, v2;

        for (i = 0; i < n2; i++)
          {
            if (i == 0)
              {
                hn = 0;
              }
            else
              {
                hn = n2 - i;
              }

            // reconstruction of FFT of even and odd part
            // from combined fft
            // get complex values of frequency i and negated frequency

            rv = re[i];
            iv = im[i];
            rn = re[hn];
            in = im[hn];

            // combination of odd and even parts to complex
            // coefficients
            v1 = (iv + in) * (cosinus - sinus) + (rv - rn) * (sinus + cosinus);
            v2 = rv + rn - iv + in;

            // hartley coefficients
            res[i] = (v1 + v2) * w22;
            res[i + n2] = (v2 - v1) * w22;

            // iterative calculation of sin/cos
            c1 = cosinus * dcos - sinus * dsin;
            sinus = sinus * dcos + cosinus * dsin;
            cosinus = c1;
          }

        delete [] im;
        delete [] re;
      }
    else
      {
        // odd number of values

        im = new double[n];
        re = new double[n];

        for (i = 0; i < n; i++)
          {
            re[i] = src[i];
            im[i] = 0;
          }

        FourierD(re, im, n, NORMAL, re, im);

        for (i = 0; i < n; i++)
          {
            res[i] = (re[i] - im[i]);
          }

        delete [] im;
        delete [] re;
      }

    return OK;
  }
#undef FNAME

#define FNAME "Hartley"
  int Hartley(const Vector& src, Vector& dst)
  {
    dst.Resize(src.Size());
    const double* s = src.getDataPointer();
    double* d = dst.getDataPointer();

    int n = src.Size();
    IF_FAILED(HartleyD(s, n, d))
    {
      Message(FNAME, M_0, ERROR);
      return ERROR;
    }
    return OK;
  }

  int Hartley(Vector& vec)
  {
    RETURN_ERROR_IF_FAILED(Hartley(vec, vec));
    return OK;
  }
#undef FNAME
}
