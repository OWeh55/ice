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
#ifndef _COLORVALUE_H
#define _COLORVALUE_H

#include <cmath>

#include <iostream>

#include "defs.h"
#include "numbase.h"

namespace ice
{
  class ColorValue
  {
  public:
    int red, green, blue;

    ColorValue(): red(0), green(0), blue(0) {}
    explicit ColorValue(int v): red(v), green(v), blue(v) {};
    ColorValue(int rp, int gp, int bp): red(rp), green(gp), blue(bp) {};

    int& operator[](int i)
    {
      switch (i)
        {
        case 0:
          return red;
        case 1:
          return green;
        default:
          return blue;
        }
    }

    const int& operator[](int i) const
    {
      switch (i)
        {
        case 0:
          return red;
        case 1:
          return green;
        default:
          return blue;
        }
    }

    bool operator==(const ColorValue& b2) const
    {
      return red == b2.red && green == b2.green && blue == b2.blue;
    }

    bool operator!=(const ColorValue& b2) const
    {
      return red != b2.red || green != b2.green || blue != b2.blue;
    }

    bool operator<(const ColorValue& b2) const
    {
      if (red != b2.red) return red < b2.red;
      if (green != b2.green) return green < b2.green;
      return blue < b2.blue;
    }

    const ColorValue& operator+=(const ColorValue& b2)
    {
      red += b2.red;
      green += b2.green;
      blue += b2.blue;
      return *this;
    }

    ColorValue operator+(const ColorValue& b2) const
    {
      ColorValue res = *this;
      res += b2;
      return res;
    }

    const ColorValue& operator-=(const ColorValue& b2)
    {
      red -= b2.red;
      green -= b2.green;
      blue -= b2.blue;
      return *this;
    }

    ColorValue operator-(const ColorValue& b2) const
    {
      ColorValue res = *this;
      res -= b2;
      return res;
    }

    const ColorValue& operator*=(int d)
    {
      red *= d;
      green *= d;
      blue *= d;
      return *this;
    }

    ColorValue operator*(int d) const
    {
      ColorValue res = *this;
      res *= d;
      return res;
    }

    const ColorValue& operator/=(int d)
    {
      red /= d;
      green /= d;
      blue /= d;
      return *this;
    }

    ColorValue operator/(int d) const
    {
      ColorValue res = *this;
      res /= d;
      return res;
    }

    const ColorValue& operator*=(double d)
    {
      red = RoundInt(red * d);
      green = RoundInt(green * d);
      blue = RoundInt(blue * d);
      return *this;
    }

    ColorValue operator*(double d) const
    {
      ColorValue res = *this;
      res *= d;
      return res;
    }

    const ColorValue& operator/=(double d)
    {
      red = RoundInt(red / d);
      green = RoundInt(green / d);
      blue = RoundInt(blue / d);
      return *this;
    }

    ColorValue operator/(double d) const
    {
      ColorValue res = *this;
      res /= d;
      return res;
    }

    int abs2() const
    {
      return red * red + green * green + blue * blue;
    }

    double abs() const
    {
      return sqrt(abs2());
    }

    int absL1() const
    {
      return std::abs(red) + std::abs(green) + std::abs(blue);
    }

    int getGray() const
    {
      return (red + green + blue + 1) / 3;
    }

    ColorValue Limited(int mv)
    {
      ColorValue result = *this;
      if (result.red < 0)
        result.red = 0;
      if (result.red > mv)
        result.red = mv;

      if (result.green < 0)
        result.green = 0;
      if (result.green > mv)
        result.green = mv;

      if (result.blue < 0)
        result.blue = 0;
      if (result.blue > mv)
        result.blue = mv;

      return result;
    }
  };

  std::ostream& operator<<(std::ostream& os, ColorValue v);
}

#endif  /* IFNDEF _COLORVALUE_H */
