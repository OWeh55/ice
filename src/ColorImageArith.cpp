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
  void setImg(const ColorImage& img, unsigned int val)
  {
    img.redImage().set(val);
    img.greenImage().set(val);
    img.blueImage().set(val);
  }

  void setImg(const ColorImage& img, const ColorValue& val)
  {
    int maxval = img.maxval;
    img.redImage().set(maxval - val.red);
    img.greenImage().set(maxval - val.green);
    img.blueImage().set(maxval - val.blue);
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
        addImg(img1.redImage(), img2.redImage(), dest.redImage(), mode);
        addImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode);
        addImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode);
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
        subImg(img1.redImage(), img2.redImage(), dest.redImage(), mode, smode);
        subImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode, smode);
        subImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode, smode);
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
        maxImg(img1.redImage(), img2.redImage(), dest.redImage(), mode);
        maxImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode);
        maxImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode);
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
        minImg(img1.redImage(), img2.redImage(), dest.redImage(), mode);
        minImg(img1.greenImage(), img2.greenImage(), dest.greenImage(), mode);
        minImg(img1.blueImage(), img2.blueImage(), dest.blueImage(), mode);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "renormImg"
  void renormImg(const ColorImage& src, const ColorImage& dest)
  {
    try
      {
        renormImg(src.redImage(), dest.redImage());
        renormImg(src.greenImage(), dest.greenImage());
        renormImg(src.blueImage(), dest.blueImage());
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
