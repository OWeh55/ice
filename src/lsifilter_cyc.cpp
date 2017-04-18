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

namespace ice
{
// Elementare Filterfunktionen mit zyklischer Behandlung der Werte
// werden typischerweise über LSIImg aufgerufen

  /**
     * This template function applies a given LSI filter on an image with
     * values with cyclic properties, like the Hue channel in HSI colorspace
     * @param src the source image
     * @param dest the destination image (doesn't need to differ from src)
     * @param nx the horizontal size of the filter mask
     * @param ny the vertical size of the filter mask
     * @param mask the filter mask
     * @param norm the scaling factor
     * @param offset the value representing 0 (to handle negative results)
     */
  template<typename SrcType, typename DestType>
  int lsiimgcyc(const Image& src, const Image& dest,
                int nx, int ny, int* mask,
                int norm, int offset)
  {
    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int nx2 = nx / 2;
    int ny2 = ny / 2;
    int maxVal = dest.maxval;
    int maxVal1 = maxVal + 1;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int y = 0; y < src->ysize - (ny2 + 1); y++)
      {
        for (int x = 0; x < src->xsize - (nx2 + 1); x++)
          {
            int tmpVal = 0;
            int xBest = 0;
            int xBVal = 0;

            // get sme without shifting
            int mid = 0;

            // get mean value
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                {
                  mid += Pixels[y + b][x + a];
                }

            mid /= (nx * ny);

            // get the square mean error
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                xBVal += (Pixels[y + b][x + a] - mid)
                         * (Pixels[y + b][x + a] - mid);

            xBVal /= (nx * ny - 1);

            // test the nx*ny different shifts xAkt
            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  int xAkt = maxVal - Pixels[y + j][x + i] + 1;
                  int mid = 0;
                  int sme = 0;

                  // get mean value by shifting pixelvalues by xAkt
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      {
                        mid += (xAkt + Pixels[y + b][x + a]) % maxVal1;
                      }

                  mid /= (nx * ny);

                  // get the square mean error
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      sme += (((xAkt + Pixels[y + b][x + a]) % maxVal1) - mid)
                             * (((xAkt + Pixels[y + b][x + a]) % maxVal1) - mid);

                  sme /= (nx * ny - 1);

                  if (sme < xBVal)
                    {
                      xBVal = sme;
                      xBest = xAkt;
                    }
                }

            for (int j = 0; j < ny; j++)
              for (int i = 0; i < nx; i++)
                {
                  tmpVal += (mask[j * nx + i] * ((xBest + Pixels[y + j][x + i]) % maxVal1));
                }

            Pixeld[y + ny2][x + nx2] = limited(offset + (((tmpVal / norm) - xBest) % maxVal1), maxVal);
          }
      }

#ifdef CONTROLLED_REFRESH
    dest->needRefresh();
#endif
    return OK;
  }

  int lsiimgcyc_std(const Image& src, const Image& dest,
                    int nx, int ny, int* mask,
                    int norm, int offset)
  {
    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int nx2 = nx / 2;
    int ny2 = ny / 2;
    int maxVal = dest.maxval;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, (dest.maxval + 1) / 2);

    for (int y = 0; y < src->ysize - (ny2 + 1); y++)
      {
        for (int x = 0; x < src->xsize - (nx2 + 1); x++)
          {
            int tmpVal = 0;
            int xBest = 0;
            int xBVal = 0;

            // get sme without shifting
            int mid = 0;

            // get mean value
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                {
                  mid += tmp.getPixelUnchecked(x + a, y + b);
                }

            mid /= (nx * ny);

            // get the square mean error
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                xBVal += tmp.getPixelUnchecked(x + a, y + b)
                         * tmp.getPixelUnchecked(x + a, y + b);

            xBVal /= (nx * ny - 1);

            // test the nx*ny different shifts xAkt
            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  int xAkt = maxVal - tmp.getPixelUnchecked(x + i, y + j) + 1;
                  int mid = 0;
                  int sme = 0;

                  // get mean value by shifting pixelvalues by xAkt
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      {
                        mid += (xAkt + tmp.getPixelUnchecked(x + a, y + b)) % maxVal;
                      }

                  mid /= (nx * ny);

                  // get the square mean error
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      sme += ((xAkt + tmp.getPixelUnchecked(x + a, y + b)) % maxVal - mid)
                             * ((xAkt + tmp.getPixelUnchecked(x + a, y + b)) % maxVal - mid);

                  sme /= (nx * ny - 1);

                  if (sme < xBVal)
                    {
                      xBVal = sme;
                      xBest = xAkt;
                    }
                }

            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  tmpVal += mask[i] * ((xBest + tmp.getPixelUnchecked(x + i, y + j)) % maxVal);
                }

            dest.setPixelUnchecked(x + nx2, y + ny2, limited(offset + tmpVal / norm, maxVal));
          }
      }

