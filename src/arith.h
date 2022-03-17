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
/*********************************************************************/
/*  Bildarithmetik                                                   */
/*********************************************************************/

#ifndef _ARITH_H
#define _ARITH_H

#include "base.h"

/* Prozeduren der Bildarithmetik */
namespace ice
{

#define MD_NORMALIZE 1
#define MD_LIMIT 2
#define MD_MODULO 3

#define SMD_ABSOLUTE 1
#define SMD_POSITIVE 2
#define SMD_SHIFT 3

// unäre Operatoren Op(imgs) -> imgd
  void binImg(const Image& imgs, int thr, const Image& imgd);
  void binImg(const Image& imgs, const Image& imgd, int thr = 1, int val = -1);
  void binImg(const Image& imgs, int thr = 1);

  void scaleImg(const Image& imgs, int a, int b, const Image& imgd);
  void scaleImg(const Image& imgs, int a, int b);
  void invertImg(const Image& imgs, const Image& imgd);
  void invertImg(const Image& imgs);

// binäre Operatoren Op(imgs1,imgs2) -> imgd
  void addImg(const Image& imgs1, const Image& imgs2, const Image& imgd, int mode = MD_LIMIT);

  void subImg(const Image& imgs1, const Image& imgs2,
              int smode, const Image& imgd, int mode = MD_LIMIT);
  void subImg(const Image& imgs1, const Image& imgs2, const Image& imgd,
              int smode = SMD_ABSOLUTE, int mode = MD_LIMIT);

  void maxImg(const Image& imgs1, const Image& imgs2, const Image& imgd,
              int mode = MD_LIMIT);
  void minImg(const Image& imgs1, const Image& imgs2, const Image& imgd,
              int mode = MD_LIMIT);

  void renormImg(const Image& img, const Image& imgd);

  void shift(const Image& simg, const Image& dimg, int dx, int dy, int mode = 0);

  bool equalImg(const Image& img1, const Image& img2);

  /**
   * Searches the image for its maximum value
   *
   * @param img the image to be browsed
   * @param x the x-position of the found pixel
   * @param y the y-position of the found pixel
   * @return the maximum grey value in the image
   */
  int findMax(const Image& img, int& x, int& y);
  int findMax(const Image& img, IPoint& p);
  int findMax(const Image& img);

  /**
   * Searches the image for its minimum value
   *
   * @param img the image to be browsed
   * @param x the x-position of the found pixel
   * @param y the y-position of the found pixel
   * @return the maximum grey value in the image
   */
  int findMin(const Image& img, int& x, int& y);
  int findMin(const Image& img, IPoint& p);
  int findMin(const Image& img);

}
#endif
