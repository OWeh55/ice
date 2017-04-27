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
#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "util.h"
#include "darith.h"

#include "filter.h"

namespace ice
{
  /************* gradient filters ****************/

#define FNAME "GradYImg"
  /**
   * This template function applies the vertical Prewitt operator on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  int GradYImg(const Image& src, const Image& dest, int norm = 1)
  {
    int vx1, vx2, vx3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int gmax1 = src.maxval * 6;
    int gmax2 = dest.maxval * norm;
    int offset = (dest.maxval + 1) / 2;
    int dmax = dest.maxval;
    int srcwyam1 = src->ysize - 2;
    int srcwxa1 = src->xsize;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int y = 0; y < srcwyam1; y++)
      {
        vx1 = -Pixels[y][0] + Pixels[y + 2][0];
        vx2 = -Pixels[y][1] + Pixels[y + 2][1];
        vx3 = -Pixels[y][2] + Pixels[y + 2][2];
        Pixeld[y + 1][1] = limited((vx1 + vx2 + vx3) * gmax2 / gmax1 + offset, dmax);

        for (int x = 3; x < srcwxa1; x++)
          {
            vx1 = vx2;
            vx2 = vx3;
            vx3 = -Pixels[y][x] + Pixels[y + 2][x];
            Pixeld[y + 1][x - 1] = limited((vx1 + vx2 + vx3) * gmax2 / gmax1 + offset, dmax);
          }
      }

    return OK;
  }

  int GradYImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int vx1, vx2, vx3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int gmax1 = src.maxval * 6;
    int gmax2 = dest.maxval * norm;
    int offset = (dest.maxval + 1) / 2;
    int dmax = dest.maxval;
    int srcwyam1 = src.ysize - 2;
    int srcwxa1 = src.xsize;

    // the border of width 1 around the image will be filled with offset
    setborder(dest, 1, offset);

    for (int y = 0; y < srcwyam1; y++)
      {
        vx1 = -tmp.getPixelUnchecked(0, y)
              + tmp.getPixelUnchecked(0, y + 2);
        vx2 = -tmp.getPixelUnchecked(1, y)
              + tmp.getPixelUnchecked(1, y + 2);
        vx3 = -tmp.getPixelUnchecked(2, y)
              + tmp.getPixelUnchecked(2, y + 2);
        dest.setPixelUnchecked(1, y + 1, limited((vx1 + vx2 + vx3)*gmax2 / gmax1 + offset, dmax));

        for (int x = 3; x < srcwxa1; x++)
          {
            vx1 = vx2;
            vx2 = vx3;
            vx3 = -tmp.getPixelUnchecked(x, y)
                  + tmp.getPixelUnchecked(x, y + 2);
            dest.setPixelUnchecked(x - 1, y + 1, limited((vx1 + vx2 + vx3)*gmax2 / gmax1 + offset, dmax));
          }
      }

    return OK;
  }
#undef FNAME

#if 1
#define FNAME "GradXImg"
  /**
   * This template function applies the horizontal Prewitt operator on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  int GradXImg(const Image& src, const Image& dest, int norm = 1)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    Image tmp = src;

    // since we write directly in dest and need to read from positions
    // where we wrote before we need to copy the source image if
    // it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int gmax1 = src.maxval * 6;
    int gmax2 = dest.maxval * norm;
    int dmax = dest.maxval;
    int offset = (dest.maxval + 1) / 2;
    int srcwya = src->ysize - 1;
    int srcwxa = src->xsize - 1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int y = 1; y < srcwya; y++)
      for (int x = 0; x < srcwxa - 1; x++)
        {
          int v = (- Pixels[y - 1][x] + Pixels[y - 1][x + 2] - Pixels[y][x]
                   + Pixels[y][x + 2] - Pixels[y + 1][x] + Pixels[y + 1][x + 2]
                  ) * gmax2 / gmax1 + offset;
          Pixeld[y][x + 1] = limited(v, dmax);
        }

    return OK;
  }
#else
// experimentelle fassung
// --  mit Zeiger-Jongliererei
#define FNAME "GradXImg"
  /**
   * This template function applies the horizontal Prewitt operator on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  int GradXImg(const Image& src, const Image& dest, int norm = 1)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    Image tmp = src;

    // since we write directly in dest and need to read from positions
    // where we wrote before we need to copy the source image if
    // it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int gmax1 = src.maxval * 6;
    int gmax2 = dest.maxval * norm;
    int dmax = dest.maxval;
    int offset = (dest.maxval + 1) / 2;
    int xmax = dest->xsize;
    int ymax = dest->ysize;

    // the border of width 1 around the image will be filled with maxval/2
    border(dest, 1, offset);

    const SrcType** Pixels = (const SrcType**)tmp->GetDataPtr();
    DestType** Pixeld = (DestType**)dest->GetDataPtr();

    for (int y = 1; y < ymax - 1; y++)
      {
        const SrcType* z1 = Pixels[y - 1];
        const SrcType* z2 = Pixels[y];
        const SrcType* z3 = Pixels[y + 1];

        for (int x1 = 0, x = 1, x2 = 2; x < xmax - 1; x1++, x2++, x++)
          {
            int v = (
                      - z1[0] + z1[2]
                      - z2[0] + z2[2]
                      - z3[0] + z3[2]
                    ) * gmax2 / gmax1 + offset;
            z1++;
            z2++;
            z3++;
            Pixeld[y][x] = limited(v, dmax);
          }
      }

    return OK;
  }

#endif
  int GradXImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int vy1, vy2, vy3;

    // test if both images have valid pixel arrays and
    // equal size of the active windows
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int gmax1 = src.maxval * 6;
    int gmax2 = dest.maxval * norm;
    int dmax = dest.maxval;
    int offset = (dest.maxval + 1) / 2;

    int srcwyi = 0;
    int srcwya1 = src->ysize;
    int srcwxi = 0;
    int srcwxam1 = src->xsize - 2;
    int destwyi1 = 1;

    int dsxoff = 1;
    int dsyoff = -1;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    for (int x = srcwxi; x < srcwxam1; x++)
      {
        vy1 = -tmp.getPixelUnchecked(x, srcwyi)
              + tmp.getPixelUnchecked(x + 2, srcwyi);
        vy2 = -tmp.getPixelUnchecked(x, srcwyi + 1)
              + tmp.getPixelUnchecked(x + 2, srcwyi + 1);
        vy3 = -tmp.getPixelUnchecked(x, srcwyi + 2)
              + tmp.getPixelUnchecked(x + 2, srcwyi + 2);
        dest.setPixelUnchecked(x + dsxoff, destwyi1, limited((vy1 + vy2 + vy3) * gmax2 / gmax1 + offset, dmax));

        for (int y = srcwyi + 3; y < srcwya1; y++)
          {
            vy1 = vy2;
            vy2 = vy3;
            vy3 = -tmp.getPixelUnchecked(x, y)
                  + tmp.getPixelUnchecked(x + 2, y);
            dest.setPixelUnchecked(x + dsxoff, y + dsyoff, limited((vy1 + vy2 + vy3) * gmax2 / gmax1 + offset, dmax));
          }
      }

    return OK;
  }
#undef FNAME

#define FNAME "GradXImg"
  int GradXImg(const Image& src, const Image& dest, int norm)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return GradXImg<unsigned char, unsigned char>(src, dest, norm);
      case 18:
        return GradXImg<unsigned char, unsigned short>(src, dest, norm);
      case 19:
        return GradXImg<unsigned char, unsigned int>(src, dest, norm);
      case 33:
        return GradXImg<unsigned short, unsigned char>(src, dest, norm);
      case 34:
        return GradXImg<unsigned short, unsigned short>(src, dest, norm);
      case 35:
        return GradXImg<unsigned short, unsigned int>(src, dest, norm);
      case 49:
        return GradXImg<unsigned int, unsigned char>(src, dest, norm);
      case 50:
        return GradXImg<unsigned int, unsigned short>(src, dest, norm);
      case 51:
        return GradXImg<unsigned int, unsigned int>(src, dest, norm);
      default:
        return GradXImg_std(src, dest, norm);
      }
  }
#undef FNAME

  int GradYImg(const Image& src, const Image& dest, int norm)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return GradYImg<unsigned char, unsigned char>(src, dest, norm);
      case 18:
        return GradYImg<unsigned char, unsigned short>(src, dest, norm);
      case 19:
        return GradYImg<unsigned char, unsigned int>(src, dest, norm);
      case 33:
        return GradYImg<unsigned short, unsigned char>(src, dest, norm);
      case 34:
        return GradYImg<unsigned short, unsigned short>(src, dest, norm);
      case 35:
        return GradYImg<unsigned short, unsigned int>(src, dest, norm);
      case 49:
        return GradYImg<unsigned int, unsigned char>(src, dest, norm);
      case 50:
        return GradYImg<unsigned int, unsigned short>(src, dest, norm);
      case 51:
        return GradYImg<unsigned int, unsigned int>(src, dest, norm);
      default:
        return GradYImg_std(src, dest, norm);
      }
  }

  /**
   * The versions with old parameter order just call the new ones
   */
  int GradXImg(const Image& src, int norm, const Image& dest)
  {
    return GradXImg(src, dest, norm);
  }

