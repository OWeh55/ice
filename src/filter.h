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
/************************************************/
/*            Filter                            */
/************************************************/
#ifndef _FILTER_H
#define _FILTER_H

#include "base.h"

#include "Matrix.h"
#include "based.h"
#include "arith.h"
#include "ColorImage.h"

namespace ice
{
  // filter1.cpp
  void GradXImg(const Image& imgs, const Image& imgd, int norm = 1);
  void GradYImg(const Image& imgs, const Image& imgd, int norm = 1);

  void GradImg(const Image& imgs, const Image& imgd, int norm = 1);

  int CalcDirectionStructImg(const Image& pic, // zu untersuchendes Bild
                             const Image& dest, // "Winkel"-Bild
                             int detectionsize, // Umgebung
                             ImageD lambda1 = ImageD(), ImageD lambda2 = ImageD());

  int CalcDirectionImg(const Image& pic, // zu untersuchendes Bild
                       const Image& dest, // "Winkel"-Bild
                       int detectionsize = 11, // Umgebung
                       ImageD eval = ImageD()
                      );

  void GradDirImg(const Image& imgs, const Image& imgd);

  // filter2.cpp
  int LaplaceXImg(const Image& imgs, int norm, const Image& imgd);
  int LaplaceYImg(const Image& imgs, int norm, const Image& imgd);
  int LaplaceImg(const Image& imgs, int norm, const Image& imgd);

  int LaplaceXImg(const Image& imgs, const Image& imgd, int norm = 1);
  int LaplaceYImg(const Image& imgs, const Image& imgd, int norm = 1);
  int LaplaceImg(const Image& imgs, const Image& imgd, int norm = 1);

  // filter3.cpp
  void smearImg(const Image& imgs, const Image& imgd, int sx, int sy);
  void smearImg(const Image& imgs, const Image& imgd, int sx = 3);
  void smearImg(const Image& imgs, int sx = 3);

  int BoxImg(const Image& imgs, const Image& imgd, int sx, int sy);
  int BoxImg(const Image& imgs, const Image& imgd, int sx = 3);
  int BoxImg(const Image& imgs, int sx = 3);

  int DoBImg(const Image& imgs, const Image& imgd, int n1, int n2, int mode = SMD_SHIFT);
  int DoBImg(const Image& img, int n1, int n2, int mode = SMD_SHIFT);

  int DoBMultiImg(const Image& imgs, const Image& imgd,
                  const IMatrix& filtersizes,
                  int boundary_x, int boundary_y, int scalef = 10,
                  bool use_gauss_filter = false);

  int MeanImg(const Image& imgs, const Image& imgd);

  void GaussImg(const Image& imgs, const Image& imgd, int neighb, double sigma);
  void GaussImg(const ImageD& imgs, const ImageD& imgd, int neighb, double sigma);

  void MexicanHatImg(const Image& imgs, const Image& imgd, double sigma, int size = 0);
  void MexicanHatImg(const Image& imgs, ImageD imgd, double sigma, int size = 0);
  void MexicanHatImg(ImageD imgs, ImageD imgd, double sigma, int size = 0);

  // filter4.cpp
#define DirectedSmearImg OrientedSmearImg
#define DirectedDoBImg OrientedDoBImg
#define DirectedEdgeImg OrientedEdgeImg

  void OrientedSmearImg(const Image& pic,
                        const Image& dir,
                        const Image& dest,
                        int filter_size = 11, int filter_length = 10, int filter_width = 1);

  int OrientedDoBImg(const Image& pic,
                     const Image& dir,
                     const Image& dest,
                     int filter_size = 11, int filter_length = 10, int filter_width = 1);

  int OrientedEdgeImg(const Image& pic,
                      const Image& dir,
                      const Image& dest,
                      int filter_size = 11, int filter_rad = 10);

  void OrientedSmearImg(const ColorImage& pic,
                        const Image& dir,
                        const ColorImage& dest,
                        int filter_size = 11, int filter_length = 10, int filter_width = 1);

  void OrientedDoBImg(const ColorImage& pic,
                      const Image& dir,
                      const ColorImage& dest,
                      int filter_size = 11, int filter_length = 10, int filter_width = 1);

  void OrientedEdgeImg(const ColorImage& pic,
                       const Image& dir,
                       const ColorImage& dest,
                       int filter_size = 11, int filter_rad = 10);
}
#endif
