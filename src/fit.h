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
 *  Apassung von Konturen/Punktlisten an elementare Funktionen
 *      (Gerad, Kreis, Ellipse, quadrat. Formen)
 *    Approximation von Punktlisten an Segmente
 *      (Geradensegment,Kreissegment,Ellipsensegment)
 */
#ifndef _FIT_H
#define _FIT_H

#include <vector>
#include "Contur.h"
#include "Matrix.h"

#include "lineseg.h"
#include "circle.h"
#include "ellipse.h"

namespace ice
{

//  result: LineSeg
  LineSeg FitLine(const std::vector<Point>& pl);
  LineSeg FitLine(const std::vector<Point>& pl, const std::vector<double>& weight);
  LineSeg FitLine(const std::vector<Point>& pl, int n);

  LineSeg FitLine(const Matrix& pl, int step = 0);

// Objekte als Punktliste (Matrix) gegeben
  int FitLine(const Matrix& pl, double* par, int step = 0);
  int FitLine(const Matrix& pl, double& p, double& fi, int step = 0);

  Circle FitCircle(const std::vector<Point>& pl);
  Circle FitCircle(const std::vector<Point>& pl, const std::vector<double>& weight);
  Circle FitCircle(const std::vector<Point>& pl, int n);

// compatibility
  int FitCircle(const Matrix& pl, double* par, int step = 0);
  int FitCircle(const Matrix& pl, double& xm, double& ym, double& rad, int step = 0);

  Circle FitCircle(const Matrix& pl, int step = 0);

  // parameter vector, result double*
  int FitEllipse(const std::vector<Point> pl, std::vector<double> wv,
                 double* par, int step = 0, int mode = 1);
  int FitEllipse(const std::vector<Point>& pl,
                 double* par, int step = 0, int mode = 1);
  // parameter vector, result Ellipse
  Ellipse FitEllipse(const std::vector<Point>& pl, std::vector<double> wv,
                     int step = 0, int mode = 1);
  Ellipse FitEllipse(const std::vector<Point>& pl, int step = 0, int mode = 1);

  // parameter Matrix, result double*
  int FitEllipse(const Matrix& pl, double* par, int step = 0, int mode = 1);
  // parameter Matrix, result Ellipse
  Ellipse FitEllipse(const Matrix& pl, int step = 0, int mode = 1);

  /******************************************************************/
  void Matrix2pl(const Matrix& m, std::vector<Point>& pl);
  void Matrix2w(const Matrix& m, std::vector<double>& w);

  void normalizeShift(std::vector<Point>& pl, double& x, double& y);
  void normalizeScale(std::vector<Point>& pl, double& fac);
}
#endif
