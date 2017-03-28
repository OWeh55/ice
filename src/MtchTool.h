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
#ifndef _PEAKTOOLS_H_
#define _PEAKTOOLS_H_

#include <string.h>

#include "defs.h"
#include "base.h"
#include "based.h"
#include "message.h"
#include "Contur.h"
#include "contlist.h"
#include "contools.h"
#include "numbase.h"
#include "arith.h"
#include "polygonim.h"
#include "fit.h"

namespace ice
{
  double Gauss2D(double x, double y,
                 double xm, double ym, double s1, double s2, double s12);

//

  void Get2DGaussParamsD(ImageD imgd, Image imgo,
                         double& x0, double& y0,
                         double& sx, double& sy, double& sxy,
                         double& a, double& b);

//

#define CF_CUT 1
#define CF_SET 2

  int CutFrameImg(Image, int dx, int dy, int mode, Image);

//

//PointList PointListBinObj(Image img,int zykl,int sx,int sy);
//int SegmentBinObj(Image img,int zykl,double mdist,int sx,int sy,Contur* cont,Segment* segm);

  ConturList BinObj2ConturList(Image img, int sx, int sy);

//

  double EntropyImg(Image img);
  double EntropyImgD(ImageD img, double& maxentro);
}
#endif
