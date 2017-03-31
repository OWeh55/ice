/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2013 FSU Jena, Digital Image Processing Group
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
#ifndef _IMAGE3D_FUNC_H
#define _IMAGE3D_FUNC_H

#include "image3d.h"

namespace ice
{
#define FNAME "BinImg"
  template<typename T>
  int BinImg(const Image3d<T>& source, const Image3d<T>& dest,
             typename T::ValueType threshold = 1, typename T::ValueType value = -1)
  {
    ReturnErrorIfFailed(source.match(dest));

    for (int z = 0; z < source.zsize; z++)
      {
        BinImg(source[z], dest[z], threshold, value);
      }
    return OK;
  }

  template<typename T>
  int BinImg(const Image3d<T>& image, typename T::ValueType threshold = 1)
  {
    for (int z = 0; z < image.zsize; z++)
      {
        BinImg(image[z], threshold);
      }
    return OK;
  }
#undef FNAME
#define FNAME "InvertImg"
  template<typename T>
  int InvertImg(const Image3d<T>& source, const Image3d<T>& dest)
  {
    ReturnErrorIfFailed(source.match(dest));

    for (int z = 0; z < source.zsize; z++)
      {
        InvertImg(source[z], dest[z]);
      }
    return OK;
  }
  template<typename T>
  int InvertImg(const Image3d<T>& image)
  {
    for (int z = 0; z < image.zsize; z++)
      {
        InvertImg(image[z]);
      }
    return OK;
  }
#undef FNAME
  // binary operators - op(imgs1,imgs2) -> imgd
  template<typename T>
  int AddImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd,
             int mode = MD_LIMIT);

  template<typename T>
  int SubImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd,
             int smode = SMD_ABSOLUTE, int mode = MD_LIMIT);

  template<typename T>
  int MaxImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd, int mode = MD_LIMIT);

  template<typename T>
  int MinImg(const Image3d<T>& imgs1, const Image3d<T>& imgs2, const Image3d<T>& imgd, int mode = MD_LIMIT);
}
#endif
