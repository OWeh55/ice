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

#include <vector>
#include <algorithm>
#include "based.h"
#include "util.h"
#include "filter.h"
#include "LsiFilter.h"
#include "vectortools.h"

using namespace std;
namespace ice
{

  template<typename T>
  int getBest(vector<T>& v, int modulo)
  {
    // find biggest "hole" in distribution
    sort(v.begin(), v.end());
    int i_best = 0;
    int diff_best = modulo + v[0] - v.back();
    for (int i = 1; i < v.size(); i++)
      {
        int ndiff = v[i] - v[i - 1];
        if (ndiff > diff_best)
          {
            diff_best = ndiff;
            i_best = i;
          }
      }
    return modulo - v[i_best];
  }

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
  void lsiimgcyc(const Image& src, const Image& dest,
                 int nx, int ny, const int* mask,
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
    int maxVal = src.maxval;
    int modulo = maxVal + 1;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    std::vector<SrcType> v(nx * ny);

    for (int y = 0; y < src.ysize - ny; y++)
      for (int x = 0; x < src.xsize - nx; x++)
        {
          // read pixels in neighbourhood
          int i = 0;
          for (int b = 0; b < ny; b++)
            for (int a = 0; a < nx; a++)
              {
                v[i] = Pixels[y + b][x + a];
                i++;
              }

          SrcType xBest = getBest(v, modulo);

          int sum = 0;
          for (int j = 0; j < ny; j++)
            for (int i = 0; i < nx; i++)
              {
                int val = (Pixels[y + j][x + i] + xBest) % modulo;
                sum += mask[j * nx + i] * val;
              }

          int val = offset + sum / norm - xBest;
          Pixeld[y + ny2][x + nx2] = (val % modulo + modulo) % modulo;
        }
  }


  void lsiimgcyc_std(const Image& src, const Image& dest,
                     int nx, int ny, const int* mask,
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
    int modulo = maxVal + 1;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, (dest.maxval + 1) / 2);

    std::vector<int> v(nx * ny);

    for (int y = 0; y < src.ysize - ny; y++)
      for (int x = 0; x < src.xsize - nx; x++)
        {
          // read pixels in neighbourhood
          int i = 0;
          for (int b = 0; b < ny; b++)
            for (int a = 0; a < nx; a++)
              {
                v[i] = src.getPixelUnchecked(x + a, y + b);
                i++;
              }

          int xBest = getBest(v, modulo);

          int sum = 0;
          for (int j = 0; j < ny; j++)
            for (int i = 0; i < nx; i++)
              {
                int val = (src.getPixelUnchecked(x + i, y + j) + xBest) % modulo;
                sum += mask[j * nx + i] * val;
              }
          int val = offset + sum / norm - xBest;
          dest.setPixelUnchecked(x + nx2, y + ny2, (val % modulo + modulo) % modulo);
        }
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
  void lsiimgcyc(const Image& src, const Image& dest,
                 int nx, int ny, const double* mask,
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
    int maxVal = src.maxval;
    int modulo = maxVal + 1;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, offset);

    const SrcType** Pixels = (const SrcType**)tmp->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    std::vector<SrcType> v(nx * ny);

