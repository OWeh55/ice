/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
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
#ifndef ICE_CLASS_SAMPLE_H
#define ICE_CLASS_SAMPLE_H

#include <iostream>
#include <vector>

namespace ice
{
  /**
   * class for samples - helperclass for classifiers
   */

  class ClassSample
  {
  public:
    ClassSample(): classNr(-3) {}
    ClassSample(int c, const std::vector<double>& f):
      classNr(c), features(f) {}

    // class number
    int classNr;

    // feature vector
    std::vector<double> features;

    const double& operator[](int idx) const
    {
      return features[idx];
    }
    double& operator[](int idx)
    {
      return features[idx];
    }
    int size() const
    {
      return features.size();
    }

  };

  std::ostream& operator<<(std::ostream& os, const ClassSample& s);
  std::istream& operator>>(std::istream& is, ClassSample& s);
}


#endif
