/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
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

#include "based.h"
#include "util.h"
#include "filter.h"
#include "lsifilter.h"

// Elementare Filterfunktionen
// werden typischerweise über LSIImg aufgerufen

namespace ice
{

#define FNAME "lsiimg"
  /**
   * This template function applies a LSI filter to an image, typically used
   * internally, not by user.
   * no parameter check here !
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (_integer_ values)
   * @param norm the normalization factor
   * @param offset the value representing 0 in destination image
   */
  template<typename SrcType, typename DestType>
  void lsiimg(const Image& src, const Image& dest,
              int nx, int ny, int* mask,
              int norm, int offset)
  {
    int offset_dest_x = nx / 2;
    int offset_dest_y = ny / 2;

    int dmax = dest.maxval;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx / 2, ny / 2, offset);

    // direct access to pixel
    const SrcType** PixelSource = (const SrcType**)src->getDataPtr();
    DestType** PixelDestination = (DestType**)dest->getDataPtr();

    if (!(ny == 3 && nx == 3))
      {
        for (int y = 0; y < src->ysize + 1 - ny; y++)
          {
            int ye = y + ny;
            DestType* DestinationRow = PixelDestination[y + offset_dest_y];

            for (int x = 0; x < src->xsize + 1 - nx; x++)
              {
                int* mi = mask;
                int xe = x + nx;
                int tmpVal = 0;

                for (int j = y; j < ye; j++)
                  {
                    const SrcType* Zeile = PixelSource[j];

                    for (int i = x; i < xe; i++)
                      {
                        tmpVal += *mi * Zeile[i];
                        ++mi;
                      }
                  }

                DestinationRow[x + offset_dest_x] = limited(offset + tmpVal / norm, dmax);
              }
          }
      }
    else
      {
        for (int y = 0; y < src->ysize + 1 - 3; y++)
          {
            DestType* DestinationRow = PixelDestination[y + 1];

            for (int x = 0; x < src->xsize + 1 - 3; x++)
              {
                int tmpVal = 0;
                tmpVal =
                  PixelSource[y][x] * mask[0] +
                  PixelSource[y][x + 1] * mask[1] +
                  PixelSource[y][x + 2] * mask[2] +
                  PixelSource[y + 1][x] * mask[3] +
                  PixelSource[y + 1][x + 1] * mask[4] +
                  PixelSource[y + 1][x + 2] * mask[5] +
                  PixelSource[y + 2][x] * mask[6] +
                  PixelSource[y + 2][x + 1] * mask[7] +
                  PixelSource[y + 2][x + 2] * mask[8];
                DestinationRow[x + 1] = limited(offset + tmpVal / norm, dmax);
              }
          }
      }

  }

  void lsiimg_std(const Image& src, const Image& dest,
                  int nx, int ny, int* mask,
                  int norm, int offset)
  {
    int offset_dest_x = nx / 2;
    int offset_dest_y = ny / 2;
    int dmax = dest.maxval;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx / 2, ny / 2, offset);

    for (int y = 0; y < src->ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src->xsize + 1 - nx; x++)
          {
            int tmpVal = 0;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * src.getPixelUnchecked(x + i, y + j);
                }

            dest.setPixelUnchecked(x + offset_dest_x, y + offset_dest_y, limited(offset + tmpVal / norm, dmax));
          }
      }
  }

  /**
   * This template function applies a LSI filter to an image, typically used
   * internally, not by user.
   * no parameter check here !
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   * @param offset the value representing 0 in destination image
   */
  template<typename SrcType, typename DestType>
  void lsiimg(const Image& src, const Image& dest,
              int nx, int ny, double* mask,
              int offset)
  {
    int nx2 = nx / 2;
    int ny2 = ny / 2;
    int dmax = dest.maxval;
    // the border of width nx/2 and height ny/2 around
    // the image will be filled with offset
    setborder(dest, nx2, ny2, offset);

    const SrcType** PixelSource = (const SrcType**)src->getDataPtr();
    DestType** PixelDestination = (DestType**)dest->getDataPtr();

    for (int y = 0; y < src->ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src->xsize + 1 - nx; x++)
          {
            double tmpVal = offset;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * PixelSource[y + j][x + i];
                }

            PixelDestination[y + ny2][x + nx2] = limited(RoundInt(tmpVal), dmax);
          }
      }

  }

  void lsiimg_std(const Image& src, const Image& dest,
                  int nx, int ny, double* mask,
                  int offset)
  {
    int nx2 = nx / 2;
    int ny2 = ny / 2;
    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, (dest.maxval + 1) / 2);

    for (int y = 0; y < src->ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src->xsize + 1 - nx; x++)
          {
            double tmpVal = offset;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * src.getPixelUnchecked(x + i, y + j);
                }

            dest.setPixelUnchecked(x + nx2, y + ny2, RoundInt(tmpVal));
          }
      }

  }

  void lsiimg(const Image& src, const Image& dest, int nx, int ny, int* mask, int norm, int off)
  {
    // if source == dest we need a copy !
    Image tmp = src;

    if (src == dest)
      {
        tmp = NewImg(src, true);
      }

    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        lsiimg<PixelType1, PixelType1>(tmp, dest, nx, ny, mask, norm, off);
      case 18:
        lsiimg<PixelType1, PixelType2>(tmp, dest, nx, ny, mask, norm, off);
      case 19:
        lsiimg<PixelType1, PixelType3>(tmp, dest, nx, ny, mask, norm, off);

      case 33:
        lsiimg<PixelType2, PixelType1>(tmp, dest, nx, ny, mask, norm, off);
      case 34:
        lsiimg<PixelType2, PixelType2>(tmp, dest, nx, ny, mask, norm, off);
      case 35:
        lsiimg<PixelType2, PixelType3>(tmp, dest, nx, ny, mask, norm, off);

      case 49:
        lsiimg<PixelType3, PixelType1>(tmp, dest, nx, ny, mask, norm, off);
      case 50:
        lsiimg<PixelType3, PixelType2>(tmp, dest, nx, ny, mask, norm, off);
      case 51:
        lsiimg<PixelType3, PixelType3>(tmp, dest, nx, ny, mask, norm, off);

      default:
        // non standard images
        lsiimg_std(tmp, dest, nx, ny, mask, norm, off);
      }
  }

  void lsiimg(const Image& src, const Image& dest, int nx, int ny, double* mask, int off)
  {
    // if source == dest we need a copy !
    Image tmp = src;

    if (src == dest)
      {
        tmp = NewImg(src, true);
      }

    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        lsiimg<PixelType1, PixelType1>(tmp, dest, nx, ny, mask, off);
      case 18:
        lsiimg<PixelType1, PixelType2>(tmp, dest, nx, ny, mask, off);
      case 19:
        lsiimg<PixelType1, PixelType3>(tmp, dest, nx, ny, mask, off);
      case 33:
        lsiimg<PixelType2, PixelType1>(tmp, dest, nx, ny, mask, off);
      case 34:
        lsiimg<PixelType2, PixelType2>(tmp, dest, nx, ny, mask, off);
      case 35:
        lsiimg<PixelType2, PixelType3>(tmp, dest, nx, ny, mask, off);
      case 49:
        lsiimg<PixelType3, PixelType1>(tmp, dest, nx, ny, mask, off);
      case 50:
        lsiimg<PixelType3, PixelType2>(tmp, dest, nx, ny, mask, off);
      case 51:
        lsiimg<PixelType3, PixelType3>(tmp, dest, nx, ny, mask, off);

      default:
        // non standard images
        lsiimg_std(tmp, dest, nx, ny, mask, off);
      }
  }

  /**
   * This template function applies a LSI filter to an image, typically used
   * internally, not by user.
   * no parameter check here !
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (_integer_ values)
   * @param norm the normalization factor
   * @param offset the value representing 0 in destination image
   */
  template<typename SrcType>
  void lsiimg(const Image& src, ImageD dest,
              int nx, int ny, int* mask,
              int norm)
  {
    int offset_dest_x = nx / 2;
    int offset_dest_y = ny / 2;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx / 2, ny / 2, 0.0);

    // direct access to pixel
    const SrcType** PixelSource = (const SrcType**)src->getDataPtr();

    if (!(ny == 3 && nx == 3))
      {
        for (int y = 0; y < src->ysize + 1 - ny; y++)
          {
            int ye = y + ny;

            for (int x = 0; x < src->xsize + 1 - nx; x++)
              {
                int* mi = mask;
                int xe = x + nx;
                int tmpVal = 0;

                for (int j = y; j < ye; j++)
                  {
                    const SrcType* Zeile = PixelSource[j];

                    for (int i = x; i < xe; i++)
                      {
                        tmpVal += *mi * Zeile[i];
                        ++mi;
                      }
                  }

                PutValD(dest, x + offset_dest_x, y + offset_dest_y, tmpVal / norm);
              }
          }
      }
    else
      {
        for (int y = 0; y < src->ysize + 1 - 3; y++)
          {
            for (int x = 0; x < src->xsize + 1 - 3; x++)
              {
                int tmpVal = 0;
                tmpVal =
                  PixelSource[y][x] * mask[0] +
                  PixelSource[y][x + 1] * mask[1] +
                  PixelSource[y][x + 2] * mask[2] +
                  PixelSource[y + 1][x] * mask[3] +
                  PixelSource[y + 1][x + 1] * mask[4] +
                  PixelSource[y + 1][x + 2] * mask[5] +
                  PixelSource[y + 2][x] * mask[6] +
                  PixelSource[y + 2][x + 1] * mask[7] +
                  PixelSource[y + 2][x + 2] * mask[8];
                PutValD(dest, x + 1, y + 1, tmpVal / norm);
              }
          }
      }

  }

  void lsiimg_std(const Image& src, ImageD dest,
                  int nx, int ny, int* mask,
                  int norm)
  {
    int offset_dest_x = nx / 2;
    int offset_dest_y = ny / 2;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx / 2, ny / 2, 0.0);

    for (int y = 0; y < src->ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src->xsize + 1 - nx; x++)
          {
            int tmpVal = 0;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * src.getPixelUnchecked(x + i, y + j);
                }

            PutValD(dest, x + offset_dest_x, y + offset_dest_y, tmpVal / norm);
          }
      }
  }

  /**
   * This template function applies a LSI filter to an image, typically used
   * internally, not by user.
   * no parameter check here !
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   * @param offset the value representing 0 in destination image
   */
  template<typename SrcType>
  void lsiimg(const Image& src, ImageD dest,
              int nx, int ny, double* mask)
  {
    int nx2 = nx / 2;
    int ny2 = ny / 2;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with offset
    setborder(dest, nx2, ny2, 0.0);

    const SrcType** PixelSource = (const SrcType**)src->getDataPtr();

    for (int y = 0; y < src->ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src->xsize + 1 - nx; x++)
          {
            double tmpVal = 0;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * PixelSource[y + j][x + i];
                }

            PutValD(dest, x + nx2, y + ny2, tmpVal);
          }
      }

  }

  void lsiimg_std(const Image& src, ImageD dest,
                  int nx, int ny, double* mask)
  {
    int nx2 = nx / 2;
    int ny2 = ny / 2;
    // the border of width nx/2 and height ny/2 around
    // the image will be filled with 0.0
    setborder(dest, nx2, ny2, 0.0);

    for (int y = 0; y < src->ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src->xsize + 1 - nx; x++)
          {
            double tmpVal = 0.0;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * src.getPixelUnchecked(x + i, y + j);
                }

            PutValD(dest, x + nx2, y + ny2, tmpVal);
          }
      }
  }

  void lsiimg(const Image& src, ImageD dest, int nx, int ny, int* mask, int norm)
  {
    switch (src->ImageType())
      {
      case 1:
        lsiimg<PixelType1>(src, dest, nx, ny, mask, norm);
      case 2:
        lsiimg<PixelType2>(src, dest, nx, ny, mask, norm);
      case 3:
        lsiimg<PixelType3>(src, dest, nx, ny, mask, norm);
      default:
        lsiimg_std(src, dest, nx, ny, mask, norm);
      }
  }

  void lsiimg(const Image& src, ImageD dest, int nx, int ny, double* mask)
  {
    switch (src->ImageType())
      {
      case 1:
        lsiimg<PixelType1>(src, dest, nx, ny, mask);
      case 2:
        lsiimg<PixelType2>(src, dest, nx, ny, mask);
      case 3:
        lsiimg<PixelType3>(src, dest, nx, ny, mask);
      default:
        lsiimg_std(src, dest, nx, ny, mask);
      }
  }

  void lsiimg(ImageD src, ImageD dest,
              int nx, int ny, int* mask,
              int norm)
  {
    ImageD tmp = src;

    // since we will write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if src == dest
    if (src == dest)
      {
        tmp = NewImgD(src, true);
      }

    int offset_dest_x = nx / 2;
    int offset_dest_y = ny / 2;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with 0.0
    setborder(dest, nx / 2, ny / 2, 0);

    for (int y = 0; y < src.ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src.xsize + 1 - nx; x++)
          {
            double tmpVal = 0;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * GetValD(tmp, x + i, y + j);
                }

            PutValD(dest, x + offset_dest_x, y + offset_dest_y, tmpVal / norm);
          }
      }
  }

  void lsiimg(ImageD src, ImageD dest,
              int nx, int ny, double* mask)
  {
    ImageD tmp = src;

    // since we will write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if src == dest
    if (src == dest)
      {
        tmp = NewImgD(src, true);
      }

    int offset_dest_x = nx / 2;
    int offset_dest_y = ny / 2;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with 0.0
    setborder(dest, nx / 2, ny / 2, 0.0);

    for (int y = 0; y < src.ysize + 1 - ny; y++)
      {
        for (int x = 0; x < src.xsize + 1 - nx; x++)
          {
            double tmpVal = 0;

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += mask[j * nx + i] * GetValD(tmp, x + i, y + j);
                }

            PutValD(dest, x + offset_dest_x, y + offset_dest_y, tmpVal);
          }
      }
  }

}
