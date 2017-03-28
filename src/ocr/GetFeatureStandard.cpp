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

#include "arith.h"
#include "lintrans.h"
#include "ocr/GetFeatureStandard.h"

#include "ocr/OcrTools.h"

using namespace std;
using namespace ice;

int GetFeatureStandard::get_feature(const Image& pic,
                                    const Contur& c, Vector& feature,
                                    Window& win) const
{
  int threshold = 0;
  int x_position = 0;
  int anz_gesamt, i, ii, j, jj, g;
  double summe;

  c.getRect(win); //Kontur umschreibendes Rechteck ermitteln

  threshold = CalcThreshold(pic(win));

  if (quantil_border)   // umgebendes Rechteck mittels Quantilen bestimmen, bei Training abschalten
    {
      if (get_quantil_border(pic, c, threshold, win, 0, 0,
                             x_position) != 0)
        return 0;
    }

  Image temp = NewImg(win.Width(), win.Height(), 255);

  Image normalized = NewImg(gitter1 * 5, gitter2 * 5, 255);
  ClearImg(normalized);
  //FIXME gibt es hier einen besseren Weg das Fenster in einem Bild zu speichern?
  int top = win.YI();
  int left = win.XI();
  for (int y = 0; y < win.Height(); y++)
    {
      for (int x = 0; x < win.Width(); x++)
        {
          int xn = x + left;
          int yn = y + top;
          int foo = GetVal(pic, xn, yn);
          PutVal(temp, x, y, foo);
        }
    }

  if (temp->xsize > 1 && temp->ysize > 5)
    normalizeSeg(temp, normalized, gitter1, gitter2);
  double breite = (double) normalized->xsize;
  double hoehe = (double) normalized->ysize;
  int k = 0;
  for (i = 0; i < gitter1; i++)
    {
      for (j = 0; j < gitter2; j++, k++)
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
                  g = GetValUnchecked(normalized, ii, jj);
                  anz_gesamt++;
                  summe += (double) g / 255.0;
                }
            }
          if (anz_gesamt == 0)
            {
              feature[k] = 0;
            }
          else
            {
              // Durchschnittlichen Pixelwert speichern
              feature[k] = summe / double(anz_gesamt);
            }
        }
    }

  // aufräumen
  return 0;
}

int
GetFeatureStandard::get_feature(const Image& pic, Vector& feature) const
{
  Window win;

  /* Attention: threshold may be above ANY value in pic */
  int threshold = CalcThreshold(pic);

  /* cut off extra dirt at the edges, remember that rec_4 holds the clean
   * image coords
   * remember to *disable* this in the training phase!
   * */
  if (quantil_border)
    {
      get_quantil_border(pic, threshold, win);
    }

  /* make a copy of the Window described by win */
  //Image temp = NewImg(pic, Window(rec_4[0],rec_4[1], rec_4[2], rec_4[3]));
  Image temp = NewImg(win.Width(), win.Height(), 255);

  //FIXME gibt es hier einen besseren Weg das Fenster in einem Bild zu speichern?
  int left = win.XI();
  int top  = win.YI();
  for (int y = 0; y <= win.Height(); y++)
    {
      for (int x = 0; x <= win.Width(); x++)
        {
          PutVal(temp, x, y, GetVal(pic, x + left, y + top));
        }
    }

  Image normalized = NewImg(gitter1 * 5, gitter2 * 5, 255);
  ClearImg(normalized);
  if (temp->xsize > 1 && temp->ysize > 5)
    {
      normalizeSeg(temp, normalized, gitter1, gitter2);
    }

  /* see below why this mess is even necessary */
  double normalized_width = (double) normalized->xsize;
  double normalized_height = (double) normalized->ysize;
  double dgitter1 = (double) gitter1;
  double dgitter2 = (double) gitter2;

  /* actually compute the features, as a normalized median over a window
   * (dgitter1, dgitter2)
   * */
  int k = 0;
  for (int i = 0; i < gitter1; ++i)
    {
      for (int j = 0; j < gitter2; ++j, ++k)
        {
          double summe = 0.0;
          int anz_gesamt = 0;
          /* this is FUBAR */
          for (int ii =
                 int (i * normalized_width / dgitter1);
               ii < int ((i + 1) * normalized_width / dgitter1); ++ii)
            {
              for (int jj =
                     int (j * normalized_height /
                          dgitter2);
                   jj <
                   int ((j + 1) * normalized_height / dgitter2); ++jj)
                {
                  summe += (double)
                           GetValUnchecked(normalized, ii, jj) / 255.0;
                  anz_gesamt++;
                }
            }
          feature[k] =
            (anz_gesamt == 0) ? 0 : summe / double(anz_gesamt);
        }
    }

  FreeImg(temp);
  FreeImg(normalized);

  return 0;
}
