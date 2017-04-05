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
 * int setImg(const ColorImage &img, unsigned int val)
 * int setImg(const ColorImage &img, ColorVal &val)
 * int setImg(const ColorImage &img, Func2D &fn)
 * int setImg(const ColorImage &img, Func2D &fn1, Func2D &fn2, Func2D &fn3)
 * int AddImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int mode=MD_NORMALIZE)
 * int SubImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int smode=SMD_ABSOLUTE, int mode=MD_NORMALIZE)
 * int MaxImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int mode=MD_NORMALIZE)
 * int MinImg(const ColorImage &img1, const ColorImage &img2, const ColorImage &dest, int mode=MD_NORMALIZE)
 * int renormImg(const ColorImage &src, const ColorImage &dest)
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

#define FNAME "setImg"
  /**
   * does not depend on the colorspace
   * the user has the posibility to fill the channels independent
   * from the current colorspace
   */
  void setImg(const ColorImage& img, unsigned int val)
  {
    RETURN_ERROR_IF_FAILED((setImg(img.redImage(), val)));
    RETURN_ERROR_IF_FAILED((setImg(img.greenImage(), val)));
    RETURN_ERROR_IF_FAILED((setImg(img.blueImage(), val)));
  }

  void setImg(const ColorImage& img, const ColorValue& val)
  {
    int maxval = img.maxval;
    RETURN_ERROR_IF_FAILED((setImg(img.redImage(), maxval - val.red)));
    RETURN_ERROR_IF_FAILED((setImg(img.greenImage(), maxval - val.green)));
    RETURN_ERROR_IF_FAILED((setImg(img.blueImage(), maxval - val.blue)));
  }

  void setImg(const ColorImage& img, const Function2d& fn)
  {
    RETURN_ERROR_IF_FAILED((setImg(img.redImage(), fn)));
    RETURN_ERROR_IF_FAILED((setImg(img.greenImage(), fn)));
    RETURN_ERROR_IF_FAILED((setImg(img.blueImage(), fn)));
  }

  void setImg(const ColorImage& img, const Function2d& fn1,
              const Function2d& fn2, const Function2d& fn3)
  {
    RETURN_ERROR_IF_FAILED((setImg(img.redImage(), fn1)));
    RETURN_ERROR_IF_FAILED((setImg(img.greenImage(), fn2)));
    RETURN_ERROR_IF_FAILED((setImg(img.blueImage(), fn3)));
  }
#undef FNAME

#define FNAME "AddImg"
  void addImg(const ColorImage& img1, const ColorImage& img2,
              const ColorImage& dest, int mode)
  {
    try
      {
        AddImg(img1.redImage(), img2.redImage(), dest.redImage(), mode);
        AddImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode);
        AddImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "SubImg"
  void subImg(const ColorImage& img1, const ColorImage& img2,
              const ColorImage& dest, int mode, int smode)
  {
    try
      {
        SubImg(img1.redImage(), img2.redImage(), dest.redImage(), mode, smode);
        SubImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode, smode);
        SubImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode, smode);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "MaxImg"
  void maxImg(const ColorImage& img1, const ColorImage& img2,
              const ColorImage& dest, int mode)
  {
    try
      {
        MaxImg(img1.redImage(), img2.redImage(), dest.redImage(), mode);
        MaxImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode);
        MaxImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "MinImg"
  void minImg(const ColorImage& img1, const ColorImage& img2,
              const ColorImage& dest, int mode)
  {
    try
      {
        MinImg(img1.redImage(), img2.redImage(), dest.redImage(), mode);
        MinImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode);
        MinImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "renormImg"
  void renormImg(const ColorImage& src, const ColorImage& dest)
  {
    try
      {
        RenormImg(src.redImage(), dest.redImage());
        RenormImg(src.greenImage(), dest.greenImage());
        RenormImg(src.blueImage(), dest.blueImage());
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "HistogramEqual"
  void HistogramEqualization(const ColorImage& src, const ColorImage& dst)
  {
    try
      {
        HistogramEqualization(src.redImage(), dst.redImage());
        HistogramEqualization(src.greenImage(), dst.greenImage());
        HistogramEqualization(src.blueImage(), dst.blueImage());
      }
    RETHROW;
  }
  void HistogramEqualization(const ColorImage& img)
  {
    HistogramEqualization(img, img);
  }
#undef FNAME
} /* namespace ice */
