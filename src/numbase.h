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
#ifndef _NUMBASE_H
#define _NUMBASE_H

#include <algorithm>
#include <cmath>

namespace ice
{
  double  Arcus(double degree);
  double  Degree(double arcus);
  double  FMod(double v, double div);
  int     Mod(int v, int div);
  double  CubRoot(double val);
  int     Sign(int val);
  int     Sign(double val);
  double  SignD(double val);

  void    Randomize();
  int     Random(int val);
  double  RandomD(double max = 1.0);
  double  GaussRandom(double sigma);

  /**
   * gaussian function / normal distribution
   */
  double  gaussian(double x, double my = 0.0, double sigma = 1.0);

// Value for differences, that should be treated as zero
// valid for distances of points ...
#define EPSILON 1e-15

#ifdef WIN32
  inline double  Round(double val)
  {
    return floor(val + 0.5);
  }
  inline void    Round(double val, int& i)
  {
    i = (int)floor(val + 0.5);
  }
  inline int     RoundInt(double val)
  {
    return (int)floor(val + 0.5);
  }
#else
  inline double Round(double val)
  {
    return rint(val);
  }

  inline int RoundInt(double val)
  {
    return lrint(val);
  }
#endif

  template<typename T>
  inline T Abs(T x)
  {
    return x < 0 ? -x : x;
  }

  template<typename T>
  inline void UpdateMax(T& var, T val)
  {
    if (val > var)
      var = val;
  }

  template<typename T>
  inline void UpdateMin(T& var, T val)
  {
    if (val < var)
      var = val;
  }

  template<typename T>
  inline void UpdateLimits(T& min, T& max, T val)
  {
    if (val < min)
      min = val;
    if (val > max)
      max = val;
  }

  template<typename T>
  void order(T& a, T& b)
  {
    if (a > b)
      {
        std::swap(a, b);
      }
  }

  inline double Sqr(double val)
  {
    return val * val;
  }
  inline double Cub(double val)
  {
    return val * val * val;
  }

  inline double Arcus(double degree)
  {
    return degree * M_PI / 180;
  }

  inline double Degree(double arcus)
  {
    return arcus * 180 / M_PI;
  }

  // solve linear equation system with 2 equations and 2 variables
  //    a1 * x1 + b1 * x2 = i1
  //    a2 * x1 + b2 * x2 = i2
  bool Solve2(double a1, double b1, double i1,
              double a2, double b2, double i2,
              double& x1, double& x2);

}
#endif
