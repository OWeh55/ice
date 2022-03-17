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

#include "quaternion.h"
#include "rotquaternion.h"
#include "dualquaternion.h"
#include "trafodualquaternion.h"
#include "quatmatch.h"

#include "IceException.h"
#include "Matrix.h"
#include "Vector.h"
#include "mateigen.h"
#include "defs.h"

namespace ice
{

#define FNAME "estimateTransformation(const Matrix& orig,const Matrix& trans)"
  TrafoDualQuaternion estimateTransformation(const Matrix& orig, const Matrix& trans)
  {
    TrafoDualQuaternion out = TrafoDualQuaternion();

    if (!(orig.cols() == 3 && trans.cols() == 3))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    if (!(orig.rows() == trans.rows()))
      throw IceException(FNAME, M_WRONG_DIMENSION);

    int k = orig.rows();

    Matrix c1 = Matrix(4, 4, 0);

    Matrix c2 = Matrix(4, 4, 0);

    Matrix lxi = Matrix(4, 4, 0);

    Matrix myi = Matrix(4, 4, 0);

    double x1;
    double x2;
    double x3;

    double y1;
    double y2;
    double y3;

    for (int i = 0; i < k; i++)
      {
        x1 = orig[i][0];
        x2 = orig[i][1];
        x3 = orig[i][2];

        y1 = trans[i][0];
        y2 = trans[i][1];
        y3 = trans[i][2];

        lxi[0][1] = x1;
        lxi[0][2] = x2;
        lxi[0][3] = x3;

        lxi[1][0] = -x1;
        lxi[1][2] = -x3;
        lxi[1][3] = x2;

        lxi[2][0] = -x2;
        lxi[2][1] = x3;
        lxi[2][3] = -x1;

        lxi[3][0] = -x3;
        lxi[3][1] = -x2;
        lxi[3][2] = x1;

        myi[0][1] = y1;
        myi[0][2] = y2;
        myi[0][3] = y3;

        myi[1][0] = -y1;
        myi[1][2] = y3;
        myi[1][3] = -y2;

        myi[2][0] = -y2;
        myi[2][1] = -y3;
        myi[2][3] = y1;

        myi[3][0] = -y3;
        myi[3][1] = y2;
        myi[3][2] = -y1;

        c1 = c1 + lxi * (!myi);
        c2 = c2 + !(lxi - myi);
      }

    Matrix A = (1.0 / k) * (!c2) * c2 + 2.0 * c1;

    Matrix eval;
    Matrix evec;

    Eigenvalue(A, eval, evec);

    Vector x = Vector(evec[0][0], evec[1][0], evec[2][0], evec[3][0]);

    Quaternion r = Quaternion(x[0], x[1], x[2], x[3]);
    RotQuaternion rot = convertToRotQuaternion(r);

    Vector s = -0.5 / k * (c2 * x);

    Quaternion q2 = Quaternion(s[0], s[1], s[2], s[3]);
    Quaternion t = 2 * q2 * rot.getConjugate();

    Vector3d tr = Vector3d(t.getI(), t.getJ(), t.getK());

    out.setRotation(rot);
    out.setTranslation(tr);

    return out;
  }
#undef FNAME

#define FNAME "estimateTransformationWeighted(const Matrix& orig,const Matrix& trans,const Matrix &weights)"
  TrafoDualQuaternion estimateTransformationWeighted(const Matrix& orig, const Matrix& trans, const Matrix& weights)
  {
    TrafoDualQuaternion out = TrafoDualQuaternion();

    if (!(orig.cols() == 3 && trans.cols() == 3 && weights.cols() == 1))
      {
        throw IceException(FNAME, M_WRONG_DIMENSION);
      }

    if (!(orig.rows() == trans.rows() && orig.rows() == weights.rows()))
      {
        throw IceException(FNAME, M_WRONG_DIMENSION);
      }

    int k = orig.rows();

    Matrix c1 = Matrix(4, 4, 0);

    Matrix c2 = Matrix(4, 4, 0);

    Matrix c3 = Matrix(4, 4, 0);

    Matrix lxi = Matrix(4, 4, 0);

    Matrix myi = Matrix(4, 4, 0);

    double x1;
    double x2;
    double x3;

    double y1;
    double y2;
    double y3;

    double sum_weights = 0;

    for (int i = 0; i < k; i++)
      {
        double wi = weights[i][0];

        sum_weights += wi;

        x1 = orig[i][0];
        x2 = orig[i][1];
        x3 = orig[i][2];

        y1 = trans[i][0];
        y2 = trans[i][1];
        y3 = trans[i][2];

        lxi[0][1] = x1;
        lxi[0][2] = x2;
        lxi[0][3] = x3;

        lxi[1][0] = -x1;
        lxi[1][2] = -x3;
        lxi[1][3] = x2;

        lxi[2][0] = -x2;
        lxi[2][1] = x3;
        lxi[2][3] = -x1;

        lxi[3][0] = -x3;
        lxi[3][1] = -x2;
        lxi[3][2] = x1;

        myi[0][1] = y1;
        myi[0][2] = y2;
        myi[0][3] = y3;

        myi[1][0] = -y1;
        myi[1][2] = y3;
        myi[1][3] = -y2;

        myi[2][0] = -y2;
        myi[2][1] = -y3;
        myi[2][3] = y1;

        myi[3][0] = -y3;
        myi[3][1] = y2;
        myi[3][2] = -y1;

        c1 = c1 + wi * (lxi * (!myi));
        c2 = c2 + wi * !(lxi - myi);
      }

    for (int i = 0; i < 4; i++)
      {
        c3[i][i] = 0.5 / sum_weights;  //in fact, this is (c3 + !c3)^(-1)
      }

    //Matrix A = (1.0/double(k))*(!c2)*c2 + 2*c1;
    Matrix A = 2.0 * (!c2) * c3 * c2 + c1 + (!c1);

    Matrix eval;
    Matrix evec;

    Eigenvalue(A, eval, evec);

    Vector x = Vector(evec[0][0], evec[1][0], evec[2][0], evec[3][0]);

    Quaternion r = Quaternion(x[0], x[1], x[2], x[3]);
    RotQuaternion rot = convertToRotQuaternion(r);

    //Vector s = -0.5/k*(c2*x);
    Vector s = -c3 * (c2 * x);

    Quaternion q2 = Quaternion(s[0], s[1], s[2], s[3]);
    Quaternion t = 2 * q2 * rot.getConjugate();

    Vector3d tr = Vector3d(t.getI(), t.getJ(), t.getK());

    out.setRotation(rot);
    out.setTranslation(tr);

    return out;
  }
#undef FNAME
}
