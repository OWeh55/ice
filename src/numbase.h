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

  double  gauss(double my, double sigma, double x);

#if 0
  /* Numerische Konstanten */
#ifndef M_E
#define M_E         2.7182818284590452354
#endif
#ifndef M_LOG2E
#define M_LOG2E     1.4426950408889634074
#endif
#ifndef M_LOG10E
#define M_LOG10E    0.43429448190325182765
#endif
#ifndef M_LN2
#define M_LN2       0.69314718055994530942
#endif
#ifndef M_LN10
#define M_LN10      2.30258509299404568402
#endif
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2      1.57079632679489661923
#endif
#ifndef M_1_PI
#define M_1_PI      0.31830988618379067154
#endif
#ifndef M_PI_4
#define M_PI_4      0.78539816339744830962
#endif
#ifndef M_2_PI
#define M_2_PI      0.63661977236758134308
#endif
#ifndef M_2_SQRTPI
#define M_2_SQRTPI  1.12837916709551257390
#endif
#ifndef M_SQRT2
#define M_SQRT2     1.41421356237309504880
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2   0.70710678118654752440
#endif
#endif

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

#if(defined(__i386)||defined(__i486)||defined(__x86_64))
  inline int RoundInt(double val)
  {
    int ival;
    __asm("fldl %1 \n fistpl %0 " : "=m"(ival) : "m"(val) : "memory");
    return ival;
  }
#else
  inline int RoundInt(double val)
  {
    return lrint(val);
  }
#endif
#endif

  template<typename T>
  inline T Abs(T x)
  {
    return x < 0 ? -x : x;
  }

  template<typename T>
  inline T Max(const T& val1, const T& val2)
  {
    if (val1 > val2) return val1;

    return val2;
  }

  template<typename T>
  inline T Min(const T& val1, const T& val2)
  {
    if (val1 < val2) return val1;

    return val2;
  }

  template<typename T>
  inline void UpdateMax(T& var, T val)
  {
    if (val > var) var = val;
  }

  template<typename T>
  inline void UpdateMin(T& var, T val)
  {
    if (val < var) var = val;
  }

  template<typename T>
  inline void UpdateLimits(T& min, T& max, T val)
  {
    if (val < min) min = val;
    if (val > max) max = val;
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
