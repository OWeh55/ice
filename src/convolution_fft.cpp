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

// convolution and deconvolution using ft

#include <math.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "base.h"
#include "based.h"
#include "fouriertrafo.h"
#include "fourier.h"
#include "convolution_fft.h"

namespace ice
{
#define FNAME "Convolution"
  void Convolution(const Vector& s1, const Vector& s2, Vector& d)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);
        std::vector<double> h1(size);
        std::vector<double> h2(size);
        for (int i = 0; i < size; i++)
          h1[i] = s1[i];
        for (int i = 0; i < size; i++)
          h2[i] = s2[i];
        std::vector<double> dh;
        Convolution(s1, s2, dh);
        d.Resize(size);
        for (int i = 0; i < size; i++)
          d[i] = dh[i];
      }
    RETHROW;
  }

  void Convolution(const std::vector<double>& s1,
                   const std::vector<double>& s2, std::vector<double>& d)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);

        FourierTrafo ft(size, true, false);
        ft.setInput(s1, s2); // combined FT for two functions: fc = s1 + i * s2

        std::vector<double> r1(size), i1(size);
        std::vector<double> r2(size), i2(size);

        ft.getResultFromReal(r1, i1);
        ft.getResultFromImag(r2, i2);

        std::vector<double> rc(size), ic(size);
        double efac = sqrt((double)size);

        for (int i = 0; i < size; i++)
          {
            // zc = sqrt(n) * z1 * z2
            rc[i] = (r1[i] * r2[i] - i1[i] * i2[i]) * efac;
            ic[i] = (r1[i] * i2[i] + r2[i] * i1[i]) * efac;
          }

        FourierTrafo fti(size, false, false);
        fti.setInput(rc, ic);
        fti.getResult(d);
      }
    RETHROW;
  }

#undef FNAME
#define FNAME "InvConvolution"
  void InvConvolution(const Vector& s1, const Vector& s2,
                      Vector& d,
                      double noise)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);
        std::vector<double> h1(size);
        std::vector<double> h2(size);
        for (int i = 0; i < size; i++)
          h1[i] = s1[i];
        for (int i = 0; i < size; i++)
          h2[i] = s2[i];
        std::vector<double> dh;
        InvConvolution(s1, s2, dh, noise);
        d.Resize(size);
        for (int i = 0; i < size; i++)
          d[i] = dh[i];
      }
    RETHROW;
  }

  void InvConvolution(const std::vector<double>& s1,
                      const std::vector<double>& s2,
                      std::vector<double>& d, double noise)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);

        FourierTrafo ft(size, true, false);
        ft.setInput(s1, s2); // combined FT for two functions: fc = s1 + i * s2

        std::vector<double> r1(size), i1(size);
        std::vector<double> r2(size), i2(size);

        ft.getResultFromReal(r1, i1);
        ft.getResultFromImag(r2, i2);

        std::vector<double> rc(size), ic(size);

        double efac = 1.0 / sqrt((double)size);
        double noise2 = noise * noise;

        for (int i = 0; i < size; i++)
          {
            double r3 = 0;
            double i3 = 0;
            double b1 = r1[i] * r1[i] + i1[i] * i1[i];
            if (noise == 0)
              {
                if (b1 != 0)
                  {
                    // z3 = (1/sqrt(N)) * z1 / z2
                    r3 = (r2[i] * r1[i] + i2[i] * i1[i]) / b1 * efac;
                    i3 = (r1[i] * i2[i] - r2[i] * i1[i]) / b1 * efac;
                  }
              }
            else
              {
                // z3 = (1/sqrt(N) * (z1*~z2)/(z2 * ~z2 + noise^2)
                b1 += noise2;
                r3 = (r2[i] * r1[i] + i2[i] * i1[i]) / b1 * efac;
                i3 = (r1[i] * i2[i] - r2[i] * i1[i]) / b1 * efac;
              }

            rc[i] = r3;
            ic[i] = i3;
          }

        FourierTrafo fti(size, false, false);
        fti.setInput(rc, ic);
        fti.getResult(d);
      }
    RETHROW;
  }
#undef FNAME
}
