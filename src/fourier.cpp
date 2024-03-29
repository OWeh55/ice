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
/*
 * Fourier transform for ice data types Vector and Matrix
 * basic fourier transform is in class fouriertrafo
 * Wolfgang Ortmann, 1997, 1998, 2013
 */

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "FourierTrafo.h"

#include "fourier.h"

using namespace std;

namespace ice
{
#define FNAME "Fourier"
  void Fourier(const Vector& re, const Vector& im,
               int option,
               Vector& rre, Vector& rim)
  {
    int dim = re.size();
    if (dim != (int)im.size())
      throw IceException(FNAME, M_WRONG_DIMENSION);
    try
      {
        vector<double> real(dim);
        vector<double> imag(dim);

        FourierTrafo ft(dim, option == NORMAL, false);
        for (int i = 0; i < dim; ++i)
          {
            real[i] = re[i];
            imag[i] = im[i];
          }

        ft.setInput(real, imag);
        ft.getResult(real, imag);

        rre.Resize(dim);
        rim.Resize(dim);
        for (int i = 0; i < dim; ++i)
          {
            rre[i] = real[i];
            rim[i] = imag[i];
          }
      }
    RETHROW;
  }

  void Fourier(Vector& re, Vector& im, int option)
  {
    Fourier(re, im, option, re, im);
  }

  void Fourier(const Matrix& ms, int option, Matrix& md)
  {
    if ((ms.rows() != 2) && (ms.cols() != 2))
      throw IceException(FNAME, M_MATRIXFORMAT);

    if (ms.rows() != 2)
      {
        md = !ms;
      }
    else
      {
        md = ms;
      }

    Fourier(md[0], md[1], option);

    if (ms.rows() != 2)
      {
        md = !md;
      }
  }

  void Fourier(Matrix& ms, int option)
  {
    Fourier(ms, option, ms);
  }
#undef FNAME
}
