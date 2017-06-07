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
 * Implementation of methods for class LSIFilter and helpers
 */

#include <limits.h>
#include "LsiFilter.h"

using namespace std;

namespace ice
{
// -- Methoden der Klasse LsiRepresentationD für Gleitkomma-Repraesentation von LSI-Filtern

  LsiRepresentationD::LsiRepresentationD(const LsiRepresentationD& r):
    LsiRepresentation(r.dimx, r.dimy), mask(r.mask)
  {
  }

  LsiRepresentationD::LsiRepresentationD(const Matrix& m):
    LsiRepresentation(m.cols(), m.rows()), mask(m.rows(), m.cols())
  {
    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          mask[y][x] = m[y][x];
        }
  }

  LsiRepresentationD::LsiRepresentationD(const matrix<double>& m):
    LsiRepresentation(m.cols(), m.rows()), mask(m)
  {
  }

  void LsiRepresentationD::getMask(matrix<double>& m) const
  {
    m = mask;
  }

  void LsiRepresentationD::sumPlusSumMinus(double& sump, double& summ) const
  {
    sump = 0;
    summ = 0;

    for (int x = 0; x < dimx; x++)
      for (int y = 0; y < dimy; y++)
        {
          double val = mask[y][x];
          if (val > 0)
            sump += val;
          else
            summ += val;
        }
  }

  void LsiRepresentationD::normalize()
  {
    double p, m;
    sumPlusSumMinus(p, m);
    double range = p - m;

    for (int x = 0; x < dimx; x++)
      for (int y = 0; y < dimy; y++)
        {
          mask[y][x] /= range;
        }
  }

  void LsiRepresentationD::negateMask()
  {
    for (int x = 0; x < dimx; x++)
      for (int y = 0; y < dimy; y++)
        {
          mask[y][x] = -mask[y][x];
        }
  }

  void LsiRepresentationD::reflectMask()
  {
    matrix<double> mask2(dimy, dimx);
    for (int y = 0; y < dimy; ++y)
      {
        int y2 = dimy - 1 - y;
        for (int x = 0; x < dimx; ++x)
          {
            int x2 = dimx - 1 - x;
            mask2[y][x] = mask[y2][x2];
          }
      }
    mask = mask2;
  }

  void LsiRepresentationD::filter(const Image& src, const Image& dest,
                                  int offset) const
  {
    // we use int mask if possible
    double sump = 0.0;
    double summ = 0.0;
    sumPlusSumMinus(sump, summ);

    double sumc = Max(sump, summ);

    int norm = RoundInt(INT_MAX / 2.0 / sumc / src.maxval);

    if (norm > 0)
      {
        matrix<int> imask(dimy, dimx);
        for (int y = 0; y < dimy; y++)
          for (int x = 0; x < dimx; x++)
            imask[y][x] = RoundInt(mask[y][x] * norm);
        LSIImg(src, dest, dimx, dimy, imask.getData(), norm, offset);
      }
    else
      LSIImg(src, dest, dimx, dimy, mask.getData(), offset);
  }

  void LsiRepresentationD::filter(const Image& src, ImageD dest) const
  {
    LSIImg(src, dest,
           dimx, dimy, mask.getData());
  }

  void LsiRepresentationD::filter(ImageD src, ImageD dest) const
  {
    LSIImg(src, dest,
           dimx, dimy, mask.getData());
  }
}
