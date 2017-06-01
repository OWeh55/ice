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

#include <map>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "arith.h"
#include "util.h"
#include "filter.h"
#include "lsifilter.h"

namespace ice
{
  /************* some linear local filters ****************/
#define FNAME "MeanImg"
  /**
   * This template function applies the Mean filter on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   */
  template<typename SrcType, typename DestType>
  int MeanImg(const Image& src, const Image& dest)
  {
    int dx, dy;
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp = NewImg(dest, true);
      }

    int gmax1 = src.maxval * 9;
    int gmax2 = dest.maxval;
    int dmax = dest.maxval;
    int srcwyi = 0;
    int srcwxi = 0;
    int srcwxi1 = 1;
    int srcwyim1 = -1;
    int srcwyi1 = 1;
    int destwyi1 =  1;
    int destwyim1 = -1;
    int destwxi1 = +1;

    // the border of width 1 around the image will be filled with value 0
    setborder(dest, 1, 0);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int x = 0; x < dx - 2; x++)
      {
        vy1 = 2 * Pixels[srcwyi][x + srcwxi + 1]
              - (Pixels[srcwyi][x + srcwxi] + Pixels[srcwyi][x + srcwxi + 2]);
        vy2 = 2 * Pixels[srcwyi + 1][x + srcwxi + 1]
              - (Pixels[srcwyi + 1][x + srcwxi] + Pixels[srcwyi + 1][x + srcwxi + 2]);
        vy3 = 2 * Pixels[srcwyi + 2][x + srcwxi + 1]
              - (Pixels[srcwyi + 2][x + srcwxi] + Pixels[srcwyi + 2][x + srcwxi + 2]);
        Pixeld[destwyi1][x + destwxi1] = limited(((vy1 + vy3) - 2 * vy2 + 9 * Pixels[srcwyi1][x + srcwxi1]) * gmax2 / gmax1, dmax);

        for (int y = 3; y < dy; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = 2 * Pixels[y + srcwyi][x + srcwxi + 1]
                  - (Pixels[y + srcwyi][x + srcwxi] + Pixels[y + srcwyi][x + srcwxi + 2]);
            Pixeld[y + destwyim1][x + destwxi1] = limited(((vy1 + vy3) - 2 * vy2 + 9 * Pixels[y + srcwyim1][x + srcwxi1]) * gmax2 / gmax1, dmax);
          }
      }

    return OK;
  }

  int MeanImg_std(const Image& src, const Image& dest)
  {
    int dx, dy;
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest, dx, dy));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp = NewImg(dest, true);
      }

    int gmax1 = src.maxval * 9;
    int gmax2 = dest.maxval;
    int dmax = dest.maxval;
    int srcwyi = 0;
    int srcwxi = 0;
    int srcwxi1 = + 1;
    int srcwyim1 = - 1;
    int srcwyi1 = + 1;
    int destwyi1 =  + 1;
    int destwyim1 =  - 1;
    int destwxi1 =  + 1;

    // the border of width 1 around the image will be filled with value 0
    setborder(dest, 1, 0);

    for (int x = 0; x < dx - 2; x++)
      {
        vy1 = 2 * tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi)
              - (tmp.getPixelUnchecked(x + srcwxi, srcwyi)
                 + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi));
        vy2 = 2 * tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi + 1)
              - (tmp.getPixelUnchecked(x + srcwxi, srcwyi + 1)
                 + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi + 1));
        vy3 = 2 * tmp.getPixelUnchecked(x + srcwxi + 1, srcwyi + 2)
              - (tmp.getPixelUnchecked(x + srcwxi, srcwyi + 2)
                 + tmp.getPixelUnchecked(x + srcwxi + 2, srcwyi + 2));
        dest.setPixelUnchecked(x + destwxi1, destwyi1,
                               limited(((vy1 + vy3) - 2 * vy2 + 9 * tmp.getPixelUnchecked(x + srcwxi1, srcwyi1)) * gmax2 / gmax1, dmax));

        for (int y = 3; y < dy; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = 2 * tmp.getPixelUnchecked(x + srcwxi + 1, y + srcwyi)
                  - (tmp.getPixelUnchecked(x + srcwxi, y + srcwyi)
                     + tmp.getPixelUnchecked(x + srcwxi + 2, y + srcwyi));
            dest.setPixelUnchecked(x + destwxi1, y + destwyim1,
                                   limited(((vy1 + vy3) - 2 * vy2 + 9 * tmp.getPixelUnchecked(x + srcwxi1, y + srcwyim1)) * gmax2 / gmax1, dmax));
          }
      }
    return OK;
  }
