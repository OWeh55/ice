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

#include "ColorImageFunctions.h"
#include "filter.h"
#include "LsiFilter.h"
#include "macro.h"

namespace ice
{
  /********************* SMOOTHNESS FILTERS ********************/

#define FNAME "smearImg"
  void smearImg(const ColorImage& src, const ColorImage& dest, int n)
  {
    smearImg(src, dest, n, n);
  }

  void smearImg(const ColorImage& src, const ColorImage& dest, int nx, int ny)
  {
    try
      {
        smearImg(src.redImage(), dest.redImage(), nx, ny);
        smearImg(src.greenImage(), dest.greenImage(), nx, ny);
        smearImg(src.blueImage(), dest.blueImage(), nx, ny);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "DoBImg"
  void DoBImg(const ColorImage& src, const ColorImage& dest, int n1, int n2, int smode)
  {
    try
      {
        DoBImg(src.redImage(), dest.redImage(), n1, n2, smode);
        DoBImg(src.greenImage(), dest.greenImage(), n1, n2, smode);
        DoBImg(src.blueImage(), dest.blueImage(), n1, n2, smode);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "MeanImg"
  void MeanImg(const ColorImage& src, const ColorImage& dest)
  {
    try
      {
        MeanImg(src.redImage(), dest.redImage());
        MeanImg(src.greenImage(), dest.greenImage());
        MeanImg(src.blueImage(), dest.blueImage());
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "GaussImg"
  void GaussImg(const ColorImage& src, int neighb, double sigma, const ColorImage& dest)
  {
    try
      {
        GaussImg(src.redImage(), dest.redImage(), neighb, sigma);
        GaussImg(src.greenImage(), dest.greenImage(), neighb, sigma);
        GaussImg(src.blueImage(), dest.blueImage(), neighb, sigma);
      }
    RETHROW;
  }
#undef FNAME

  /****************************** ****************************/

#define FNAME "GradXImg"
  void GradXImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    try
      {
        GradXImg(src.redImage(), dest.redImage());
        GradXImg(src.greenImage(), dest.greenImage());
        GradXImg(src.blueImage(), dest.blueImage());
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "GradYImg"
  void GradYImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    try
      {
        GradYImg(src.redImage(), dest.redImage(), norm);
        GradYImg(src.greenImage(), dest.greenImage(), norm);
        GradYImg(src.blueImage(), dest.blueImage(), norm);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "LaplaceXImg"
  void LaplaceXImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    try
      {
        LaplaceXImg(src.redImage(), dest.redImage(), norm);
        LaplaceXImg(src.greenImage(), dest.greenImage(), norm);
        LaplaceXImg(src.blueImage(), dest.blueImage(), norm);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "LaplaceYImg"
  void LaplaceYImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    try
      {
        LaplaceYImg(src.redImage(), dest.redImage(), norm);
        LaplaceYImg(src.greenImage(), dest.greenImage(), norm);
        LaplaceYImg(src.blueImage(), dest.blueImage(), norm);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "LaplaceImg"
  void LaplaceImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    try
      {
        LaplaceImg(src.redImage(), dest.redImage(), norm);
        LaplaceImg(src.greenImage(), dest.greenImage(), norm);
        LaplaceImg(src.blueImage(), dest.blueImage(), norm);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "LSIImg"
  void LSIImg(const ColorImage& src, const ColorImage& dest,
              int nx, int ny, int* mask, int norm, int offset)
  {
    try
      {
        LSIImg(src.redImage(), dest.redImage(), nx, ny, mask, norm, offset);
        LSIImg(src.greenImage(), dest.greenImage(), nx, ny, mask, norm, offset);
        LSIImg(src.blueImage(), dest.blueImage(), nx, ny, mask, norm, offset);
      }
    RETHROW;
  }

  void LSIImg(const ColorImage& src, const ColorImage& dest,
              int nx, int ny, double* mask, int offset)
  {
    try
      {
        LSIImg(src.redImage(), dest.redImage(), nx, ny, mask, offset);
        LSIImg(src.greenImage(), dest.greenImage(), nx, ny, mask, offset);
        LSIImg(src.blueImage(), dest.blueImage(), nx, ny, mask, offset);
      }
    RETHROW;
  }

  void LSIImg(const ColorImage& src, const ColorImage& dest, const Matrix& mask, int offset)
  {
    try
      {
        LSIImg(src.redImage(), dest.redImage(), mask, offset);
        LSIImg(src.greenImage(), dest.greenImage(), mask, offset);
        LSIImg(src.blueImage(), dest.blueImage(), mask, offset);
      }
    RETHROW;
  }

  void LSIImg(const ColorImage& src, const ColorImage& dest,
              const IMatrix& mask, int norm, int offset)
  {
    try
      {
        LSIImg(src.redImage(), dest.redImage(), mask, norm, offset);
        LSIImg(src.greenImage(), dest.greenImage(), mask, norm, offset);
        LSIImg(src.blueImage(), dest.blueImage(), mask, norm, offset);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "MexicanHatImg"
  void MexicanHatImg(const ColorImage& src, const ColorImage& dest, double sigma, int neighb)
  {
    try
      {
        MexicanHatImg(src.redImage(), dest.redImage(), sigma, neighb);
        MexicanHatImg(src.greenImage(), dest.greenImage(), sigma, neighb);
        MexicanHatImg(src.blueImage(), dest.blueImage(), sigma, neighb);
      }
    RETHROW;
  }
#undef FNAME

} /* namespace ice */
