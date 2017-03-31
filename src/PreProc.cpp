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
/* Vorverarbeitung von Bildern zur Unterdrückung von Fourierkreuzen
   im Hinblick auf Bildmatching */

/* ttbb 20.05.1998 */

#include <float.h>

#include "macro.h"
#include "PreProc.h"
#include "MtchTool.h"

namespace ice
{
  static const int mask[3][3] = {{1, 1, 1}, {0, 0, 0}, { -1, -1, -1}};

#define FNAME "MyGradImg"
  static Image MyGradImg(Image img, double& gmean)
  {
    Image imgd = NewImg(img);

    if (!IsImg(imgd))
      {
        throw IceException(FNAME, M_NO_MEM, NO_MEM);
        return Image();
      }

    int g, x, y;
    gmean = 0;
    int gradmax = 0;

    for (y = 1; y < img->xsize - 1; y++)
      for (x = 1; x < img->xsize - 1; x++)
        {
          double sum1 = 0, sum2 = 0;
          gmean += GetVal(img, x, y);

          for (int k = -1; k < 2; k++)
            for (int l = -1; l < 2; l++)
              {
                int xx = k + x, yy = l + y;
                sum1 += (g = GetVal(img, xx, yy)) * mask[k + 1][l + 1];
                sum2 += g * mask[l + 1][k + 1];
              }

          g = Min(imgd->maxval, Max(0, (int)(Sqr(sum1 / 9.0) + Sqr(sum2 / 9.0))));

          if (gradmax < g)
            {
              gradmax = g;
            }
        }

    for (x = 1; x < img->xsize - 1; x++)
      for (y = 1; y < img->ysize - 1; y++)
        {
          double sum1 = 0, sum2 = 0;

          for (int k = -1; k < 2; k++)
            for (int l = -1; l < 2; l++)
              {
                int xx = k + x, yy = l + y;
                sum1 += (g = GetVal(img, xx, yy)) * mask[k + 1][l + 1];
                sum2 += g * mask[l + 1][k + 1];
              }

          PutVal(imgd, x, y, Min(imgd->maxval, Max(0, (int)(imgd->maxval * sqrt(Sqr(sum1 / 9.0) + Sqr(sum2 / 9.0)) / gradmax))));
        }

    gmean /= (img->xsize * img->ysize);

    for (x = 0; x < img->xsize; x++)
      {
        PutVal(imgd, x, 0, GetVal(imgd, x, 1));
        PutVal(imgd, x, img->ysize - 1, GetVal(imgd, x, img->ysize - 2));
      }

    for (y = 0; y < img->ysize; y++)
      {
        PutVal(imgd, 0, y, GetVal(imgd, 1, y));
        PutVal(imgd, img->xsize - 1, y, GetVal(imgd, img->xsize - 2, y));
      }

    return imgd;
  }
#undef FNAME

#define FNAME "PreprocessImg"
  int PreprocessImg(Image img1, Image img2)
  {
    int dx, dy;
    RETURN_ERROR_IF_FAILED(MatchImg(img1, img2, dx, dy));
    int   newimg = false;
    Image imgd = img2;
    Image imgs = img1;

    if (img2 == img1)
      {
        imgs = NewImg(img1, true); // Kopie als Quelle erzeugen

        if (!IsImg(imgs))
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
            return NO_MEM;
          }

        newimg = true;
      }

    double gmean = 0.0; // avoid warning
    Image temp = MyGradImg(imgs, gmean);

    if (!IsImg(temp))
      {
        if (newimg)
          {
            FreeImg(imgs);
          }

        return NO_MEM;
      }

    int x, y, max = 0, g;
    double maxgauss = Gauss2D(0, 0, 0, 0, (img1->xsize) / 3.5, (img1->ysize) / 3.5, 0);


    for (y = 0; y < imgs->ysize; y++)
      {
        for (x = 0; x < imgs->xsize; x++)
          {
            g = GetVal(temp, x, y);
            g = (int)(g * (Gauss2D(x, y, (imgs->xsize) / 2, (imgs->ysize) / 2, (imgs->xsize) / 3.5, (imgs->ysize) / 3.5, 0) / maxgauss));
            PutVal(temp, x, y, g);

            if (g > max)
              {
                max = g;
              }
          }
      }

    if (max == 0)
      {
        for (y = 0; y < imgs->ysize; y++)
          for (x = 0; x < imgs->xsize; x++)
            {
              PutVal(imgd, x, y, GetVal(imgs, x, y));
            }
      }
    else
      {
        ImageD id = NewImgD(imgd->xsize, imgd->ysize, -DBL_MAX, DBL_MAX);

        for (y = 0; y < imgs->ysize; y++)
          for (x = 0; x < imgs->xsize; x++)
            {
              PutValD(id, x, y, ((GetVal(imgs, x, y) - gmean) * (20 * sqrt(double(GetVal(temp, x, y)) / double(max)) + 1.0) / 21.0));
            }

        ConvImgDImg(id, imgd);

        FreeImgD(id);
      }

    FreeImg(temp);

    if (newimg)
      {
        FreeImg(imgs);
      }

    return OK;
  }

#undef FNAME
}