#undef FNAME

#define FNAME "MeanImg"
  int MeanImg(const Image& src, const Image& dest)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return MeanImg<unsigned char, unsigned char>(src, dest);
      case 18:
        return MeanImg<unsigned char, unsigned short>(src, dest);
      case 19:
        return MeanImg<unsigned char, unsigned int>(src, dest);
      case 33:
        return MeanImg<unsigned short, unsigned char>(src, dest);
      case 34:
        return MeanImg<unsigned short, unsigned short>(src, dest);
      case 35:
        return MeanImg<unsigned short, unsigned int>(src, dest);
      case 49:
        return MeanImg<unsigned int, unsigned char>(src, dest);
      case 50:
        return MeanImg<unsigned int, unsigned short>(src, dest);
      case 51:
        return MeanImg<unsigned int, unsigned int>(src, dest);
      default:
        return MeanImg_std(src, dest);
      }
  }
#undef FNAME

#define FNAME "smearImg"
  /****************** smearImg *************************/

  static void smear_core_std(const Image& pn1, const Image& pn2, int sx, int sy)
  {
    int dx = pn1.xsize;
    int dy = pn1.ysize;
    int sx1 = sx / 2;
    int sy1 = sy / 2;

    Image tmp = NewImg(dx - sx + 1, dy, sx * pn1.maxval);

    // horizontale Filterung
    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        int y1 = y;
        int x1 = 0, x2 = 0;
        int gsum = 0;

        while (x2 < sx - 1)
          {
            gsum += GetValUnchecked(pn1, x2, y1);
            x2++;
          }

        while (x2 < dx)
          {
            gsum += GetValUnchecked(pn1, x2, y1);
            PutValUnchecked(tmp, x1, y, gsum);
            gsum -= GetValUnchecked(pn1, x1, y1);
            x1++;
            x2++;
          }
      }

    // vertikale Filterung
    for (int x = 0; x < sx1; x++)
      for (int y = 0; y < dy; y++)
        {
          PutValUnchecked(pn2, x, y, 0);
        }

    int ct = sx * sy;
    int adjustment = ct / 2; // adjustment for rounding

    for (int x = 0; x < dx - sx + 1; x++)   // alle spalten
      {
        int y1 = 0, y2 = 0;
        int yt = 0;
        int gsum = 0;

        while (y2 < sy - 1)
          {
            gsum += GetValUnchecked(tmp, x, y2);
            y2++;
          }

        while (yt < sy1)
          {
            PutValUnchecked(pn2, x + sx1, yt, 0);
            yt++;
          }

        while (y2 < dy)
          {
            gsum += GetValUnchecked(tmp, x, y2);
            PutValUnchecked(pn2, x + sx1, yt, (gsum + adjustment) / ct);
            gsum -= GetValUnchecked(tmp, x, y1);
            y1++;
            y2++;
            yt++;
          }

        while (yt < dy)
          {
            PutValUnchecked(pn2, x + sx1, yt, 0);
            yt++;
          }
      }

    for (int x = dx - sx1; x < dx; x++)
      for (int y = 0; y < dy; y++)
        {
          PutValUnchecked(pn2, x, y, 0);
        }
  }

  template<class T>
  static void smear_core(const Image& pn1, const Image& pn2, int sx, int sy)
  {
    const T** p1 = (const T**)pn1->getDataPtr();
    T** p2 = (T**)pn2->getDataPtr();

    int dx = pn1->xsize;
    int dy = pn1->ysize;

    int sx1 = sx / 2;
    int sy1 = sy / 2;

    int hx = dx - sx + 1;
    int** tmp = new int* [hx];

    for (int i = 0; i < hx; i++)
      {
        tmp[i] = new int[dy];
      }

    // horizontale Filterung
    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        int x1 = 0, x2 = 0;
        int gsum = 0;

        while (x2 < sx - 1)
          {
            gsum += p1[y][x2]; // GetValUnchecked(pn1,x2+pn1->wxi,y+pn1->wyi);
            x2++;
          }

        while (x2 < dx)
          {
            gsum += p1[y][x2]; // GetValUnchecked(pn1,x2+pn1->wxi,y+pn2->wyi);
            tmp[x1][y] = gsum; // PutValUnchecked(tmp,x1,y,gsum);
            gsum -= p1[y][x1]; // GetValUnchecked(pn1,x1+pn1->wxi,y+pn2->wyi);
            x1++;
            x2++;
          }
      }

    int size = sx * sy;
    int adjustment = size / 2; // adjustment for rounding

    // vertikale Filterung
    for (int x = 0; x < sx1; x++)
      for (int y = 0; y < dy; y++)
        {
          p2[y][x] = 0;
        }

    for (int x = 0; x < dx - sx + 1; x++)   // alle spalten
      {
        int y1 = 0, y2 = 0, yt = 0;
        int gsum = 0;
        int xdest = x + sx1;

        while (y2 < sy - 1)
          {
            gsum += tmp[x][y2];
            y2++;
          }

        while (yt < sy1)
          {
            p2[yt][xdest] = 0;
            yt++;
          }

        while (y2 < dy)
          {
            gsum += tmp[x][y2];
            p2[yt][xdest] = (gsum + adjustment) / size;
            gsum -= tmp[x][y1];
            y1++;
            y2++;
            yt++;
          }

        while (yt < dy)
          {
            p2[yt][xdest] = 0;
            yt++;
          }
      }

    for (int x = dx - sx1; x < dx; x++)
      for (int y = 0; y < dy; y++)
        {
          p2[y][x] = 0;
        }

    for (int i = 0; i < hx; i++)
      {
        delete [] tmp[i];
      }

    delete [] tmp;
  }

  void smearImg(const Image& pn1, const Image& pn2, int sx, int sy)
  {
    int dx, dy;

    if ((sx < 1) || ((sx & 1) != 1) || (sy < 1) || ((sy & 1) != 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, dx, dy));

    int typ1 = pn1->ImageType();
    int typ2 = pn2->ImageType();

    if (typ1 != typ2)
      {
        smear_core_std(pn1, pn2, sx, sy);
      }
    else
      {
        switch (typ1)
          {
          case 1:
            smear_core<PixelType1>(pn1, pn2, sx, sy);
            break;
          case 2:
            smear_core<PixelType2>(pn1, pn2, sx, sy);
            break;
          case 3:
            smear_core<PixelType3>(pn1, pn2, sx, sy);
            break;
          default:
            smear_core_std(pn1, pn2, sx, sy);
            break;
          }
      }
  }

  void smearImg(const Image& pn1, const Image& pn2, int sx)
  {
    smearImg(pn1, pn2, sx, sx);
  }

  void smearImg(const Image& imgs, int sx)
  {
    smearImg(imgs, imgs, sx, sx);
  }
