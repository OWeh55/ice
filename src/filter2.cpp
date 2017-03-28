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

#include <stdlib.h>
#include <stddef.h>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "arith.h"
#include "util.h"

#include "filter.h"

namespace ice
{
  /************* laplace filters ****************/

#define FNAME "LaplaceYImg"
  /**
   * This template function applies the vertical Laplace filter on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  int LaplaceYImg(const Image& src, const Image& dest, int norm = 1)
  {
    int dx, dy;
    int vx1, vx2, vx3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    ReturnErrorIfFailed(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      tmp.copy(src);

    int gmax1 = src->maxval * 12;
    int gmax2 = dest->maxval * norm;
    int dmax = dest->maxval;
    int offset = (dest->maxval + 1) / 2;
    int srcwyi = 0;
    int srcwxi = 0;
    int destwyi1 = 1;
    int destwxi1 = 1;
    int destwxim1 = -1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int y = 0; y < dy - 2; y++)
      {
        vx1 = Pixels[y + srcwyi][srcwxi]
              - 2 * Pixels[y + srcwyi + 1][srcwxi]
              + Pixels[y + srcwyi + 2][srcwxi];
        vx2 = Pixels[y + srcwyi][srcwxi + 1]
              - 2 * Pixels[y + srcwyi + 1][srcwxi + 1]
              + Pixels[y + srcwyi + 2][srcwxi + 1];
        vx3 = Pixels[y + srcwyi][srcwxi + 2]
              - 2 * Pixels[y + srcwyi + 1][srcwxi + 2]
              + Pixels[y + srcwyi + 2][srcwxi + 2];
        Pixeld[y + destwyi1][destwxi1] = limited((vx1 + vx3 + vx2) * gmax2 / gmax1 + offset, dmax);

        for (int x = 3; x < dx; x++)
          {
            vx1 = vx2;
            vx2 = vx3;
            vx3 = Pixels[y + srcwyi][x + srcwxi]
                  - 2 * Pixels[y + srcwyi + 1][x + srcwxi]
                  + Pixels[y + srcwyi + 2][x + srcwxi];
            Pixeld[y + destwyi1][x + destwxim1] = limited((vx1 + vx3 + vx2) * gmax2 / gmax1 + offset, dmax);
          }
      }

#ifdef CONTROLLED_REFRESH
#ifndef NOVISUAL
    dest->needRefresh();
#endif
#endif
    return OK;
  }

  int LaplaceYImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int dx, dy;
    int vx1, vx2, vx3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    ReturnErrorIfFailed(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      tmp.copy(src);

    int gmax1 = src->maxval * 12;
    int gmax2 = dest->maxval * norm;
    int offset = (dest->maxval + 1) / 2;
    int dmax = dest->maxval;
    int srcwyi = 0;
    int srcwxi = 0;
    int destwyi1 = 1;
    int destwxi1 = 1;
    int destwxim1 = -1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    for (int y = 0; y < dy - 2; y++)
      {
        vx1 = tmp.getPixelUnchecked(srcwxi, y + srcwyi)
              - 2 * tmp.getPixelUnchecked(srcwxi, y + srcwyi + 1)
              + tmp.getPixelUnchecked(srcwxi, y + srcwyi + 2);
        vx2 = tmp.getPixelUnchecked(srcwxi + 1, y + srcwyi)
              - 2 * tmp.getPixelUnchecked(srcwxi + 1, y + srcwyi + 1)
              + tmp.getPixelUnchecked(srcwxi + 1, y + srcwyi + 2);
        vx3 = tmp.getPixelUnchecked(srcwxi + 2, y + srcwyi)
              - 2 * tmp.getPixelUnchecked(srcwxi + 2, y + srcwyi + 1)
              + tmp.getPixelUnchecked(srcwxi + 2, y + srcwyi + 2);
        dest.setPixelUnchecked(destwxi1, y + destwyi1, limited((vx1 + vx3 + vx2)*gmax2 / gmax1 + offset, dmax));

        for (int x = 3; x < dx; x++)
          {
            vx1 = vx2;
            vx2 = vx3;
            vx3 = tmp.getPixelUnchecked(x + srcwxi, y + srcwyi)
                  - 2 * tmp.getPixelUnchecked(x + srcwxi, y + srcwyi + 1)
                  + tmp.getPixelUnchecked(x + srcwxi, y + srcwyi + 2);
            dest.setPixelUnchecked(x + destwxim1, y + destwyi1, limited((vx1 + vx3 + vx2)*gmax2 / gmax1 + offset, dmax));
          }
      }

#ifdef CONTROLLED_REFRESH
#ifndef NOVISUAL
    dest->needRefresh();
#endif
#endif
    return OK;
  }
#undef FNAME

#define FNAME "LaplaceXImg"
  /**
   * This template function applies the horizontal Laplace filter on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  int LaplaceXImg(const Image& src, const Image& dest, int norm = 1)
  {
    int dx, dy;
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    ReturnErrorIfFailed(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      tmp.copy(src);

    int gmax1 = src->maxval * 12;
    int gmax2 = dest->maxval * norm;
    int offset = (dest->maxval + 1) / 2;
    int dmax = dest->maxval;
    int srcwyi = 0;
    int srcwxi = 0;
    int destwyi1 = +1;
    int destwxi1 = +1;
    int destwyim1 = -1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int x = 0; x < dx - 2; x++)
      {
        vy1 = Pixels[srcwyi][x + srcwxi]
              - 2 * Pixels[srcwyi][x + srcwxi + 1]
              + Pixels[srcwyi][x + srcwxi + 2];
        vy2 = Pixels[srcwyi + 1][x + srcwxi]
              - 2 * Pixels[srcwyi + 1][x + srcwxi + 1]
              + Pixels[srcwyi + 1][x + srcwxi + 2];
        vy3 = Pixels[srcwyi + 2][x + srcwxi]
              - 2 * Pixels[srcwyi + 2][x + srcwxi + 1]
              + Pixels[srcwyi + 2][x + srcwxi + 2];
        Pixeld[destwyi1][x + destwxi1] = limited((vy1 + vy3 + vy2) * gmax2 / gmax1 + offset, dmax);

        for (int y = 3; y < dy; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = Pixels[y + srcwyi][x + srcwxi]
                  - 2 * Pixels[y + srcwyi][x + srcwxi + 1]
                  + Pixels[y + srcwyi][x + srcwxi + 2];
            Pixeld[y + destwyim1][x + destwxi1] = limited((vy1 + vy3 + vy2) * gmax2 / gmax1 + offset, dmax);
          }
      }

#ifdef CONTROLLED_REFRESH
#ifndef NOVISUAL
    dest->needRefresh();
#endif
#endif
    return OK;
  }

  int LaplaceXImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int dx, dy;
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    ReturnErrorIfFailed(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      tmp.copy(dest);

    int gmax1 = src->maxval * 12;
    int gmax2 = dest->maxval * norm;
    int offset = (dest->maxval + 1) / 2;
    int dmax = dest->maxval;
    int srcwyi = 0;
    int srcwxi = 0;
    int destwyi1 = +1;
    int destwxi1 = +1;
    int destwyim1 = -1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    for (int x = 0; x < dx - 2; x++)
      {
        vy1 = tmp.getPixelUnchecked(x + srcwxi, srcwyi)
              - 2 * tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi)
              + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi);
        vy2 = tmp.getPixelUnchecked(x + srcwxi, srcwyi + 1)
              - 2 * tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi + 1)
              + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi + 1);
        vy3 = tmp.getPixelUnchecked(x + srcwxi, srcwyi + 2)
              - 2 * tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi + 2)
              + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi + 2);
        dest.setPixelUnchecked(x + destwxi1, destwyi1, limited((vy1 + vy3 + vy2)*gmax2 / gmax1 + offset, dmax));

        for (int y = 3; y < dy; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = tmp.getPixelUnchecked(x + srcwxi, y + srcwyi)
                  - 2 * tmp.getPixelUnchecked(x + srcwxi + 1, y + srcwyi)
                  + tmp.getPixelUnchecked(x + srcwxi + 2, y + srcwyi);
            dest.setPixelUnchecked(x + destwxi1, y + destwyim1, limited((vy1 + vy3 + vy2)*gmax2 / gmax1 + offset, dmax));
          }
      }

#ifdef CONTROLLED_REFRESH
#ifndef NOVISUAL
    dest->needRefresh();
#endif
#endif
    return OK;
  }

#undef FNAME

#define FNAME "LaplaceImg"
  /**
   * This template function applies the Laplace filter on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  int LaplaceImg(const Image& src, const Image& dest, int norm = 1)
  {
    int dx, dy;
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    ReturnErrorIfFailed(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      tmp.copy(src);

    int gmax1 = src->maxval * 16;
    int gmax2 = dest->maxval * norm;
    int dmax = dest->maxval;
    int offset = (dest->maxval + 1) / 2;
    int srcwyi = 0;
    int srcwxi = 0;
    int srcwxi1 = + 1;
    int srcwyim1 = - 1;
    int srcwyi1 =  + 1;
    int destwyi1 =  + 1;
    int destwyim1 =  - 1;
    int destwxi1 = + 1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int x = 0; x < dx - 2; x++)
      {
        vy1 = 2 * (Pixels[srcwyi][x + srcwxi]
                   + Pixels[srcwyi][x + srcwxi + 2])
              -  Pixels[srcwyi][x + srcwxi + 1];
        vy2 = 2 * (Pixels[srcwyi + 1][x + srcwxi]
                   + Pixels[srcwyi + 1][x + srcwxi + 2])
              - Pixels[srcwyi + 1][x + srcwxi + 1];
        vy3 = 2 * (Pixels[srcwyi + 2][x + srcwxi]
                   + Pixels[srcwyi + 2][x + srcwxi + 2])
              - Pixels[srcwyi + 2][x + srcwxi + 1];
        Pixeld[destwyi1][x + destwxi1] = limited(((vy1 + vy3) - (vy2 + 9 * Pixels[srcwyi1][x + srcwxi1]) / 2) * gmax2 / gmax1 + offset, dmax);

        for (int y = 3; y < dy; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = 2 * (Pixels[y + srcwyi][x + srcwxi] + Pixels[y + srcwyi][x + srcwxi + 2])
                  - Pixels[y + srcwyi][x + srcwxi + 1];
            Pixeld[y + destwyim1][x + destwxi1] = limited(((vy1 + vy3) - (vy2 + 9 * Pixels[y + srcwyim1][x + srcwxi1]) / 2) * gmax2 / gmax1 + offset, dmax);
          }
      }

#ifdef CONTROLLED_REFRESH
#ifndef NOVISUAL
    dest->needRefresh();
#endif
#endif
    return OK;
  }

  int LaplaceImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int dx, dy;
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    ReturnErrorIfFailed(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      tmp.copy(src);

    int gmax1 = src->maxval * 16;
    int gmax2 = dest->maxval * norm;
    int dmax = dest->maxval;
    int offset = (dest->maxval + 1) / 2;
    int srcwyi = 0;
    int srcwxi = 0;
    int srcwxi1 =  + 1;
    int srcwyim1 =  - 1;
    int srcwyi1 =  + 1;
    int destwyi1 =  + 1;
    int destwyim1 =  - 1;
    int destwxi1 = + 1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    for (int x = 0; x < dx - 2; x++)
      {
        vy1 = 2 * (tmp.getPixelUnchecked(x + srcwxi, srcwyi)
                   + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi))
              - tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi);
        vy2 = 2 * (tmp.getPixelUnchecked(x + srcwxi, srcwyi + 1)
                   + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi + 1))
              - tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi + 1);
        vy3 = 2 * (tmp.getPixelUnchecked(x + srcwxi, srcwyi + 2)
                   + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi + 2))
              - tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi + 2);
        dest.setPixelUnchecked(x + destwxi1, destwyi1,
                               limited(((vy1 + vy3) - (vy2 + 9 * tmp.getPixelUnchecked(x + srcwxi1, srcwyi1)) / 2) * gmax2 / gmax1 + offset, dmax));

        for (int y = 3; y < dy; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = 2 * (tmp.getPixelUnchecked(x + srcwxi, y + srcwyi)
                       + tmp.getPixelUnchecked(x + srcwxi + 2, y + srcwyi))
                  - tmp.getPixelUnchecked(x + srcwxi + 1, y + srcwyi);
            dest.setPixelUnchecked(x + destwxi1, y + destwyim1,
                                   limited(((vy1 + vy3) - (vy2 + 9 * tmp.getPixelUnchecked(x + srcwxi1, y + srcwyim1)) / 2)*gmax2 / gmax1 + offset, dmax));
          }
      }

#ifndef NOVISUAL
#ifdef CONTROLLED_REFRESH
    dest->needRefresh();
#endif
#endif
    return OK;
  }
#undef FNAME

  int LaplaceXImg(const Image& src, const Image& dest, int norm)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return LaplaceXImg<unsigned char, unsigned char>(src, dest, norm);
      case 18:
        return LaplaceXImg<unsigned char, unsigned short>(src, dest, norm);
      case 19:
        return LaplaceXImg<unsigned char, unsigned int>(src, dest, norm);
      case 33:
        return LaplaceXImg<unsigned short, unsigned char>(src, dest, norm);
      case 34:
        return LaplaceXImg<unsigned short, unsigned short>(src, dest, norm);
      case 35:
        return LaplaceXImg<unsigned short, unsigned int>(src, dest, norm);
      case 49:
        return LaplaceXImg<unsigned int, unsigned char>(src, dest, norm);
      case 50:
        return LaplaceXImg<unsigned int, unsigned short>(src, dest, norm);
      case 51:
        return LaplaceXImg<unsigned int, unsigned int>(src, dest, norm);
      default:
        return LaplaceXImg_std(src, dest, norm);
      }
  }

  int LaplaceYImg(const Image& src, const Image& dest, int norm)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return LaplaceYImg<unsigned char, unsigned char>(src, dest, norm);
      case 18:
        return LaplaceYImg<unsigned char, unsigned short>(src, dest, norm);
      case 19:
        return LaplaceYImg<unsigned char, unsigned int>(src, dest, norm);
      case 33:
        return LaplaceYImg<unsigned short, unsigned char>(src, dest, norm);
      case 34:
        return LaplaceYImg<unsigned short, unsigned short>(src, dest, norm);
      case 35:
        return LaplaceYImg<unsigned short, unsigned int>(src, dest, norm);
      case 49:
        return LaplaceYImg<unsigned int, unsigned char>(src, dest, norm);
      case 50:
        return LaplaceYImg<unsigned int, unsigned short>(src, dest, norm);
      case 51:
        return LaplaceYImg<unsigned int, unsigned int>(src, dest, norm);
      default:
        return LaplaceYImg_std(src, dest, norm);
      }
  }

#define FNAME "LaplaceImg"
  int LaplaceImg(const Image& src, const Image& dest, int norm)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return LaplaceImg<unsigned char, unsigned char>(src, dest, norm);
      case 18:
        return LaplaceImg<unsigned char, unsigned short>(src, dest, norm);
      case 19:
        return LaplaceImg<unsigned char, unsigned int>(src, dest, norm);
      case 33:
        return LaplaceImg<unsigned short, unsigned char>(src, dest, norm);
      case 34:
        return LaplaceImg<unsigned short, unsigned short>(src, dest, norm);
      case 35:
        return LaplaceImg<unsigned short, unsigned int>(src, dest, norm);
      case 49:
        return LaplaceImg<unsigned int, unsigned char>(src, dest, norm);
      case 50:
        return LaplaceImg<unsigned int, unsigned short>(src, dest, norm);
      case 51:
        return LaplaceImg<unsigned int, unsigned int>(src, dest, norm);
      default:
        return LaplaceImg_std(src, dest, norm);
      }
  }
#undef FNAME

  /**
   * The versions with old parameter order just call the new ones
   */
  int LaplaceXImg(const Image& src, int norm, const Image& dest)
  {
    return LaplaceXImg(src, dest, norm);
  }

  int LaplaceYImg(const Image& src, int norm, const Image& dest)
  {
    return LaplaceYImg(src, dest, norm);
  }

  int LaplaceImg(const Image& src, int norm, const Image& dest)
  {
    return LaplaceImg(src, dest, norm);
  }

#undef FNAME
}
