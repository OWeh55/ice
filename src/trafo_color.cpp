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
      throw IceException(FNAME, M_INVALID_IMAGE);

    RETURN_ERROR_IF_FAILED(Transform(tr, simg.redImage(), dimg.redImage(), mode));
    RETURN_ERROR_IF_FAILED(Transform(tr, simg.greenImage(), dimg.greenImage(), mode));
    RETURN_ERROR_IF_FAILED(Transform(tr, simg.blueImage(), dimg.blueImage(), mode));
    return OK;
  }

#undef FNAME
}
