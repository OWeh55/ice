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

/*
 * Author: Alexander Lärz, 2005
 */


#include <math.h>

#include "macro.h"
#include "IceException.h"
#include "defs.h"

#include "quaternion.h"

namespace ice
{

  int sign(double x)
  {
    if (x < 0.0)
      {
        return -1;
      }

    if (x > 0.0)
      {
        return 1;
      }

    return 0;
  }

  Quaternion :: Quaternion()
  {
    this->real = 0;
    this->i = 0;
    this->j = 0;
    this->k = 0;
  }

  Quaternion :: Quaternion(double real, double i, double j, double k)
  {
    this->real = real;
    this->i = i;
    this->j = j;
    this->k = k;
  }

  Quaternion :: Quaternion(double real, Vector3d& imaginary)
  {
    this->real = real;

    i = imaginary.x;
    j = imaginary.y;
    k = imaginary.z;
  }

  Quaternion :: ~Quaternion()
  {
  }

  void Quaternion :: setReal(double real)
  {
    this->real = real;
  }

  void Quaternion :: setI(double i)
  {
    this->i = i;
  }

  void Quaternion :: setJ(double j)
  {
    this->j = j;
  }

  void Quaternion :: setK(double k)
  {
    this->k = k;
  }

  void Quaternion :: setImaginary(Vector3d& imaginary)
  {
    i = imaginary.x;
    j = imaginary.y;
    k = imaginary.z;
  }

  double Quaternion :: getSquareNorm()
  {
    return real * real + i * i + j * j + k * k;
  }

  double Quaternion :: getNorm()
  {
    return sqrt(getSquareNorm());
  }

#define FNAME "Quaternion :: getEigenwinkel()"
  double Quaternion :: getEigenwinkel()
  {
    if (real != 0)
      {
        if (real > 0)
          {
            return atan(sqrt(i * i + j * j + k * k) / real);
          }
        else
          {
            return atan(sqrt(i * i + j * j + k * k) / real) + M_PI;
          }
      }
    else
      {
        if (!(i == 0 && j == 0 && k == 0))
          {
            return M_PI_2;
          }
      }

    throw IceException(FNAME, M_NO_QUATERNIONEIGENWINKEL, NO_QUATERNIONEIGENWINKEL);

    return -1.0;
  }
#undef FNAME

#define FNAME "Quaternion :: getEigenachse()"
  Quaternion Quaternion:: getEigenachse()
  {
    if (i == 0 && j == 0 && k == 0)
      {
        throw IceException(FNAME, M_NO_QUATERNIONEIGENACHSE, NO_QUATERNIONEIGENACHSE);

        return Quaternion(0, 0, 0, 0);
      }

    double v = sqrt(i * i + j * j + k * k);

    return Quaternion(0, i / v, j / v, k / v);
  }
#undef FNAME

#define FNAME "Quaternion :: getPhases()"
  Vector3d Quaternion :: getPhases()
  {
    double alpha;
    double beta;
    double delta;

    Quaternion temp;

    if (getSquareNorm() == 0)
      {
        throw IceException(FNAME, M_NO_QUATERNIONPHASES, NO_QUATERNIONPHASES);

        return Vector3d(0, 0, 0);
      }

    temp = getNormalize();

    double x = 2 * (temp.i * temp.j - temp.real * temp.k);

    if (x > -1.0 && x < 1.0)
      {
        delta = -0.5 * asin(x);
      }
    else
      {
        if (x < 0)
          {
            delta = -M_PI_4;
          }
        else
          {
            delta = M_PI_4;
          }
      }

    if (delta == M_PI_4 || delta == -M_PI_4)
      {
        beta = 0;
        alpha = 0.5 * atan2(2 * (temp.real * temp.i - temp.j * temp.k),
                            temp.real * temp.real - temp.i * temp.i - temp.j * temp.j + temp.k * temp.k);
      }
    else
      {
        alpha = 0.5 * atan2(2 * (temp.real * temp.i + temp.j * temp.k),
                            temp.real * temp.real - temp.i * temp.i + temp.j * temp.j - temp.k * temp.k);

        beta = 0.5 * atan2(2 * (temp.i * temp.k + temp.real * temp.j),
                           temp.real * temp.real + temp.i * temp.i - temp.j * temp.j - temp.k * temp.k);
      }

    Quaternion e1 = qexp(Quaternion(0, alpha, 0, 0));
    Quaternion e2 = qexp(Quaternion(0, 0, 0, delta));
    Quaternion e3 = qexp(Quaternion(0, 0, beta, 0));

    Quaternion r = e1 * e2 * e3;

    if ((sign(r.getReal()) != sign(temp.getReal())) ||
        (sign(r.getI()) != sign(temp.getI())) ||
        (sign(r.getJ()) != sign(temp.getJ())) ||
        (sign(r.getK()) != sign(temp.getK())))

      {
        if (alpha >= 0)
          {
            alpha = alpha - M_PI;
          }
        else
          {
            alpha = alpha + M_PI;
          }
      }


    return Vector3d(alpha, beta, delta);
  }
#undef FNAME

