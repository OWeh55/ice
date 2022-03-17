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
 * Implementation of methods for class LsiFilter and helpers
 */

#include <utility>
#include <limits.h>
#include "MatrixAlgebra.h"
#include "matrixtools.h"
#include "LsiRepresentation.h"
#include "LsiRepresentationD.h"
//#include "LsiRepresentationI.h"
#include "LsiFilter.h"

using namespace std;

namespace ice
{
// -- Methoden der Klasse LsiFilter

  LsiFilter& LsiFilter::operator=(LsiFilter f)
  {
    std::swap(type, f.type);
    std::swap(rep, f.rep);
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

    vector<double> res = solveLinearEquation(A, i);
    for (int r2 = -s2r; r2 <= s2r; ++r2)
      for (int c2 = -s2c; c2 <= s2c; ++c2)
        {
          m2[r2 + s2r][c2 + s2c] = res[c2 + s2c + n2c * (r2 + s2r)];
        }
    return 0;
  }

#define FNAME "LsiFilter::getInverse"
  LsiFilter LsiFilter::getInverse(int sizex, int sizey) const
  {
    matrix<double> m;
    getMask(m);

    matrix<double> m1(sizex, sizey);
    if (mInvert(m, m1) != 0)
      throw IceException(FNAME, M_MATRIXFORMAT);
    return LsiFilter(m1);
  }
#undef FNAME

  std::ostream& operator<<(std::ostream& os, const ice::LsiFilter& f)
  {
    matrix<double> mat;
    f.getMask(mat);
    os << mat;
    return os;
  }

  /*
   * Bildpunktzugriff mit Filterung
   */

  int getValueFiltered(const Image& img, int x, int y, const LsiFilter& f)
  {
    const LsiRepresentation* rep = f.getRepresentation();
    int dx = rep->getDX();
    int dy = rep->getDY();

    if (!img.inside(x, y))
      {
        return 0;
      }

    if (!img.inside(x - dx, y - dy) || !img.inside(x + dx, y + dy))
      {
        return 0;
      }

    matrix<double> mask;
    f.getRepresentation()->getMask(mask);
    int mi = 0;
    double sum = 0;
    const double* maskp = mask.getData();

    for (int yi = y - dy; yi <= y + dy; yi++)
      for (int xi = x - dx; xi <= x + dx; xi++)
        {
          sum += img.getPixelUnchecked(xi, yi) * maskp[mi];
          ++mi;
        }

    return sum;
  }

  int getValueFiltered(const Image& img, IPoint p, const LsiFilter& f)
  {
    return getValueFiltered(img, p.x, p.y, f);
  }
}
