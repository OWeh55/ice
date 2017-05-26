/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2016 FSU Jena, Digital Image Processing Group
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
#include <ImageC.h>
#include <morph.h>
#include <tophat.h>

#define FNAME "topHatTransform"
namespace ice
{
  void topHatTransformMin(const ice::Image& source,
                          ice::Image& destination, int nx, int ny)
  {
    try
      {
        Image opened;
        opened.create(source);

        openingImg(source, opened, nx, ny);
        subImg(source, opened, destination, SMD_POSITIVE);
      }
    RETHROW;
  }

  void topHatTransformMax(const ice::Image& source,
                          ice::Image& destination,
                          int nx, int ny)
  {
    // this should be difference of "closed" image and
    // original image.
    // because we would need to shift the values we
    // simply implement this by inverting image and topHatTransformMin
    try
      {
        Image result;
        result.copy(source);
        result.invert();
        topHatTransformMin(result, result, nx, ny);
        result.invert();
      }
    RETHROW;
  }

  void topHatTransform(const ice::Image& source,
                       ice::Image& destination,
                       int nx, int ny)
  {
    if (ny < 0)
      ny = nx;

    if (nx > 0)
      topHatTransformMin(source, destination, nx, ny);
    else
      topHatTransformMax(source, destination, -nx, -ny);
  }
#undef FNAME
}