  bool Quaternion :: isUnitQuaternion()
  {
    if (getSquareNorm() == 1)
      {
        return true;
      }

    return false;
  }

  Quaternion Quaternion :: getNegate()
  {
    double nreal = -real;
    double ni = -i;
    double nj = -j;
    double nk = -k;

    return Quaternion(nreal, ni, nj, nk);
  }

  Quaternion Quaternion :: getConjugate()
  {
    double ni = -i;
    double nj = -j;
    double nk = -k;

    return Quaternion(real, ni, nj, nk);
  }

  Quaternion Quaternion :: getSquare()
  {
    double temp = 2 * real;
    double nreal = real * real - (i * i + j * j + k * k);
    double ni = i * temp;
    double nj = j * temp;
    double nk = k * temp;

    return Quaternion(nreal, ni, nj, nk);
  }


#define FNAME "Quaternion :: getInverse()"
  Quaternion Quaternion :: getInverse()
  {
    double norm = getNorm();

    if (norm == 0)
      {
        throw IceException(FNAME, M_NO_QUATERNIONINVERSE, NO_QUATERNIONINVERSE);

        return (*this);
      }

    double nreal = real / norm;
    double ni = -i / norm;
    double nj = -j / norm;
    double nk = -k / norm;

    return Quaternion(nreal, ni, nj, nk);
  }
#undef FNAME

#define FNAME "Quaternion :: getNormalize()"
  Quaternion Quaternion :: getNormalize()
  {
    double norm = getNorm();

    if (norm == 0)
      {
        throw IceException(FNAME, M_NOT_NORMALIZEABLE, NOT_NORMALIZEABLE);

        return (*this);
      }

    double nreal = real / norm;
    double ni = i / norm;
    double nj = j / norm;
    double nk = k / norm;

    return Quaternion(nreal, ni, nj, nk);
  }
#undef FNAME

  Quaternion operator-(Quaternion& x)
  {
    return x.getNegate();
  }

  bool operator==(const Quaternion& a, const Quaternion& b)
  {
    return (a.real == b.real && a.i == b.i && a.j == b.j && a.k == b.k);
  }

  bool operator!=(const Quaternion& a, const Quaternion& b)
  {
    return !(a == b);
  }

  Quaternion& Quaternion :: operator+=(const Quaternion& b)
  {
    real += b.real;
    i += b.i;
    j += b.j;
    k += b.k;

    return *this;
  }

  Quaternion& Quaternion :: operator-=(const Quaternion& b)
  {
    real -= b.real;
    i -= b.i;
    j -= b.j;
    k -= b.k;

    return *this;
  }

  Quaternion& Quaternion :: operator*=(const double b)
  {
    real *= b;
    i *= b;
    j *= b;
    k *= b;

    return *this;
  }

  Quaternion& Quaternion :: operator/=(const double b)
  {
    real /= b;
    i /= b;
    j /= b;
    k /= b;

    return *this;
  }

  Quaternion& Quaternion :: operator*=(const Quaternion& b)
  {
    double t0 = (k - j) * (b.j - b.k);
    double t1 = (real + i) * (b.real + b.i);
    double t2 = (real - i) * (b.j + b.k);
    double t3 = (k + j) * (b.real - b.i);
    double t4 = (k - i) * (b.i - b.j);
    double t5 = (k + i) * (b.i + b.j);
    double t6 = (real + j) * (b.real - b.k);
    double t7 = (real - j) * (b.real + b.k);
    double t8 = t5 + t6 + t7;
    double t9 = (t4 + t8) / 2;

    real = t0 + t9 - t5;
    i = t1 + t9 - t8;
    j = t2 + t9 - t7;
    k = t3 + t9 - t6;

    return *this;
  }

  ostream& operator<<(ostream& ost, const Quaternion& x)
  {
    const char* str1 = "+";
    const char* str2 = "+";
    const char* str3 = "+";

    if (x.i < 0)
      {
        str1 = "";
      }

    if (x.j < 0)
      {
        str2 = "";
      }

    if (x.k < 0)
      {
        str3 = "";
      }

    return ost << x.real << str1 << x.i << "i" << str2 << x.j << "j" << str3 << x.k << "k";
  }


  Quaternion qexp(const Quaternion& x)
  {
    double vectornorm = sqrt(x.i * x.i + x.j * x.j + x.k * x.k);

    double e = exp(x.real);
    double s = sin(vectornorm);

    double real = e * cos(vectornorm);

    if (vectornorm != 0)
      {
        double i = x.i * s / vectornorm;
        double j = x.j * s / vectornorm;
        double k = x.k * s / vectornorm;

        return Quaternion(real, i, j, k);
      }

    return Quaternion(real, 0, 0, 0);
  }

  double Quaternion :: myatan2(double x, double y)
  {
    if (y > 0)
      {
        return atan(x / y);
      }

    if (y == 0)
      {
        if (x > 0)
          {
            return M_PI;
          }
        else
          {
            return -M_PI;
          }
      }

    if (y < 0 && x < 0)
      {
        return atan(x / y) - M_PI;
      }

    if (y < 0 && x >= 0)
      {
        return atan(x / y) + M_PI;
      }

    return 0;
  }

}


