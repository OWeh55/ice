/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#include "util.h"
#include "matrix3d.h"

namespace ice
{

  void Image2Matrix(const Image3d<Image>& img, Matrix3d& m)
  {
    int xs = img.xsize;
    int ys = img.ysize;
    int zs = img.zsize;
    m.resize(xs, ys, zs);
    for (int z = 0; z < zs; z++)
      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          {
            m(x, y, z) = img.getPixel(x, y, z);
          }
  }

  void Matrix2Image(const Matrix3d& m, Image3d<Image>& img, int mode)
  {
    int xs = m.sizeX();
    int ys = m.sizeY();
    int zs = m.sizeZ();
    if (mode == 0)
      {
        for (int z = 0; z < zs; z++)
          for (int y = 0; y < ys; y++)
            for (int x = 0; x < xs; x++)
              {
                img.setPixel(x, y, z, limited(m(x, y, z), 0, 255));
              }
      }
    else if (mode == 1)
      {
        double min, max;
        m.MaxAndMin(max, min);

        double fak = 255.0 / (max - min + 0.01);
        for (int z = 0; z < zs; z++)
          {
            for (int y = 0; y < ys; y++)
              {
                for (int x = 0; x < xs; x++)
                  {
                    double val = m(x, y, z);
                    img.setPixel(x, y, z, RoundInt((val - min) * fak));
                  }
              }
          }
      }
  }
}
