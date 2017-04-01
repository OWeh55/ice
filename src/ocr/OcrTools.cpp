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

// #include <image_nonvis.h>

#include "trafo_img.h"
#include "lintrans.h"
#include "ocr/OcrTools.h"

using namespace ice;

namespace ice
{
  int get_quantil_border(const Image& pic, const Contur& c,
                         int threshold, Window& win, int seg_flag, int status,
                         int& x_position)
  {
    int i, j, g, k = 0;
    int i_lower, i_upper;
    double x_seg, x_hilf;
    int x_min = 0;
    int y_min = 0;
    int x_max = 0;
    int y_max = 0;
    double x_min_neu = 1.0e20;
    double y_min_neu = 1.0e20;
    double x_max_neu = -1.0e20;
    double y_max_neu = -1.0e20;

    c.getRect(x_min, y_min, x_max, y_max);

    Hist hist_x(int (x_max - x_min) + 1, 1.0, x_min); // Histogramme erzeugen
    Hist hist_y(int (y_max - y_min) + 1, 1.0, y_min);
    int count = 0;
    for (i = int (x_min); i <= int (x_max); ++i)
      for (j = int (y_min); j <= int (y_max); ++j)
        {
          g = GetValUnchecked(pic, i, j);
          if (g >= threshold)
            {
              count++;
              hist_x.Add(double(i));
              hist_y.Add(double(j));
            }
        }

    if ((count == 0) ||
        (hist_x.Limits(x_min_neu, x_max_neu, 0.01) == ERROR) ||
        (hist_y.Limits(y_min_neu, y_max_neu, 0.01) == ERROR))
      {
        return 2;
      }

    int left  = (int)x_min_neu;
    int right = (int)x_max_neu;
    int top   = (int)y_min_neu;
    int bottom = (int)y_max_neu;

    k = left;

    if (seg_flag > 0)
      {
        switch (status)
          {
          case 0:
            i_lower = left;
            i_upper = 2 * left / 3 + right / 3;
            break;
          case 1:
            i_lower = 2 * left / 3 + right / 3;
            i_upper = left / 3 + 2 * right / 3;
            break;
          case 2:
            i_lower = left / 3 + 2 * right / 3;
            i_upper = right;
            break;
          default:
            i_lower = 2 * left / 3 + right / 3;
            i_upper = left / 3 + 2 * right / 3;
            break;
          }
        x_seg = 1.0e20;
        for (i = i_lower; i <= i_upper; ++i)
          {
            x_hilf = hist_x.Rel(i - int (x_min));
            if (x_hilf == 0.0)
              {
                x_hilf += 0.5;
              }
            if (x_hilf < x_seg)
              {
                x_seg = x_hilf;
                k = i;
              }
          }
      }

    x_position = k;
    win = Window(left, top, right, bottom);

    return 0;
  }

  int get_quantil_border(const Image& pic, int threshold, Window& win)
  {
    win = Window(0, 0, pic->xsize - 1, pic->ysize - 1);

    bool test = false;

    /* hist_{x,y} hold the amount of values on a given row/column above a
     * threshold
     */
    Histogram hist_x(pic->xsize);
    Histogram hist_y(pic->ysize);

    for (int y = 0; y < pic->ysize; ++y)
      {
        for (int x = 0; x < pic->xsize; ++x)
          {
            if (GetVal(pic, x, y) >= threshold)
              {
                hist_x.addValue(x);
                hist_y.addValue(y);
                test = true;
              }
          }
      }

    /* slice off 1% on minimum *and* maximum
     */
    int left, top, right, bottom;
    left = top = right = bottom = 0;
    if (test)
      {
        hist_x.getLimits(left, right, 0.01);
        hist_y.getLimits(top, bottom, 0.01);
      }

    win = Window(left, top, right, bottom);

    return 0;
  }

  void binImgAdaptive(Image& img)
  {
    unsigned char** imgp = (unsigned char**) img->getDataPtr();
    bool allhom = true;

    int val = imgp[0][0];
    // prüfen ob Bild mehr als einen Grauwert enthält

    for (int y = 0; y < img.ysize && allhom; y++)
      for (int x = 0; x < img.xsize && allhom; x++)
        {
          if (imgp[y][x] != val)
            {
              allhom = false;
            }
        }
    // Binarisieren
    if (allhom)
      val = 128;
    else
      val = CalcThreshold(img);

    for (int y = 0; y < img.ysize && allhom; y++)
      for (int x = 0; x < img.xsize && allhom; x++)
        {
          if (imgp[y][x] <= val)
            imgp[y][x] = 0;
          else
            imgp[y][x] = 255;
        }
  }

  int normalizeSeg(const Image& segImgInput, Image normedSeg,
                   int gitter1, int gitter2)
  {
    Image segImg = NewImg(segImgInput, 1);
    unsigned char** segImgp = (unsigned char**) segImg->getDataPtr();

    //histogram adaption
    Image bin = NewImg(segImg, 1);

    unsigned char** binp = (unsigned char**) bin->getDataPtr();
    int min, max, countmax, countmin, summax, summin;
    binImgAdaptive(bin);

    //Durchschnittswert der Objektpixel -> new maximum
    //Durchschnittswert der Hintergrundpixel -> new minimum
    countmax = countmin = summax = summin = 0;
    min = 255;
    for (int y = 0; y < bin.ysize; y++)
      for (int x = 0; x < bin.xsize; x++)
        {
          if (binp[y][x] == 255)
            {
              countmax++;
              // alle Grauwerte summieren, die zum Objekt gehören
              summax += segImgp[y][x];
            }
          else
            {
              countmin++;
              // alle Grauwerte summieren, die zum Hintergrund gehören
              summin += segImgp[y][x];
            }
        }
    max = RoundInt(summax / double(countmax));  // Durchschnittlicher Grauwert für Hintergrund
    min = RoundInt(summin / double(countmin));  // Durchschnittlicher Grauwert für Objekt
    // Grenzen vielleicht noch leicht nach links oder rechts verschieben
    GrayTransformLimits(segImg, min, max);  // eigentliches Normalisieren
    //normalize size
    Trafo t = Trafo();
    t.Scale(0, 0, ((double) gitter1 * 5.0) / double(segImg->xsize - 1),
            ((double) gitter2 * 5.0) / double(segImg->ysize - 1));
    Transform(t, segImg, normedSeg, INTERPOL);

    return 0;
  }
}
