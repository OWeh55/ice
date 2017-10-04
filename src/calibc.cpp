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
#include "defs.h"
#include "IceException.h"

#include "Matrix.h"
#include "lmdif.h"
#include "Camera.h"
#include "calibc.h"

using namespace std;

namespace ice
{

  Matrix CalibPoint = Matrix(0, 3) &&
                      Vector(20.0,  60.0,   0.0) &&
                      Vector(100.0,  60.0,   0.0) &&
                      Vector(180.0,  60.0,   0.0) &&
                      Vector(260.0,  60.0,   0.0) &&
                      Vector(20.0, 140.0,   0.0) &&
                      Vector(100.0, 140.0,   0.0) &&
                      Vector(180.0, 140.0,   0.0) &&
                      Vector(260.0, 140.0,   0.0) &&
                      Vector(20.0, 220.0,   0.0) &&
                      Vector(100.0, 220.0,   0.0) &&
                      Vector(180.0, 220.0,   0.0) &&
                      Vector(260.0, 220.0,   0.0) &&
                      Vector(20.0,   0.0,  20.0) &&
                      Vector(20.0,   0.0, 100.0) &&
                      Vector(20.0,   0.0, 180.0) &&
                      Vector(20.0,   0.0, 260.0) &&
                      Vector(100.0,   0.0,  20.0) &&
                      Vector(100.0,   0.0, 100.0) &&
                      Vector(100.0,   0.0, 180.0) &&
                      Vector(100.0,   0.0, 260.0) &&
                      Vector(180.0,   0.0,  20.0) &&
                      Vector(180.0,   0.0, 100.0) &&
                      Vector(180.0,   0.0, 180.0) &&
                      Vector(180.0,   0.0, 260.0) &&
                      Vector(260.0,   0.0,  20.0) &&
                      Vector(260.0,   0.0, 100.0) &&
                      Vector(260.0,   0.0, 180.0) &&
                      Vector(260.0,   0.0, 260.0) &&
                      Vector(0.0,  60.0,  60.0) &&
                      Vector(0.0,  60.0, 140.0) &&
                      Vector(0.0,  60.0, 220.0) &&
                      Vector(0.0, 140.0,  60.0) &&
                      Vector(0.0, 140.0, 140.0) &&
                      Vector(0.0, 140.0, 220.0) &&
                      Vector(0.0, 220.0,  60.0) &&
                      Vector(0.0, 220.0, 140.0) &&
                      Vector(0.0, 220.0, 220.0)
                      ;

// Allgemeine Funktion zur Kamerakalibrierung

#define FNAME "Calib"
// Kalibrierung mittels Referenz-Punktpaaren

// Funktor für LMDif
  class CamDif: public LMFunctionObject
  {
  private:
// Referenzen auf die dem Konstruktor übergebenen Parameter
//
    Vector& camp;      // zu optimierende Kamera-Parameter
    const Matrix& xyz; // Raumpunkte
    const Matrix& uv;  // Bildpunkte
    int disttyp;
    int points;
  public:
    // Konstruktor
    // übernimmt (als Referenz) die zu optimierende Kamera
    // und die Punktreferenzen
    CamDif(Vector& cp, const Matrix& xyzp, const Matrix& uvp, int distt):
      camp(cp), xyz(xyzp), uv(uvp), disttyp(distt)
    {
      points = xyz.rows();

      if ((uv.rows() != points) || (xyz.cols() < 3) || (uv.cols() < 2))
        throw IceException(FNAME, M_WRONG_PARAM);
    };
    // Dimension der Fehlerfunktion
    int funcdim() const
    {
      return 2 * points;  // Zwei Fehlerwerte pro Referenz-Punktpaar
    }
    // Fehler-Funktion
    int operator()(Vector& result) const
    {
      Camera cam(camp, disttyp);
      int i, j;

      for (i = 0, j = 0; j < points; j++)   // für jeden Funktionswert
        {
          double us, vs;
          cam.transform(xyz.at(j).at(0), xyz.at(j).at(1), xyz.at(j).at(2), us, vs);
          result.at(i) = us - uv.at(j).at(0);
          i++;
          result.at(i) = vs - uv.at(j).at(1);
          i++;
        }

      return 1;
    }
  };

  int Calib(Camera& cam, const Matrix& xyz, const Matrix& uv, int mode)
  {
    int info = 0;
    IVector pnr(8, 9, 10, 5, 6, 7);
    pnr.Append(IVector(0, 3, 4));
    int dtyp = cam.DistType();

    if (mode & 1)
      {
        // Verzeichnung optimieren
        switch (dtyp)
          {
          case 1:
            pnr.Append(IVector(13, 14, 11, 12));
            break;
          case 2:
            pnr.Append(IVector(13, 14, 15, 11, 12));
            break;
          case 3:
            pnr.Append(IVector(13, 14, 15, 11, 12));
            break;
          }
      }

    if (mode & 2)
      {
        // Anisotropie und Scherung optimieren
        pnr.Append(IVector(1, 2));
      }

    Vector camp = cam.makeVector(Camera::all);

    vector<double*> op;  // Pointer auf zu optimierende Parameter

    for (int i = 0; i < pnr.Size(); i++)
      {
        op.push_back(&camp[pnr[i]]);

        info = LMDif(op,      // Liste von Zeigern auf zu optimierende Parameter
                     CamDif(camp, xyz, uv, cam.DistType()), // Konstruktion des Funktors
                     100000);    // maximale Zahl der Iterationsschritte

      }

    // im Moment wird nur der letzte Rückgabewert ausgewertet,
    // Fehler in Zwischenschritten werden ignoriert

    if (info > 3)
      {
        throw IceException(FNAME, "LMDif:" + LMDifMessage(info));
      }

    cam.set(camp);

    return OK;
  }
#undef FNAME
}
