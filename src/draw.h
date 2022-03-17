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
#ifndef DRAW_H
#define DRAW_H

#include "base.h"
#include "circle.h"
#include "circleseg.h"
#include "ellipse.h"
#include "lineseg.h"
#include "polygon.h"
#include "triangle.h"
#include "forest.h"

namespace ice
{

  int draw(const Circle& c, const Image& img, int val, int fval = -1);
  int draw(const CircleSeg& cs, const Image& img, int val, int fval = -1);
  int draw(const Ellipse& e, const Image& img, int val, int fval = -1);
  int draw(const EllipseSeg& es, const Image& img, int val, int fval = -1);
  int draw(const LineSeg& ls, const Image& img, int val, int fval = -1);
//  int draw(const Polygon &p,const Image & img,int val,int fval=-1);
  int draw(const PolygonalCurve& p, const Image& img, int val, int fval = -1);
  int draw(const Triangle& p, const Image& img, int val, int fval = -1);
  int draw(const Forest& f, const std::vector<Point>& pl,
           const Image& img, int val);

}

#endif