#undef FNAME

#define FNAME "BoxImg"
  /****************** BoxImg *************************/
  static void box_core_std(const Image& pn1, const Image& pn2, int sx, int sy)
  {
    int dx = pn1->xsize;
    int dy = pn1->ysize;
    int sx1 = sx / 2;
    int sy1 = sy / 2;
    int mv = pn2.maxval;

    Image tmp = NewImg(dx - sx + 1, dy, sx * pn1.maxval);

    // horizontale Filterung
    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        int y1 = y;
        int x1 = 0, x2 = 0;
        int gsum = 0;

        while (x2 < sx - 1)
          {
            gsum += GetValUnchecked(pn1, x2, y1);
            x2++;
          }

        while (x2 < dx)
          {
            gsum += GetValUnchecked(pn1, x2, y1);
            PutValUnchecked(tmp, x1, y, gsum);
            gsum -= GetValUnchecked(pn1, x1, y1);
            x1++;
            x2++;
          }
      }

    // vertikale Filterung
    for (int x = 0; x < sx1; x++)
      for (int y = 0; y < dy; y++)
        {
          PutValUnchecked(pn2, x, y, 0);
        }

    for (int x = 0; x < dx - sx + 1; x++)   // alle spalten
      {
        int y1 = 0, y2 = 0;
        int yt = 0;
        int gsum = 0;

        while (y2 < sy - 1)
          {
            gsum += GetValUnchecked(tmp, x, y2);
            y2++;
          }

        while (yt < sy1)
          {
            PutValUnchecked(pn2, x + sx1, yt, 0);
            yt++;
          }

        while (y2 < dy)
          {
            gsum += GetValUnchecked(tmp, x, y2);
            PutValUnchecked(pn2, x + sx1, yt, gsum < mv ? gsum : mv);
            // PutValUnchecked(pn2,x+sx1,yt,gsum);
            gsum -= GetValUnchecked(tmp, x, y1);
            y1++;
            y2++;
            yt++;
          }

        while (yt < dy)
          {
            PutValUnchecked(pn2, x + sx1, yt, 0);
            yt++;
          }
      }

    for (int x = dx - sx1; x < dx; x++)
      for (int y = 0; y < dy; y++)
        {
          PutValUnchecked(pn2, x, y, 0);
        }
  }

  template<class T1, class T2>
  static void box_core(const Image& pn1, const Image& pn2, int sx, int sy)
  {
    const T1** p1 = (const T1**)pn1->getDataPtr();
    T2** p2 = (T2**)pn2->getDataPtr();

    int dx = pn1->xsize;
    int dy = pn1->ysize;

    int sx1 = sx / 2;
    int sy1 = sy / 2;
    int mv = pn2.maxval;

    int hx = dx - sx + 1;
    int** tmp = new int* [hx];

    for (int i = 0; i < hx; i++)
      {
        tmp[i] = new int[dy];
      }

    // horizontale Filterung
    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        int x1 = 0, x2 = 0;
        int gsum = 0;

        while (x2 < sx - 1)
          {
            gsum += p1[y][x2]; // GetValUnchecked(pn1,x2+pn1->wxi,y+pn1->wyi);
            x2++;
          }

        while (x2 < dx)
          {
            gsum += p1[y][x2]; // GetValUnchecked(pn1,x2+pn1->wxi,y+pn2->wyi);
            tmp[x1][y] = gsum; // PutValUnchecked(tmp,x1,y,gsum);
            gsum -= p1[y][x1]; // GetValUnchecked(pn1,x1+pn1->wxi,y+pn2->wyi);
            x1++;
            x2++;
          }
      }

    // vertikale Filterung
    for (int x = 0; x < sx1; x++)
      for (int y = 0; y < dy; y++)
        {
          p2[y][x] = 0;
        }

    for (int x = 0; x < dx - sx + 1; x++)   // alle spalten
      {
        int y1 = 0, y2 = 0, yt = 0;
        int gsum = 0;
        int xdest = x + sx1;

        while (y2 < sy - 1)
          {
            gsum += tmp[x][y2];
            y2++;
          }

        while (yt < sy1)
          {
            p2[yt][xdest] = 0;
            yt++;
          }

        while (y2 < dy)
          {
            gsum += tmp[x][y2];
            p2[yt][xdest] = gsum < mv ? gsum : mv;
            gsum -= tmp[x][y1];
            y1++;
            y2++;
            yt++;
          }

        while (yt < dy)
          {
            p2[yt][xdest] = 0;
            yt++;
          }
      }

    for (int x = dx - sx1; x < dx; x++)
      for (int y = 0; y < dy; y++)
        {
          p2[y][x] = 0;
        }

    for (int i = 0; i < hx; i++)
      {
        delete [] tmp[i];
      }

    delete [] tmp;
  }

  int BoxImg(const Image& pn1, const Image& pn2, int sx, int sy)
  {
    int dx, dy;

    /*
      if ((sx<1)||((sx & 1)!=1)||(sy<1)||((sy & 1)!=1))
      {
    throw IceException(FNAME,M_WRONG_PARAM,WRONG_PARAM);
      }
    */

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, dx, dy));

    int typ1 = pn1->ImageType();
    int typ2 = pn2->ImageType();
    int typx = (typ1 << 4) + typ2;

    switch (typx)
      {
      case 1*16+1:
        box_core<PixelType1, PixelType1>(pn1, pn2, sx, sy);
        break;
      case 1*16+2:
        box_core<PixelType1, PixelType2>(pn1, pn2, sx, sy);
        break;
      case 1*16+3:
        box_core<PixelType1, PixelType3>(pn1, pn2, sx, sy);
        break;

      case 2*16+1:
        box_core<PixelType2, PixelType1>(pn1, pn2, sx, sy);
        break;
      case 2*16+2:
        box_core<PixelType2, PixelType2>(pn1, pn2, sx, sy);
        break;
      case 2*16+3:
        box_core<PixelType2, PixelType3>(pn1, pn2, sx, sy);
        break;

      case 3*16+1:
        box_core<PixelType3, PixelType1>(pn1, pn2, sx, sy);
        break;
      case 3*16+2:
        box_core<PixelType3, PixelType2>(pn1, pn2, sx, sy);
        break;
      case 3*16+3:
        box_core<PixelType3, PixelType3>(pn1, pn2, sx, sy);
        break;

      default:
        box_core_std(pn1, pn2, sx, sy);
        break;
      }

    return OK;
  }

  int BoxImg(const Image& pn1, const Image& pn2, int sx)
  {
    return BoxImg(pn1, pn2, sx, sx);
  }

  int BoxImg(const Image& imgs, int sx)
  {
    return BoxImg(imgs, imgs, sx, sx);
  }
