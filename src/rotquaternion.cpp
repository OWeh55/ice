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

#include "defs.h"
#include "Matrix.h"
#include "IceException.h"
#include "Vector3d.h"

#include "rotquaternion.h"

namespace ice
{

  RotQuaternion::RotQuaternion()
  {
  }

  RotQuaternion::~RotQuaternion()
  {
  }

  RotQuaternion::RotQuaternion(double angle, Vector3d& axis)
  {
    double real = cos(angle / 2);
    double i = axis.x;
    double j = axis.y;
    double k = axis.z;

    if (!(i == 0 && j == 0 && k == 0))
      {
        if (real == 1 || real == -1)
          {
            if (real == 1)
              {
                rot.real = 1;
              }
            else
              {
                rot.real = -1;
              }

          }
        else
          {
            double s = sin(angle / 2);
            double norm = sqrt(i * i + j * j + k * k);

            rot.real = real;
            rot.i = s * i / norm;
            rot.j = s * j / norm;
            rot.k = s * k / norm;
          }
      }
    else
      {
        rot.real = 1;
        rot.i = 0;
        rot.j = 0;
        rot.k = 0;
      }
  }

  RotQuaternion::RotQuaternion(double angle, double x, double y,
                               double z)
  {
    double real = cos(angle / 2);
    double i = x;
    double j = y;
    double k = z;

    if (!(i == 0 && j == 0 && k == 0))
      {
        if (real == 1 || real == -1)
          {
            if (real == 1)
              {
                rot.real = 1;
              }
            else
              {
                rot.real = -1;
              }

            rot.i = 0;
            rot.j = 0;
            rot.k = 0;
          }
        else
          {
            double s = sin(angle / 2);
            double norm = sqrt(i * i + j * j + k * k);

            rot.real = real;
            rot.i = i / norm * s;
            rot.j = j / norm * s;
            rot.k = k / norm * s;
          }
      }
    else
      {
        rot.real = 1;
        rot.i = 0;
        rot.j = 0;
        rot.k = 0;
      }
  }

  void RotQuaternion::setRotationAngle(double angle)
  {
    Vector3d axis = getRotationAxis();

    double real = cos(angle / 2);
    double i = axis.x;
    double j = axis.y;
    double k = axis.z;

    if (!(i == 0 && j == 0 && k == 0))
      {
        if (real == 1 || real == -1)
          {
            if (real == 1)
              {
                rot.real = 1;
              }
            else
              {
                rot.real = -1;
              }

            rot.i = 0;
            rot.j = 0;
            rot.k = 0;
          }
        else
          {
            double s = sin(angle / 2);

            rot.real = real;
            rot.i = i * s;
            rot.j = j * s;
            rot.k = k * s;
          }
      }
    else
      {
        rot.real = 1;
        rot.i = 0;
        rot.j = 0;
        rot.k = 0;
      }
  }

  void RotQuaternion::setRotationAxis(Vector3d& axis)
  {
    double angle = getRotationAngle() / 2;
    double i = axis.x;
    double j = axis.y;
    double k = axis.z;

    double norm = sqrt(i * i + j * j + k * k);

    if (!(i == 0 && j == 0 && k == 0))
      {
        if (rot.real == 1 || rot.real == -1)
          {
            rot.i = 0;
            rot.j = 0;
            rot.k = 0;
          }
        else
          {
            double s = sin(angle / 2);

            rot.i = i * s / norm;
            rot.j = j * s / norm;
            rot.k = k * s / norm;
          }
      }
    else
      {
        rot.real = 1;
        rot.i = 0;
        rot.j = 0;
        rot.k = 0;
      }

  }

  double RotQuaternion::getRotationAngle()
  {
    if (rot.real >= 1 || rot.real <= -1)
      {
        return 0;
      }

    return 2 * acos(rot.real);
  }

  Vector3d RotQuaternion::getRotationAxis()
  {
    double angle = getRotationAngle() / 2;

    if (angle == 0)
      {
        return Vector3d(0, 0, 0);
      }

    double s = sin(angle);
    double x = rot.i / s;
    double y = rot.j / s;
    double z = rot.k / s;

    return Vector3d(x, y, z);
  }

  RotQuaternion RotQuaternion::getNegate()
  {
    RotQuaternion a = RotQuaternion();

    a.rot.real = -rot.real;
    a.rot.i = -rot.i;
    a.rot.j = -rot.j;
    a.rot.k = -rot.k;

    return a;
  }

  RotQuaternion RotQuaternion::getConjugate()
  {
    RotQuaternion a = RotQuaternion();

    a.rot.real = rot.real;
    a.rot.i = -rot.i;
    a.rot.j = -rot.j;
    a.rot.k = -rot.k;

    return a;
  }

