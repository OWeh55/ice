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
  // -- Methoden der Klasse LsiRepresentationI für ganzzahlige Repraesentation von LSI-Filtern

  LsiRepresentationI::LsiRepresentationI(const IMatrix& m, int normp):
    LsiRepresentation(m.cols(), m.rows()), mask(m.rows(), m.cols()), norm(normp)
  {
    int teiler = norm;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int v = m[y][x];
          mask[y][x] = v;
          teiler = ggt(teiler, v);
        }

    if (teiler > 1)
      {
        norm /= teiler;

        for (int y = 0; y < dimy; ++y)
          for (int x = 0; x < dimx; ++x)
            {
              mask[y][x] /= teiler;
            }
      }
  }

  LsiRepresentationI::LsiRepresentationI(const matrix<int>& m, int normp):
    LsiRepresentation(m.cols(), m.rows()), mask(m), norm(normp)
  {
    int teiler = norm;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int v = mask[y][x];
          teiler = ggt(teiler, v);
        }

    if (teiler > 1)
      {
        norm /= teiler;

        for (int y = 0; y < dimy; ++y)
          for (int x = 0; x < dimx; ++x)
            {
              mask[y][x] /= teiler;
            }
      }
  }

  void LsiRepresentationI::getMask(matrix<double>& m) const
  {
    m.resize(dimy, dimx);
    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          m[y][x] = static_cast<double>(mask[y][x]) / norm;
        }
  }

  void LsiRepresentationI::sumPlusSumMinus(double& sump, double& summ) const
  {
    sump = 0;
    summ = 0;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int val = mask[y][x];
          if (val > 0)
            sump += val;
          else
            summ += val;
        }
    sump /= norm;
    summ /= norm;
  }

  void LsiRepresentationI::normalize()
  {
    double p, m;
    sumPlusSumMinus(p, m);
    double range = p - m;
    norm = RoundInt(norm / range);
  }

  LsiRepresentationI::LsiRepresentationI(const int* m, int xsize, int ysize,
                                         int normp):
    LsiRepresentation(xsize, ysize), mask(ysize, xsize), norm(normp)
  {
    int teiler = norm;
    int i = 0;
    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int v = m[i];
          mask[y][x] = v;
          teiler = ggt(teiler, v);
          i++;
        }

    if (teiler > 1)
      {
        norm /= teiler;

        for (int y = 0; y < dimy; ++y)
          for (int x = 0; x < dimx; ++x)
            {
              mask[y][x] /= teiler;
            }
      }
  }


  LsiRepresentationI::LsiRepresentationI(const LsiRepresentationI& r):
    LsiRepresentation(r.dimx, r.dimy), mask(r.mask), norm(r.norm)
  {
  }

  void LsiRepresentationI::reflectMask()
  {
    matrix<int> mask2(dimy, dimx);
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

  int LsiRepresentationI::ggt(int a, int b) const
  {
    a = abs(a);
    b = abs(b);

    while (a != 0)
      {
        int aa = b % a;
        b = a;
        a = aa;
      }

    return b;
  }

  LsiRepresentationI LsiRepresentationI::normalized(int max) const
  {
    LsiRepresentationI res(*this);

    double sump = 0.0;
    double summ = 0.0;
    sumPlusSumMinus(sump, summ);

    double sumc = Max(sump, summ);
    double mv = sumc * max;

    if (mv < INT_MAX)
      {
        return res;
      }

    int nfac = int((mv + INT_MAX - 1) / INT_MAX);

    // cout << "normalisiere mit " << nfac << endl;


    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          res.mask[y][x] = mask[y][x] / nfac;
        }

    res.norm = norm / nfac;
    return res;
  }

  void LsiRepresentationI::negateMask()
  {

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          mask[y][x] = -mask[y][x];
        }
  }

  void LsiRepresentationI::filter(const Image& src,
                                  const Image& dest, int offset) const
  {
    LsiRepresentationI n = normalized(src.maxval);

    LSIImg(src, dest,
           n.dimx, n.dimy, n.mask.getData(),
           n.norm,
           offset);
  }

  void LsiRepresentationI::filter(const Image& src, ImageD dest) const
  {
    LSIImg(src, dest,
           dimx, dimy, mask.getData(),
           norm);
  }

  void LsiRepresentationI::filter(ImageD src, ImageD dest) const
  {
    LSIImg(src, dest,
           dimx, dimy, mask.getData(),
           norm);
  }

}
