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

#include "quaternion.h"
#include "dualquaternion.h"

#include "defs.h"
#include "message.h"

namespace ice
{

  DualQuaternion :: DualQuaternion()
  {
    real = Quaternion();
    dual = Quaternion();
  }

  DualQuaternion :: DualQuaternion(const Quaternion& real, const Quaternion& dual)
  {
    this->real = real;
    this->dual = dual;
  }

  void DualQuaternion :: setReal(const Quaternion& real)
  {
    this->real = real;
  }

  void DualQuaternion :: setDual(const Quaternion& dual)
  {
    this->dual = dual;
  }

  DualQuaternion DualQuaternion :: getQuasiNorm()
  {
    return (*this) * getConjugate();
  }

  bool DualQuaternion :: isUnitDualQuaternion()
  {
    if (real.getSquareNorm() == 1 && dual.getSquareNorm() == 0)
      {
        return true;
      }

    return false;
  }

  DualQuaternion DualQuaternion :: getNegate()
  {
    Quaternion nreal = (*this).real.getNegate();
    Quaternion ndual = (*this).dual.getNegate();

    return DualQuaternion(nreal, ndual);
  }

  DualQuaternion DualQuaternion :: getConjugate()
  {
    Quaternion nreal = (*this).real.getConjugate();
    Quaternion ndual = (*this).dual.getConjugate();

    return DualQuaternion(nreal, ndual);
  }

#define FNAME "DualQuaternion :: getInverse()"
  DualQuaternion DualQuaternion :: getInverse()
  {
    Quaternion nreal = (*this).real.getInverse();

    if (GetError() == NO_QUATERNIONINVERSE)
      {
        Message(FNAME, M_NO_DUALQUATERNIONINVERSE, NO_DUALQUATERNIONINVERSE);
        return (*this);
      }

    Quaternion ndual = real * dual * nreal;

    return DualQuaternion(nreal, ndual);
  }
#undef FNAME

  DualQuaternion DualQuaternion :: getTilde()
  {
    Quaternion nreal = (*this).real;
    Quaternion ndual = (*this).real.getNegate();

    return DualQuaternion(nreal, ndual);
  }

  DualQuaternion& DualQuaternion :: operator+=(const DualQuaternion& b)
  {
    real += b.real;
    dual += b.dual;

    return *this;
  }

  DualQuaternion& DualQuaternion :: operator-=(const DualQuaternion& b)
  {
    real -= b.real;
    dual -= b.dual;

    return *this;
  }

  DualQuaternion& DualQuaternion :: operator*=(const double b)
  {
    real *= b;
    dual *= b;

    return *this;
  }

  DualQuaternion& DualQuaternion :: operator*=(const DualQuaternion& b)
  {
    Quaternion r1 = this->real;
    Quaternion d1 = this->dual;

    real = r1 * b.real;
    dual = r1 * b.dual + d1 * b.real;

    return *this;
  }

  bool operator==(const DualQuaternion& a, const DualQuaternion& b)
  {
    return (a.real == b.real && a.dual == b.dual);
  }

  bool operator!=(const DualQuaternion& a, const DualQuaternion& b)
  {
    return !(a == b);
  }

  DualQuaternion operator-(DualQuaternion& x)
  {
    DualQuaternion temp;

    return temp;
  }

  ostream& operator<<(ostream& ost, const DualQuaternion& x)
  {
    return ost << "(" << x.real << ") + (" << x.dual << ")Epsilon";
  }
}

