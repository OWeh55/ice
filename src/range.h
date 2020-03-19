#ifndef _RANGE_H_
#define _RANGE_H_

#include <limits>

namespace ice
{
  template<typename T>
  class Range
  {
    T lowest;
    T highest;
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
      highest = lowest;
      highest--;
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

    bool operator==(const Range& sec)
    {
      return highest == sec.highest && lowest == sec.lowest;
    }

    // requested operator< for use in map, set ...
    bool operator<(const Range& sec)
    {
      return (lowest < sec.lowest) ||
             ((lowest == sec.lowest) && (highest < sec.highest));
    }
  };
} // namespace ice
#endif
