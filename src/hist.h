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
    friend int PrintHist(const Hist& h);

  public:
    Hist();
    Hist(const Hist&);
    Hist(int number, double diff = 1.0, double minval = -0.5);
    ~Hist();

    //      const int &number;

    //      const double &minval;
    //      const double &diff;
    //      const int &valid;

    double minValue() const
    {
      return lowerLimit;
    }
    double classWidth() const
    {
      return classwidth;
    }
    bool valid() const
    {
      return isInit;
    }
    int classes() const
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

    int Statistic(int& n) const ;
    int Statistic(int& n, double& xm, double& xs) const;
    int Statistic(int& n, double& xm, double& xs, double& skew) const;

    int Vis(int val, const Image& img) const;

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
