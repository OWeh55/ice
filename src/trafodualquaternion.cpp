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


/*
 * Author: Alexander Lärz, 2005
 */

#include "defs.h"
#include "IceException.h"

#include "Matrix.h"
#include "Vector.h"

#include "rotquaternion.h"
#include "dualquaternion.h"
#include "trafodualquaternion.h"


namespace ice
{

  TrafoDualQuaternion :: TrafoDualQuaternion()
  {
  }

  TrafoDualQuaternion :: TrafoDualQuaternion(RotQuaternion& rot, Vector3d& trans)
  {
    this->trans.real = rot;
    this->trans.dual = 0.5 * Quaternion(0, trans) * rot;;
  }

  TrafoDualQuaternion :: TrafoDualQuaternion(RotQuaternion& rot, double x, double y, double z)
  {
    this->trans.real = rot;
    this->trans.dual = 0.5 * Quaternion(0, x, y, z) * rot;
  }

  TrafoDualQuaternion :: ~TrafoDualQuaternion()
  {
  }

  void TrafoDualQuaternion :: setRotation(RotQuaternion& rot)
  {
    Quaternion t = 2 * trans.dual * trans.real.getConjugate();

    trans.real = rot;
    trans.dual = 0.5 * t * rot;
  }

  void TrafoDualQuaternion :: setTranslation(Vector3d& trans)
  {
    this->trans.dual = 0.5 * Quaternion(0, trans) * this->trans.real;
  }

  TrafoDualQuaternion TrafoDualQuaternion :: getNegate()
  {
    Quaternion real = (*this).trans.real.getNegate();
    Quaternion dual = (*this).trans.dual.getNegate();

    TrafoDualQuaternion out = TrafoDualQuaternion();
    out.trans.real = real;
    out.trans.dual = dual;

    return out;
  }

  TrafoDualQuaternion TrafoDualQuaternion :: getConjugate()
  {
    Quaternion real = (*this).trans.real.getConjugate();
    Quaternion dual = (*this).trans.dual.getConjugate();

    TrafoDualQuaternion out = TrafoDualQuaternion();
    out.trans.real = real;
    out.trans.dual = dual;

    return out;
  }

  TrafoDualQuaternion TrafoDualQuaternion :: getInverse()
  {
    Quaternion real = trans.real.getConjugate();
    Quaternion dual = real * trans.dual * real;

    TrafoDualQuaternion out = TrafoDualQuaternion();

    out.trans.real = real;
    out.trans.dual = dual.getNegate();

    return out;
  }

  TrafoDualQuaternion TrafoDualQuaternion :: getTilde()
  {
    Quaternion real = (*this).trans.real;
    Quaternion dual = (*this).trans.dual.getNegate();

    TrafoDualQuaternion out = TrafoDualQuaternion();

    out.trans.real = real;
    out.trans.dual = dual;

    return out;
  }

#define FNAME "TrafoDualQuaternion :: getTransformationMatrix()"
  Matrix TrafoDualQuaternion :: getTransformationMatrix()
  {
    Matrix transm = Matrix(4, 4, 0);

    RotQuaternion rot = convertToRotQuaternion(trans.real);
    Matrix rotm = rot.getRotationMatrix();

    Quaternion t = 2 * trans.dual * trans.real.getConjugate();

    Vector transv = Vector(t.getI(), t.getJ(), t.getK(), 1);

    transm = (rotm && Vector(0, 0, 0)) || transv;

    return transm;
  }
#undef FNAME

  TrafoDualQuaternion& TrafoDualQuaternion :: operator*=(const TrafoDualQuaternion& b)
  {
    trans *= b.trans;

    return (*this);
  }

  TrafoDualQuaternion :: operator DualQuaternion() const
  {
    return (*this).trans;
  }

#define FNAME "convertToTrafoDualQuaternion( Matrix& in)"
  TrafoDualQuaternion convertToTrafoDualQuaternion(const Matrix& in)
  {
    TrafoDualQuaternion out = TrafoDualQuaternion();

    if (!(in.rows() == 4 && in.cols() == 4))
      {
        throw IceException(FNAME, M_MATRIXFORMAT);
      }

    Matrix rotm = in(0, 0, 2, 2);

    Quaternion rot = convertToRotQuaternion(rotm);

    Quaternion dual = 0.5 * Quaternion(0, in[0][3], in[1][3], in[2][3]) * rot;

    out.trans.setReal(rot);
    out.trans.setDual(dual);

    return out;
  }
#undef FNAME
}
