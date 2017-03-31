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
// -- Methoden der Klasse LsiRepresentationD für Gleitkomma-Repraesentation von LSI-Filtern

  LsiRepresentationD::LsiRepresentationD(const LsiRepresentationD& r):
    LsiRepresentation(r.dimx, r.dimy)
  {
    mask = new double[dimx * dimy];

    for (int i = 0; i < dimx * dimy; i++)
      {
        mask[i] = r.mask[i];
      }
  }

  LsiRepresentationD::LsiRepresentationD(const Matrix& m):
    LsiRepresentation(m.cols(), m.rows())
  {
    //    cout << dimx << "x" << dimy << endl;
    mask = new double[dimx * dimy];
    int i = 0;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          mask[i] = m[y][x];
          i++;
        }
  }

  LsiRepresentationD::LsiRepresentationD(const matrix<double>& m):
    LsiRepresentation(m.cols(), m.rows())
  {
    //    cout << dimx << "x" << dimy << endl;
    mask = new double[dimx * dimy];
    int i = 0;

    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          mask[i] = m[y][x];
          i++;
        }
  }

  void LsiRepresentationD::getMask(matrix<double>& m) const
  {
    m.resize(dimy, dimx);
    int i = 0;
    for (int y = 0; y < dimy; ++y)
      for (int x = 0; x < dimx; ++x)
        {
          m[y][x] = static_cast<double>(mask[i]);
          ++i;
        }
  }

  void LsiRepresentationD::sumPlusSumMinus(double& sump, double& summ) const
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
  }

  void LsiRepresentationD::normalize()
  {
    double p, m;
    sumPlusSumMinus(p, m);
    double range = p - m;

    for (int i = 0; i < dimx * dimy; i++)
      {
        mask[i] /= range;
      }
  }

  void LsiRepresentationD::negateMask()
  {
    for (int i = 0; i < dimy * dimy; i++)
      {
        mask[i] = -mask[i];
      }
  }

  void LsiRepresentationD::reflectMask()
  {
    double* mask2 = new double[dimx * dimy];
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

  int LsiRepresentationD::Filter(const Image& src, const Image& dest, int offset) const
  {
    return LSIImg(src, dest,
                  dimx, dimy, mask,
                  offset);
  }

  int LsiRepresentationD::Filter(const Image& src, ImageD dest) const
  {
    return LSIImg(src, dest,
                  dimx, dimy, mask);
  }

  int LsiRepresentationD::Filter(ImageD src, ImageD dest) const
  {
    return LSIImg(src, dest,
                  dimx, dimy, mask);
  }
}
