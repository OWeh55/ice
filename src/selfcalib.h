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

/****************************************************************************/
/* selfcalib.h  planare Selbstkalibrierung einer Kamera mit                 */
/*              einem festen Muster                                         */
/*                                                                          */
/* (C) Michael Schwarz 04/2007                                              */
/****************************************************************************/


#ifndef _SELFCALIB_H_
#define _SELFCALIB_H_

// Debug-Modus, wenn definiert, dann werden Zwischenschritte als
// Bilder gezeigt und Informationen in der Konsole ausgegeben
// #define selfcalib_debug

#include <vector>

#include "base.h"
#include "Matrix.h"
#include "Camera.h"

namespace ice
{
// Kalibrierung mit mindestens 3 Bildern,
// Ausgabe mit oder ohne externe Kameraparameter
// Berechnung mit oder ohne Entzeichnung
// im Debugmodus werden 2 Bilder zurückgegeben, die man mit Show(OVERLAY,...) übereinander legt

  int Calibrate(vector<Image>& images, Camera& c, bool dist = true,
                Image debug_image = Image(), Image debug_mark = Image());

  int Calibrate(vector<Image>& images, vector<Camera>& cv, bool dist = true,
                Image debug_image = Image(), Image debug_mark = Image());

// Zuweisung des Kalibriermusters aus einem Bild
// Berechnung mit oder ohne Entzeichnung
// im Debugmodus werden 2 Bilder zurückgegeben, die man mit Show(OVERLAY,...) übereinander legt
  int AssignCalibPattern(Image& image, Matrix& imagepoints, Matrix& worldpoints,
                         Image debug_image = Image(), Image debug_mark = Image());
  int AssignCalibPattern(Image& image, Matrix& imagepoints, Matrix& worldpoints, Camera& c,
                         Image debug_image = Image(), Image debug_mark = Image());

// Berechnung einer Homographie
  int ComputeHomography(Matrix& imagepoints, Matrix& worldpoints, Trafo& H);

// Kalibrierung mit mindestens 3 Referenz-Matrizen,
// Ausgabe mit oder ohne externe Kameraparameter
  int CalibrateWithPattern(vector<Matrix>& imagepoints, vector<Matrix>& worldpoints, Camera& c);
  int CalibrateWithPattern(vector<Matrix>& imagepoints, vector<Matrix>& worldpoints, vector<Camera>& cv);

// Kalibrierung mit mindestens 3 Homographien,
// Ausgabe mit oder ohne externe Kameraparameter
  int CalibrateWithHomographies(vector<Trafo>& H, Camera& c);
  int CalibrateWithHomographies(vector<Trafo>& H, vector<Camera>& cv);

  Matrix calib_intrinsic_zhang(const vector<Trafo>& H);