  int GradYImg(const Image& src, int norm, const Image& dest)
  {
    return GradYImg(src, dest, norm);
  }

  /****************** GradImg *************************/
#define FNAME "GradImg"
  int GradImg(const Image& pn1p, int norm, const Image& pn2)
  {
    int x, y, dx, dy, gmax1, gmax2, val;
    int vy1, vy2, vy3, vx, vy, xoff, yoff;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1p, pn2, dx, dy));

    Image pn1 = pn1p;

    if (pn2 == pn1)   /*Quellbild=Zielbild*/
      {
        pn1 = NewImg(pn2, true);  /*temporaeres Bild anlegen*/
      }

    gmax1 = pn1.maxval * 6;
    gmax2 = pn2.maxval * norm;

    setborder(pn2, 1, 0);

    xoff = -1;
    yoff = -1;

    for (x = 2; x < dx; x++)
      {
        vy1 = GetValUnchecked(pn1, x - 2, 0)
              + GetValUnchecked(pn1, x - 1, 0)
              + GetValUnchecked(pn1, x  , 0);
        vy2 = GetValUnchecked(pn1, x - 2, 1)
              + GetValUnchecked(pn1, x - 1, 1)
              + GetValUnchecked(pn1, x  , 1);
        vy3 = GetValUnchecked(pn1, x - 2, 2)
              + GetValUnchecked(pn1, x - 1, 2)
              + GetValUnchecked(pn1, x  , 2);

        vx = GetValUnchecked(pn1, x, 0)
             + GetValUnchecked(pn1, x  , 1)
             + GetValUnchecked(pn1, x  , 2)
             - GetValUnchecked(pn1, x - 2, 0)
             - GetValUnchecked(pn1, x - 2, 1)
             - GetValUnchecked(pn1, x - 2, 2);
        vy = vy3 - vy1;
        val = (int)sqrt((double)(vy * vy + vx * vx));
        PutValUnchecked(pn2, x + xoff, 1,
                        limited(val * gmax2 / gmax1, pn2));

        for (y = 3; y < dy; y++)
          {
            vx = vx
                 + GetValUnchecked(pn1, x - 2, y - 3)
                 - GetValUnchecked(pn1, x - 2, y)
                 - GetValUnchecked(pn1, x  , y - 3)
                 + GetValUnchecked(pn1, x  , y);
            vy1 = vy2;
            vy2 = vy3;
            vy3 = GetValUnchecked(pn1, x - 2, y)
                  + GetValUnchecked(pn1, x - 1, y)
                  + GetValUnchecked(pn1, x  , y);
            vy = vy3 - vy1;
            val = RoundInt(sqrt((double)(vy * vy + vx * vx)));
            PutValUnchecked(pn2, x + xoff, y + yoff,
                            limited(val * gmax2 / gmax1, pn2));
          }
      }

    return OK;
  }
