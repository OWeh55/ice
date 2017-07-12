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

#include <iostream>

#include "histogramequalization.h"
#include "pointlist.h"
#include "ocr/GetFeatureHist.h"
#include "ocr/OcrTools.h"

using namespace std;
using namespace ice;

int GetFeatureHist::get_feature(const Image& pic,
                                const Contur& c,
                                Vector& feature, Window& win) const
{
  int x_position = 0;
  int anz_rec = 0;
  int anz_gesamt, i, ii, j, jj, g;
  double summe;

  c.getRect(win); //Kontur umschreibendes Rechteck ermitteln

  int threshold = CalcThreshold(pic(win));

  if (quantil_border)   // umgebendes Rechteck mittels Quantilen bestimmen, bei Training abschalten
    {
      if (get_quantil_border(pic, c,
                             threshold, win, 0, 0, x_position) != 0)
        {
          return 0;
        }
    }


  Image temp = NewImg(win.Width(), win.Height(), 255);

  int top = win.YI();
  int left = win.XI();
  for (int y = 0; y < win.Height(); y++)
    {
      for (int x = 0; x <= win.Width(); x++)
        {
          PutVal(temp, x, y, GetVal(pic, x + left, y + top));
        }
    }

  // Histogrammausgleich
  HistogramEqualization(temp);

  // Fourierdeskriptoren
  PointList pl1, pl2;
  double* r_feature = new double[fd];
  double* i_feature = new double[fd];
  double phi_1 = 0;

  pl1 = ConturPointList(c, 1);
  pl2 = NewPointList(pl1->lng);

  FourierD(pl1->xptr, pl1->yptr, pl1->lng, NORMAL, pl2->xptr, pl2->yptr); // Fourierdeskriptoren berechnen

  if (fabs(pl2->xptr[1]) < 1.0e-12  && fabs(pl2->yptr[1]) < 1.0e-12)
    {
      return 3;
    }
  else
    {
      phi_1 = atan2(pl2->yptr[1], pl2->xptr[1]);  // Phase des 1. FK
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
      feature[anz_rec] =
        r_feature[i +
                  1] / sqrt(r_feature[0] * r_feature[0] +
                            i_feature[0] * i_feature[0]);
      anz_rec++;
    }
  for (i = fd - 1; i < 2 * fd - 2; ++i)
    {
      feature[anz_rec] =
        i_feature[i - fd +
                  2] / sqrt(r_feature[0] * r_feature[0] +
                            i_feature[0] * i_feature[0]);
      anz_rec++;
    }

  double breite = (double) temp->xsize;
  double hoehe = (double) temp->ysize;
  for (i = 0; i < gitter1; i++)
    {
      for (j = 0; j < gitter2; j++)
        {
          summe = 0.0;
          anz_gesamt = 0;
          for (ii =
                 int (double(i) * breite / (double)gitter1);
               ii <
               int (double(i + 1) * breite / (double)gitter1); ii++)
            {
              for (jj =
                     int (double(j) * hoehe /
                          (double)gitter2);
                   jj <
                   int (double(j + 1) * hoehe /
                        (double)gitter2); jj++)
                {
                  g = GetValUnchecked(temp, ii, jj);
                  anz_gesamt++;
                  summe += (double) g / 255.0;
                }
            }
          if (anz_gesamt == 0)
            {
              feature[anz_rec] = 0;
            }
          else
            {
              // Durchschnittlichen Pixelwert speichern
              feature[anz_rec] = summe / double(anz_gesamt);
            }
          anz_rec++;
        }
    }

  // aufräumen
  FreePointList(pl1);
  FreePointList(pl2);

  delete [] r_feature;
  delete [] i_feature;

  return 0;
}
