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
#include "lsifilter.h"

using namespace std;

namespace ice
{
  // -- Methoden der Klasse LsiRepresentationI für ganzzahlige Repraesentation von LSI-Filtern

  LsiRepresentationI::LsiRepresentationI(const IMatrix& m, int normp):
    LsiRepresentation(m.cols(), m.rows()), norm(normp)
  {
    mask = new int[dimx * dimy];
    int teiler = norm;
    int i = 0;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int v = m[y][x];
          mask[i] = v;
          teiler = ggt(teiler, v);
          i++;
        }

    if (teiler > 1)
      {
        norm /= teiler;
        i = 0;

        for (int y = 0; y < dimy; ++y)
          for (int x = 0; x < dimx; ++x)
            {
              mask[i] /= teiler;
              i++;
            }
      }
  }

  LsiRepresentationI::LsiRepresentationI(const matrix<int>& m, int normp):
    LsiRepresentation(m.cols(), m.rows()), norm(normp)
  {
    mask = new int[dimx * dimy];
    int teiler = norm;
    int i = 0;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int v = m[y][x];
          mask[i] = v;
          teiler = ggt(teiler, v);
          i++;
        }

    if (teiler > 1)
      {
        norm = norm / teiler;
        i = 0;

        for (int y = 0; y < dimy; ++y)
          for (int x = 0; x < dimx; ++x)
            {
              mask[i] /= teiler;
              i++;
            }
      }
  }

  void LsiRepresentationI::getMask(matrix<double>& m) const
  {
    m.resize(dimy, dimx);
    int i = 0;
    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          m[y][x] = static_cast<double>(mask[i]) / norm;
          ++i;
        }
  }

  void LsiRepresentationI::sumPlusSumMinus(double& sump, double& summ) const
  {
    sump = 0;
    summ = 0;

    for (int i = 0; i < dimx * dimy; i++)
      if (mask[i] > 0)
        {
          sump += mask[i];
        }
      else
        {
          summ += mask[i];
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

  LsiRepresentationI::LsiRepresentationI(int* m, int xsize, int ysize,
                                         int normp):
    LsiRepresentation(xsize, ysize), norm(normp)
  {
    mask = new int [dimx * dimy];
    int teiler = norm;
    for (int i = 0; i < dimx * dimy; i++)
      {
        int v = m[i];
        teiler = ggt(teiler, v);
        mask[i] = v;
      }

    if (teiler > 1)
      {
        norm = norm / teiler;
        int i = 0;

        for (int y = 0; y < dimy; ++y)
          for (int x = 0; x < dimx; ++x)
            {
              mask[i] /= teiler;
              i++;
            }
      }
  }


  LsiRepresentationI::LsiRepresentationI(const LsiRepresentationI& r):
    LsiRepresentation(r.dimx, r.dimy), norm(r.norm)
  {
    mask = new int[dimx * dimy];

    for (int i = 0; i < dimx * dimy; i++)
      {
        mask[i] = r.mask[i];
      }
  }

  void LsiRepresentationI::reflectMask()
  {
    int* mask2 = new int[dimx * dimy];
    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          int idx1 = idx(x, y);
          int idx2 = idx(dimx - 1 - x, dimy - 1 - y);
          mask2[idx2] = mask[idx1];
        }
    delete [] mask;
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

    for (int i = 0; i < dimx * dimy; i++)
      {
        int v = mask[i];

        if (v > 0)
          {
            sump += v;
          }
        else
          {
            summ -= v;
          }
      }

    double sumc = Max(sump, summ);
    double mv = sumc * max;

    if (mv < INT_MAX)
      {
        return res;
      }

    int nfac = int((mv + INT_MAX - 1) / INT_MAX);

    // cout << "normalisiere mit " << nfac << endl;

    for (int i = 0; i < dimy * dimy; i++)
      {
        res.mask[i] = mask[i] / nfac;
      }

    res.norm = norm / nfac;
    return res;
  }

  void LsiRepresentationI::negateMask()
  {
    for (int i = 0; i < dimy * dimy; i++)
      {
        mask[i] = -mask[i];
      }
  }

  void LsiRepresentationI::filter(const Image& src,
                                  const Image& dest, int offset) const
  {
    LsiRepresentationI n = normalized(src.maxval);

    LSIImg(src, dest,
           n.dimx, n.dimy, n.mask,
           n.norm,
           offset);
  }

  void LsiRepresentationI::filter(const Image& src, ImageD dest) const
  {
    LSIImg(src, dest,
           dimx, dimy, mask,
           norm);
  }

  void LsiRepresentationI::filter(ImageD src, ImageD dest) const
  {
    LSIImg(src, dest,
           dimx, dimy, mask,
           norm);
  }

}
