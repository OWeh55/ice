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
/* Phasenkorellation */
// ttbb (c) 1998

#include <float.h>
#include <stdlib.h>

#include "macro.h"
#include "PhaseCor.h"
#include "MtchTool.h"
#include "complex.h"
#include "darith.h"
#include "fourier.h"

/**************************************************************************/
namespace ice
{
#define FNAME "PhaseCorrelationImgD"
  int PhaseCorrelationImgD(const ImageD& imgd1, const ImageD& imgd2, const ImageD& img_dest)
  {
    int xs, ys;

    RETURN_ERROR_IF_FAILED(MatchImgD(imgd1, imgd2, img_dest, xs, ys));

    ImageD imgd1i = NewImgD(xs, ys, -DBL_MAX, DBL_MAX);
    ImageD imgd2i = NewImgD(xs, ys, -DBL_MAX, DBL_MAX);
    ImageD imgd3  = NewImgD(xs, ys, -DBL_MAX, DBL_MAX);
    ImageD imgd3i = NewImgD(xs, ys, -DBL_MAX, DBL_MAX);

    SetImgD(imgd1i, 0.0);
    SetImgD(imgd2i, 0.0);

    FourierImgD(imgd1, imgd1i, NORMAL, imgd3, imgd3i);
    FourierImgD(imgd2, imgd2i, NORMAL, imgd1, imgd1i);

    for (int i = 0; i < xs; ++i)
      for (int j = 0; j < ys; ++j)
        {
          double r1 = GetValD(imgd3, i, j);
          double i1 = GetValD(imgd3i, i, j);
          double r2 = GetValD(imgd1, i, j);
          double i2 = GetValD(imgd1i, i, j);

          // rc = \overline{c1} * c2
          double rr = r1 * r2 + i1 * i2;
          double ir = r1 * i2 - i1 * r2;

          double abs12 = r1 * r1 + i1 * i1;
          double abs22 = r2 * r2 + i2 * i2;

          double arc1 = sqrt(abs12 * abs22);

          if (arc1 < 1e-10)
            {
              rr = 0.0;
              ir = 0.0;
            }
          else
            {
              rr /= arc1;
              ir /= arc1;
            }

          PutValD(imgd2, i, j, rr);
          PutValD(imgd2i, i, j, ir);
        }

    FourierImgD(imgd2, imgd2i, INVERS, img_dest, imgd1i);
    return OK;
  }

#undef  FNAME
#define FNAME "PhaseCorrelationImg"

  int PhaseCorrelationImg(Image img1, Image img2, Image img)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(img1, img2, img, xs, ys));

    ImageD imgd1 = NewImgD(img1, true);
    ImageD imgd2 = NewImgD(img2, true);
    ImageD imgd = NewImgD(img);

    RETURN_ERROR_IF_FAILED(PhaseCorrelationImgD(imgd1, imgd2, imgd));

    int x, y;
    double gmaxabs = 0, gmin = DBL_MAX, gmax = -DBL_MAX, g;
    wloop(img, x, y)
    {
      if (fabs(g = GetValD(imgd, x, y)) > gmaxabs)
        {
          gmaxabs = fabs(g);
        }

      if (g < gmin)
        {
          gmin = g;
        }

      if (g > gmax)
        {
          gmax = g;
        }
    }

    if (gmaxabs > 0)
      {

        if ((gmax >= 0 && gmin >= 0) || (gmax <= 0 && gmin <= 0))
          {
            if (gmax != gmin)
              {
                wloop(img, x, y)
                PutVal(img, x, y, int((GetValD(imgd, x, y) - gmin) * ((img->maxval - 1) / 2) / (gmax - gmin)) + (img->maxval + 1) / 2);
              }
            else
              {
                SetImg(img, (img->maxval + 1) / 2);
              }
          }
        else
          {
            gmaxabs += gmaxabs;
            wloop(img, x, y)
            PutVal(img, x, y, int(GetValD(imgd, x, y) / gmaxabs * (img->maxval - 1) / 2) + (img->maxval + 1) / 2);
          }
      }
    else
      {
        SetImg(img, (img->maxval + 1) / 2);
      }

    FreeImgD(imgd);

    return OK;
  }
#undef FNAME
}
