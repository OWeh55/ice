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
//  Konturfolge und Kantendetektion
//
//
//   (C) m.schubert 9.93
//       Ortmann 10/99
//
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "contools.h"
#include "contfeat.h"

#include "conturfunctions.h"

namespace ice
{
  /*****************************************************/
//  int ObjectThr(Image imgv,Image imgo,int x,int y,int pgl)
//   {
//     if (x<imgv->wxi) return OR_OUTSIDE;
//     if (y<imgv->wyi) return OR_OUTSIDE;
//     if (x>imgv->wxa) return OR_OUTSIDE;
//     if (y>imgv->wya) return OR_OUTSIDE;
//     if (imgo!=NULL)
//       if (GetVal(imgo,x,y)>0) return OR_MARKER;
//     if (GetVal(imgv,x,y)<pgl) return OR_UNDERGROUND;
//     return OR_OBJECT;
//   }

//   int ObjectThrInv(Image imgv,Image imgo,int x,int y,int pgl)
//   {
//     if (x<imgv->wxi) return OR_OUTSIDE;
//     if (y<imgv->wyi) return OR_OUTSIDE;
//     if (x>imgv->wxa) return OR_OUTSIDE;
//     if (y>imgv->wya) return OR_OUTSIDE;
//     if (imgo!=NULL)
//       if (GetVal(imgo,x,y)>0) return OR_MARKER;
//     if (GetVal(imgv,x,y)>pgl) return OR_UNDERGROUND;
//     return OR_OBJECT;
//   }

  object_rc ObjectThr(const Image& imgv, int x, int y, int pgl)
  {
    if (GetValUnchecked(imgv, x, y) < pgl)
      {
        return isunderground;
      }

    return isobject;
  }

  object_rc ObjectThrInv(const Image& imgv, int x, int y, int pgl)
  {
    if (GetValUnchecked(imgv, x, y) >= pgl)
      {
        return isunderground;
      }

    return isobject;
  }

  /**********************************************************/
}
