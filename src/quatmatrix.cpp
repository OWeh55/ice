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

#include "message.h"
#include "defs.h"

#include "quatmatrix.h"

namespace ice
{
// Konstruktoren

  QuatMatrix::QuatMatrix()
  {
    rows = 0;
    columns = 0;
    data = nullptr;
  }

#define FNAME "QuatMatrix::QuatMatrix(unsigned int rows, unsigned int columns)"
  QuatMatrix::QuatMatrix(unsigned int rows, unsigned int columns)
  {
    this->rows = rows;
    this->columns = columns;

    if (rows > 0 && columns > 0)
      {
        data = (QuatVector**)malloc(rows * sizeof(QuatVector*));

        if (data == nullptr)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return;
          }
      }
    else
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_MATRIX);
        data = nullptr;
        return;
      }

    for (int i = 0; i < int(rows); i++)
      {
        data[i] = new QuatVector(columns);
      }
  }
#undef FNAME

#define FNAME "QuatMatrix::QuatMatrix(const QuatMatrix &qm)"
  QuatMatrix::QuatMatrix(const QuatMatrix& qm)
  {
    rows = qm.rows;
    columns = qm.columns;

    if (rows > 0 && columns > 0)
      {
        data = (QuatVector**)malloc(rows * sizeof(QuatVector*));

        if (data == nullptr)
          {
            Message(FNAME, M_NO_MEM, NO_MEM);
            return;
          }
      }
    else
      {
        data = nullptr;
        return;
      }

    for (int i = 0; i < int(rows); i++)
      {
        data[i] = new QuatVector(*qm.data[i]);
      }
  }

  QuatMatrix& QuatMatrix::operator= (const QuatMatrix& qm)
  {
    if (&qm == this)
      {
        return *this;
      }

    if (data != nullptr)
      {
        for (int i = 0; i < int(rows); i++)
          {
            delete data[i];
          }

        free(data);
      }

    rows = qm.rows;
    columns = qm.columns;

    if (rows > 0 && columns > 0)
      {
        data = (QuatVector**)malloc(rows * sizeof(QuatVector*));

        if (data == nullptr)
          {
            Message("operator=", M_NO_MEM, NO_MEM);
            return *this;
          }
      }
    else
      {
        data = nullptr;
        return *this;
      }

    for (int i = 0; i < int(rows); i++)
      {
        data[i] = new QuatVector(*qm.data[i]);
      }

    return *this;
  }

  QuatMatrix::~QuatMatrix()
  {
    if (data != nullptr)
      {
        for (int i = 0; i < int(rows); i++)
          {
            delete data[i];
          }

        free(data);
      }
  }

  unsigned int QuatMatrix::getRows() const
  {
    return rows;
  }

  unsigned int QuatMatrix::getColumns() const
  {
    return columns;
  }


  const QuatVector& QuatMatrix::operator[](int i) const
  {
    if ((i < 0) || (i >= int(rows)))
      {
        Message("operator[]", "Wrong index", WRONG_PARAM);
        return *data[0];
      }

    return *data[i];
  }

  QuatVector& QuatMatrix::operator[](int i)
  {
    if ((i < 0) || (i >= int(rows)))
      {
        Message("operator[]", "Wrong index", WRONG_PARAM);
        return *data[0];
      }

    return *data[i];
  }

  QuatMatrix operator+(const QuatMatrix& qm1, const QuatMatrix& qm2)
  {
    if ((qm1.rows != qm2.rows) || (qm1.columns != qm2.columns))
      {
        Message("operator+", "Format doesn't match", WRONG_PARAM);
        return qm1;
      }

    QuatMatrix res(qm1.rows, qm1.columns);

    for (int i = 0; i < int(qm1.rows); i++)
      {
        *res.data[i] = *qm1.data[i] + *qm2.data[i];
      }

    return res;
  }

  QuatMatrix operator-(const QuatMatrix& qm1, const QuatMatrix& qm2)
  {
    if ((qm1.rows != qm2.rows) || (qm1.columns != qm2.columns))
      {
        Message("operator+", "Format doesn't match", WRONG_PARAM);
        return qm1;
      }

    QuatMatrix res(qm1.rows, qm1.columns);

    for (int i = 0; i < int(qm1.rows); i++)
      {
        *res.data[i] = *qm1.data[i] - *qm2.data[i];
      }

    return res;
  }

  QuatMatrix operator*(const double k, const QuatMatrix& qm)
  {
    QuatMatrix res(qm.rows, qm.columns);

    for (int i = 0; i < int(qm.rows); i++)
      {
        *res.data[i] = k** qm.data[i];
      }

    return res;
  }

  QuatMatrix operator*(const QuatMatrix& qm, const double k)
  {
    QuatMatrix res(qm.rows, qm.columns);

    for (int i = 0; i < int(qm.rows); i++)
      {
        *res.data[i] = *qm.data[i] * k;
      }

    return res;
  }

  QuatMatrix operator*(const Quaternion& q, const QuatMatrix& qm)
  {
    QuatMatrix res(qm.rows, qm.columns);

    for (int i = 0; i < int(qm.rows); i++)
      {
        *res.data[i] = q** qm.data[i];
      }

    return res;
  }

  QuatMatrix operator*(const QuatMatrix& qm, const Quaternion& q)
  {
    QuatMatrix res(qm.rows, qm.columns);

    for (int i = 0; i < int(qm.rows); i++)
      {
        *res.data[i] = *qm.data[i] * q;
      }

    return res;
  }

  bool operator==(const QuatMatrix& qm1, const QuatMatrix& qm2)
  {
    if (qm1.rows != qm2.rows)
      {
        return false;
      }

    if (qm2.columns != qm2.columns)
      {
        return false;
      }

    for (int i = 0; i < int(qm1.rows); i++)
      {
        if (*qm1.data[i] != *qm2.data[i])
          {
            return false;
          }

      }

    return true;
  }

  bool operator!=(const QuatMatrix& qm1, const QuatMatrix& qm2)
  {
    return !(qm1 == qm2);
  }

  ostream& operator<<(ostream& os, const QuatMatrix& qm)
  {
    os << "<" << endl ;

    for (int i = 0; i < int(qm.rows); i++)
      {
        os << *qm.data[i];

        if (i < int(qm.rows) - 1)
          {
            os << ',' ;
          }

        os << endl ;
      }

    os << ">" << endl ;

    return os;
  }

}
