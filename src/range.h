/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2020 FSU Jena, Digital Image Processing Group
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

#ifndef _RANGE_H_
#define _RANGE_H_

#include <limits>

namespace ice
{
  template<typename T>
  class Range
  {
  public:
    Range(): lowest(std::numeric_limits<T>::max()),
	     highest(std::numeric_limits<T>::min()) {}
    
    Range(T min, T max): lowest(min), highest(max) {}
    
    void update(T val)
    {
      if (val < lowest)
        {
          lowest = val;
        }
      if (val > highest)
        {
          highest = val;
        }
    }
    
    bool isValid() const
    {
      return lowest <= highest;
    }
    
    void setInvalid()
    {
      highest = 0;
      lowest = 1;
    }
    
    bool isInRange(T val) const
    {
      return val >= lowest && val <= highest;
    }

    T getLowest() const
    {
      return lowest;
    }

    T getHighest() const
    {
      return highest;
    }

    T length() const
    {
      return highest - lowest;
    }

    bool operator==(const Range& sec) const
    {
      return highest == sec.highest && lowest == sec.lowest;
    }
    
    // required operator< for use in map, set ...
    bool operator<(const Range& sec) const
    {
      return (lowest < sec.lowest) ||
	((lowest == sec.lowest) && (highest < sec.highest));
    }
  private:
    T lowest;
    T highest;
  };
} // namespace ice
#endif
