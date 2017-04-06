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

#include "IceException.h"
#include "defs.h"
#include "macro.h"

#include "quatvector.h"

namespace ice
{

// Konstruktoren
  QuatVector::QuatVector()
  {
    dimension = 0;
    data = nullptr;
  }

#define FNAME "QuatVector::QuatVector(unsigned int dimension)"
  QuatVector::QuatVector(unsigned int dimension)
  {
    if (dimension > 0)
      {
        this->dimension = dimension;

        data = (Quaternion*)malloc(this->dimension * sizeof(Quaternion));

        if (data == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM);
          }
      }
    else
      {
        throw IceException(FNAME, M_VECTORDIM);
        data = nullptr;
        return;
      }

    for (int i = 0; i < int(dimension); i++)
      {
        data[i] = Quaternion(0, 0, 0, 0);
      }
  }
#undef FNAME

#define FNAME "QuatVector::QuatVector(const QuatVector &qv)"
  QuatVector::QuatVector(const QuatVector& qv)
  {
    this->dimension = qv.dimension;

    if (dimension > 0)
      {
        data = (Quaternion*)malloc(this->dimension * sizeof(Quaternion));

        if (data == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM);
          }
      }
    else
      {
        data = nullptr;
        return;
      }

    for (int i = 0; i < int(this->dimension); i++)
      {
        data[i] = qv.data[i];
      }
  }
#undef FNAME


  QuatVector& QuatVector::operator= (const QuatVector& qv)
  {
    if (this == &qv)
      {
        return *this;
      }

    if (dimension != qv.dimension)
      {
        if (data != nullptr)
          {
            free(data);
          }

        if (qv.dimension > 0)
          {
            data = (Quaternion*)malloc(qv.dimension * sizeof(Quaternion));

            if (data == nullptr)
              {
                throw IceException("operator=", M_NO_MEM);
              }
          }
        else
          {
            data = nullptr;
            return *this;
          }
      }

    this->dimension = qv.dimension;

    for (int i = 0; i < int(this->dimension); i++)
      {
        data[i] = qv.data[i];
      }

    return *this;
  }

  QuatVector::~QuatVector()
  {
    if (data != nullptr)
      {
        free(data);
      }
  }

  const Quaternion& QuatVector::operator[](int i) const
  {
    if ((i < 0) || (i >= int(dimension)))
      {
        throw IceException("operator[]", "Wrong index");
      }

    return data[i];
  }

  unsigned int QuatVector::getDimension() const
  {
    return this->dimension;
  }

  QuatVector operator+ (const QuatVector& qv1, const QuatVector& qv2)
  {
    QuatVector p(qv1.dimension);

    if (qv1.dimension != qv2.dimension)
      {
        throw IceException("Operator+", "Wrong dimension");
      }

    for (int i = 0; i < int(qv1.dimension); i++)
      {
        p.data[i] = qv1.data[i] + qv2.data[i];
      }

    return p;
  }

  QuatVector operator- (const QuatVector& qv1, const QuatVector& qv2)
  {
    QuatVector p(qv1.dimension);

    if (qv1.dimension != qv2.dimension)
      {
        throw IceException("Operator+", "Wrong dimension");
      }

    for (int i = 0; i < int(qv1.dimension); i++)
      {
        p.data[i] = qv1.data[i] - qv2.data[i];
      }

    return p;
  }

  QuatVector operator*(double k, const QuatVector& qv)
  {
    QuatVector p(qv.dimension);

    for (int i = 0; i < int(p.dimension); i++)
      {
        p.data[i] = k * qv.data[i];
      }

    return p;
  }

  QuatVector operator*(const QuatVector& qv, double k)
  {
    QuatVector p(qv.dimension);

    for (int i = 0; i < int(p.dimension); i++)
      {
        p.data[i] = k * qv.data[i];
      }

    return p;
  }

  QuatVector operator*(const Quaternion& q, const QuatVector& qv)
  {
    QuatVector p(qv.dimension);

    for (int i = 0; i < int(p.dimension); i++)
      {
        p.data[i] = q * qv.data[i];
      }

    return p;
  }

  QuatVector operator*(const QuatVector& qv, const Quaternion& q)
  {
    QuatVector p(qv.dimension);

    for (int i = 0; i < int(p.dimension); i++)
      {
        p.data[i] = qv.data[i] * q;
      }

    return p;
  }

  bool operator== (const QuatVector& qv1, const QuatVector& qv2)
  {
    if (qv2.dimension != qv1.dimension)
      {
        return false;
      }

    for (int i = 0; i < int(qv1.dimension); i++)
      {
        if (qv1[i] != qv2[i])
          {
            return false;
          }
      }

    return true;
  }

  bool operator!= (const QuatVector& qv1, const QuatVector& qv2)
  {
    return !(qv1 == qv2);
  }

  ostream& operator<<(ostream& os, const QuatVector& qv)
  {
    os << "<";

    if (qv.dimension > 0)
      {
        for (int i = 0; i < int(qv.dimension) - 1; i++)
          {
            os << qv.data[i] << ",";
          }

        os << qv.data[qv.dimension - 1];
      }

    os << ">";
    return os;
  }
}
