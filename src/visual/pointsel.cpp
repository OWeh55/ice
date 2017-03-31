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
    RETURN_ERROR_IF_FAILED(v = GetVisual(img));
    if (v == NULL)
      {
        Message(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }
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
    int point[2];
    IF_FAILED(rc = SelPoint(mode, img, point))
    {
      Message(FNAME, M_0, ERROR);
      return IVector();
    }
    return IVector(point[0], point[1]);
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
    IF_FAILED(rc = SelPoint(mode, img, point))
    {
      Message(FNAME, M_0, ERROR);
      return IPoint(point[0], point[1]);
    }
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
    Visual v = GetVisual(img);
    if (NULL == v)
      {
        Message(FNAME, M_NOT_VIS, WRONG_PARAM);
        return IPoint(0, 0);
      }
    IPoint p(0, 0);
    v->SelPoint(DEFAULT, p);
    return p;
  }

#undef FNAME
}
