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
  int SmearImg(const ColorImage& src, const ColorImage& dest, int n)
  {
    return SmearImg(src, dest, n, n);
  }

  int SmearImg(const ColorImage& src, const ColorImage& dest, int nx, int ny)
  {
    if (!src.isValid() || !dest.isValid())
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED((SmearImg(src.redImage(), dest.redImage(), nx, ny)));
    RETURN_ERROR_IF_FAILED((SmearImg(src.greenImage(), dest.greenImage(), nx, ny)));
    RETURN_ERROR_IF_FAILED((SmearImg(src.blueImage(), dest.blueImage(), nx, ny)));

    return OK;
  }
#undef FNAME

#define FNAME "DoBImg"
  int DoBImg(const ColorImage& src, const ColorImage& dest, int n1, int n2, int smode)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));
    RETURN_ERROR_IF_FAILED(DoBImg(src.redImage(), dest.redImage(), n1, n2, smode));
    RETURN_ERROR_IF_FAILED(DoBImg(src.greenImage(), dest.greenImage(), n1, n2, smode));
    RETURN_ERROR_IF_FAILED(DoBImg(src.blueImage(), dest.blueImage(), n1, n2, smode));
    return OK;
  }
#undef FNAME

#define FNAME "MeanImg"
  int MeanImg(const ColorImage& src, const ColorImage& dest)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));
    RETURN_ERROR_IF_FAILED((MeanImg(src.redImage(), dest.redImage())));
    RETURN_ERROR_IF_FAILED((MeanImg(src.greenImage(), dest.greenImage())));
    RETURN_ERROR_IF_FAILED((MeanImg(src.blueImage(), dest.blueImage())));

    return OK;
  }
#undef FNAME

#define FNAME "GaussImg"
  int GaussImg(const ColorImage& src, int neighb, double sigma, const ColorImage& dest)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));
    RETURN_ERROR_IF_FAILED((GaussImg(src.redImage(), dest.redImage(), neighb, sigma)));
    RETURN_ERROR_IF_FAILED((GaussImg(src.greenImage(), dest.greenImage(), neighb, sigma)));
    RETURN_ERROR_IF_FAILED((GaussImg(src.blueImage(), dest.blueImage(), neighb, sigma)));

    return OK;
  }
#undef FNAME

  /****************************** ****************************/

#define FNAME "GradXImg"
  int GradXImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((GradXImg(src.redImage(), dest.redImage())));
    RETURN_ERROR_IF_FAILED((GradXImg(src.greenImage(), dest.greenImage())));
    RETURN_ERROR_IF_FAILED((GradXImg(src.blueImage(), dest.blueImage())));

    return OK;
  }
#undef FNAME

#define FNAME "GradYImg"
  int GradYImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((GradYImg(src.redImage(), dest.redImage(), norm)));
    RETURN_ERROR_IF_FAILED((GradYImg(src.greenImage(), dest.greenImage(), norm)));
    RETURN_ERROR_IF_FAILED((GradYImg(src.blueImage(), dest.blueImage(), norm)));

    return OK;
  }
#undef FNAME

#define FNAME "LaplaceXImg"
  int LaplaceXImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LaplaceXImg(src.redImage(), dest.redImage(), norm)));
    RETURN_ERROR_IF_FAILED((LaplaceXImg(src.greenImage(), dest.greenImage(), norm)));
    RETURN_ERROR_IF_FAILED((LaplaceXImg(src.blueImage(), dest.blueImage(), norm)));

    return OK;
  }
#undef FNAME

#define FNAME "LaplaceYImg"
  int LaplaceYImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LaplaceYImg(src.redImage(), dest.redImage(), norm)));
    RETURN_ERROR_IF_FAILED((LaplaceYImg(src.greenImage(), dest.greenImage(), norm)));
    RETURN_ERROR_IF_FAILED((LaplaceYImg(src.blueImage(), dest.blueImage(), norm)));

    return OK;
  }
#undef FNAME

#define FNAME "LaplaceImg"
  int LaplaceImg(const ColorImage& src, const ColorImage& dest, int norm)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LaplaceImg(src.redImage(), dest.redImage(), norm)));
    RETURN_ERROR_IF_FAILED((LaplaceImg(src.greenImage(), dest.greenImage(), norm)));
    RETURN_ERROR_IF_FAILED((LaplaceImg(src.blueImage(), dest.blueImage(), norm)));

    return OK;
  }
#undef FNAME

#define FNAME "LSIImg"
  int LSIImg(const ColorImage& src, const ColorImage& dest, int nx, int ny, int* mask, int norm, int offset)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LSIImg(src.redImage(), dest.redImage(), nx, ny, mask, norm, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.greenImage(), dest.greenImage(), nx, ny, mask, norm, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.blueImage(), dest.blueImage(), nx, ny, mask, norm, offset)));

    return OK;
  }

  int LSIImg(const ColorImage& src, const ColorImage& dest, int nx, int ny, double* mask, int offset)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LSIImg(src.redImage(), dest.redImage(), nx, ny, mask, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.greenImage(), dest.greenImage(), nx, ny, mask, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.blueImage(), dest.blueImage(), nx, ny, mask, offset)));

    return OK;
  }

  int LSIImg(const ColorImage& src, const ColorImage& dest, const Matrix& mask, int offset)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LSIImg(src.redImage(), dest.redImage(), mask, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.greenImage(), dest.greenImage(), mask, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.blueImage(), dest.blueImage(), mask, offset)));

    return OK;
  }

  int LSIImg(const ColorImage& src, const ColorImage& dest, const IMatrix& mask, int norm, int offset)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((LSIImg(src.redImage(), dest.redImage(), mask, norm, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.greenImage(), dest.greenImage(), mask, norm, offset)));
    RETURN_ERROR_IF_FAILED((LSIImg(src.blueImage(), dest.blueImage(), mask, norm, offset)));

    return OK;
  }
#undef FNAME

#define FNAME "MexicanHatImg"
  int MexicanHatImg(const ColorImage& src, const ColorImage& dest, double sigma, int neighb)
  {
    RETURN_ERROR_IF_FAILED(src.match(dest));

    RETURN_ERROR_IF_FAILED((MexicanHatImg(src.redImage(), dest.redImage(), sigma, neighb)));
    RETURN_ERROR_IF_FAILED((MexicanHatImg(src.greenImage(), dest.greenImage(), sigma, neighb)));
    RETURN_ERROR_IF_FAILED((MexicanHatImg(src.blueImage(), dest.blueImage(), sigma, neighb)));

    return OK;
  }
#undef FNAME

} /* namespace ice */
