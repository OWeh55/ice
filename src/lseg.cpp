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

#include "base.h"
#include "morph.h"
#include "lseg.h"

namespace ice
{
  void lsegcore_std(const Image& source,
                    const Image& minimg, const Image& maximg,
                    const Image& oimg,
                    int level)
  {
    int g, gi, ga;

    for (int y = 0; y < source.ysize; y++)
      for (int x = 0; x < source.xsize; x++)
        {
          g = GetValUnchecked(source, x, y);
          gi = GetValUnchecked(minimg, x, y);
          ga = GetValUnchecked(maximg, x, y);

          if (ga - gi < level)   // genügend Unterschiede in der Umgebung ?
            {
              PutVal(oimg, x, y, LSunknown); // nein: "undefiniert"
            }
          else
            {
              if (g + g < gi + ga)
                {
                  PutVal(oimg, x, y, LSunderground); // Untergrund
                }
              else
                {
                  PutVal(oimg, x, y, LSobject); // Objekt
                }
            }
        }
  }

  template<class T>
  void lsegcore(const Image& source,
                const Image& minimg, const Image& maximg,
                const Image& oimg,
                int level)
  {
    int g, gi, ga;

    const T** dps = (const T**) source->getDataPtr();
    const T** dpi = (const T**) minimg->getDataPtr();
    const T** dpa = (const T**) maximg->getDataPtr();
    PixelType1** dpo = (PixelType1**) oimg->getDataPtr();

    for (int y = 0; y < source.ysize; y++)
      for (int x = 0; x < source.xsize; x++)
        {
          g = dps[y][x];
          gi = dpi[y][x];
          ga = dpa[y][x];

          if (ga - gi < level)   // genügend Unterschiede in der Umgebung ?
            {
              dpo[y][x] = LSunknown; // nein: "undefiniert"
            }
          else
            {
              if (g + g < gi + ga)
                {
                  dpo[y][x] = LSunderground; // Untergrund
                }
              else
                {
                  dpo[y][x] = LSobject; // Objekt
                }
            }
        }
  }

#define FNAME "LocalSeg"
  void LocalSeg(const Image& source, const Image& oimg, int neighb, int level)
  {
    // Lokale Segmentierung
    if (source == oimg)
      {
        return LocalSeg(oimg, neighb, level);
      }

    if ((!IsImg(source)) || (!IsImg(oimg)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!(neighb & 1))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((oimg->xsize != source->xsize) ||
        (oimg->ysize != source->ysize) ||
        (oimg.maxval < 2)
       )
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    try
      {
        int xs = source->xsize;
        int ys = source->ysize;
        int mv = source.maxval;

        Image minimg = NewImg(xs, ys, mv);
        Image maximg = NewImg(xs, ys, mv);

        MinMaxImg(source, neighb, neighb, minimg, maximg); // max and min in neighborhood

        if (oimg->ImageType() != 1)   // für Zielbild sehr seltene Ausnahme, deshalb Verzicht auf Optimierung
          {
            lsegcore_std(source, minimg, maximg, oimg, level);
          }
        else

          {
            switch (source->ImageType())
              {
              case 1:
                lsegcore<PixelType1>(source, minimg, maximg, oimg, level);
                break;
              case 2:
                lsegcore<PixelType2>(source, minimg, maximg, oimg, level);
                break;
              case 3:
                lsegcore<PixelType3>(source, minimg, maximg, oimg, level);
                break;
              default:
                lsegcore_std(source, minimg, maximg, oimg, level);
                break;
              }
          }
      }
    RETHROW;
  }

  void LocalSeg(const Image& source, const Image& oimg, int neighb, int level, int level2)
  {
    // Lokale Segmentierung
    if (source == oimg)
      {
        return LocalSeg(oimg, neighb, level, level2);
      }

    if ((!IsImg(source)) || (!IsImg(oimg)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (!(neighb & 1))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((oimg->xsize != source->xsize) ||
        (oimg->ysize != source->ysize) ||
        (oimg.maxval < 4)
       )
      throw IceException(FNAME, M_WRONG_IMGSIZE);

    try
      {
        int xs = source->xsize;
        int ys = source->ysize;
        int mv = source.maxval;

        Image minimg = NewImg(xs, ys, mv);
        Image maximg = NewImg(xs, ys, mv);

        MinMaxImg(source, neighb, neighb, minimg, maximg); // max and min in neighborhood

        int g, gi, ga;

        for (int y = 0; y < source.ysize; y++)
          for (int x = 0; x < source.xsize; x++)
            {
              g = GetValUnchecked(source, x, y);
              gi = GetValUnchecked(minimg, x, y);
              ga = GetValUnchecked(maximg, x, y);
              int dg = ga - gi;

              if (dg < level2)   // genügend Unterschiede in der Umgebung ?
                {
                  PutVal(oimg, x, y, LSunknown); // nein: "undefiniert"
                }
              else if (dg < level)
                {
                  // "schwaches" Objekt/Untergrund
                  if (g + g < gi + ga)
                    {
                      PutVal(oimg, x, y, LSweakunderground); // Untergrund
                    }
                  else
                    {
                      PutVal(oimg, x, y, LSweakobject); // Objekt
                    }
                }
              else
                {
                  // "Starkes" Objekt/Untergrund
                  if (g + g < gi + ga)
                    {
                      PutVal(oimg, x, y, LSunderground); // Untergrund
                    }
                  else
                    {
                      PutVal(oimg, x, y, LSobject); // Objekt
                    }
                }
            }
      }
    RETHROW;
  }
  /*
   * variants with img = source==dest
   */

  void LocalSeg(const Image& img, int neighb, int level)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);
    try
      {
        Image hsource = NewImg(img);
        LocalSeg(hsource, img, neighb, level);
      }
    RETHROW;
  }

  void LocalSeg(const Image& img, int neighb, int level, int level2)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    try
      {
        Image hsource = NewImg(img);
        LocalSeg(hsource, img, neighb, level, level2);
      }
    RETHROW;
  }

#undef FNAME

  object_rc LocalSegObj(const Image& i, int x, int y, int thr_dummy)
  {
    static object_rc table[] = {isunknown, isunderground, isobject, isunderground, isobject};
    return table[GetVal(i, x, y)];
  }

  object_rc LocalSegObjInv(const Image& i, int x, int y, int thr_dummy)
  {
    static object_rc table[] = {isunknown, isobject, isunderground, isobject, isunderground};
    return table[GetVal(i, x, y)];
  }

  object_rc LocalSegObjHigh(const Image& i, int x, int y, int thr_dummy)
  {
    static object_rc table[] = {isunknown, isunderground, isobject, isunknown, isunknown};
    return table[GetVal(i, x, y)];
  }
}