  Image DrawPattern(int dimx, int dimy);

// interne Kamerastruktur
  typedef struct _T_campar
  {
    ice::Matrix R;
    ice::Vector t;
    ice::Matrix K;
  }* CameraParameters;

// Eckpunkte der Quadrate - genau 50 Objekte im Muster
  const double pattern[50][8] =
  {
    {0.25, 0.25, 0.75, 0.25, 0.75, 0.75, 0.25, 0.75},
    {1.25, 0.25, 1.75, 0.25, 1.75, 0.75, 1.25, 0.75},
    {2.25, 0.25, 2.75, 0.25, 2.75, 0.75, 2.25, 0.75},
    {4.25, 0.25, 4.75, 0.25, 4.75, 0.75, 4.25, 0.75},
    {5.25, 0.25, 5.75, 0.25, 5.75, 0.75, 5.25, 0.75},
    {0.25, 1.25, 0.75, 1.25, 0.75, 1.75, 0.25, 1.75},
    {1.25, 1.25, 1.75, 1.25, 1.75, 1.75, 1.25, 1.75},
    {2.25, 1.25, 2.75, 1.25, 2.75, 1.75, 2.25, 1.75},
    {3.25, 1.25, 3.75, 1.25, 3.75, 1.75, 3.25, 1.75},
    {6.25, 1.25, 6.75, 1.25, 6.75, 1.75, 6.25, 1.75},
    {0.25, 2.25, 0.75, 2.25, 0.75, 2.75, 0.25, 2.75},
    {2.25, 2.25, 2.75, 2.25, 2.75, 2.75, 2.25, 2.75},
    {3.25, 2.25, 3.75, 2.25, 3.75, 2.75, 3.25, 2.75},
    {5.25, 2.25, 5.75, 2.25, 5.75, 2.75, 5.25, 2.75},
    {6.25, 2.25, 6.75, 2.25, 6.75, 2.75, 6.25, 2.75},
    {1.25, 3.25, 1.75, 3.25, 1.75, 3.75, 1.25, 3.75},
    {2.25, 3.25, 2.75, 3.25, 2.75, 3.75, 2.25, 3.75},
    {3.25, 3.25, 3.75, 3.25, 3.75, 3.75, 3.25, 3.75},
    {5.25, 3.25, 5.75, 3.25, 5.75, 3.75, 5.25, 3.75},
    {6.25, 3.25, 6.75, 3.25, 6.75, 3.75, 6.25, 3.75},
    {0.25, 4.25, 0.75, 4.25, 0.75, 4.75, 0.25, 4.75},
    {2.25, 4.25, 2.75, 4.25, 2.75, 4.75, 2.25, 4.75},
    {3.25, 4.25, 3.75, 4.25, 3.75, 4.75, 3.25, 4.75},
    {4.25, 4.25, 4.75, 4.25, 4.75, 4.75, 4.25, 4.75},
    {5.25, 4.25, 5.75, 4.25, 5.75, 4.75, 5.25, 4.75},
    {0.25, 5.25, 0.75, 5.25, 0.75, 5.75, 0.25, 5.75},
    {2.25, 5.25, 2.75, 5.25, 2.75, 5.75, 2.25, 5.75},
    {3.25, 5.25, 3.75, 5.25, 3.75, 5.75, 3.25, 5.75},
    {4.25, 5.25, 4.75, 5.25, 4.75, 5.75, 4.25, 5.75},
    {6.25, 5.25, 6.75, 5.25, 6.75, 5.75, 6.25, 5.75},
    {0.25, 6.25, 0.75, 6.25, 0.75, 6.75, 0.25, 6.75},
    {3.25, 6.25, 3.75, 6.25, 3.75, 6.75, 3.25, 6.75},
    {4.25, 6.25, 4.75, 6.25, 4.75, 6.75, 4.25, 6.75},
    {5.25, 6.25, 5.75, 6.25, 5.75, 6.75, 5.25, 6.75},
    {6.25, 6.25, 6.75, 6.25, 6.75, 6.75, 6.25, 6.75},
    {0.25, 7.25, 0.75, 7.25, 0.75, 7.75, 0.25, 7.75},
    {2.25, 7.25, 2.75, 7.25, 2.75, 7.75, 2.25, 7.75},
    {3.25, 7.25, 3.75, 7.25, 3.75, 7.75, 3.25, 7.75},
    {4.25, 7.25, 4.75, 7.25, 4.75, 7.75, 4.25, 7.75},
    {5.25, 7.25, 5.75, 7.25, 5.75, 7.75, 5.25, 7.75},
    {0.25, 8.25, 0.75, 8.25, 0.75, 8.75, 0.25, 8.75},
    {2.25, 8.25, 2.75, 8.25, 2.75, 8.75, 2.25, 8.75},
    {4.25, 8.25, 4.75, 8.25, 4.75, 8.75, 4.25, 8.75},
    {5.25, 8.25, 5.75, 8.25, 5.75, 8.75, 5.25, 8.75},
    {6.25, 8.25, 6.75, 8.25, 6.75, 8.75, 6.25, 8.75},
    {0.25, 9.25, 0.75, 9.25, 0.75, 9.75, 0.25, 9.75},
    {1.25, 9.25, 1.75, 9.25, 1.75, 9.75, 1.25, 9.75},
    {2.25, 9.25, 2.75, 9.25, 2.75, 9.75, 2.25, 9.75},
    {3.25, 9.25, 3.75, 9.25, 3.75, 9.75, 3.25, 9.75},
    {6.25, 9.25, 6.75, 9.25, 6.75, 9.75, 6.25, 9.75}
  };

// genau 50 Objekte
  const int CPOINTS = 50;

// Summe der Abstaende zwischen den Musterpunkten im Bild und
// den rueckprojizierten Punkten, die nicht ueberschritten werden darf
// fuer die meissten Bilder reicht 20.0
  const double CSUMABST = 60.0;
}

#endif
