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
/************************************************/
/*            Filter                            */
/************************************************/
#ifndef _ICE_MORPH_H
#define _ICE_MORPH_H

#include <limits.h>
#include "base.h"

#include "Matrix.h"
#include "based.h"
#include "arith.h"
#include "ColorImage.h"

namespace ice
{
  // Erode / Dilate
  // with rectangular structuring element

  void erodeImg(const Image& img1, const Image& img2, int nx = 3, int ny = -1);
  void dilateImg(const Image& img1, const Image& img2, int nx = 3, int ny = -1);

  void openingImg(const Image& img1, const Image& img2, int nx = 3, int ny = -1);
  void closingImg(const Image& img1, const Image& img2, int nx = 3, int ny = -1);


  // structuring element defined by mask

  void dilateImg(const Image& img1, int neighb, int* mask, const Image& img2);
  void erodeImg(const Image& img1, int neighb, int* mask, const Image& img2);

  void dilateImg(const Image& img1, int nx, int ny, int* mask,
                 const Image& img2);
  void erodeImg(const Image& img1, int nx, int ny, int* mask,
                const Image& img2);

  void dilateImg(const Image& imgss, const IMatrix& msk, const Image& imgd);
  void erodeImg(const Image& imgss, const IMatrix& msk, const Image& imgd);

  void dilateImg(const Image& imgss, const Image& imgd, const IMatrix& msk);
  void erodeImg(const Image& imgss, const Image& imgd, const IMatrix& msk);

  void dilateImg(const Image& imgss, const Image& imgd, const matrix<int>& msk);
  void erodeImg(const Image& imgss, const Image& imgd, const matrix<int>& msk);

#define FNAME "openingImg"
  template<typename T>
  void openingImg(const Image& img1, const Image& img2, const T& m)
  {
    try
      {
        erodeImg(img1, img2, m);
        dilateImg(img2, img2, m);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "closingImg"
  template<typename T>
  void closingImg(const Image& img1, const Image& img2, const T& m)
  {
    try
      {
        dilateImg(img1, img2, m);
        erodeImg(img2, img2, m);
      }
    RETHROW;
  }
#undef FNAME

  // erode and dilate
  int MinMaxImg(const Image& pn1, int sx, int sy,
                const Image& pn2, const Image& pn3);

  // Skelettierung - skelett.cpp
  void skeletonImg(const Image& img1, const Image& img2, int lvl = 1, int nIter=INT_MAX);
}
#endif
