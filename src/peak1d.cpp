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
#include <string.h>

#include "macro.h"
#include "float.h"
#include "defs.h"
#include "IceException.h"

#include "peak1d.h"

using namespace std;

namespace ice
{
  static int getMax1D(const vector<double>& values, const vector<bool>& mark)
  {
    double max = -DBL_MAX;
    int index = -1;

    for (unsigned int i = 0; i < values.size(); i++)
      if (!mark[i] && max < values[i])
        {
          max = values[i];
          index = i;
        }

    return index;
  }

  static void follow(const vector<double>& values,
                     vector<bool>& mark,
                     int index, int step,
                     bool cyclic, double noise)
  {
    int pos = index;
    mark[pos] = true;
    int minpos = index;
    double min = values[pos];
    int nValues = values.size();
    bool minFound = false;
    while (!minFound)
      {
        pos += step;
        if (!cyclic)
          {
            if (pos < 0)
              {
                minpos = 0;
                minFound = true;
              }
            else if (pos >= nValues)
              {
                minpos = nValues - 1;
                minFound = true;
              }
          }

        if (!minFound)
          {
            pos = (pos % nValues + nValues) % nValues;

            if (mark[pos])
              {
                minpos = pos;
                minFound = true;
              }
            else if (values[pos] <= min)
              {
                minpos = pos;
                min = values[pos];
              }
            else if (values[pos] > noise + min)
              {
                minFound = true;
              }
          }
      }

    for (int i = index; i != minpos; i = (i + step + nValues) % nValues)
      {
        mark[i] = true;
      }

    mark[minpos] = true;
  }

  static int PlateauMitte(const vector<double>& values, bool cyclic, int index)
  {
    int start = index;
    int end = index;

    int nValues = values.size();
    int value = values[index];

    while (values[(start + nValues - 1) % nValues] == value)
      {
        start--;
      }

    while (values[(end + nValues + 1) % nValues] == value)
      {
        end++;
      }

    return ((start + end) / 2 + nValues) % nValues;
  }

#define FNAME "peak1d"

// values enthält die diskrete 1D-Funktion (Funktionswerte)
// cyclic gibt an, ob die Funktion als zyklisch anzusehen ist.
// Die Funktion gibt ein int-Feld mit den Indizes der Maxima innerhalb des
// Feldes values zurück.

  vector<int> peak1d(const vector<double>& values, bool cyclic, double noise)
  {
    vector<int> result;

    int nValues = values.size();

    if (nValues == 0)
      {
        return result;
      }

    vector<bool> mark(nValues, false);

    // Test auf konstante Funktion

    double val1 = values[0];
    int i = 1;
    while (i < nValues && values[i] == val1)
      {
        i++;
      }

    if (i >= nValues)
      {
        return result;
      }

    int index;

    while ((index = getMax1D(values, mark)) >= 0)   // max found?
      {
        follow(values, mark, index, -1, cyclic, noise);
        follow(values, mark, index, 1, cyclic, noise);
        result.push_back(PlateauMitte(values, cyclic, index));
      }

    return result;
  }

  IVector peak1d(const Vector& v, bool cyclic, double noise)
  {
    try
      {
        IVector ret;
        std::vector<double> vv(v.size());

        for (unsigned int i = 0; i < v.size(); i++)
          {
            vv[i] = v[i];
          }

        std::vector<int> ip = peak1d(vv, cyclic, noise);

        ret.Resize(ip.size());

        for (unsigned int i = 0; i < ip.size(); i++)
          {
            ret[i] = ip[i];
          }

        return ret;
      }
    RETHROW;
  }
#undef FNAME
}
