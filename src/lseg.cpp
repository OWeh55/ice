/*
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
 */

#include "base.h"
#include "macro.h"
#include "filter.h"
#include "lseg.h"

namespace ice
{

  void lsegcore_std(const Image& source,
                    Image& minimg, Image& maximg,
                    Image& oimg,
                    int level)
  {
    int x, y;
    int g, gi, ga;

    wloop(source, x, y)
    {
      g = GetValUnchecked(source, x, y);
      gi = GetValUnchecked(minimg, x, y);
      ga = GetValUnchecked(maximg, x, y);

      if (ga - gi < level) // genügend Unterschiede in der Umgebung ?
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
                Image& oimg,
                int level)
  {
    int x, y;
    int g, gi, ga;

    const T** dps = (const T**) source->getDataPtr();
    const T** dpi = (const T**) minimg->getDataPtr();
    const T** dpa = (const T**) maximg->getDataPtr();
    PixelType1** dpo = (PixelType1**) oimg->getDataPtr();
    wloop(source, x, y)
    {
      g = dps[y][x];
      gi = dpi[y][x];
      ga = dpa[y][x];

      if (ga - gi < level) // genügend Unterschiede in der Umgebung ?
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
#ifdef CONTROLLED_REFRESH
    oimg->needRefresh();
#endif
  }


#define FNAME "LocalSeg"
  int LocalSeg(Image source, Image oimg, int neighb, int level)
  {
    // Lokale Segmentierung
    if (source == oimg) return LocalSeg(oimg, neighb, level);

    if ((!IsImg(source)) || (!IsImg(oimg)))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (!(neighb & 1))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((oimg->xsize != source->xsize) ||
        (oimg->ysize != source->ysize) ||
        (oimg->maxval < 2)
       )
      {
        Message(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int xs = source->xsize;
    int ys = source->ysize;
    int mv = source->maxval;

    Image minimg = NewImg(xs, ys, mv);
    Image maximg = NewImg(xs, ys, mv);

    MinMaxImg(source, neighb, neighb, minimg, maximg); // max and min in neighborhood

    if (oimg->ImageType() != 1) // für Zielebild sehr seltene Ausnahme, deshalb Verzicht auf Optimierung
      lsegcore_std(source, minimg, maximg, oimg, level);
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

    FreeImg(minimg);
    FreeImg(maximg);
    return OK;
  }

  int LocalSeg(Image source, Image oimg, int neighb, int level, int level2)
  {
    // Lokale Segmentierung
    if (source == oimg) return LocalSeg(oimg, neighb, level, level2);

    if ((!IsImg(source)) || (!IsImg(oimg)))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (!(neighb & 1))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((oimg->xsize != source->xsize) ||
        (oimg->ysize != source->ysize) ||
        (oimg->maxval < 4)
       )
      {
        Message(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int xs = source->xsize;
    int ys = source->ysize;
    int mv = source->maxval;

    Image minimg = NewImg(xs, ys, mv);
    Image maximg = NewImg(xs, ys, mv);

    MinMaxImg(source, neighb, neighb, minimg, maximg); // max and min in neighborhood

    int x, y;
    int g, gi, ga;

    wloop(source, x, y)
    {
      g = GetValUnchecked(source, x, y);
      gi = GetValUnchecked(minimg, x, y);
      ga = GetValUnchecked(maximg, x, y);
      int dg = ga - gi;

      if (dg < level2) // genügend Unterschiede in der Umgebung ?
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

    FreeImg(minimg);
    FreeImg(maximg);
    return OK;
  }
  /*
   * variants with img = source==dest
   */

  int LocalSeg(Image img, int neighb, int level)
  {
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Image hsource = NewImg(img);
    IF_FAILED(LocalSeg(hsource, img, neighb, level))
    {
      FreeImg(hsource);
      Message(FNAME, M_0, ERROR);
      return ERROR;
    }
    FreeImg(hsource);
    return OK;
  }

  int LocalSeg(Image img, int neighb, int level, int level2)
  {
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Image hsource = NewImg(img);
    IF_FAILED(LocalSeg(hsource, img, neighb, level, level2))
    {
      FreeImg(hsource);
      Message(FNAME, M_0, ERROR);
      return ERROR;
    }
    FreeImg(hsource);
    return OK;
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
