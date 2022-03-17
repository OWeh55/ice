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

#include "InvConvolution.h"
#include "FourierTrafo.h"

using std::vector;

namespace ice
{
#define CNAME "InvConvolution"

  void InvConvolution::transform() const
  {
    FourierTrafo ft1(size);
    ft1.setInput(input[0], input[1]);
    // spectrum first input
    vector<double> real1(size);
    vector<double> imag1(size);
    ft1.getResultFromReal(real1, imag1); // z1
    // spectrum first input
    vector<double> real2(size);
    vector<double> imag2(size);
    ft1.getResultFromImag(real2, imag2); // z2
    // spectrum result                   // z3 = z1 / z2  *  1 / sqrt(n)
    vector<double> real3(size);
    vector<double> imag3(size);

    double sqrtN = 1.0 / sqrt(size);

    if (beta == 0)
      {
        for (int i = 0; i < size; i++)
          {
            double denominator = real2[i] * real2[i] + imag2[i] * imag2[i];
            if (denominator != 0.0)
              {
                // z3 = (1/sqrt(N)) * z1 / z2
                real3[i] = (real1[i] * real2[i] + imag1[i] * imag2[i]) / denominator * sqrtN;
                imag3[i] = (-real1[i] * imag2[i] + imag1[i] * real2[i]) / denominator * sqrtN;
              }
            else
              {
                real3[i] = 0.0;
                imag3[i] = 0.0;
              }
          }
      }
    else
      {
        for (int i = 0; i < size; i++)
          {
            // z3 = (1/sqrt(N) * (z1*~z2)/(z2 * ~z2 + beta^2)
            double denominator = real2[i] * real2[i] + imag2[i] * imag2[i] + beta * beta;
            real3[i] = (real1[i] * real2[i] + imag1[i] * imag2[i]) / denominator * sqrtN;
            imag3[i] = (-real1[i] * imag2[i] + imag1[i] * real2[i]) / denominator * sqrtN;
          }
      }

    FourierTrafo ft2(size, false);
    ft2.setInput(real3, imag3);
    result[0].resize(size);
    ft2.getResult(result[0]);
  }

  void InvConvolution::deconvolute(const std::vector<double>& src1,
                                   const std::vector<double>& src2,
                                   std::vector<double>& dst,
                                   double beta)
  {
    InvConvolution convolution;
    convolution.setInput(src1, src2);
    convolution.getResult(dst);
  }

}
