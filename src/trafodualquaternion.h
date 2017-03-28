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

#ifndef TRAFODUALQUATERNION_H
#define TRAFODUALQUATERNION_H

#include "Matrix.h"

#include "quaternion.h"
#include "rotquaternion.h"
#include "dualquaternion.h"


using namespace std;

namespace ice
{

  class TrafoDualQuaternion
  {
    friend TrafoDualQuaternion convertToTrafoDualQuaternion(const Matrix& in);

    friend bool operator==(const TrafoDualQuaternion& a,
                           const TrafoDualQuaternion& b);

    friend bool operator!=(const TrafoDualQuaternion& a,
                           const TrafoDualQuaternion& b);

    friend bool operator==(const TrafoDualQuaternion& a,
                           const DualQuaternion& b);

    friend bool operator==(const DualQuaternion& a,
                           const TrafoDualQuaternion& b);

    friend bool operator!=(const TrafoDualQuaternion& a,
                           const DualQuaternion& b);

    friend bool operator!=(const DualQuaternion& a,
                           const TrafoDualQuaternion& b);

    friend DualQuaternion operator+(const TrafoDualQuaternion& x,
                                    const TrafoDualQuaternion& y);

    friend DualQuaternion operator+(const TrafoDualQuaternion& x,
                                    const DualQuaternion& y);

    friend DualQuaternion operator+(const DualQuaternion& x,
                                    const TrafoDualQuaternion& y);

    friend DualQuaternion operator-(const TrafoDualQuaternion& x,
                                    const TrafoDualQuaternion& y);

    friend DualQuaternion operator-(const TrafoDualQuaternion& x,
                                    const DualQuaternion& y);

    friend DualQuaternion operator-(const DualQuaternion& x,
                                    const TrafoDualQuaternion& y);

    friend TrafoDualQuaternion operator-(TrafoDualQuaternion& x);

    friend DualQuaternion operator*(const TrafoDualQuaternion& x,
                                    const double y);

    friend DualQuaternion operator*(const double x,
                                    const TrafoDualQuaternion& y);

    friend DualQuaternion operator/(const TrafoDualQuaternion& x,
                                    const double y);

    friend TrafoDualQuaternion operator*(const TrafoDualQuaternion& x,
                                         const TrafoDualQuaternion& y);

    friend DualQuaternion operator*(const TrafoDualQuaternion& x,
                                    const DualQuaternion& y);

    friend DualQuaternion operator*(const DualQuaternion& x,
                                    const TrafoDualQuaternion& y);

    friend ostream& operator<<(ostream& ost, const TrafoDualQuaternion& x);

  public:
    TrafoDualQuaternion();
    TrafoDualQuaternion(RotQuaternion& rot, Vector3d& translation);
    TrafoDualQuaternion(RotQuaternion& rot, double x, double y, double z);

    ~TrafoDualQuaternion();

    void setRotation(RotQuaternion& rot);
    void setTranslation(Vector3d& trans);

    Matrix getTransformationMatrix();

    Quaternion getReal()
    {
      return trans.real;
    };
    Quaternion getDual()
    {
      return trans.dual;
    };

    TrafoDualQuaternion getNegate();
    TrafoDualQuaternion getConjugate();
    TrafoDualQuaternion getTilde();
    TrafoDualQuaternion getInverse();

    TrafoDualQuaternion& operator*=(const TrafoDualQuaternion& b);

    operator DualQuaternion() const;

  protected:
    DualQuaternion trans;
  };

  inline bool operator==(const TrafoDualQuaternion& a, const TrafoDualQuaternion& b)
  {
    return (a.trans == b.trans);
  }

  inline bool operator!=(const TrafoDualQuaternion& a, const TrafoDualQuaternion& b)
  {
    return !(a == b);
  }

  inline bool operator==(const TrafoDualQuaternion& a, const DualQuaternion& b)
  {
    return (a.trans == b);
  }

  inline bool operator==(const DualQuaternion& a, const TrafoDualQuaternion& b)
  {
    return (a == b.trans);
  }

  inline bool operator!=(const TrafoDualQuaternion& a, const DualQuaternion& b)
  {
    return !(a == b);
  }

  inline bool operator!=(const DualQuaternion& a, const TrafoDualQuaternion& b)
  {
    return !(a == b);
  }

  inline DualQuaternion operator+(const TrafoDualQuaternion& x,
                                  const TrafoDualQuaternion& y)
  {
    return x.trans + y.trans;
  }

  inline DualQuaternion operator+(const TrafoDualQuaternion& x,
                                  const DualQuaternion& y)
  {
    return x.trans + y;
  }

  inline DualQuaternion operator+(const DualQuaternion& x,
                                  const TrafoDualQuaternion& y)
  {
    return x + y.trans;
  }

  inline TrafoDualQuaternion operator-(TrafoDualQuaternion& x)
  {
    return x.getNegate();
  }

  inline DualQuaternion operator-(const TrafoDualQuaternion& x,
                                  const TrafoDualQuaternion& y)
  {
    return x.trans - y.trans;
  }

  inline DualQuaternion operator-(const TrafoDualQuaternion& x,
                                  const DualQuaternion& y)
  {
    return x.trans - y;
  }

  inline DualQuaternion operator-(const DualQuaternion& x,
                                  const TrafoDualQuaternion& y)
  {
    return x - y.trans;
  }

  inline DualQuaternion operator*(const TrafoDualQuaternion& x, const double y)
  {
    return x.trans * y;
  }

  inline DualQuaternion operator*(const double x, const TrafoDualQuaternion& y)
  {
    return x * y.trans;
  }

  inline DualQuaternion operator/(const TrafoDualQuaternion& x, const double y)
  {
    return x.trans / y;
  }

  inline TrafoDualQuaternion operator*(const TrafoDualQuaternion& x,
                                       const TrafoDualQuaternion& y)
  {
    TrafoDualQuaternion a;
    a.trans = x.trans * y.trans;

    return a;
  }

  inline DualQuaternion operator*(const TrafoDualQuaternion& x,
                                  const DualQuaternion& y)
  {
    return x.trans * y;
  }

  inline DualQuaternion operator*(const DualQuaternion& x,
                                  const TrafoDualQuaternion& y)
  {
    return x * y.trans;
  }

  inline ostream& operator<<(ostream& ost, const TrafoDualQuaternion& x)
  {
    return ost << x.trans;
  }

  TrafoDualQuaternion convertToTrafoDualQuaternion(const Matrix& in);

}
#endif
