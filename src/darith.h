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
/*********************************************************************/
/*  Bildarithmetik, ImageD;                                          */
/*********************************************************************/

#ifndef _DARITH_H
#define _DARITH_H

#include "based.h"

namespace ice
{
  /* Prozeduren der Bildarithmetik */
  void clearImgD(ImageD img);
  void setImgD(ImageD img, double val);
  void moveImgD(ImageD imgs, ImageD imgd);

  void smearImgD(const ImageD src, ImageD dest, int sx, int sy);
  void smearImgD(const ImageD src, ImageD dest, int nx);

  void addImgD(ImageD imgs1, ImageD imgs2, ImageD imgd);
  void mulImgD(ImageD imgs1, ImageD imgs2, ImageD imgd);

  /**
   * Searches the image for its maximum value
   *
   * @param img the image to be browsed
   * @param x the x-position of the found pixel
   * @param y the y-position of the found pixel
   * @return the maximum grey value in the image
   */
  double findMax(const ImageD& img, int& x, int& y);
  double findMax(const ImageD& img, IPoint& p);
  double findMax(const ImageD& img);

  /**
   * Searches the image for its maximum value
   *
   * @param img the image to be browsed
   * @param mark values != 0 in this image mark ignored pixel
   * @return the position of unmarked maximum
   */
  IPoint findMax(const ImageD& imgD, const Image& mark);

  /**
   * Searches the image for its minimum value
   *
   * @param img the image to be browsed
   * @param x the x-position of the found pixel
   * @param y the y-position of the found pixel
   * @return the maximum grey value in the image
   */
  double findMin(const ImageD& img, int& x, int& y);
  double findMin(const ImageD& img, IPoint& p);
  double findMin(const ImageD& img);

  int LogPolarImgD(ImageD imgs, ImageD imgd = ImageD(),
                   double r1 = 1, double r2 = 0, int sym = 2);
  int LogPolarImg(Image imgs, Image imgd = Image(),
                  double r1 = 1, double r2 = 0, int sym = 2);
  int LogPolarC(ImageD imgs, ImageD imgd,
                double x, double y, double& r, double& fi,
                double r1 = 1, double r2 = 0, int sym = 2);

  ImageD PolarImgD(ImageD imgs, ImageD imgd = ImageD(),
                   double r1 = 1, double r2 = 0, int sym = 2);
  int PolarImg(Image imgs, Image imgd = Image(),
               double r1 = 1, double r2 = 0, int sym = 2);
  int PolarC(ImageD imgs, ImageD imgd,
             double x, double y, double& r, double& fi,
             double r1 = 1, double r2 = 0, int sym = 2);
}
#endif


