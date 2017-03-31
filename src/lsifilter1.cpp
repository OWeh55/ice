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
#include "message.h"
#include "macro.h"

#include "util.h"

#include "filter.h"
#include "lsifilter.h"

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
  int LSIImg(const Image& src, const Image& dest,
             int nx, int ny, int* mask,
             int norm, int offset)
  {
    if (norm == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    return lsiimg(src, dest, nx, ny, mask, norm, offset);
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
  int LSIImg(const Image& src, const Image& dest,
             int nx, int ny, double* mask,
             int offset)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    return lsiimg(src, dest, nx, ny, mask, offset);
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
  int LSIImgCyc(const Image& src, const Image& dest,
                int nx, int ny, int* mask,
                int norm, int offset)
  {
    if (norm == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    return lsiimgcyc(src, dest, nx, ny, mask, norm, offset);
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
  int LSIImgCyc(const Image& src, const Image& dest,
                int nx, int ny, double* mask,
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
        tmp = Image::createImage(dest, true);
      }

    return lsiimgcyc(src, dest, nx, ny, mask, offset);
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
   * @param offset the value representing 0 (to handle negative results)
   */
  int LSIImg(const Image& src, ImageD dest,
             int nx, int ny, int* mask,
             int norm)
  {
    if (norm == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (!IsImg(src))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (!IsImgD(dest))
      {
        Message(FNAME, M_WRONG_IMAGED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((src->xsize != dest.xsize) || (src->ysize != dest.ysize))
      {
        Message(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return lsiimg(src, dest, nx, ny, mask, norm);
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
  int LSIImg(const Image& src, ImageD dest,
             int nx, int ny, double* mask)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows

    if (!IsImg(src))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (!IsImgD(dest))
      {
        Message(FNAME, M_WRONG_IMAGED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((src->xsize != dest.xsize) || (src->ysize != dest.ysize))
      {
        Message(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return lsiimg(src, dest, nx, ny, mask);
  }

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
  int LSIImg(ImageD src, ImageD dest,
             int nx, int ny, int* mask,
             int norm)
  {
    if (norm == 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImgD(src, dest));

    return lsiimg(src, dest, nx, ny, mask, norm);
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
  int LSIImg(ImageD src, ImageD dest,
             int nx, int ny, double* mask)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImgD(src, dest));

    return lsiimg(src, dest, nx, ny, mask);
  }

//####################################################################################################
  int LSIImg(const Image& src, const Image& dest, const LSIFilter& f, int offset)
  {
    return f.Filter(src, dest, offset);
  }

// old versions with different parameter order
  int LSIImg(const Image& src, int nx, int ny, int* kernel, int norm, int off, const Image& dest)
  {
    return LSIImg(src, dest, nx, ny, kernel, norm, off);
  }

  int LSIImg(const Image& src, int nx, int ny, double* kernel, int off, const Image& dest)
  {
    return LSIImg(src, dest, nx, ny, kernel, off);
  }

  int LSIImg(const Image& src, const Matrix& mask, int off, const Image& dest)
  {
    return LSIImg(src, dest, mask, off);
  }

  int LSIImg(const Image& src, const IMatrix& mask, int norm, int off, const Image& dest)
  {
    return LSIImg(src, dest, mask, norm, off);
  }

  int LSIImg(const Image& src, const Image& dest, const Matrix& mask, int off)
  {
    LSIFilter fm(mask);
    return fm.Filter(src, dest, off);
  }

  int LSIImg(const Image& src, const Image& dest, const IMatrix& mask, int norm, int off)
  {
    LSIFilter fm(mask, norm);
    return fm.Filter(src, dest, off);
  }
#undef FNAME
}
