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
// Klasse für Histogramm
// Funktionen zur Nutzung von Histogrammen
// Ortmann 11/99, 04/13

#ifndef _HIST_H
#define _HIST_H

#include "base.h"

namespace ice
{
  /* Klasse  Histogramm */
  class Hist
  {
    friend void printHist(const Hist& h);

  public:
    Hist();
    Hist(const Hist&);
    Hist(int number, double diff = 1.0, double minval = -0.5);
    ~Hist();

    double minValue() const
    {
      return lowerLimit;
    }

    double classWidth() const
    {
      return classwidth;
    }

    bool isValid() const
    {
      return isInit;
    }

    int getNClasses() const
    {
      return nClasses;
    }

    int add(double v, int count = 1);

    int count(int nr) const;
    double Rel(int nr) const;

    double ClassValue(int nr) const;
    double ClassValue(int nr, double& min, double& max) const;

    int reset();
    int reset(int number, double diff = 1.0, double minval = 0.0);

    int Limits(double& min, double& max) const;
    int Limits(double& min, double& max, double quantil) const;

    void getStatistics(int& n) const ;
    void getStatistics(int& n, double& xm, double& xs) const;
    void getStatistics(int& n, double& xm, double& xs, double& skew) const;

    void draw(const Image& img, int value) const;

    Hist& operator=(const Hist& h);

  protected:
    bool isInit;
    double lowerLimit;      /* untere Grenze, unterste Klasse Histogramm */
    double classwidth;      /* Klassenbreite */
    int nClasses;           /* Anzahl der Klassen des Histogramms */
    double min;             /* kleinster Wert */
    double max;             /* größter Wert */
    int sum;

    unsigned int* klasse;   /* Anzahl der Werte in entsprechender Klasse
           klasse[0] Werte kleiner Histogrammbereich
           klasse[nklassen+2] Werte größer Histogrammbereich */
  };

  Hist HistImg(const Image& b, int diff = 1, int klbr = 1);
}
#endif
