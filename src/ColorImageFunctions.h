/************************************************************************
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
 *
 * "ColorImageFunctions.h" - Niels Oertel 2008, Wolfgang Ortmann 02/213
 ************************************************************************/

#ifndef _COLORIMAGE_FUNCTIONS_H
#define _COLORIMAGE_FUNCTIONS_H

#include "ColorImage.h"

#define GV_QUANTILE 0

namespace ice
{
  /*******************************************************************
   * Basic Arithmetics on ColorImages                                *
   * This section contains some easy functions to modify ColorImages *
   * Implementation in ColorImageArith.cpp                           *
   *******************************************************************/

  /**
   * Fills the three channels with value val
   *
   * @param img the image we want to process
   * @param val the value all channels will be set to
   */
  void setImg(const ColorImage& img, unsigned int val);

  /**
   * Fills each channel with its corresponding value in val
   *
   * @param img the image we want to process
   * @param val contains the value information for each channel
   */
  void setImg(const ColorImage& img, const ColorValue& val);

  /**
   * Adds the two input images. Depending on the mode the channels are
   * treated independtly or by using the overloaded operator+
   * The destination image will be overwritten
   * available modes:
   * MD_NORMALIZE: the histogramm is linear stretched
   * MD_LIMIT: values greater maxVal are set to maxVal
   * MD_MODULO: val = (val1 + val2) % maxVal
   * MD_STANDARD: use the overloaded operator+
   *
   * @param img1 the first addend
   * @param img2 the second addend
   * @param dest the destination image
   * @param mode defines how the result is adjusted to the properties of dest
   */

  void addImg(const ColorImage& img1, const ColorImage& img2, const ColorImage& dest,
              int mode = MD_NORMALIZE);

  /**
   * Subtracts the two input images. Depending on the mode the channels are
   * treated independtly or by using the overloaded operator-
   * The destination image will be overwritten
   * available smodes:
   * SMD_ABSOLUTE: values less 0 are multiplied with -1
   * SMD_POSITIVE: values less 0 are set to 0
   * SMD_SHIFT: the values are increased by maxVal of img2
   * available modes:
   * MD_NORMALIZE: the histogramm is linear stretched
   * MD_LIMIT: values greater maxVal are set to maxVal
   * MD_MODULO: val = (val1 - val2) % maxVal
   * MD_STANDARD: use the overloaded operator-
   *
   * @param img1 the minuend
   * @param img2 the subtrahend
   * @param dest the destination image
   * @param smode defines how negative values are treated
   * @param mode defines how the result is adjusted to the properties of dest
   */
  void subImg(const ColorImage& img1, const ColorImage& img2,
              const ColorImage& dest,
              int mode = MD_NORMALIZE, int smode = SMD_ABSOLUTE);

  /**
   * Creates a new image with the greater value of the two source images at each pixel
   * Depending on the mode the channels are compared independtly or by using the overloaded operator>
   * The destination image will be overwritten
   *
   * @param img1 the first source image
   * @param img2 the second source image
   * @param dest the destination image
   * @param mode defines how the result is adjusted to the properties of dest
   */
  void maxImg(const ColorImage& img1, const ColorImage& img2, const ColorImage& dest,
              int mode = MD_LIMIT);

  /**
   * Creates a new image with the smaller value of the two source images at each pixel
   * Depending on the mode the channels are compared independtly or by using the overloaded operator<
   * The destination image will be overwritten
   *
   * @param img1 the first source image
   * @param img2 the second source image
   * @param dest the destination image
   * @param mode defines how the result is adjusted to the properties of dest
   */
  void minImg(const ColorImage& img1, const ColorImage& img2,
              const ColorImage& dest,
              int mode = MD_LIMIT);

  /**
   * Copies src to dest by adapting the pixelvalues (linear histogram stretching)
   * and pixel positions (zooming by either interpolation or taking the nearest neighbor)
   * The pixel values get multiplied by dest.maxVal/src.maxVal
   *
   * @param src: the source image
   * @param dest: the destination image
   */
  void renormImg(const ColorImage& src, const ColorImage& dest);

  /**
   * equalizes the histogram of intensity channel of an image
   * This function is usable in arbitrary colorspaces that implement functions
   * to convert to YUV and back
   *
   * @param src source image & destination image
   * @return error value (returns 0 if operation succeeded)
   */
  void HistogramEqualization(const ColorImage& src);
  void HistogramEqualization(const ColorImage& src, const ColorImage& dst);

