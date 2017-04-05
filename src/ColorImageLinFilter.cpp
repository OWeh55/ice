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
 * This source file provides the implementation of some linear local
 * filters for modifying ColorImages.
 * Applying them in RGB or YUV color space will lead to the same result
 * In HSI the result may look strange, because the H channel is periodic
 * and so value 1 and 255 are closer together than 1 and 200 for example.
 * Anyway no automatic conversion will be done, to provide the filters to be
 * used in any color space.
 *
 * int SmearImg(const ColorImage &src, const ColorImage &dest, int n=3)
 * int SmearImg(const ColorImage &src, const ColorImage &dest, int nx, int ny)
 * int DoBImg(const ColorImage &src, const ColorImage &dest, int n1, int n2, int mode=MD_NORMALIZE)
 * int GradXImg(const ColorImage &src, const ColorImage &dest, int norm=1)
 * int GradYImg(const ColorImage &src, const ColorImage &dest, int norm=1)
 * int MeanImg(const ColorImage &src, const ColorImage &dest)
 * int LaplaceXImg(const ColorImage &src, const ColorImage &dest, int norm=1)
 * int LaplaceYImg(const ColorImage &src, const ColorImage &dest, int norm=1)
 * int LaplaceImg(const ColorImage &src, const ColorImage &dest, int norm=1)
 * int LSIImg(const ColorImage &src, const ColorImage &dest, int nx, int ny, int *mask, int norm, int offset)
 * int LSIImg(const ColorImage &src, const ColorImage &dest, int nx, int ny, double *mask, int offset)
 * int LSIImg(const ColorImage &src, const ColorImage &dest, Matrix *mask, int offset)
 * int LSIImg(const ColorImage &src, const ColorImage &dest, IMatrix *mask, int norm, int offset)
 * int MexicanImg(const ColorImage &src, const ColorImage &dest, int neighb, double sigma1, double sigma2)
 *
 * "ColorFunctionsLinLoc.cpp" - Niels Oertel 2008
 ***********************************************************************/

#include "ColorImageFunctions.h"
#include "filter.h"
#include "lsifilter.h"
#include "macro.h"

namespace ice
{
  /********************* SMOOTHNESS FILTERS ********************/

#define FNAME "SmearImg"
  void smearImg(const ColorImage& src, const ColorImage& dest, int n)
  {
    smearImg(src, dest, n, n);
  }

  void smearImg(const ColorImage& src, const ColorImage& dest, int nx, int ny)
  {
    try
      {
        SmearImg(src.redImage(), dest.redImage(), nx, ny);
        SmearImg(src.greenImage(), dest.greenImage(), nx, ny);
        SmearImg(src.blueImage(), dest.blueImage(), nx, ny);
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
