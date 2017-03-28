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
