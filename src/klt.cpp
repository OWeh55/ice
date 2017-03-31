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
#include "message.h"
#include "macro.h"
#include "statistics.h"
#include "mateigen.h"
#include "klt.h"

namespace ice
{
  // Calculation of KLT matrix from feature list
#define FNAME "KLT"
  Matrix KLT(const Matrix& m, Vector& percent)
  {
    Statistics st(m.cols());
    Put(st, m);
    return KLT(st, percent);
  }

  Matrix KLT(const Matrix& m)
  {
    Vector percent;
    return KLT(m, percent);
  }

// Calculation of KLT from statistics
  Matrix KLT(const Statistics& st)
  {
    Vector percent;
    return KLT(st, percent);
  }

  Matrix KLT(const Statistics& st, Vector& percent)
  {
    int nFeatures = st.Dim();

    Matrix klt(nFeatures, nFeatures);

    Matrix sigma1 = Correlation(st);
    Matrix eigen1;
    Vector eigenvalue1;
    Eigenvalue(sigma1, eigenvalue1, eigen1);

    Matrix sigma2 = Covariance(st);
    Matrix eigen2;
    Vector eigenvalue2;
    Eigenvalue(sigma2, eigenvalue2, eigen2);

    double sum = 0;
    for (int i = 0; i < nFeatures; ++i)
      {
        sum += eigenvalue1[i];
      };

    percent.Resize(nFeatures);

    double sum1 = 0;
    for (int i = 0; i < nFeatures; ++i)
      {
        sum1 += eigenvalue1[i];
        percent[i] = sum1 / sum * 100;
      }

    for (int i = 0; i < nFeatures; ++i)
      for (int j = 0; j < nFeatures; ++j)
        {
          klt[j][i] = eigen2[i][j];
        }

    return klt;
  }

#undef FNAME
}