  RotQuaternion RotQuaternion::getSquare()
  {
    RotQuaternion a = RotQuaternion();

    double temp = 2 * rot.real;
    double real =
      rot.real * rot.real - (rot.i * rot.i + rot.j * rot.j +
                             rot.k * rot.k);
    double i = rot.i * temp;
    double j = rot.j * temp;
    double k = rot.k * temp;

    a.rot.real = real;
    a.rot.i = i;
    a.rot.j = j;
    a.rot.k = k;

    return a;
  }

  inline RotQuaternion RotQuaternion::getInverse()
  {
    return (*this).getConjugate();
  }

  double RotQuaternion::getSquareNorm()
  {
    return rot.getSquareNorm();
  }

  double RotQuaternion::getNorm()
  {
    return rot.getNorm();
  }

  double RotQuaternion::getEigenwinkel()
  {
    return rot.getEigenwinkel();
  }

  Quaternion RotQuaternion::getEigenachse()
  {
    return rot.getEigenachse();
  }

  Vector3d RotQuaternion::getPhases()
  {
    return rot.getPhases();
  }

#define FNAME "RotQuaternion :: getRotationMatrix()"
  Matrix RotQuaternion::getRotationMatrix()
  {
    Matrix rotm = Matrix(3, 3, 0);

    double real = rot.real;
    double i = rot.i;
    double j = rot.j;
    double k = rot.k;

    rotm[0][0] = real * real + i * i - j * j - k * k;
    rotm[0][1] = 2 * (i * j - real * k);
    rotm[0][2] = 2 * (real * j + i * k);

    rotm[1][0] = 2 * (real * k + i * j);
    rotm[1][1] = real * real - i * i + j * j - k * k;
    rotm[1][2] = 2 * (j * k - real * i);

    rotm[2][0] = 2 * (i * k - real * j);
    rotm[2][1] = 2 * (real * i + j * k);
    rotm[2][2] = real * real - i * i - j * j + k * k;

    return rotm;
  }
#undef FNAME

  RotQuaternion& RotQuaternion::operator*=(const RotQuaternion& b)
  {
    rot *= b.rot;

    return (*this);
  }

  RotQuaternion::operator  Quaternion()const
  {
    return (*this).rot;
  }

#define FNAME "convertToRotQuaternion(Quaternion& in)"
  RotQuaternion convertToRotQuaternion(Quaternion& in)
  {
    Quaternion temp = in.getNormalize();
    RotQuaternion out = RotQuaternion();

    out.rot.setReal(temp.getReal());
    out.rot.setI(temp.getI());
    out.rot.setJ(temp.getJ());
    out.rot.setK(temp.getK());

    return out;
  }
#undef FNAME

#define FNAME "convertToRotQuaternion(Matrix& in)"
  RotQuaternion convertToRotQuaternion(Matrix& in)
  {
    RotQuaternion out = RotQuaternion();

    if (!(in.rows() == 3 && in.cols() == 3))
      {
        throw IceException(FNAME, M_WRONG_MATRIXTYPE);

        return out;
      }

    double a = 0.5 * sqrt(fabs(1.0 + in[0][0] + in[1][1] + in[2][2]));

    if (a != 0)
      {
        double a4 = 4 * a;

        double b = in[2][1] - in[1][2];
        double c = in[0][2] - in[2][0];
        double d = in[1][0] - in[0][1];

        out.rot.setReal(a);
        out.rot.setI(b / a4);
        out.rot.setJ(c / a4);
        out.rot.setK(d / a4);

        return out;
      }

    double r1 = 0.5 * in[0][0] + 0.5;
    double r2 = 0.5 * in[0][1];
    double r3 = 0.5 * in[0][2];

    double temp = sqrt(r1 * r1 + r2 * r2 + r3 * r3);

    if (!(r1 == 0 && r2 == 0 && r3 == 0))
      {
        out.rot.setReal(0);
        out.rot.setI(r1 / temp);
        out.rot.setJ(r2 / temp);
        out.rot.setK(r3 / temp);

        return out;
      }

    r1 = 0.5 * in[1][0];
    r2 = 0.5 * in[1][1] + 0.5;
    r3 = 0.5 * in[1][2];

    temp = sqrt(r1 * r1 + r2 * r2 + r3 * r3);

    if (!(r1 == 0 && r2 == 0 && r3 == 0))
      {
        out.rot.setReal(0);
        out.rot.setI(r1 / temp);
        out.rot.setJ(r2 / temp);
        out.rot.setK(r3 / temp);

        return out;
      }

    r1 = 0.5 * in[2][0];
    r2 = 0.5 * in[2][1];
    r3 = 0.5 * in[2][2] + 0.5;

    temp = sqrt(r1 * r1 + r2 * r2 + r3 * r3);

    if (!(r1 == 0 && r2 == 0 && r3 == 0))
      {
        out.rot.setReal(0);
        out.rot.setI(r1 / temp);
        out.rot.setJ(r2 / temp);
        out.rot.setK(r3 / temp);

        return out;
      }

    throw IceException(FNAME, M_CONVERT_NOT_POSSIBLE);

    return out;
  }
#undef FNAME
}
