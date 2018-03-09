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
#include "IceException.h"
#include "macro.h"

#include "util.h"

#include "filter.h"
#include "LsiFilter.h"

namespace ice
{

#define FNAME "LSIImg"
  /**
   * function applies a given LSI filter to an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (only integer values)
   * @param norm the scaling factor
   * @param offset the value representing 0 (to handle negative results)
   */
  void LSIImg(const Image& src, const Image& dest,
              int nx, int ny, const int* mask,
              int norm, int offset)
  {
    try
      {
        if (norm == 0)
          throw IceException(FNAME, M_WRONG_PARAM);

        // test if both images have valid pixel arrays and
        // equal size
        checkSizes(src, dest);

        lsiimg(src, dest, nx, ny, mask, norm, offset);
      }
    RETHROW;
  }

  /**
   * function applies a given LSI filter to an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   * @param offset the value representing 0 (to handle negative results)
   */
  void LSIImg(const Image& src, const Image& dest,
              int nx, int ny, const double* mask,
              int offset)
  {
    try
      {
        // test if both images have valid pixel arrays and
        // equal size
        checkSizes(src, dest);
        lsiimg(src, dest, nx, ny, mask, offset);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "LSIImgCyc"
  /**
   * This function applies a given LSI filter on an image with
   * cyclic properties, like the Hue channel in HSI colorspace
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask
   * @param norm the scaling factor
   * @param offset the value representing 0 (to handle negative results)
   */
  void LSIImgCyc(const Image& src, const Image& dest,
                 int nx, int ny, const int* mask,
                 int norm, int offset)
  {
    if (norm == 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    lsiimgcyc(src, dest, nx, ny, mask, norm, offset);
  }

  /**
   * This template function applies a given LSI filter on an image with
   * cyclic properties, like the Hue channel in HSI colorspace
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   * @param offset the value representing 0 (to handle negative results)
   */
  void LSIImgCyc(const Image& src, const Image& dest,
                 int nx, int ny, const double* mask,
                 int offset)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    lsiimgcyc(src, dest, nx, ny, mask, offset);
  }

#undef FNAME
#define FNAME "LSIImg"
  /**
   * function applies a given LSI filter to an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (only integer values)
   * @param norm the scaling factor
   */
  void LSIImg(const Image& src, ImageD dest,
              int nx, int ny, const int* mask,
              int norm)
  {
    if (norm == 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (!IsImg(src))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!IsImgD(dest))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if ((src->xsize != dest.xsize) || (src->ysize != dest.ysize))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    lsiimg(src, dest, nx, ny, mask, norm);
  }

  /**
   * function applies a given LSI filter to an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   */
  void LSIImg(const Image& src, ImageD dest,
              int nx, int ny, const double* mask)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows

    if (!IsImg(src))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!IsImgD(dest))
      throw IceException(FNAME, M_WRONG_IMAGED);

    if ((src->xsize != dest.xsize) || (src->ysize != dest.ysize))
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    lsiimg(src, dest, nx, ny, mask);
  }

  /**
   * function applies a given LSI filter to an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (only integer values)
   * @param norm the scaling factor
   */
  void LSIImg(ImageD src, ImageD dest,
              int nx, int ny, const int* mask,
              int norm)
  {
    if (norm == 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImgD(src, dest));

    lsiimg(src, dest, nx, ny, mask, norm);
  }

  /**
   * function applies a given LSI filter to an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   */
  void LSIImg(ImageD src, ImageD dest,
              int nx, int ny, const double* mask)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImgD(src, dest));

    lsiimg(src, dest, nx, ny, mask);
  }

//####################################################################################################
  void LSIImg(const Image& src, const Image& dest, const LsiFilter& f, int offset)
  {
    f.filter(src, dest, offset);
  }

// old versions with different parameter order
  void LSIImg(const Image& src, int nx, int ny, const int* kernel, int norm, int off, const Image& dest)
  {
    LSIImg(src, dest, nx, ny, kernel, norm, off);
  }

  void LSIImg(const Image& src, int nx, int ny, const double* kernel, int off, const Image& dest)
  {
    LSIImg(src, dest, nx, ny, kernel, off);
  }

  void LSIImg(const Image& src, const Matrix& mask, int off, const Image& dest)
  {
    LSIImg(src, dest, mask, off);
  }

  void LSIImg(const Image& src, const IMatrix& mask, int norm, int off, const Image& dest)
  {
    LSIImg(src, dest, mask, norm, off);
  }

  void LSIImg(const Image& src, const Image& dest, const Matrix& mask, int off)
  {
    LsiFilter fm(mask);
    fm.filter(src, dest, off);
  }

  void LSIImg(const Image& src, const Image& dest, const IMatrix& mask, int norm, int off)
  {
    LsiFilter fm(mask, norm);
    fm.filter(src, dest, off);
  }
#undef FNAME
}