#ifdef CONTROLLED_REFRESH
    dest->needRefresh();
#endif
    return OK;
  }

  /**
   * This template function applies a given LSI filter on an image with
   * cyclic properties, like the Hue channel in HSI colorspace
   * @param src the source image
   * @param dest the destination image (doesn't need to differ from src)
   * @param nx the horizontal size of the filter mask
   * @param ny the vertical size of the filter mask
   * @param mask the filter mask (scaling factor included in the double values)
   * @param offset the value representing 0 (to handle negative results)
   */
  template<typename SrcType, typename DestType>
  int lsiimgcyc(const Image& src, const Image& dest,
                int nx, int ny, double* mask,
                int offset)
  {
    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int nx2 = nx / 2;
    int ny2 = ny / 2;
    int maxVal = dest.maxval;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, (dest.maxval + 1) / 2);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int y = 0; y < src->ysize - (ny2 + 1); y++)
      {
        for (int x = 0; x < src->xsize - (nx2 + 1); x++)
          {
            double tmpVal = offset;
            int xBest = 0;
            int xBVal = 0;

            // get sme without shifting
            int mid = 0;

            // get mean value
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                {
                  mid += Pixels[y + b][x + a];
                }

            mid /= (nx * ny);

            // get the square mean error
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                xBVal += (Pixels[y + b][x + a] - mid)
                         * (Pixels[y + b][x + a] - mid);

            xBVal /= (nx * ny - 1);

            // test the nx*ny different shifts xAkt
            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  int xAkt = maxVal - Pixels[y + j][x + i] + 1;
                  int mid = 0;
                  int sme = 0;

                  // get mean value by shifting pixelvalues by xAkt
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      {
                        mid += (xAkt + Pixels[y + b][x + a]) % maxVal;
                      }

                  mid /= (nx * ny);

                  // get the square mean error
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      sme += ((xAkt + Pixels[y + b][x + a]) % maxVal - mid)
                             * ((xAkt + Pixels[y + b][x + a]) % maxVal - mid);

                  sme /= (nx * ny - 1);

                  if (sme < xBVal)
                    {
                      xBVal = sme;
                      xBest = xAkt;
                    }
                }

            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  tmpVal += mask[i] * ((xBest + Pixels[y + j][x + i]) % maxVal);
                }

            Pixeld[y + ny2][x + nx2] = limited(RoundInt(tmpVal), maxVal);
          }
      }

#ifdef CONTROLLED_REFRESH
    dest->needRefresh();