#undef FNAME

#define FNAME "CalcDirectionImg"
  int CalcDirectionStructImg(const Image& pic, // zu untersuchendes Bild
                             const Image& dest, // "Winkel"-Bild
                             int detectionsize, // Umgebung
                             ImageD lambda1, ImageD lambda2 // Eigenwerte zurückgeben
                            )
  {
    RETURN_ERROR_IF_FAILED(MatchImg(pic, dest));

    if (detectionsize <= 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    int dimx = pic->xsize;
    int dimy = pic->ysize;

    int maxval = pic.maxval;
    int maxval_d = maxval * 6; // maxval für Gradienten

    int detection_rad = detectionsize / 2;

    // Gradientenbilder
    Image p_x;
    p_x.create(dimx, dimy, maxval_d);

    GradXImg(pic, 1, p_x);

    Image p_y;
    p_y.create(dimx, dimy, maxval_d);

    GradYImg(pic, 1, p_y);

    // Koeffizienten des Struktur-Tensors in double-Bildern
    ImageD xx = NewImgD(dimx, dimy);
    ImageD xy = NewImgD(dimx, dimy);
    ImageD yy = NewImgD(dimx, dimy);

    const int null = (maxval_d + 1) / 2; // Nullwert in Gradientenbildern

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          int gx = GetValUnchecked(p_x, x, y) - null;
          int gy = GetValUnchecked(p_y, x, y) - null;
          PutValD(xx, x, y, gx * gx);
          PutValD(xy, x, y, gx * gy);
          PutValD(yy, x, y, gy * gy);
        }

    // Mittelwertbildung
    smearImgD(xx, xx, detectionsize, detectionsize);
    smearImgD(xy, xy, detectionsize, detectionsize);
    smearImgD(yy, yy, detectionsize, detectionsize);

    double area = detection_rad * 2 + 1;
    area = area * area;

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          // Richtungsdetektion mit Struktur-Tensor
          double s_xx = GetValD(xx, x, y) / area;
          double s_xy = GetValD(xy, x, y) / area;
          double s_yy = GetValD(yy, x, y) / area;

          Point eigen; // Eigenvektor

          /* Struktur-Tensor ist symmetrische Matrix
          *  / s_xx s_xy \
          *  \ s_xy s_yy /
          */
          const double eps = 1.0e-10;

          double p = (s_xx + s_yy) / 2.0;
          double d = (s_xx - s_yy);
          double w = sqrt(d * d + 4 * s_xy * s_xy) / 2;

          double lambda_1 = p + w;
          double lambda_2 = p - w;

          // es gilt immer lambda_1 > lambda_2
          double a_1 = s_xx - lambda_1;

          if (fabs(s_xy) > eps)
            {
              if (fabs(a_1) > eps)
                {
                  eigen.y = a_1;
                  eigen.x = -s_xy;
                }
              else
                {
                  eigen.x = 1;
                  eigen.y = 0;
                }
            }
          else
            {
              if (fabs(a_1) > eps)
                {
                  eigen.x = 0;
                  eigen.y = 1;
                }
              else
                {
                  eigen.x = 1;
                  eigen.y = 0;
                }
            }

          // store eigenvalues in ImageD if given
          if (lambda1.isValid())
            {
              PutValD(lambda1, x, y, lambda_1);
            }

          if (lambda2.isValid())
            {
              PutValD(lambda2, x, y, lambda_2);
            }

          // calculate angle
          double direction = eigen.phi();
          // normlisiert auf maximalen Grauwert in Ergebnisbild eintragen
          int directionGrayValue = Mod(RoundInt(direction * (dest.maxval + 1) / M_PI), (dest.maxval + 1));

          PutVal(dest, x, y, directionGrayValue);
        }
    return 0;
  }

  int CalcDirectionImg(const Image& pic, // zu untersuchendes Bild
                       const Image& dest, // "Winkel"-Bild
                       int detectionsize, // Umgebung
                       ImageD lambda1 // Gradienten-Betrag ^ 2 als Gütemass
                      )
  {
    RETURN_ERROR_IF_FAILED(MatchImg(pic, dest));

    if (detectionsize <= 0)
      throw IceException(FNAME, M_WRONG_PARAM);

    int dimx = pic->xsize;
    int dimy = pic->ysize;

    int maxval = pic.maxval;
    int maxval_d = maxval * 6; // maxval für Gradienten

    int detection_rad = detectionsize / 2;

    // Gradientenbilder
    Image p_x;
    p_x.create(dimx, dimy, maxval_d);
    GradXImg(pic, 1, p_x);

    Image p_y;
    p_y.create(dimx, dimy, maxval_d);
    GradYImg(pic, 1, p_y);

    // Gradientenbild als ImageD
    ImageD gx = NewImgD(dimx, dimy);
    ImageD gy = NewImgD(dimx, dimy);

    int null = (maxval_d + 1) / 2; // Nullwert in Gradientenbildern

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          int gxv = GetValUnchecked(p_x, x, y) - null;
          int gyv = GetValUnchecked(p_y, x, y) - null;
          PutValD(gx, x, y, gxv);
          PutValD(gy, x, y, gyv);
        }

    // Mittelwertbildung
    smearImgD(gx, gx, detectionsize, detectionsize);
    smearImgD(gy, gy, detectionsize, detectionsize);

    double area = detection_rad * 2 + 1;
    area = area * area;

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          // Richtungsdetektion mit Gradienten-Mittelwert
          double gxv = GetValD(gx, x, y) / area;
          double gyv = GetValD(gy, x, y) / area;

          // Guetemass Gradientenbetrag^2
          double lambda_1 = gxv * gxv + gyv * gyv;

          if (lambda1.isValid())
            {
              PutValD(lambda1, x, y, lambda_1);
            }

          // Jetzt Winkel  bestimmen !
          double direction = atan2(gyv, gxv);
          // normlisiert auf maximalen Grauwert in Ergebnisbild eintragen
          int direction_grv = Mod(RoundInt(direction * (dest.maxval + 1) / M_PI / 2.0), (dest.maxval + 1));

          PutVal(dest, x, y, direction_grv);
        }

    return 0;
  }