    for (int y = 0; y < src.ysize - ny; y++)
      for (int x = 0; x < src.xsize - nx; x++)
        {
          // read pixels in neighbourhood
          int i = 0;
          for (int b = 0; b < ny; b++)
            for (int a = 0; a < nx; a++)
              {
                v[i] = Pixels[y + b][x + a];
                i++;
              }

          SrcType xBest = getBest(v, modulo);

          double sum = 0;
          for (int j = 0; j < ny; j++)
            for (int i = 0; i < nx; i++)
              {
                int val = (Pixels[y + j][x + i] + xBest) % modulo;
                sum += mask[j * nx + i] * val;
              }

          int val = offset + sum - xBest;
          Pixeld[y + ny2][x + nx2] = (val % modulo + modulo) % modulo;
        }
  }

  void lsiimgcyc_std(const Image& src, const Image& dest,
                     int nx, int ny, const double* mask,
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
    int modulo = maxVal + 1;

    // the border of width nx/2 and height ny/2 around
    // the image will be filled with maxval/2
    setborder(dest, nx2, ny2, (dest.maxval + 1) / 2);

    std::vector<int> v(nx * ny);

    for (int y = 0; y < src.ysize - ny; y++)
      for (int x = 0; x < src.xsize - nx; x++)
        {
          // read pixels in neighbourhood
          int i = 0;
          for (int b = 0; b < ny; b++)
            for (int a = 0; a < nx; a++)
              {
                v[i] = src.getPixelUnchecked(x + a, y + b);
                i++;
              }

          int xBest = getBest(v, modulo);

          double sum = 0;
          for (int j = 0; j < ny; j++)
            for (int i = 0; i < nx; i++)
              {
                int val = (src.getPixelUnchecked(x + i, y + j) + xBest) % modulo;
                sum += mask[j * nx + i] * val;
              }
          int val = offset + sum - xBest;
          dest.setPixelUnchecked(x + nx2, y + ny2, (val % modulo + modulo) % modulo);
        }
  }

  void lsiimgcyc(const Image& src, const Image& dest, int nx, int ny, const int* mask, int norm, int off)
  {
    int typSelector = (src->ImageType() * 16) + dest->ImageType();

    switch (typSelector)
      {
      case 1*16+1:
        lsiimgcyc<PixelType1, PixelType1>(src, dest, nx, ny, mask, norm, off);
        break;
      case 1*16+2:
        lsiimgcyc<PixelType1, PixelType2>(src, dest, nx, ny, mask, norm, off);
        break;
      case 1*16+3:
        lsiimgcyc<PixelType1, PixelType3>(src, dest, nx, ny, mask, norm, off);
        break;
      case 2*16+1:
        lsiimgcyc<PixelType2, PixelType1>(src, dest, nx, ny, mask, norm, off);
        break;
      case 2*16+2:
        lsiimgcyc<PixelType2, PixelType2>(src, dest, nx, ny, mask, norm, off);
        break;
      case 2*16+3:
        lsiimgcyc<PixelType2, PixelType3>(src, dest, nx, ny, mask, norm, off);
        break;
      case 3*16+1:
        lsiimgcyc<PixelType3, PixelType1>(src, dest, nx, ny, mask, norm, off);
        break;
      case 3*16+2:
        lsiimgcyc<PixelType3, PixelType2>(src, dest, nx, ny, mask, norm, off);
        break;
      case 3*16+3:
        lsiimgcyc<PixelType3, PixelType3>(src, dest, nx, ny, mask, norm, off);
        break;
      default:
        lsiimgcyc_std(src, dest, nx, ny, mask, norm, off);
        break;
      }
  }

  void lsiimgcyc(const Image& src, const Image& dest, int nx, int ny, const double* mask, int off)
  {
    int typSelector = (src->ImageType() * 16) + dest->ImageType();

    switch (typSelector)
      {
      case 17:
        lsiimgcyc<PixelType1, PixelType1>(src, dest, nx, ny, mask, off);
        break;
      case 18:
        lsiimgcyc<PixelType1, PixelType2>(src, dest, nx, ny, mask, off);
        break;
      case 19:
        lsiimgcyc<PixelType1, PixelType3>(src, dest, nx, ny, mask, off);
        break;
      case 33:
        lsiimgcyc<PixelType2, PixelType1>(src, dest, nx, ny, mask, off);
        break;
      case 34:
        lsiimgcyc<PixelType2, PixelType2>(src, dest, nx, ny, mask, off);
        break;
      case 35:
        lsiimgcyc<PixelType2, PixelType3>(src, dest, nx, ny, mask, off);
        break;
      case 49:
        lsiimgcyc<PixelType3, PixelType1>(src, dest, nx, ny, mask, off);
        break;
      case 50:
        lsiimgcyc<PixelType3, PixelType2>(src, dest, nx, ny, mask, off);
        break;
      case 51:
        lsiimgcyc<PixelType3, PixelType3>(src, dest, nx, ny, mask, off);
        break;
      default:
        lsiimgcyc_std(src, dest, nx, ny, mask, off);
        break;
      }
  }
#undef FNAME
}
