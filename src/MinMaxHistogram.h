/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2016 FSU Jena, Digital Image Processing Group
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
#ifndef MIN_MAX_HISTOGRAM_H
#define MIN_MAX_HISTOGRAM_H

class MinMaxHistogram
{
  // very simple and fast histogram
  // used for incremental calculation of min and max

  int* data;
  int size;
  int min, max;

public:
  MinMaxHistogram(int maxval)
  {
    size = maxval + 1;
    data = new int[size];
    reset();
  }

  void reset()
  {
    for (int i = 0; i < size; i++)
      {
        data[i] = 0;
      }

    min = size - 1;
    max = 0;
  }

  ~MinMaxHistogram()
  {
    delete [] data;
  }

  inline void inc(int i)
  {
    int oldValue = data[i]++;

    if (oldValue == 0)
      {
        // was empty
        if (i < min)   // new min?
          {
            min = i;
          }

        if (i > max)   // new max?
          {
            max = i;
          }
      }
  }

  inline void dec(int i)
  {
    int newValue = --data[i];
    if (newValue == 0)
      {
        // now empty
        if (i == min)   // if i was min
          {
            // find new min
            while (min < size - 1 && data[min] == 0)
              {
                min++;
              }
          }

        if (i == max)   // if i was max
          {
            // find new max
            while (max > 0 && data[max] == 0)
              {
                max--;
              }
          }
      }
  }
  int getMin() const
  {
    return min;
  }
  int getMax() const
  {
    return max;
  }
};
#endif
