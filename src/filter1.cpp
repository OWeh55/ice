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
  void GradYImg_template(const Image& src, const Image& dest, int norm = 1)
  {
    int sizeX = src.xsize;
    int sizeY = src.ysize;

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
    int srcwyam1 = sizeY - 2;
    int srcwxa1 = sizeX;

    // the border of width 1 around the image will be filled with maxval/2
    setborder(dest, 1, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    int vx1, vx2, vx3;
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
  }

  void GradYImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int vx1, vx2, vx3;

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
  }
#undef FNAME

#define FNAME "GradXImg"
  /**
   * This template function applies the horizontal Prewitt operator on an image
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param norm some (optional) extra norming factor
   */
  template<typename SrcType, typename DestType>
  void GradXImg_template(const Image& src, const Image& dest, int norm = 1)
  {
    // test if both images have valid pixel arrays and
    // equal size of the active windows

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

  }

  void GradXImg_std(const Image& src, const Image& dest, int norm = 1)
  {
    int vy1, vy2, vy3;

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
  }
#undef FNAME

#define FNAME "GradXImg"
  void GradXImg(const Image& src, const Image& dest, int norm)
  {
    try
      {
        checkSizes(src, dest);
        switch ((src->ImageType() << 4) + dest->ImageType())
          {
          case 17:
            GradXImg_template<PixelType1, PixelType1>(src, dest, norm);
            break;
          case 18:
            GradXImg_template<PixelType1, PixelType2>(src, dest, norm);
            break;
          case 19:
            GradXImg_template<PixelType1, PixelType3>(src, dest, norm);
            break;
          case 33:
            GradXImg_template<PixelType2, PixelType1>(src, dest, norm);
            break;
          case 34:
            GradXImg_template<PixelType2, PixelType2>(src, dest, norm);
            break;
          case 35:
            GradXImg_template<PixelType2, PixelType3>(src, dest, norm);
            break;
          case 49:
            GradXImg_template<PixelType3, PixelType1>(src, dest, norm);
            break;
          case 50:
            GradXImg_template<PixelType3, PixelType2>(src, dest, norm);
            break;
          case 51:
            GradXImg_template<PixelType3, PixelType3>(src, dest, norm);
            break;
          default:
            GradXImg_std(src, dest, norm);
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "GradYImg"
  void GradYImg(const Image& src, const Image& dest, int norm)
  {
    try
      {
        switch ((src->ImageType() << 4) + dest->ImageType())
          {
          case 17:
            GradYImg_template<PixelType1, PixelType1>(src, dest, norm);
            break;
          case 18:
            GradYImg_template<PixelType1, PixelType2>(src, dest, norm);
            break;
          case 19:
            GradYImg_template<PixelType1, PixelType3>(src, dest, norm);
            break;
          case 33:
            GradYImg_template<PixelType2, PixelType1>(src, dest, norm);
            break;
          case 34:
            GradYImg_template<PixelType2, PixelType2>(src, dest, norm);
            break;
          case 35:
            GradYImg_template<PixelType2, PixelType3>(src, dest, norm);
            break;
          case 49:
            GradYImg_template<PixelType3, PixelType1>(src, dest, norm);
            break;
          case 50:
            GradYImg_template<PixelType3, PixelType2>(src, dest, norm);
            break;
          case 51:
            GradYImg_template<PixelType3, PixelType3>(src, dest, norm);
            break;
          default:
            GradYImg_std(src, dest, norm);
          }
      }
    RETHROW;
  }
#undef FNAME
  /****************** GradImg *************************/
#define FNAME "GradImg"
  void GradImg(const Image& pn1p, const Image& pn2, int norm)
  {
    try
      {
        int sizeX, sizeY;
        //    int x, y, dx, dy, gmax1, gmax2, val;
        //    int vy1, vy2, vy3, vx, vy, xoff, yoff;

        checkSizes(pn1p, pn2, sizeX, sizeY);

        Image pn1 = pn1p;

        if (pn2 == pn1)   /*Quellbild=Zielbild*/
          {
            pn1 = NewImg(pn2, true);  /*temporaeres Bild anlegen*/
          }

        int gmax1 = pn1.maxval * 6;
        int gmax2 = pn2.maxval * norm;

        setborder(pn2, 1, 0);

        int xoff = -1;
        int yoff = -1;
        int vx, vy;
        int vy1, vy2, vy3;
        for (int x = 2; x < sizeX; x++)
          {
            vy1 = GetValUnchecked(pn1, x - 2, 0)
                  + GetValUnchecked(pn1, x - 1, 0)
                  + GetValUnchecked(pn1, x, 0);
            vy2 = GetValUnchecked(pn1, x - 2, 1)
                  + GetValUnchecked(pn1, x - 1, 1)
                  + GetValUnchecked(pn1, x, 1);
            vy3 = GetValUnchecked(pn1, x - 2, 2)
                  + GetValUnchecked(pn1, x - 1, 2)
                  + GetValUnchecked(pn1, x, 2);

            vx = GetValUnchecked(pn1, x, 0)
                 + GetValUnchecked(pn1, x, 1)
                 + GetValUnchecked(pn1, x, 2)
                 - GetValUnchecked(pn1, x - 2, 0)
                 - GetValUnchecked(pn1, x - 2, 1)
                 - GetValUnchecked(pn1, x - 2, 2);

            vy = vy3 - vy1;

            int val = (int)sqrt((double)(vy * vy + vx * vx));
            PutValUnchecked(pn2, x + xoff, 1,
                            limited(val * gmax2 / gmax1, pn2));

            for (int y = 3; y < sizeY; y++)
              {
                vx = vx
                     + GetValUnchecked(pn1, x - 2, y - 3)
                     - GetValUnchecked(pn1, x - 2, y)
                     - GetValUnchecked(pn1, x, y - 3)
                     + GetValUnchecked(pn1, x, y);
                vy1 = vy2;
                vy2 = vy3;
                vy3 = GetValUnchecked(pn1, x - 2, y)
                      + GetValUnchecked(pn1, x - 1, y)
                      + GetValUnchecked(pn1, x, y);
                vy = vy3 - vy1;
                val = RoundInt(sqrt((double)(vy * vy + vx * vx)));
                PutValUnchecked(pn2, x + xoff, y + yoff,
                                limited(val * gmax2 / gmax1, pn2));
              }
          }
      }
    RETHROW;
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
      throw IceException(FNAME, M_WRONG_PARAMETER);

    int dimx = pic->xsize;
    int dimy = pic->ysize;

    int maxval = pic.maxval;
    int maxval_d = maxval * 6; // maxval für Gradienten

    int detection_rad = detectionsize / 2;

    // Gradientenbilder
    Image p_x;
    p_x.create(dimx, dimy, maxval_d);

    GradXImg(pic, p_x, 1);

    Image p_y;
    p_y.create(dimx, dimy, maxval_d);

    GradYImg(pic, p_y, 1);

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
      throw IceException(FNAME, M_WRONG_PARAMETER);

    int dimx = pic->xsize;
    int dimy = pic->ysize;

    int maxval = pic.maxval;
    int maxval_d = maxval * 6; // maxval für Gradienten

    int detection_rad = detectionsize / 2;

    // Gradientenbilder
    Image p_x;
    p_x.create(dimx, dimy, maxval_d);
    GradXImg(pic, p_x);

    Image p_y;
    p_y.create(dimx, dimy, maxval_d);
    GradYImg(pic, p_y);

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
  int getDir(double deltaX, double deltaY)
  {
    int dir;
    if (deltaY > 0)
      {
        if (deltaX > 0)
          {
            if (deltaX > deltaY)
              dir = 0;
            else
              dir = 1;
          }
        else
          {
            if (-deltaX < deltaY)
              dir = 2;
            else
              dir = 3;
          }
      }
    else
      {
        if (deltaX < 0)
          {
            if (-deltaX > -deltaY)
              dir = 4;
            else
              dir = 5;
          }
        else
          {
            if (deltaX < -deltaY)
              dir = 6;
            else
              dir = 7;
          }
      }
    return dir;
  }

  void GradDirImg(const Image& pn1p, const Image& pn2)
  {

    static const double dfi = M_PI / 8;
    static const double tan1 = tan(dfi);

    int xSize, ySize;
    checkSizes(pn1p, pn2, xSize, ySize);

    if ((pn2.maxval < 7))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    Image pn1 = pn1p;

    if (pn2 == pn1)   /* Quellbild = Zielbild */
      {
        pn1 = NewImg(pn2, true);  /* temporaeres Quellbild anlegen */
      }

    for (int x = 0; x < xSize; x++)
      {
        pn2.setPixelUnchecked(x, 0, 2);
        pn2.setPixelUnchecked(x, pn2->ysize - 1, 6);
      }

    for (int y = 1; y < ySize - 1; y++)
      {
        pn2.setPixelUnchecked(0, y, 0);
        pn2.setPixelUnchecked(pn2->xsize - 1, y, 4);
      }

    int xoff = -1;
    int yoff = -1;

    for (int x = 2; x < xSize; x++)
      {
        int vy1 = pn1.getPixelUnchecked(x - 2, 0)
                  + pn1.getPixelUnchecked(x - 1, 0)
                  + pn1.getPixelUnchecked(x, 0);
        int vy2 = pn1.getPixelUnchecked(x - 2, 1)
                  + pn1.getPixelUnchecked(x - 1, 1)
                  + pn1.getPixelUnchecked(x, 1);
        int vy3 = pn1.getPixelUnchecked(x - 2, 2)
                  + pn1.getPixelUnchecked(x - 1, 2)
                  + pn1.getPixelUnchecked(x, 2);
        // force cast here
        double deltax = pn1.getPixelUnchecked(x, 0)
                        + pn1.getPixelUnchecked(x, 1)
                        + pn1.getPixelUnchecked(x, 2)
                        - pn1.getPixelUnchecked(x - 2, 0)
                        - pn1.getPixelUnchecked(x - 2, 1)
                        - pn1.getPixelUnchecked(x - 2, 2);
        // force cast here
        double deltay = vy3 - vy1;

        // rotation by M_PI/8
        double deltax1 = deltax        - tan1 * deltay;
        double deltay1 = tan1 * deltax + deltay;

        // sectors can be detected comparing deltax1 and deltay1 and zero

        pn2.setPixelUnchecked(x + xoff, 1, getDir(deltax1, deltay1));

        for (int y = 3; y < ySize; y++)
          {
            deltax = deltax
                     + pn1.getPixelUnchecked(x - 2, y - 3)
                     - pn1.getPixelUnchecked(x - 2, y)
                     - pn1.getPixelUnchecked(x, y - 3)
                     + pn1.getPixelUnchecked(x, y);
            vy1 = vy2;
            vy2 = vy3;
            vy3 = pn1.getPixelUnchecked(x - 2, y)
                  + pn1.getPixelUnchecked(x - 1, y)
                  + pn1.getPixelUnchecked(x, y);
            deltay = vy3 - vy1;

            // rotation by M_PI/8
            double deltax1 = deltax        - tan1 * deltay;
            double deltay1 = tan1 * deltax + deltay;

            // sectors can be detected comparing deltax1 and deltay1 and zero

            pn2.setPixelUnchecked(x + xoff, y + yoff, getDir(deltax1, deltay1));
          }
      }
  }
#undef FNAME
}
