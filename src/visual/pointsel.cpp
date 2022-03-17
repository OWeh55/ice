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
//
//   SelPoint(mode,img,point) ....
//
//      M. Schubert 27.11.92
//    + St.Abraham  anno 1994
//    + M. Schubert 02.04.96
//    + N. Oertel 2008
//    + W. Ortmann 1999,2005,2013
//

#include <stdio.h>

#include "macro.h"
//#include "analygeo.h"
#include "visual/mouse.h"
//#include "paint.h"
//#include "screen.h"
#include "visual/visual.h"
#include "visual/pointsel.h"
//#include "ImageMgr.h"

//#include "App.h"

namespace ice
{

#define FNAME "SelPoint"
  int SelPoint(int mode, const Image& img, int point[2])
  {
    Visual v;
    IPoint p;
    int rc;
    RETURN_ERROR_IF_FAILED(v = getVisual(img));
    if (v == NULL)
      throw IceException(FNAME, M_NOT_VIS);
    rc = v->SelPoint(mode, p);
    point[0] = p.x;
    point[1] = p.y;
    return rc;
  }
#undef FNAME

//------------------------------------------------------------
#define FNAME "SelVector"
  IVector SelVector(int mode, const Image& img, int& rc)
  {
    try
      {
        int point[2];
        rc = SelPoint(mode, img, point);
        return IVector(point[0], point[1]);
      }
    RETHROW;
  }

  IVector SelVector(int mode, const Image& img)
  {
    int rc;
    return SelVector(mode, img, rc);
  }

  IVector SelVector(const Image& img)
  {
    int rc;
    return SelVector(DEFAULT, img, rc);
  }

  IVector SelVector(const Image& img, int& rc)
  {
    return SelVector(DEFAULT, img, rc);
  }
#undef FNAME
//---------------------------------------------------
#define FNAME "SelPoint"
  IPoint SelPoint(int mode, const Image& img, int& rc)
  {
    int point[2];
    rc = SelPoint(mode, img, point);
    return IPoint(point[0], point[1]);
  }

  IPoint SelPoint(int mode, const Image& img)
  {
    int rc;
    return SelPoint(mode, img, rc);
  }

  IPoint SelPoint(const Image& img)
  {
    int rc;
    return SelPoint(DEFAULT, img, rc);
  }

  IPoint SelPoint(const Image& img, int& rc)
  {
    return SelPoint(DEFAULT, img, rc);
  }

  IPoint SelPoint(const ColorImage& img)
  {
    Visual v = getVisual(img);
    if (NULL == v)
      throw IceException(FNAME, M_NOT_VIS);
    IPoint p(0, 0);
    v->SelPoint(DEFAULT, p);
    return p;
  }

#undef FNAME
}