#endif
    return OK;
  }

  int lsiimgcyc_std(const Image& src, const Image& dest,
                    int nx, int ny, double* mask,
                    int offset)
  {
    Image tmp = src;

    // since we write directly in dest and need to read from positions where we wrote before
    // we need to copy the source image if it shares its pixelarray with the destination image
    if (src == dest)
      {
        tmp.copy(src);
      }

    int nx2 = nx / 2;
    int ny2 = ny / 2;
    int maxVal = dest.maxval;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, (dest.maxval + 1) / 2);

    for (int y = 0; y < src->ysize - (ny2 + 1); y++)
      {
        for (int x = 0; x < src->xsize - (nx2 + 1); x++)
          {
            double tmpVal = offset;
            int xBest = 0;
            int xBVal = 0;

            // get sme without shifting
            int mid = 0;

            // get mean value
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                {
                  mid += tmp.getPixelUnchecked(x + a, y + b);
                }

            mid /= (nx * ny);

            // get the square mean error
            for (int a = 0; a < nx; a++)
              for (int b = 0; b < ny; b++)
                xBVal += tmp.getPixelUnchecked(x + a, y + b)
                         * tmp.getPixelUnchecked(x + a, y + b);

            xBVal /= (nx * ny - 1);

            // test the nx*ny different shifts xAkt
            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  int xAkt = maxVal - tmp.getPixelUnchecked(x + i, y + j) + 1;
                  int mid = 0;
                  int sme = 0;

                  // get mean value by shifting pixelvalues by xAkt
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      {
                        mid += (xAkt + tmp.getPixelUnchecked(x + a, y + b)) % maxVal;
                      }

                  mid /= (nx * ny);

                  // get the square mean error
                  for (int a = 0; a < nx; a++)
                    for (int b = 0; b < ny; b++)
                      sme += ((xAkt + tmp.getPixelUnchecked(x + a, y + b)) % maxVal - mid)
                             * ((xAkt + tmp.getPixelUnchecked(x + a, y + b)) % maxVal - mid);

                  sme /= (nx * ny - 1);

                  if (sme < xBVal)
                    {
                      xBVal = sme;
                      xBest = xAkt;
                    }
                }

            for (int i = 0; i < nx; i++)
              for (int j = 0; j < ny; j++)
                {
                  tmpVal += mask[i] * ((xBest + tmp.getPixelUnchecked(x + i, y + j)) % maxVal);
                }

            dest.setPixelUnchecked(x + nx2, y + ny2, limited(RoundInt(tmpVal), maxVal));
          }
      }

#ifdef CONTROLLED_REFRESH
    dest->needRefresh();
#endif
    return OK;
  }
#undef FNAME

  int lsiimgcyc(const Image& src, const Image& dest, int nx, int ny, int* mask, int norm, int off)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return lsiimgcyc<PixelType1, PixelType1>(src, dest, nx, ny, mask, norm, off);
      case 18:
        return lsiimgcyc<PixelType1, PixelType2>(src, dest, nx, ny, mask, norm, off);
      case 19:
        return lsiimgcyc<PixelType1, PixelType3>(src, dest, nx, ny, mask, norm, off);
      case 33:
        return lsiimgcyc<PixelType2, PixelType1>(src, dest, nx, ny, mask, norm, off);
      case 34:
        return lsiimgcyc<PixelType2, PixelType2>(src, dest, nx, ny, mask, norm, off);
      case 35:
        return lsiimgcyc<PixelType2, PixelType3>(src, dest, nx, ny, mask, norm, off);
      case 49:
        return lsiimgcyc<PixelType3, PixelType1>(src, dest, nx, ny, mask, norm, off);
      case 50:
        return lsiimgcyc<PixelType3, PixelType2>(src, dest, nx, ny, mask, norm, off);
      case 51:
        return lsiimgcyc<PixelType3, PixelType3>(src, dest, nx, ny, mask, norm, off);
      default:
        return lsiimgcyc_std(src, dest, nx, ny, mask, norm, off);
      }
  }

  int lsiimgcyc(const Image& src, const Image& dest, int nx, int ny, double* mask, int off)
  {
    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        return lsiimgcyc<PixelType1, PixelType1>(src, dest, nx, ny, mask, off);
      case 18:
        return lsiimgcyc<PixelType1, PixelType2>(src, dest, nx, ny, mask, off);
      case 19:
        return lsiimgcyc<PixelType1, PixelType3>(src, dest, nx, ny, mask, off);
      case 33:
        return lsiimgcyc<PixelType2, PixelType1>(src, dest, nx, ny, mask, off);
      case 34:
        return lsiimgcyc<PixelType2, PixelType2>(src, dest, nx, ny, mask, off);
      case 35:
        return lsiimgcyc<PixelType2, PixelType3>(src, dest, nx, ny, mask, off);
      case 49:
        return lsiimgcyc<PixelType3, PixelType1>(src, dest, nx, ny, mask, off);
      case 50:
        return lsiimgcyc<PixelType3, PixelType2>(src, dest, nx, ny, mask, off);
      case 51:
        return lsiimgcyc<PixelType3, PixelType3>(src, dest, nx, ny, mask, off);
      default:
        return lsiimgcyc_std(src, dest, nx, ny, mask, off);
      }
  }
#undef FNAME
}
