/************************************************************************
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
 *
 * This source file provides the implementation of some useful
 * nonlinear filters
 *
 * int MedianImg(const ColorImage &src, const ColorImage &dest, int size)
 *
 ***********************************************************************/

#include "limits.h"
#include "ColorImageFunctions.h"

namespace ice
{
#define FNAME "MedianImg"
  template <typename SrcType, typename DestType>
  void MedianImg(const ColorImage& src, const ColorImage& dest, int size)
  {
    const SrcType** Pixels1 = (const SrcType**)(src.redImage()->getDataPtr());
    const SrcType** Pixels2 = (const SrcType**)(src.greenImage()->getDataPtr());
    const SrcType** Pixels3 = (const SrcType**)(src.blueImage()->getDataPtr());
    DestType** Pixeld1 = (DestType**)(dest.redImage()->getDataPtr());
    DestType** Pixeld2 = (DestType**)(dest.greenImage()->getDataPtr());
    DestType** Pixeld3 = (DestType**)(dest.blueImage()->getDataPtr());

    int xSize = src.xsize;
    int ySize = src.ysize;

    // Rand der Groesse filtersize/2 kann nicht berechnet werden
    setborder(dest.redImage(), size / 2, dest.maxval / 2);
    setborder(dest.greenImage(), size / 2, dest.maxval / 2);
    setborder(dest.blueImage(), size / 2, dest.maxval / 2);

    int yMax = ySize - size + 1;
    int xMax = xSize - size + 1;

    int size2 = size / 2;

    // Median als Pixel mit der kleinsten Summe der Distanzen in L1-Norm zu den anderen Werten
#ifdef OPENMP
    #pragma omp parallel for schedule(dynamic,30)
#endif
    for (int y = 0; y < yMax; y++)
      {
        for (int x = 0; x < xMax; x++)
          {
            int med = INT_MAX;
            int medX = 0;
            int medY = 0;
            // calculate for every pixel the mean distance to the others
            for (int ay = y; ay < y + size; ay++)
              for (int ax = x; ax < x + size; ax++)
                {
                  int tmp = 0;
                  int aktx = Pixels1[ay][ax];
                  int akty = Pixels2[ay][ax];
                  int aktz = Pixels3[ay][ax];
                  for (int my = y; my < y + size && tmp < med; my++)
                    for (int mx = x; mx < x + size && tmp < med; mx++)
                      {
                        int dx = abs((int)Pixels1[my][mx] - aktx);
                        int dy = abs((int)Pixels2[my][mx] - akty);
                        int dz = abs((int)Pixels3[my][mx] - aktz);
                        tmp += dx + dy + dz;
                      }
                  if (tmp < med)
                    {
                      medX = ax;
                      medY = ay;
                      med = tmp;
                    }
                }
            //dest.PutValue(x+size/2, y+size/2, src.GetValue(x+medX, y+medY));
            Pixeld1[y + size2][x + size2] = Pixels1[medY][medX];
            Pixeld2[y + size2][x + size2] = Pixels2[medY][medX];
            Pixeld3[y + size2][x + size2] = Pixels3[medY][medX];
          }
#ifdef CONTROLLED_REFRESH
        dest.redImage()->needRefresh();
        dest.greenImage()->needRefresh();
        dest.blueImage()->needRefresh();
#endif
      }
  }

  void MedianImg_std(const ColorImage& src, const ColorImage& dest, int size)
  {
    ColorValue akt, vgl;

    int xSize = src.xsize;
    int ySize = src.ysize;

    // Rand der Groesse filtersize/2 kann nicht berechnet werden
    setborder(dest.redImage(), size / 2, dest.maxval / 2);
    setborder(dest.greenImage(), size / 2, dest.maxval / 2);
    setborder(dest.blueImage(), size / 2, dest.maxval / 2);

    int yMax = ySize - size + 1;
    int xMax = xSize - size + 1;

    // Median als Pixel mit der kleinsten mittleren Distanz zu den anderen im RGB Wuerfel
    for (int y = 0; y < yMax; y++)
      for (int x = 0; x < xMax; x++)
        {
          int med = (unsigned)(1 << 31) - 1;
          int medX = 0;
          int medY = 0;
          // calculate for every pixel the mean distance to the others
          for (int ay = 0; ay < size; ay++)
            for (int ax = 0; ax < size; ax++)
              {
                int tmp = 0;
                akt = src.getPixel(x + ax, y + ay);
                for (int my = 0; my < size; my++)
                  for (int mx = 0; mx < size; mx++)
                    {
                      vgl = src.getPixel(x + mx, y + my);
                      tmp += (vgl - akt).absL1();
                    }
                if (tmp < med)
                  {
                    medX = ax;
                    medY = ay;
                    med = tmp;
                  }
              }
          dest.setPixel(x + size / 2, y + size / 2, src.getPixel(x + medX, y + medY));
        }
  }

  void MedianImg(const ColorImage& src, const ColorImage& dest, int size)
  {
    if (!src.isValid() || !dest.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    switch ((src.redImage()->ImageType() << 4) + dest.redImage()->ImageType())
      {
      case 17:
        MedianImg<unsigned char, unsigned char>(src, dest, size);
      case 18:
        MedianImg<unsigned char, unsigned short>(src, dest, size);
      case 19:
        MedianImg<unsigned char, unsigned int>(src, dest, size);
      case 33:
        MedianImg<unsigned short, unsigned char>(src, dest, size);
      case 34:
        MedianImg<unsigned short, unsigned short>(src, dest, size);
      case 35:
        MedianImg<unsigned short, unsigned int>(src, dest, size);
      case 49:
        MedianImg<unsigned int, unsigned char>(src, dest, size);
      case 50:
        MedianImg<unsigned int, unsigned short>(src, dest, size);
      case 51:
        MedianImg<unsigned int, unsigned int>(src, dest, size);
      default:
        MedianImg_std(src, dest, size);
      }
  }
#undef FNAME
} /* namespace ice */
