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
#ifndef _LSEG_H
#define _LSEG_H
//
// local segmentation
//

#include "defs.h"
#include "base.h"
#include "objectfunction.h"

namespace ice
{

  enum
  {
    // von _dieser_ Zuordnung hängt auch LocalSegObj ab!
    // bei Zuordnung anderer Werte hier auch Tabelle dort ändern
    LSunknown = 0,
    LSunderground = 1,
    LSobject = 2,
    LSweakunderground = 3,
    LSweakobject = 4
  };


  int LocalSeg(Image source, Image oimg, int neighb = 7, int level = 15);
  int LocalSeg(Image source, Image oimg, int neighb, int level, int level2);
  int LocalSeg(Image source, int neighb = 7, int level = 15);
  int LocalSeg(Image source, int neighb, int level, int level2);

  object_rc LocalSegObj(const Image& i, int x, int y, int thr_dummy);
  object_rc LocalSegObjInv(const Image& i, int x, int y, int thr_dummy);
  object_rc LocalSegObjHigh(const Image& i, int x, int y, int thr_dummy);
}
#endif













