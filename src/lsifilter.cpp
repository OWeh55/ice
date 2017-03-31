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
#include "MatrixAlgebra.h"
#include "matrixtools.h"
#include "lsifilter.h"

using namespace std;

namespace ice
{
// -- Methoden der Klasse LSIFilter

  LSIFilter& LSIFilter::operator=(const LSIFilter& f)
  {
    if (&f == this)
      {
        return *this;
      }

    delete rep;
    typ = f.typ;

    if (f.rep != NULL)
      {
        switch (typ)
          {
          case it_int:
            rep = new LsiRepresentationI(*(LsiRepresentationI*)f.rep);
            break;
          case it_double:
            rep = new LsiRepresentationD(*(LsiRepresentationD*)f.rep);
            break;
          }
      }
    else
      {
        rep = nullptr;
      }

    return *this;
  }

  int mInvert(const matrix<double>& m1, matrix<double>& m2)
  {
    int n1c = m1.cols();
    int n1r = m1.rows();
    if ((n1c % 2 == 0) || (n1r % 2 == 0))
      {
        return 1;
      }
    int s1c = n1c / 2;
    int s1r = n1r / 2;

    int n2c = m2.cols();
    int n2r = m2.rows();
    if ((n2c % 2 == 0) || (n2r % 2 == 0))
      {
        return 1;
      }
    int s2c = n2c / 2;
    int s2r = n2r / 2;

    int n3c = n1c + n2c - 1;
    int n3r = n1r + n2r - 1;
    int s3c = n3c / 2;
    int s3r = n3r / 2;

    int nEquations = n3c * n3r;
    int nVariables = n2c * n2r;

    matrix<double> A(nEquations, nVariables); // Koeffizienten
    vector<double> i(nEquations, 0.0); // Inhomogenität
    i[s3c + s3r * n3c] = 1.0; // Zentrum <-> (0,0)

    for (int r1 = -s1r; r1 <= s1r; ++r1)
      for (int c1 = -s1c; c1 <= s1c; ++c1)
        {
          for (int r2 = -s2r; r2 <= s2r; ++r2)
            for (int c2 = -s2c; c2 <= s2c; ++c2)
              {
                int r3 = r1 + r2;
                int c3 = c1 + c2;
                A[c3 + s3c + n3c * (r3 + s3r)][c2 + s2c + n2c * (r2 + s2r)] = m1[r1 + s1r][c1 + s1c];
              }
        }

    vector<double> res = SolveLinearEquation(A, i);
    for (int r2 = -s2r; r2 <= s2r; ++r2)
      for (int c2 = -s2c; c2 <= s2c; ++c2)
        {
          m2[r2 + s2r][c2 + s2c] = res[c2 + s2c + n2c * (r2 + s2r)];
        }
    return 0;
  }

#define FNAME "LSIFilter::getInverse"
  LSIFilter LSIFilter::getInverse(int sizex, int sizey) const
  {
    matrix<double> m;
    getMask(m);

    matrix<double> m1(sizex, sizey);
    if (mInvert(m, m1) != 0)
      {
        throw IceException(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return LSIFilter();
      }
    return LSIFilter(m1);
  }
#undef FNAME

  std::ostream& operator<<(std::ostream& os, const ice::LSIFilter& f)
  {
    matrix<double> mat;
    f.getMask(mat);
    os << mat;
    return os;
  }

  /*
   * Bildpunktzugriff mit Filterung
   */

  int GetVal(const Image& img, int x, int y, const LSIFilter& f)
  {
    int dx = f.rep->getdx();
    int dy = f.rep->getdy();

    if (!img.inside(x, y))
      {
        return 0;
      }

    if (!img.inside(x - dx, y - dy) || !img.inside(x + dx, y + dy))
      {
        return 0;
      }

    int g = 0;

    switch (f.typ)
      {
      case LSIFilter::it_int:
      {
        int mi = 0;
        double sum = 0; // größerer Wertebereich fuer Summe !!
        LsiRepresentationI* r = (LsiRepresentationI*)f.rep;
        const int* maskp = r->getMask();
        const int norm = r->getNorm();

        for (int yi = y - dy; yi <= y + dy; yi++)
          for (int xi = x - dx; xi <= x + dx; xi++)
            {
              sum += img.getPixelUnchecked(xi, yi) * maskp[mi];
              ++mi;
            }

        g = (int)(sum / norm);
        break;
      }
      case LSIFilter::it_double:
      {
        LsiRepresentationD* r = (LsiRepresentationD*)f.rep;
        int mi = 0;
        double sum = 0;
        const double* maskp = r->getMask();

        for (int yi = y - dy; yi <= y + dy; yi++)
          for (int xi = x - dx; xi <= x + dx; xi++)
            {
              sum += img.getPixelUnchecked(xi, yi) * maskp[mi];
              ++mi;
            }

        g = (int)sum;
        break;
      }
      }

    return g;
  }

  int GetVal(const Image& img, IPoint p, const LSIFilter& f)
  {
    return GetVal(img, p.x, p.y, f);
  }
}
