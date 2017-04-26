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

#include "defs.h"
#include "macro.h"
#include "IceException.h"

#include "ColorImage.h"
#include "trafo_img.h"

#include "trafo_color.h"

//--------------------------------------------
namespace ice
{
#define FNAME "Transform"
//*****************************************************************************************
  int Transform(const Trafo& tr,
                const ColorImage& simg, ColorImage& dimg, int mode)
  {
    if ((!simg.isValid()) || (!dimg.isValid()))
      throw IceException(FNAME, M_INVALID);

    RETURN_ERROR_IF_FAILED(Transform(tr, simg.redImage(), dimg.redImage(), mode));
    RETURN_ERROR_IF_FAILED(Transform(tr, simg.greenImage(), dimg.greenImage(), mode));
    RETURN_ERROR_IF_FAILED(Transform(tr, simg.blueImage(), dimg.blueImage(), mode));
    return OK;
  }

#undef FNAME
}
