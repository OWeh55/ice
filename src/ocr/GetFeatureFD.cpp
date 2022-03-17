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

#include <iostream>

#include "pointlist.h"
#include "fourier.h"
#include "ocr/GetFeatureFD.h"

using namespace std;
using namespace ice;

int GetFeatureFD::get_feature(const Image& pic,
                              const Contur& c, Vector& feature, Window& win) const
{
  // Ersten Merkmale mit Standardverfahren berechnen
  GetFeatureStandard::get_feature(pic, c, feature, win);

  // Fourierdeskriptoren
  PointList pl1, pl2;
  double* r_feature = new double[fd];
  double* i_feature = new double[fd];
  double phi_1 = 0;
  int i;
  //int k = GetFeatureStandard::getDimension();
  int k = gitter1 * gitter2;

  pl1 = ConturPointList(c, 1);
  pl2 = NewPointList(pl1->lng);

  FourierD(pl1->xptr, pl1->yptr, pl1->lng, NORMAL, pl2->xptr, pl2->yptr); // Fourierdeskriptoren berechnen

  if (fabs(pl2->xptr[1]) < 1.0e-12)
    {
      if (pl2->yptr[1] > 0)
        {
          phi_1 = M_PI / 2.0;
        }
      if (pl2->yptr[1] < 0)
        {
          phi_1 = -M_PI / 2.0;
        }
      if (fabs(pl2->yptr[1]) < 1.0e-12)
        {
          return 3;
        }
    }
  else
    {
      phi_1 = atan(pl2->yptr[1] / pl2->xptr[1]);  // Phase des 1. FK
    }

  if (pl2->xptr[1] < 0 && pl2->yptr[1] < 0)
    {
      phi_1 += M_PI;
    }
  if (pl2->xptr[1] < 0 && pl2->yptr[1] > 0)
    {
      phi_1 += M_PI;
    }

  // die ersten 2*fd-2 Merkmale berechnen
  for (i = 0; i < fd / 2; ++i)
    {
      double sinphi = sin(double(i + 1) * phi_1);
      double cosphi = cos(double(i + 1) * phi_1);
      r_feature[i] =
        pl2->xptr[i + 1] * cosphi + pl2->yptr[i + 1] * sinphi;
      i_feature[i] =
        pl2->yptr[i + 1] * cosphi - pl2->xptr[i + 1] * sinphi;
    }
  for (i = fd / 2; i < fd; ++i)
    {
      double cosphi = cos(double(-i - 1 + fd / 2) * phi_1);
      double sinphi = sin(double(-i - 1 + fd / 2) * phi_1);
      r_feature[i] =
        pl2->xptr[pl2->lng - i - 1 + fd / 2] * cosphi +
        pl2->yptr[pl2->lng - i - 1 + fd / 2] * sinphi;
      i_feature[i] =
        pl2->yptr[pl2->lng - i - 1 + fd / 2] * cosphi -
        pl2->xptr[pl2->lng - i - 1 + fd / 2] * sinphi;
    }
  for (i = 0; i < fd - 1; ++i)
    {
      feature[k] =
        r_feature[i +
                  1] / sqrt(r_feature[0] * r_feature[0] +
                            i_feature[0] * i_feature[0]);
      k++;
    }
  for (i = fd - 1; i < 2 * fd - 2; ++i)
    {
      feature[k] =
        i_feature[i - fd +
                  2] / sqrt(r_feature[0] * r_feature[0] +
                            i_feature[0] * i_feature[0]);
      k++;
    }

  // aufräumen
  FreePointList(pl1);
  FreePointList(pl2);

  delete [] r_feature;
  delete [] i_feature;

  return 0;
}