#undef FNAME

#define FNAME "DoBImg"
  int DoBImg(const Image& imgs, const Image& imgd, int n1, int n2, int mode)
  {
    int dx, dy;

    if ((n1 < 1) || ((n1 & 1) != 1) || (n2 < 1) || ((n2 & 1) != 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    RETURN_ERROR_IF_FAILED(MatchImg(imgs, imgd, dx, dy));
    Image box1 = NewImg(dx, dy, imgs.maxval);
    Image box2 = NewImg(dx, dy, imgs.maxval);
    RETURN_ERROR_IF_FAILED(smearImg(imgs, box1, n1));
    RETURN_ERROR_IF_FAILED(smearImg(imgs, box2, n2));
    RETURN_ERROR_IF_FAILED(subImg(box1, box2, imgd, mode));

    return OK;
  }

  int DoBImg(const Image& img, int n1, int n2, int mode)
  {
    return DoBImg(img, img, n1, n2, mode);
  }
#undef FNAME

#define FNAME "DoBMultiImg"
  int DoBMultiImg(const Image& imgs, const Image& imgd,
                  const IMatrix& filtersizes,
                  int boundary_x, int boundary_y,
                  int scalef, bool use_gauss_filter)
  {
    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(imgs, imgd, dx, dy));

    Image imgs_wide = NewImg(dx + 2 * boundary_x,
                             dy + 2 * boundary_y,
                             imgs.maxval);

    for (int y = 0; y < imgs_wide.ysize; y++)
      for (int x = 0; x < imgs_wide.xsize; x++)
        {
          int xo = x - boundary_x;
          int yo = y - boundary_y;

          if (xo < 0)
            {
              xo = 0;
            }

          if (yo < 0)
            {
              yo = 0;
            }

          if (xo >= dx)
            {
              xo = dx - 1;
            }

          if (yo >= dy)
            {
              yo = dy - 1;
            }

          PutValUnchecked(imgs_wide, x, y,
                          GetValUnchecked(imgs, xo, yo));
        }

    std::map<int, Image> smearimgs;

    for (int i = 0 ; i < filtersizes.rows() ; i++)
      {
        const IVector& dobcall = filtersizes[i];

        for (int j = 0 ; j <= 1 ; j++)
          {
            int nx = dobcall[2 * j];
            int ny = dobcall[2 * j + 1];

            int key = dy * nx + ny;

            std::map<int, Image>::iterator it = smearimgs.find(key);

            if (it == smearimgs.end())
              {
                Image smearresult = NewImg(imgs_wide->xsize,
                                           imgs_wide->ysize, imgs_wide.maxval);
                clearImg(smearresult);

                if (use_gauss_filter)
                  {
                    if (nx != ny)
                      throw IceException(FNAME, M_WRONG_PARAM);

                    GaussImg(imgs_wide, smearresult, nx, nx / 3.0);
                  }
                else
                  {
                    smearImg(imgs_wide, smearresult, nx, ny);
                  }

                smearimgs[key] = smearresult;
              }
          }
      }

    clearImg(imgd);

    for (int i = 0 ; i < filtersizes.rows() ; i++)
      {
        const IVector& dobcall = filtersizes[i];
        int nx1 = dobcall[0];
        int ny1 = dobcall[1];
        int nx2 = dobcall[2];
        int ny2 = dobcall[3];
        int key1 = dy * nx1 + ny1;
        int key2 = dy * nx2 + ny2;
        Image box1 = smearimgs[key1];
        Image box2 = smearimgs[key2];

        for (int x = 0; x < imgd.xsize; x++)
          for (int y = 0; y < imgd.ysize; y++)
            {
              int xn = x + boundary_x;
              int yn = y + boundary_y;
              long int box1v = GetValUnchecked(box1, xn, yn);
              long int box2v = GetValUnchecked(box2, xn, yn);
              long int diff = box1v - box2v;
              long int v = imgs.maxval;

              if (diff > 0)
                {
                  int current_val = GetValUnchecked(imgd, x, y);

                  if (scalef != 0.0)
                    {
                      long int box1_s = box1v + scalef;
                      v    = (diff * (box1_s + imgs.maxval)) / (diff + box1_s);

                      if (v > current_val)
                        {
                          PutValUnchecked(imgd, x, y, v);
                        }
                    }
                  else
                    {
                      if (diff > current_val)
                        {
                          PutValUnchecked(imgd, x, y, diff);
                        }
                    }
                }
            }
      }

    return 0;
  }
#undef FNAME

  static void calcGaussFunction(Matrix& gf, double sigma)
  {
    int size = gf.cols();
    int half = size / 2;

    double sigma2 = 2 * sigma * sigma;

    for (int i = 0; i <= half; i++)
      {
        gf[0][half - i] = gf[0][half + i] = exp(-i * i / sigma2);
      }

    // Gausfunktion normieren auf 1.0
    double summe = 0.0;

    for (int i = 0; i < size; i++)
      {
        summe += gf[0][i];
      }

    for (int i = 0; i < size; i++)
      {
        gf[0][i] /= summe;
      }
  }

  /*
   * Gaussfilter (Ortmann 04/12)
   *   img1 -  Quellbild
   *   img2 -  Zielbild
   *   size -  Groesse der Filtermaske
   *   sigma - Standardabweichung in Gaussfunktion
   *   return: img2, NULL bei Fehler
   */
#define FNAME "GaussImg"
  void GaussImg(const Image& img1, const Image& img2, int size, double sigma)
  {
    /* Parameter pruefen */
    if (sigma <= 0 || size < 1 || (size & 1) == 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM);
      };

    try
      {

        /* Bildgrößen ueberpruefen */
        int dx, dy;
        checkSizes(img1, img2, dx, dy);

        Image himg;
        himg.create(img1);

        // eindimensionale Gaussfunktion berechnen
        // Zeile
        Matrix gf(1, size);

        calcGaussFunction(gf, sigma);
        LSIFilter gaussh(gf);

        // Spalte
        gf = !gf;
        LSIFilter gaussv(gf);

        gaussv.Filter(img1, himg, 0.0);
        gaussh.Filter(himg, img2, 0.0);
      }
    RETHROW;
  };

  void GaussImg(const ImageD& img1, const ImageD& img2, int size, double sigma)
  {
    /* Parameter pruefen */
    if (sigma <= 0 || size < 1 || (size & 1) == 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM);
      };
    try
      {
        /* Bildgrößen ueberpruefen */
        int dx = img1.xsize;
        int dy = img1.ysize;

        if (dx != img2.xsize || dy != img2.ysize)
          throw IceException(FNAME, M_WRONG_IMGSIZE);

        /* eindimensionale Gaussfunktion berechnen */
        Matrix gf(1, size);

        calcGaussFunction(gf, sigma);

        LSIFilter gaussh(gf);

        gf = !gf;

        LSIFilter gaussv(gf);

        gaussv.Filter(img1, img2);

        gaussh.Filter(img2, img2);
      }
    RETHROW;
  };
