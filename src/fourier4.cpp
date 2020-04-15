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
/*
 * Operations on Fourier and Hartley spectrum (twodimensional)
 * Wolfgang Ortmann, 97, 98
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "darith.h"

#include "fourier.h"

//-----------------------------------------------------------------------
// Whitening of spectrum
//-----------------------------------------------------------------------
namespace ice
{
#define FNAME "WhiteningFImgD"
  void WhiteningFImgD(ImageD re, ImageD im, ImageD dre, ImageD dim, double beta)
  {
    try
      {

        PixelFloatType di, dr, did, drd, denom;

        if (beta < 0)
          throw IceException(FNAME, M_WRONG_PARAMETER);

        double beta2 = beta * beta;

        int xs, ys;
        MatchImgD(re, im, dre, xs, ys);
        MatchImgD(dre, dim, xs, ys);

        for (int y = 0; y < re.ysize; ++y)
          for (int x = 0; x < re.xsize; ++x)
            {
              dr = GetValD(re, x, y);
              di = GetValD(im, x, y);
              denom = sqrt(dr * dr + di * di) + beta2;

              if (denom == 0)
                {
                  drd = 0;
                  did = 0;
                }
              else
                {
                  drd = dr / denom;
                  did = di / denom;
                }

              PutValD(dre, x, y, drd);
              PutValD(dim, x, y, did);
            }
      }
    RETHROW;
  }
#undef FNAME
//-----------------------------------------
#define FNAME "WhiteningHImgD"
  void WhiteningHImgD(ImageD im, ImageD nrm, double beta)
  {
    try
      {
        if (beta < 0)
          throw IceException(FNAME, M_WRONG_PARAMETER);

        double beta2 = beta * beta;

        int xs, ys;
        MatchImgD(im, nrm, xs, ys);

        ImageD h = NewImgD(xs, ys);

        PixelFloatType r, i, p;

        for (int y = 0; y < ys; y++)
          {
            int yn = negf(y, ys);

            for (int x = 0; x < xs; x++)
              {
                int xn = negf(x, xs);
                r = GetValD(im, x, y);
                i = GetValD(im, xn, yn);
                p = sqrt((r * r + i * i) / 2) + beta2;

                if (p != 0)
                  {
                    p = r / p;
                  }

                PutValD(h, x, y, p);
              }
          }

        for (int y = 0; y < h.ysize; ++y)
          for (int x = 0; x < h.xsize; ++x)
            {
              PutValD(nrm, x, y, GetValD(h, x, y));
            }
      }
    RETHROW;
  }
#undef FNAME
//
#define FNAME "WhiteningImgD"
  void WhiteningImgD(ImageD im1, ImageD im2, double beta)
  {
    try
      {
        HartleyImgD(im1, im2);
        WhiteningHImgD(im2, im2, beta);
        HartleyImgD(im2, im2);
      }
    RETHROW;
  }
#undef FNAME
}
