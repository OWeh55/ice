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
/********************************************************************
  Header zu "gentrans.c"
  Generieren von Transformtionsmatrizen
  Suesse 7/93 - neubauer 10/95
********************************************************************/

#ifndef _GENTRANS_H
#define _GENTRANS_H

#include "base.h"
#include "pointlist.h"

namespace ice
{
  /* Initialisieren einer Transformationsmatrix (Einheitsmatrix)*/
  int InitTrans(double t[3][3]);

  /* Premultiplikation von "Trans" mit Verschiebung */
  int ShiftTrans
  (double x0, double y0, double Trans[3][3]);

  /* Premultiplikation von "Trans" mit Rotation um Punkt (x0,y0) */
  int RotTrans
  (double x0, double y0, double phi, double Trans[3][3]);

  /* Premultiplikation von "Trans" mit Skalierung um Punkt (x0,y0) */
  int ScaleTrans(double x0, double y0,
                 double facx, double faxy, double Trans[3][3]);

  /* Premultiplikation von "Trans" mit affiner Transformation */
  int AffinTrans(double a11, double a12, double a13,
                 double a21, double a22, double a23, double Trans[3][3]);

  /* Premultiplikation von "Trans" mit projektiver Transformation */
  int ProjecivTrans(double a11, double a12, double a13,
                    double a21, double a22, double a23,
                    double a31, double a32, double a33, double Trans[3][3]);

  /* Invertieren einer Transformationsmatrix */
  int InvertTrans(double Trans[3][3]);

  /* Anpassen einer affinen Transformation */
  int FitAffineTrans(PointList pl1, PointList pl2, double tr[3][3]);

  /*Anpassen einer projektiven Transformation*/
  int FitProjectiveTrans(PointList pl1, PointList pl2, double tr[3][3]);

}
#endif

