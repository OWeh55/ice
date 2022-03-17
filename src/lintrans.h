#ifndef LINTRANS_H
#define LINTRANS_H
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
 * Funktionen zur linearen Grauwerttransformation
 * * GetGrayLimits:
 *    Bestimmung der minimalen und maximalen Grauwerte für eine lineare
 *    Grauwerttransformation zur Kontarstanhebung
 *    Als Eingabe dient jeweils ein Bild oder ein Histogramm.
 * * GrayTransform:
 *    lineare Transformation bei Eingabe der Koeffizienten der
 *    Transformation  g' = a_1 * g + a_0
 * * GrayTransformLimits:
 *    lineare Transformation bei Eingabe der Werte, die im Zielbild
 *    auf maximalen bzw. minimalen Wert abgebildet werden
 *
 * * GrayNormalize:
 *    Zusammenfassung der Schritte Limit-Bestimmung und GrauwertTransformation
 */

#include "base.h"
#include "hist.h"

namespace ice
{
#define GV_QUANTILE 0      //Quantile
#define GV_MEAN 1       //Mittelwertmethode

  /*
   * get grayvalue limits suitable for linear transformation (contrast enhancement)
   */

  int GetGrayLimits(const Image& img,
                    int& l, int& r,
                    int mode = GV_QUANTILE,
                    double lquantile = 0.1);

  int GetGrayLimits(const Hist& h,
                    int& l, int& r,
                    int mode = GV_QUANTILE,
                    double lquantile = 0.1);

  /*
   * gray value transformation
   */

  int GrayTransform(const Image& sourceImage, const Image& targetImage,
                    double a1, double a0);
  int GrayTransform(const Image& image,
                    double a1, double a0);

  int GrayTransformLimits(const Image& sourceImage, const Image& targetImage,
                          int min, int max);
  int GrayTransformLimits(const Image& image,
                          int min, int max);

  /*
    Input:  Quellbild, Verweis auf Zielbild, Auswahl Methode, Randextrema entf. (j/n), ggf. Quantile
    Output: transformiertes Zielbild
  */

  int GrayNormalize(const Image& sourceImage, const Image& targetImage,
                    int mode = GV_QUANTILE,
                    double quantile = 0.01);

  int GrayNormalize(const Image& image,
                    int mode = GV_QUANTILE,
                    double quantile = 0.01);
}
#endif
