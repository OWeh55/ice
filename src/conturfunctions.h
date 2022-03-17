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
#ifndef _CONTURFUNCTIONS_H
#define _CONTURFUNCTIONS_H

#include <vector>

#include "defs.h"
#include "base.h"
#include "Matrix.h"
#include "Point.h"
#include "Window.h"
#include "Contur.h"
#include "polygon.h"
#include "objectfunctor.h"
#include "conturfollowing.h"

namespace ice
{

#define CC_EXACT 0
#define CC_FAST  1

#define HORZ DEFAULT
#define VERT (DEFAULT+1)

  IMatrix ConturSegmentlist(const Contur& c);
  double RoughnessContur(const Contur& c, int);
  int FeatureContur(const Contur& c, double& length, double& area, double& form, double& conv);

  /********* Startpunktsuche ***************************/
  int GetThreshold(const Image& img, int s[2]);

  int SearchStart(const Image& imgv, const Image& imgo,
                  OFunc object,
                  int pgl, int diff, int ps[2],
                  int direct = HORZ);

  int SearchStart(const Image& imgv, const Image& imgo,
                  OFunc object,
                  int pgl, int diff, IPoint& ps,
                  int direct = HORZ);

  int SearchGradStart(const Image& imgv, const Image& imgo,
                      int pgl, int diff, int ps[2],
                      int direct = HORZ);

  int SearchCStart(const Image& imgv, const Image& imgo,
                   Contur c, int& ptr, int pgl, int s[2]);

  int SearchCircStart(const Image& imgv, const Image& imgo,
                      int p[2], int r, int pgl, int s[2]);

  /*********  Konturfolge  **************************/
  Contur CalcContur(const Image& imgv, const Image& imgo,
                    OFunc object,
                    int pgl, int ps[2],
                    int lng = 0,
                    object_rc marked = isunderground,
                    object_rc outside = isunderground);

  Contur CalcContur(const Image& imgv, const Image& imgo,
                    OFunc object,
                    int pgl, const IPoint& p,
                    int lng = 0,
                    object_rc marked = isunderground,
                    object_rc outside = isunderground);

  Contur CalcGradContur(const Image& imgv, Image& imgo,
                        int pgl, int maxgap, int ps[2], int lng);
  Contur CalcGradContur(const Image& imgv, Image& imgo,
                        int pgl, int maxgap, int ps[2], int lng,
                        double& meangrad);
  Contur GetContur(int ps[2], int (*cls)(int, int, void*), void* par, int lng);

  Contur ConvexContur(const Contur& c, int mode = CC_EXACT);

  int LimitingPolygon(const Contur& c, Polygon& poly);
}
#endif
