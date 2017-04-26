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

// Funktionen zur Nutzung von Histogrammen
// Ortmann 11/99 .. 07/13
#ifndef _HISTOGRAM_H
#define _HISTOGRAM_H

#include <algorithm>
#include <vector>
#include <cstdio>

#include "base.h"

namespace ice
{
  /* Klasse  Histogramm */
  class Histogram
  {
  public:
    Histogram();
    Histogram(int number);
    Histogram(const Image& b, int diff = 1);

    friend void swap(Histogram& h1, Histogram& h2)
    {
      std::swap(h1.isInit, h2.isInit);
      std::swap(h1.nclasses, h2.nclasses);
      std::swap(h1.sum, h2.sum);
      std::swap(h1.classes, h2.classes);
    }

    int isValid() const
    {
      return isInit;
    }

    int nClasses() const
    {
      return nclasses;
    }

    int addValue(int val, int count = 1);
    int addValueUnchecked(int val, int count = 1);

    int addImage(const Image& img, int diff = 1);

    int getCount(int nr) const;
    std::vector<int> getCount() const;
    void getCount(std::vector<int>& ct) const;

    int getSum() const
    {
      return sum;
    }

    double getRelative(int nr) const;
    std::vector<double> getRelative() const;
    void getRelative(std::vector<double>& v) const;

    int reset();
    int reset(int number);

    int getLimits(int& min, int& max) const;
    int getLimits(int& min, int& max, double quantil) const;

    void getStatistics(int& n) const ;
    void getStatistics(int& n, double& xm, double& xs) const;
    void getStatistics(int& n, double& xm, double& xs, double& skew) const;

    void draw(const Image& img, int val) const;

    int operator[](int nr) const;

  protected:

    void addImg0(const Image& b, int diff);

    template<typename T>
    void addImg(const Image& b, int diff);

    bool isInit;
    int nclasses;           // number of classes
    int sum;
    std::vector<int> classes;   // counter for all classes
  };

  int PrintHistogram(const Histogram& h);
  double Distance(const Histogram& h1, const Histogram& h2);
}
#endif