#undef FNAME
  /****************** GradDirImg **********************/
#define FNAME "GradDirImg"
  int GradDirImg(const Image& pn1p, const Image& pn2)
  {
    int x, y, dx, dy, val, Direction;
    int vy1, vy2, vy3, vx, vy, xoff, yoff;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1p, pn2, dx, dy));

    if ((pn2.maxval < 7))
      throw IceException(FNAME, M_WRONG_PARAM);

    Image pn1 = pn1p;

    if (pn2 == pn1)   /*Quellbild=Zielbild*/
      {
        pn1 = NewImg(pn2, true);  /*temporaeres Quellbild anlegen*/
      }

    for (x = 0; x < dx; x++)
      {
        PutValUnchecked(pn2, x, 0, 2);
        PutValUnchecked(pn2, x, pn2->ysize - 1, 6);
      }

    for (y = 1; y < dy - 1; y++)
      {
        PutValUnchecked(pn2, 0, y, 0);
        PutValUnchecked(pn2, pn2->xsize - 1, y, 4);
      }

    xoff = -1;
    yoff = -1;

    for (x = 2; x < dx; x++)
      {
        vy1 = GetValUnchecked(pn1, x - 2, 0)
              + GetValUnchecked(pn1, x - 1, 0)
              + GetValUnchecked(pn1, x  , 0);
        vy2 = GetValUnchecked(pn1, x - 2, 1)
              + GetValUnchecked(pn1, x - 1, 1)
              + GetValUnchecked(pn1, x  , 1);
        vy3 = GetValUnchecked(pn1, x - 2, 2)
              + GetValUnchecked(pn1, x - 1, 2)
              + GetValUnchecked(pn1, x  , 2);
        vx = GetValUnchecked(pn1, x  , 0)
             + GetValUnchecked(pn1, x  , 1)
             + GetValUnchecked(pn1, x  , 2)
             - GetValUnchecked(pn1, x - 2, 0)
             - GetValUnchecked(pn1, x - 2, 1)
             - GetValUnchecked(pn1, x - 2, 2);
        vy = vy3 - vy1;

        if (vx == 0)
          if (vy < 0)
            {
              Direction = 6;
            }
          else
            {
              Direction = 2;
            }
        else
          {
            val = abs(10000 * vy / vx);

            if (val < 4142)
              {
                Direction = 0;
              }
            else if (val < 24142)
              {
                Direction = 1;
              }
            else
              {
                Direction = 2;
              }

            if (vx < 0)
              if (vy < 0)
                {
                  Direction = Direction + 4;
                }
              else
                {
                  Direction = 4 - Direction;
                }
            else if ((vy < 0) && (Direction > 0))
              {
                Direction = 8 - Direction;
              }
          }

        PutValUnchecked(pn2, x + xoff, 1, Direction);

        for (y = 3; y < dy; y++)
          {
            vx = vx
                 + GetValUnchecked(pn1, x - 2, y - 3)
                 - GetValUnchecked(pn1, x - 2, y)
                 - GetValUnchecked(pn1, x  , y - 3)
                 + GetValUnchecked(pn1, x  , y);
            vy1 = vy2;
            vy2 = vy3;
            vy3 = GetValUnchecked(pn1, x - 2, y)
                  + GetValUnchecked(pn1, x - 1, y)
                  + GetValUnchecked(pn1, x  , y);
            vy = vy3 - vy1;

            if (vx == 0)
              if (vy < 0)
                {
                  Direction = 6;
                }
              else
                {
                  Direction = 2;
                }
            else
              {
                val = abs(10000 * vy / vx);

                if (val < 4142)
                  {
                    Direction = 0;
                  }
                else if (val < 24142)
                  {
                    Direction = 1;
                  }
                else
                  {
                    Direction = 2;
                  }

                if (vx < 0)
                  if (vy < 0)
                    {
                      Direction = Direction + 4;
                    }
                  else
                    {
                      Direction = 4 - Direction;
                    }
                else if ((vy < 0) && (Direction > 0))
                  {
                    Direction = 8 - Direction;
                  }
              }

            PutValUnchecked(pn2, x + xoff, y + yoff, Direction);
          }
      }

    return OK;
  }
#undef FNAME
}
