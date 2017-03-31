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
/*
 * Fourier transform for ice data types Vector and Matrix
 * basic fourier transform is in class fouriertrafo
 * Wolfgang Ortmann, 1997, 1998, 2013
 */

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "fouriertrafo.h"

#include "fourier.h"

using namespace std;

namespace ice
{
#define FNAME "Fourier"
  int Fourier(const Vector& re, const Vector& im,
              int option,
              Vector& rre, Vector& rim)
  {
    int dim = re.size();
    if (dim != (int)im.size())
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

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

    return OK;
  }

  int Fourier(Vector& re, Vector& im, int option)
  {
    RETURN_ERROR_IF_FAILED(Fourier(re, im, option, re, im));
    return OK;
  }

  int Fourier(const Matrix& ms, int option, Matrix& md)
  {
    if ((ms.rows() != 2) && (ms.cols() != 2))
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (ms.rows() != 2)
      {
        md = !ms;
      }
    else
      {
        md = ms;
      }

    RETURN_ERROR_IF_FAILED(Fourier(md[0], md[1], option));

    if (ms.rows() != 2)
      {
        md = !md;
      }

    return OK;
  }

  int Fourier(Matrix& ms, int option)
  {
    RETURN_ERROR_IF_FAILED(Fourier(ms, option, ms));
    return OK;
  }
#undef FNAME
}
