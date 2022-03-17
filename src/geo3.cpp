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

#include "defs.h"
#include "macro.h"
#include "IceException.h"

#include "Trafo.h"
#include "contools.h"

#include "lmdif.h"
#include "Matrix.h"
#include "MatrixAlgebra.h"

#include "geo.h"

//--------------------------------------------
namespace ice
{
#define FNAME "MakeTransform"
  Trafo CTransform(Vector param)
  {
    // erzeugt eine 3D-2D-Trafo (Kamera)
    // Parameter in param
    Trafo c(3, 3); // Null-Transformation
    c.rotateZ(param[0]); // Rotation (Eulersche Winkel)
    c.rotateY(param[1]);
    c.rotateX(param[2]);
    c.shift(Vector(param[3], param[4], param[5]));
    c.projective();
    c.scale(0, 0, param[6], param[6]*param[7]);
    c.shearX(param[8]);
    c.shift(param[9], param[10]);
    return c;
  }

  Trafo CCTransform(Vector param)
  {
    // erzeugt eine 2D-2D-Trafo, wie sie zwischen 2
    // Bildern auftreten kann, die mit der gleichen Kamera
    // vom gleichen Punkt aus aufgenommen wurden

    Trafo R(3, 3); // "Externe Kamera-parameter"
    // translation entfällt
    R.rotateZ(param[0]); // Rotation (Eulersche Winkel)
    R.rotateY(param[1]);
    R.rotateX(param[2]);
    Matrix m = Matrix(R.getMatrix())(0, 0, 2, 2); // Rotationsmatrix

    Matrix c(3, 3); // Kamera-Matrix (interne Parameter)
    c[0][0] = param[3];
    c[0][1] = param[5];
    c[0][2] = param[6];
    c[1][0] = 0.0;
    c[1][1] = param[3] * param[4];
    c[1][2] = param[7];
    c[2][0] = 0.0;
    c[2][1] = 0.0;
    c[2][2] = 1.0;

    return Trafo(c * m * Inverse(c));
  }
#undef FNAME
#define FNAME "MatchPano"

  static int panofunc(const Vector& para, Vector& result)
  {
    // Optimierungsfunktion fuer LMDif
    // Ergebnis sind die Differenzen in x- und y-richtung beim
    // Anwenden der Transformation auf die Punktlisten
    int punkte = (result.Size()) / 2; // punktanzahl
    Trafo tr = CCTransform(para(0, 7));
    int pidx = 8;
    int ridx = 0;
    int i;
    double x1, y1; // Koordinaten Liste1
    double x2, y2; // Koordinaten Liste2
    double weight; // Gewicht des Paares
    double xt, yt; // Transformierter Punkt

    for (i = 0; i < punkte; i++)
      {
        x1 = para[pidx++];
        y1 = para[pidx++];
        x2 = para[pidx++];
        y2 = para[pidx++];
        weight = para[pidx++];
        transform(tr, x1, y1, xt, yt);
        result[ridx++] = (x2 - xt) * weight;
        result[ridx++] = (y2 - yt) * weight;
      }

    return 0;
  }

  Trafo MatchPano(const Matrix& p1, const Matrix& p2,
                  int mode, const Vector& weight)
  {
    // matcht zwei Punktlisten, die korrespondierende Punkte
    // zweier "Panorama"-Bilder enthalten
    // das sind Bilder, die mit der gleichen Kamera, vom gleichen Standpunkt
    // aufgenommen wurden. Erlaubt ist eine Drehung der Kamera

    Trafo res(2, 2);

    int points = p1.rows();

    if ((points != p2.rows()) || (points != weight.Size()))
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

    if ((p1.cols() != 2) || (p2.cols() != 2))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    Vector para;
    IVector opara;

    // Parametervektor für LMDif zusammensetzen
    // 1. Die Startwerte der Kamera-Parameter
    // externe Parameter (nur Drehung): alle Winkel=0
    int i;

    for (i = 0; i < 3; i++)
      {
        para.Append(0.0); // startwert 0.0
        opara.Append(i);  // optimieren
      }

    // "Brennweite" (fx)
    para.Append(1000.0);
    opara.Append(i);
    i++;
    // Anisotropie (fy/fx)
    para.Append(1.0);

    if (mode & PANO_ANISOTROP)
      {
        opara.Append(i);
      }

    i++;
    // Scherung
    para.Append(0.0);

    if (mode & PANO_SHEARING)
      {
        opara.Append(i);
      }

    i++;
    // Hauptpunkt (xh, yh)
    para.Append(0.0);
    opara.Append(i);
    i++;
    para.Append(0.0);
    opara.Append(i);
    i++;

    for (i = 0; i < points; i++)
      {
        // Punkt 1.Bild
        para.Append(p1[i][0]);
        para.Append(p1[i][1]);
        // Punkt 2.Bild
        para.Append(p2[i][0]);
        para.Append(p2[i][1]);
        // Gewicht
        para.Append(weight[i]);
      }

    int iter = 500000;
    int rc = LMDif(para, opara, panofunc, points * 2, iter);

    if (rc > 0 && rc < 5)
      {
        res = CCTransform(para(0, 7));
        return res;
      }

    throw IceException(FNAME, M_NUM_INSTABILITY);
  }

  Trafo MatchPano(const Matrix& p1, const Matrix& p2,
                  int mode)
  {
    Vector weights(p1.rows());
    weights.set(1.0);
    return MatchPano(p1, p2, mode, weights);
  }

#undef FNAME
}