  /*
   * The following part contains some linear local filters
   * their base functionality is taken from analog functions
   * for gray images.
   * Implementation in ColorFunctionsLinLoc.cpp
   */

  /**
   * Convolutes each channel of src with a n times n 1-matrix
   * with prefactor of 1 / (n*n), writing the result in dest
   * n has to be odd
   *
   * @param src source image
   * @param dest destination image
   * @param n size of the filter
   */
  void smearImg(const ColorImage& src, const ColorImage& dest, int n = 3);

  /**
   * Convolutes each channel of src with a n times n 1-matrix
   * with prefactor of 1 / (nx*ny), writing the result in dest
   * nx and ny have to be odd
   *
   * @param src source image
   * @param dest destination image
   * @param nx horizontal size of the filter
   * @param ny vertical size of the filter
   */
  void smearImg(const ColorImage& src, const ColorImage& dest, int nx, int ny);

  /**
   * The "Difference of Boxes" filter applies the SmearImg filter, first
   * with filtersize n1 (getting v1), then with filtersize n2 (getting v2).
   * available modes for computing the difference:
   * SMD_ABSOLUTE: values less 0 are multiplied by -1
   * SMD_POSITIVE: values less 0 are set to 0
   * SMD_SHIFT: the values are increased by maxVal of img2
   *
   * @param src source image
   * @param dest destination image
   * @param n1 size of the first filter
   * @param n2 size of the second filter
   * @param smode mode of the subtraction (see above for available modes)
   */
  void DoBImg(const ColorImage& src, const ColorImage& dest, int n1, int n2,
              int smode = SMD_SHIFT);

  /**
   * Applies the Prewitt filter in X- respective Y-direction on the image.
   * If src and dest have differnt maxValues the result is automatically stretched
   * With the optional parameter norm you can influence this
   *
   * @param src source image
   * @param dest destination image
   * @param norm specifies the norming factor of the destination values
   */
  void GradXImg(const ColorImage& src, const ColorImage& dest, int norm = 1);
  void GradYImg(const ColorImage& src, const ColorImage& dest, int norm = 1);

  /**
   * Applies the mean filter on the image.
   * If src and dest have differnt maxValues the result is automatically stretched
   *
   * @param src source image
   * @param dest destination image
   */
  void MeanImg(const ColorImage& src, const ColorImage& dest);

  /**
   * Applies the Laplace-Filter in X-,Y- or both directions on the picture.
   * If src and dest have differnt maxValues the result is automatically stretched
   * With the optional parameter norm you can influence this
   *
   * @param src source image
   * @param dest destination image
   * @param norm specifies the norming factor of the destination values
   */
  void LaplaceXImg(const ColorImage& src, const ColorImage& dest, int norm = 1);
  void LaplaceYImg(const ColorImage& src, const ColorImage& dest, int norm = 1);
  void LaplaceImg(const ColorImage& src, const ColorImage& dest, int norm = 1);

  /**
   * Applies the linear shift invariant Filter mask on src,
   * with nx the number of rows, ny the number of columns,
   * norm the normation factor of the mask and offset the basevalue to add to each pixel
   * mask can be int*, double*, Matrix or IMatrix
   *
   */
  void LSIImg(const ColorImage& src, const ColorImage& dest, int nx, int ny, int* mask, int norm, int offset);
  void LSIImg(const ColorImage& src, const ColorImage& dest, int nx, int ny, double* mask, int offset);
  void LSIImg(const ColorImage& src, const ColorImage& dest, const Matrix& mask, int offset);
  void LSIImg(const ColorImage& src, const ColorImage& dest, const IMatrix& mask, int norm, int offset);

  /**
   * The GaussImg filter is applied to src with sgima1 and sigma2.
   * The Difference of the two dest Images becomes the dest of MexicanImg.
   */
  void MexicanHatImg(const ColorImage& src, const ColorImage& dest, double sigma, int neighbor);

  /****************************************************************
   * The following section contains some nonlinear local filters  *
   * Implementation in ColorFunctionsNonLin.cpp                   *
   ****************************************************************/

  /**
   * Median filter
   * This implementation doesn't use the one for gray images.
   * Instead it computes the median as the pixel with the smallest
   * mean distance to the other pixels.
   * This filter may work well in RGB color space because of the
   * structure of the RGB cube.
   *
   * @param src the source image
   * @param dest the destination image
   * @param neighb the filtersize, has to be odd
   * @return error value (returns 0 if operation succeeded)
   */
  void MedianImg(const ColorImage& src, const ColorImage& dest, int neighb);
}

#endif /* _COLORFUNCTIONS_H */
