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

#include "macro.h"
#include "momente.h"
#include "lists.h"
#include "affinfit.h"
#include "fitmoments.h"

namespace ice
{
#define FNAME "AffineFit"
  Trafo AffineFit(const Moments& m1, const Moments& m2)
  {
    double tr[3][3];
    AffinFitMoments(m1.oldMoments(), m2.oldMoments(), tr);

    Matrix trm(3, 3);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        {
          trm[i][j] = tr[i][j];
        }

    return Trafo(trm);
  }

#undef FNAME
#define FNAME "FitTriangle"
  Matrix FitTriangle(const Moments& m)
  {
    // normalisiertes Dreieck

    double d[] =
    {
      -1.3642616, 0.7876568,
        0.0000000, -1.5753136,
        1.3642616, 0.7876568
      };

    // als Punktliste
    Matrix tri(3, 2, d);

    // Momente mit pos Vorzeichen der Fläche
    Moments mx = m.NormalizeSign();

    if (mx(0, 0) < EPSILON)
      throw IceException(FNAME, M_EMPTY_OBJECT);

    Trafo tr;
    // trafo in die Standardlage ermitteln
    mx.Normalize(tr, Moments::polynom);

    // Trafo umkehren
    tr.invert();

    // und Standard-Dreieck transformieren
    transformList(tr, tri);

    return tri;
  }
#undef FNAME
#define FNAME "FitEquilateralTriangle"
  Matrix FitEquilateralTriangle(const Moments& m)
  {
    double dummy;
    return FitEquilateralTriangle(m, dummy);
  }

  Matrix FitEquilateralTriangle(const Moments& m, double& guetemass)
  {
    try
      {
        Matrix triangle(0, 2);
        double p[3][2];
        FitEquilateraltriangleMoments(m.oldMoments(), p, guetemass);

        for (int i = 0; i < 3; i++)
          {
            triangle.append(Vector(p[i][0], p[i][1]));
          }

        return triangle;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "FitEquilateraltriangle"
  Matrix FitIsoscelesTriangle(const Moments& m)
  {
    double dummy;
    return FitIsoscelesTriangle(m, dummy);
  }
  Matrix FitIsoscelesTriangle(const Moments& m, double& guetemass)
  {
    try
      {
        Matrix triangle(0, 2);
        double p[3][2];
        FitIsoscelestriangleMoments(m.oldMoments(), p, guetemass);

        for (int i = 0; i < 3; i++)
          {
            triangle.append(Vector(p[i][0], p[i][1]));
          }

        return triangle;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "FitParallelogram"
  Matrix FitParallelogram(const Moments& m)
  {
    double dummy;
    return FitParallelogram(m, dummy);
  }
#undef FNAME

#define FNAME "FitParallelogram"
  Matrix FitParallelogram(const Moments& m, double& guetemass)
  {
    Matrix par(4, 2);
    double p[4][2];
    int  i;
    FitParallelogramMoments(m.oldMoments(), p, guetemass);

    for (i = 0; i < 4; i++)
      {
        par[i][0] = p[i][0];
        par[i][1] = p[i][1];
      }

    return par;
  }
#undef FNAME

#define FNAME "FitSquare"
  Matrix FitSquare(const Moments& m)
  {
    double dummy;
    return FitSquare(m, dummy);
  }

  Matrix FitSquare(const Moments& m, double& guetemass)
  {
    Matrix par(4, 2);
    double p[4][2];
    int  i;
    FitSquareMoments(m.oldMoments(), p, guetemass);

    for (i = 0; i < 4; i++)
      {
        par[i][0] = p[i][0];
        par[i][1] = p[i][1];
      }

    return par;
  }
#undef FNAME

#define FNAME "FitRectangle"
  Matrix FitRectangle(const Moments& m)
  {
    double dummy;
    return FitRectangle(m, dummy);
  }

  Matrix FitRectangle(const Moments& m, double& guetemass)
  {
    Matrix rec(4, 2);
    double p[4][2];
    int  i;
    FitRectangleMoments(m.oldMoments(), p, guetemass);

    for (i = 0; i < 4; i++)
      {
        rec[i][0] = p[i][0];
        rec[i][1] = p[i][1];
      }

    return rec;

  }
#undef FNAME
#define FNAME "FitPolygon"
  Matrix FitPolygon(const Moments& m, const Matrix& pl)
  {
    double dummy;
    return FitPolygon(m, pl, dummy);
  }

  Matrix FitPolygon(const Moments& m, const Matrix& pl, double& guetemass)
  {
    try
      {
        Matrix res(0, 2);
        CheckPolygon(pl);

        int n = pl.rows();

        if (n > 7)
          throw IceException(FNAME, M_MATRIXFORMAT);

        double pstart[7][2]; // initial solution

        for (int i = 0; i < n; i++)
          {
            pstart[i][0] = pl[i][0];
            pstart[i][1] = pl[i][1];
          }

        double p[7][2]; // result

        FitPolygonMoments(n, m.oldMoments(), pstart, p, guetemass);

        for (int i = 0; i < n; i++)
          {
            res.append(Vector(p[i][0], p[i][1]));
          }

        return res;
      }
    RETHROW;
  }

  Polygon FitPolygon(const Moments& m, const Polygon& pl, double& guetemass)
  {
    Polygon res(pl);

    int n = pl.size();

    if (n > 7)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    double pstart[7][2]; // initial solution

    for (int i = 0; i < n; i++)
      {
        pstart[i][0] = pl[i].x;
        pstart[i][1] = pl[i].y;
      }

    double p[7][2]; // result

    try
      {
        FitPolygonMoments(n, m.oldMoments(), pstart, p, guetemass);

        res.Reset(Point(p[0][0], p[0][1]));
        for (int i = 1; i < n; i++)
          {
            res.add(Vector(p[i][0], p[i][1]));
          }

        return res;
      }
    RETHROW;

  }

  Polygon FitPolygon(const Moments& m, const Polygon& pl)
  {
    double dummy;
    return FitPolygon(m, pl, dummy);
  }

#undef FNAME

#define FNAME "FitCircle"
  int FitCircle(const Moments& m, double& x0, double& y0, double& radius)
  {
    RETURN_ERROR_IF_FAILED(FitCircleMoments(m.oldMoments(), x0, y0, radius));
    return OK;
  }

  int FitCircle(const Moments& m, double& x0, double& y0, double& radius, double& guetemass)
  {
    RETURN_ERROR_IF_FAILED(FitCircleMoments(m.oldMoments(), x0, y0, radius, guetemass));
    return OK;
  }

  Circle FitCircle(const Moments& m)
  {
    double x0, y0, rad;
    FitCircleMoments(m.oldMoments(), x0, y0, rad);
    return Circle(x0, y0, rad);
  }
  Circle FitCircle(const Moments& m, double& guetemass)
  {
    double x0, y0, rad;
    FitCircleMoments(m.oldMoments(), x0, y0, rad, guetemass);
    return Circle(x0, y0, rad);
  }
#undef FNAME
#define FNAME "FitEllipse"
  Ellipse FitEllipse(const Moments& m)
  {
    double elpar[5];
    FitEllipseMoments(m.oldMoments(), elpar);
    return Ellipse(elpar[0], elpar[1], elpar[2], elpar[3], elpar[4]);
  }
#undef FNAME
#define FNAME "Orientation"
  double Orientation(const Moments& m)
  {
    try
      {
        double fi;
        OrientationMoments(m.oldMoments(), fi);
        return fi;
      }
    RETHROW;
  }
#undef FNAME

}
