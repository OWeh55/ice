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
#include <stdexcept>
#include "darith.h"
#include "Accumulator2.h"

using namespace std;
namespace ice
{
//
// Accumulator2
//
#define FNAME "Accumulator2::init"
  void Accumulator2::init() // (re-) initialisation, used by constructor and clear
  {
    if (nBins1 < 0 || nBins2 < 0)
      {
        throw logic_error("Accumulator2::init - not initialized");
      }

    data.resize(nBins1, nBins2);
    data.set(0);
    maxFrequency = 0.0;
    nValues = 0;
    weightOfSingleEntry = 0.0 ;
    for (int dx = -dn1; dx <= dn1; dx++)
      {
        int ddx = abs(dx) + 1;
        for (int dy = -dn2; dy <= dn2; dy++)
          {
            int ddy = abs(dy) + 1;
            int dd = ddx > ddy ? ddx : ddy;
            weightOfSingleEntry += 1.0 / dd;
          }
      }
  }
#undef FNAME
// increment of one cell
  void Accumulator2::inc(unsigned int x, unsigned int y, double val)
  {
    if (x < (unsigned int)nBins1 && y < (unsigned int)nBins2)
      {
        double v = data[x][y] + val;
        data[x][y] = v;

        // update of maximum
        if (v > maxFrequency)
          {
            xMax = x;
            yMax = y;
            maxFrequency = v;
          }
      }
  }

#define FNAME "Accumulator2::clear"
  void Accumulator2::clear()
  {
    if (nBins1 < 0 || nBins2 < 0)
      {
        throw logic_error("Accumulator2::init - not initialized");
      }

    init();
  }
#undef FNAME
#undef FNAME

#define FNAME "Accumulator2::setDim"
  void Accumulator2::setDim(int i, int n, double min, double max, bool mod, int dn)
  {
    if (i == 0)
      {
        nBins1 = n;
        min1 = min;
        dif1 = max - min;
        mod1 = mod;
        dn1 = dn;
      }
    else if (i == 1)
      {
        nBins2 = n;
        min2 = min;
        dif2 = max - min;
        mod2 = mod;
        dn2 = dn;
      }
    else
      {
        throw logic_error("Accumulator2::setDim - wrong dn");
      }
    if (nBins1 >= 0 && nBins2 >= 0)
      {
        init();
      }
  }
#undef FNAME

  Accumulator2::Accumulator2(int n1, double min1, double max1, bool mod1, int dn1,
                             int n2, double min2, double max2, bool mod2, int dn2)
  {
    setDim(0, n1, min1, max1, mod1, dn1);
    setDim(1, n2, min2, max2, mod2, dn2);
  }

#define FNAME "Accumulator2::Add"
  void Accumulator2::add(double xd, double yd)
  {
    if (nBins1 < 0 || nBins2 < 0)
      {
        throw logic_error("Accumulator2::add - not initialized");
      }

    int x = RoundInt((xd - min1) / dif1 * nBins1);

    if (mod1)
      {
        x = x % nBins1;
      }

    int y = RoundInt((yd - min2) / dif2 * nBins2);

    if (mod2)
      {
        y = y % nBins2;
      }

    // neighborhoud
    for (int dy = -dn2; dy <= dn2; dy++)
      {
        int ddy = abs(dy) + 1;

        int y1 = y + dy;
        if (mod2)
          {
            y1 %= nBins2;
          }

        for (int dx = -dn1; dx <= dn1; dx++)
          {
            int ddx = abs(dx) + 1;
            double dist = ddx > ddy ? ddx : ddy;

            int x1 = x + dx;
            if (mod1)
              {
                x1 %= nBins1;
              }

            inc(x1, y1, 1.0 / dist);
          }
      }
  }

  void Accumulator2::add(const Point& p)
  {
    add(p.x, p.y);
  }

  void Accumulator2::add(const Vector& v)
  {
    if (v.size() != 2)
      {
        throw logic_error("Accumulator2::add - wrong vector size");
      }

    add(v[0], v[1]);
  }

  void Accumulator2::getMax(double& x, double& y, double& val) const
  {
    if (nBins1 < 0 || nBins2 < 0 || maxFrequency == 0.0)
      {
        throw logic_error("Accumulator2::getMax - not initialized");
      }

    x = (double)(xMax + 0.5) / nBins1 * dif1 + min1;
    y = (double)(yMax + 0.5) / nBins2 * dif2 + min2;
    val = maxFrequency / weightOfSingleEntry;
  }

  void Accumulator2::getMax(double& x, double& y) const
  {
    double val;
    getMax(x, y, val);
  }

  Point Accumulator2::getMax() const
  {
    Point p;
    getMax(p.x, p.y);
    return p;
  }

  Point Accumulator2::getMax(double& val) const
  {
    Point p;
    getMax(p.x, p.y, val);
    return p;
  }
}
