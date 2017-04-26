/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#include <stdexcept>
#include <iostream>

#include "darith.h"
#include "Accumulator.h"

using namespace std;
namespace ice
{
//
// Accumulator
//
// increase one bin by val
  void Accumulator::inc(unsigned int x, double val)
  {
    if (x < (unsigned int)nBins)
      {
        double v = data[x];
        v += val;
        data[x] = v;

        // update of maximum
        if (v > maxFrequency)
          {
            xMax = x;
            maxFrequency = v;
          }
      }
  }

  void Accumulator::init()
  {
    for (auto& c : data)
      c = 0.0;
    xMax = 0;
    maxFrequency = 0;
    nValues = 0;
    // because of spreading each value is inserted with
    // weight greater 1.0
    weightOfSingleEntry = 1.0;
    for (int i = 1; i < dn; i++)
      weightOfSingleEntry += 2.0 / (i + 1);
  }

  void Accumulator::clear()
  {
    if (nBins < 0)
      throw logic_error("Accumulator::clear - not initialized");
    init();
  }

#define FNAME "Accumulator::set"
  void Accumulator::set(int n, double minp, double maxp, bool modp, int dnp)
  {
    if (n < 1)
      throw logic_error("Accumulator::set - wrong number of bins");
    data.resize(n);
    nBins = n;
    min = minp;
    if (minp >= maxp)
      throw logic_error("Accumulator::set - wrong range min..max");
    dif = maxp - minp;
    mod = modp;
    if (dn < 0)
      throw logic_error("Accumulator::set - wrong dn");
    dn = dnp;
    init();
  }

  Accumulator::Accumulator(int n1, double min1, double max1, bool mod1, int dn)
  {
    set(n1, min1, max1, mod1, dn);
  }

  void Accumulator::add(double xd)
  {
    if (nBins < 0)
      throw logic_error("Accumulator::add - not initialized");

    int x = RoundInt((xd - min) / dif * nBins);

    if (mod)
      x %= nBins;

    // neighborhoud
    for (int dx = -dn; dx <= dn; dx++)
      {
        double dist = abs(dx) + 1.0;
        int x1 = x + dx;

        if (mod)
          x1 %= nBins;

        inc(x1, 1.0 / dist);
      }
  }

  void Accumulator::getMax(double& x, double& val) const
  {
    if (nBins < 0 || maxFrequency == 0.0)
      throw logic_error("Accumulator::getMax - not initialized");

    x = ((double)xMax + 0.5) / ((double)nBins) * dif + min; // center of bin
    val = maxFrequency / weightOfSingleEntry;
  }

  void Accumulator::getMax(double& x) const
  {
    double val;
    getMax(x, val);
  }

  double Accumulator::getMax() const
  {
    double x, val;
    getMax(x, val);
    return x;
  }
}