#undef FNAME

  /*
   *  Mexican-Hat as LoG - Laplace of Gaussian
   *  (x*x+y*y - 2 * sigma^2)/sigma^4 * exp(-(x*x+y*y)/(2*sigma^2)
   */

  void makeLoG(Matrix& f, double sigma)
  {
    int sx = f.cols();
    int sy = f.rows();
    int xm = sx / 2;
    int ym = sy / 2;
    double sigma2 = sigma * sigma;

    double sump = 0;

    for (int y = 0; y < sy; y++)
      {
        double dy = y - ym;
        double dy2 = dy * dy;

        for (int x = 0; x < sx; x++)
          {
            double dx = x - xm;
            double r2 = dx * dx + dy2;
            double h = -r2 / sigma2;
            double c = (1 + h) * exp(h);
            f[y][x] = c;

            if (c > 0)
              {
                sump += c;
              }
          }
      }

    for (int y = 0; y < sy; y++)
      {
        for (int x = 0; x < sx; x++)
          {
            f[y][x] /= sump;
          }
      }
  }

#define FNAME "MexicanHatImg"
  LSIFilter makeMexicanHatFilter(double sigma, int size)
  {
    if (size < 0 || sigma < 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (size == 0)
      {
        size = (int)(sigma * 5) | 1;
      }

    Matrix fc(size, size);
    makeLoG(fc, sigma);
    return LSIFilter(fc);
  }

  void MexicanHatImg(const Image& img1, const Image& img2,
                     double sigma, int size)
  {
    try
      {
        LSIFilter f = makeMexicanHatFilter(sigma, size);
        f.Filter(img1, img2, img2.maxval / 2);
      }
    RETHROW;
  }

  void MexicanHatImg(const Image& img1, ImageD img2,
                     double sigma, int size)
  {
    try
      {
        LSIFilter f = makeMexicanHatFilter(sigma, size);
        f.Filter(img1, img2);
      }
    RETHROW;
  }

  void MexicanHatImg(ImageD img1, ImageD img2,
                     double sigma, int size)
  {
    try
      {
        LSIFilter f = makeMexicanHatFilter(sigma, size);
        f.Filter(img1, img2);
      }
    RETHROW;
  }

#undef FNAME
}
