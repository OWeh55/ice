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
#ifndef POINTVALUE_H
#define POINTVALUE_H

#include "Point.h"

namespace ice
{
  template<typename Tcoor, typename Tval>

  class pointvalueT: public pointT<Tcoor>
  {
  public:
    pointvalueT(Tcoor x, Tcoor y, Tval val = 0): pointT<Tcoor>(x, y), value(val) {}
    pointvalueT(pointT<Tcoor> p, Tval val = 0): pointT<Tcoor>(p), value(val) {}
    pointvalueT(): pointT<Tcoor>(), value(0) {}
    Tval getValue() const
    {
      return value;
    }
    Tval Value() const
    {
      return value;
    }
    void setValue(Tval v)
    {
      value = v;
    }
    bool operator<(const pointvalueT& rhs) const
    {
      return value < rhs.value;
    }
  private:
    Tval value;
  };

  typedef pointvalueT<double, double> PointValue;
  typedef pointvalueT<int, double> IPointValue;
}
#endif
