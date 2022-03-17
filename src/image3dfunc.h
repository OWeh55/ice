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
#ifndef _IMAGE3D_FUNC_H
#define _IMAGE3D_FUNC_H

#include "image3d.h"

namespace ice
{
#define FNAME "BinImg"
  template<typename T>
  void binImg(const Image3d<T>& source, const Image3d<T>& dest,
              typename T::ValueType threshold = 1, typename T::ValueType value = -1)
  {
    try
      {
        for (int z = 0; z < source.zsize; z++)
          {
            binImg(source[z], dest[z], threshold, value);
          }
      }
    RETHROW;
  }

  template<typename T>
  void binImg(const Image3d<T>& image, typename T::ValueType threshold = 1)
  {
    try
      {
        for (int z = 0; z < image.zsize; z++)
          {
            binImg(image[z], threshold);
          }
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "InvertImg"
  template<typename T>
  void invertImg(const Image3d<T>& source, const Image3d<T>& dest)
  {
    try
      {
        for (int z = 0; z < source.zsize; z++)
          {
            invertImg(source[z], dest[z]);
          }
      }
    RETHROW;
  }

  template<typename T>
  void invertImg(const Image3d<T>& image)
  {
    try
      {
        for (int z = 0; z < image.zsize; z++)
          {
            invertImg(image[z]);
          }
      }
    RETHROW;
  }
#undef FNAME
  // binary operators - op(imgs1,imgs2) -> imgd
  template<typename T>
  int addImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd,
             int mode = MD_LIMIT);

  template<typename T>
  int subImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd,
             int smode = SMD_ABSOLUTE, int mode = MD_LIMIT);

  template<typename T>
  int maxImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd, int mode = MD_LIMIT);

  template<typename T>
  int minImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd, int mode = MD_LIMIT);
}
#endif
