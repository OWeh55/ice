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

/*
 * This source file provides the implementation of some easy
 * arithmetic functions for modifying ColorImages.
 *
 * int SetImg(const ColorImage &img, unsigned int val)
 * int SetImg(const ColorImage &img, ColorVal &val)
 * int SetImg(const ColorImage &img, Func2D &fn)
 * int SetImg(const ColorImage &img, Func2D &fn1, Func2D &fn2, Func2D &fn3)
 * int AddImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int mode=MD_NORMALIZE)
 * int SubImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int smode=SMD_ABSOLUTE, int mode=MD_NORMALIZE)
 * int MaxImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int mode=MD_NORMALIZE)
 * int MinImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int mode=MD_NORMALIZE)
 * int RenormImg(const ColorImage &src, const ColorImage &dest)
 *
 * "ColorFunctionsArith.cpp" - Niels Oertel 2008, Wolfgang Ortmann 02/2013
 */

#include "macro.h"

#include "ColorImageFunctions.h"
#include "lintrans.h"
#include "histogramequalization.h"
#include "icefunc.h"
#include "util.h"

namespace ice
{

#define FNAME "SetImg"
  /**
   * does not depend on the colorspace
   * the user has the posibility to fill the channels independent
   * from the current colorspace
   */
  int SetImg(const ColorImage& img, unsigned int val)
  {
    RETURN_ERROR_IF_FAILED((SetImg(img.redImage(), val)));
    RETURN_ERROR_IF_FAILED((SetImg(img.greenImage(), val)));
    RETURN_ERROR_IF_FAILED((SetImg(img.blueImage(), val)));

    return OK;
  }

  int SetImg(const ColorImage& img, const ColorValue& val)
  {
    int maxval = img.maxval;
    RETURN_ERROR_IF_FAILED((SetImg(img.redImage(), maxval - val.red)));
    RETURN_ERROR_IF_FAILED((SetImg(img.greenImage(), maxval - val.green)));
    RETURN_ERROR_IF_FAILED((SetImg(img.blueImage(), maxval - val.blue)));

    return OK;
  }

  int SetImg(const ColorImage& img, const Function2d& fn)
  {
    RETURN_ERROR_IF_FAILED((SetImg(img.redImage(), fn)));
    RETURN_ERROR_IF_FAILED((SetImg(img.greenImage(), fn)));
    RETURN_ERROR_IF_FAILED((SetImg(img.blueImage(), fn)));

    return OK;
  }

  int SetImg(const ColorImage& img, const Function2d& fn1,
             const Function2d& fn2, const Function2d& fn3)
  {
    RETURN_ERROR_IF_FAILED((SetImg(img.redImage(), fn1)));
    RETURN_ERROR_IF_FAILED((SetImg(img.greenImage(), fn2)));
    RETURN_ERROR_IF_FAILED((SetImg(img.blueImage(), fn3)));

    return OK;
  }
#undef FNAME

#define FNAME "AddImg"
  int AddImg(const ColorImage& img1, const ColorImage& img2, const ColorImage& dest, int mode)
  {
    RETURN_ERROR_IF_FAILED(img1.match(img2, dest));
    RETURN_ERROR_IF_FAILED(AddImg(img1.redImage(), img2.redImage(), dest.redImage(), mode));
    RETURN_ERROR_IF_FAILED(AddImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode));
    RETURN_ERROR_IF_FAILED(AddImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode));
    return OK;
  }
#undef FNAME

#define FNAME "SubImg"
  int SubImg(const ColorImage& img1, const ColorImage& img2, const ColorImage& dest, int mode, int smode)
  {
    RETURN_ERROR_IF_FAILED(img1.match(img2, dest));
    RETURN_ERROR_IF_FAILED(SubImg(img1.redImage(), img2.redImage(), dest.redImage(), mode, smode));
    RETURN_ERROR_IF_FAILED(SubImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode, smode));
    RETURN_ERROR_IF_FAILED(SubImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode, smode));
    return OK;
  }
#undef FNAME

#define FNAME "MaxImg"
  int MaxImg(const ColorImage& img1, const ColorImage& img2, const ColorImage& dest,
             int mode)
  {
    RETURN_ERROR_IF_FAILED(img1.match(img2, dest));
    RETURN_ERROR_IF_FAILED(MaxImg(img1.redImage(), img2.redImage(), dest.redImage(), mode));
    RETURN_ERROR_IF_FAILED(MaxImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode));
    RETURN_ERROR_IF_FAILED(MaxImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode));
    return OK;
  }
#undef FNAME

#define FNAME "MinImg"
  int MinImg(const ColorImage& img1, const ColorImage& img2, const ColorImage& dest,
             int mode)
  {
    RETURN_ERROR_IF_FAILED(img1.match(img2, dest));
    RETURN_ERROR_IF_FAILED(MinImg(img1.redImage(), img2.redImage(), dest.redImage(), mode));
    RETURN_ERROR_IF_FAILED(MinImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode));
    RETURN_ERROR_IF_FAILED(MinImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode));
    return OK;
  }
#undef FNAME

#define FNAME "RenormImg"
  int RenormImg(const ColorImage& src, const ColorImage& dest)
  {
    // if both images have the same pixelarrays, we don't need to do anything
    if ((src.redImage() == dest.redImage()) &&
        (src.greenImage() == dest.greenImage()) &&
        (src.blueImage() == dest.blueImage()))
      {
        return OK;
      }

    if (!src.isValid() || !dest.isValid())
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(RenormImg(src.redImage(), dest.redImage()));
    RETURN_ERROR_IF_FAILED(RenormImg(src.greenImage(), dest.greenImage()));
    RETURN_ERROR_IF_FAILED(RenormImg(src.blueImage(), dest.blueImage()));
    return OK;
  }
#undef FNAME

#define FNAME "HistogramEqual"
  int HistogramEqualization(const ColorImage& src, const ColorImage& dst)
  {
    if (!src.isValid() || !dst.isValid())
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    HistogramEqualization(src.redImage(), dst.redImage());
    HistogramEqualization(src.greenImage(), dst.greenImage());
    HistogramEqualization(src.blueImage(), dst.blueImage());

    return OK;
  }
  int HistogramEqualization(const ColorImage& img)
  {
    if (!img.isValid())
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    HistogramEqualization(img, img);

    return OK;
  }
#undef FNAME
} /* namespace ice */
