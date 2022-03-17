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
#ifndef ICE_LINE_ACCUMULATION_H
#define ICE_LINE_ACCUMULATION_H

#include <vector>

#include "lineseg.h"

namespace ice
{
  /**
   * detectLine detects a line from set of candidate points.
   * detectLine uses accumulation to get the most
   * probable line.
   * The function returns an unlimited straight line as LineSeg.
   * @param pointList list of points
   * @see Accumulator2
   * @see LineSeg
   */
  LineSeg detectLine(const std::vector<Point>& pointList);

  /**
   * detectLine detects a line from set of candidate points.
   * detectLine uses accumulation to get the most
   * probable line.
   * The function returns an unlimited straight line as LineSeg.
   * @param pointList list of points
   * @param nPairs limit accumulation to nPairs pair of points.\n
   * This reduces runtime and accuracy.
   * @see Accumulator2
   * @see LineSeg
   */
  LineSeg detectLine(const std::vector<Point>& pointList, int nPairs);
}
#endif
