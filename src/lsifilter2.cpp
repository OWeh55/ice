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
/************************************************
 *        Filter-Grundfunktionen        *
 *                                              *
 *   W.Ortmann 8/07                             *
 ************************************************/

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "util.h"

#include "lsifilter.h"

using namespace std;

namespace ice
{

  void lsih_std(const Image& pn1, const Image& pn2,
                const std::vector<int>& mask,
                int norm, int offset)
  {
    int dx = pn1->xsize;
    int dy = pn1->ysize;
    int sx = mask.size();
    int maxval = pn2.maxval;
    int sx1 = sx / 2;

    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        int x = 0;

        while (x < sx1)
          {
            PutValUnchecked(pn2, x, y, offset);
            x++;
          }

        while (x < dx - sx1)
          {
            int x1 = x - sx1;
            int x2 = x + sx1 + 1;
            int i = 0;
            int gsum = 0;

            while (x1 < x2)
              {
                gsum += mask[i] * GetValUnchecked(pn1, x1, y);
                x1++;
                i++;
              }

            gsum = limited(gsum / norm + offset, maxval);
            PutValUnchecked(pn2, x, y, gsum);
            x++;
          }

        while (x < dx)
          {
            PutValUnchecked(pn2, x, y, offset);
            x++;
          }
      }
  }

  template<class T>
  void lsih(const Image& pn1, const Image& pn2, const vector<int>& mask,
            int norm, int offset)
  {
    int dx = pn1->xsize;
    int dy = pn1->ysize;
    int sx = mask.size();
    int maxval = pn2.maxval;
    int sx1 = sx / 2;

    const T** p1 = (const T**)pn1->getDataPtr();
    T** p2 = (T**)pn2->getDataPtr();

    for (int y = 0; y < dy; y++)   // alle zeilen
      {
        const T* Zeile = p1[y];
        int x = 0;

        while (x < sx1)   // linker Rand
          {
            p2[y][x] = offset;
            x++;
          }

        while (x < dx - sx1)
          {
            int x1 = x - sx1;
            int x2 = x + sx1 + 1;
            int i = 0;
            int gsum = 0;

            while (x1 < x2)
              {
                gsum += mask[i] * Zeile[x1];
                x1++;
                i++;
              }

            p2[y][x] = limited(gsum / norm + offset, 0, maxval);
            x++;
          }

        while (x < dx)
          {
            p2[y][x] = offset;
            x++;
          }
      }
  }

#define FNAME "LSIHImg"
  void LSIHImg(const Image& pn1, const Image& pn2, const vector<int>& mask,
               int norm, int offset)
  {
    int dx, dy;
    int sx = mask.size();

    if ((sx < 1) || ((sx & 1) != 1) || (norm == 0))
      throw IceException(FNAME, M_WRONG_PARAM);

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, dx, dy));

    if (pn1->getDataPtr() == pn2->getDataPtr())
      {
        Image src = NewImg(pn1, true);
        LSIHImg(src, pn2, mask, norm, offset);
      }

    int typ1 = pn1->ImageType();
    int typ2 = pn2->ImageType();

    if (typ1 != typ2)
      {
        lsih_std(pn1, pn2, mask, norm, offset);
      }
    else
      {
        switch (typ1)
          {
          case 1:
            lsih<PixelType1>(pn1, pn2, mask, norm, offset);
            break;
          case 2:
            lsih<PixelType2>(pn1, pn2, mask, norm, offset);
            break;
          case 3:
            lsih<PixelType3>(pn1, pn2, mask, norm, offset);
            break;
          default:
            lsih_std(pn1, pn2, mask, norm, offset);
            break;
          }
      }
  }

  void LSIHImg(const Image& pn1, const Image& pn2, const IVector& mask,
               int norm, int offset)
  {
    vector<int> vmask;

    for (unsigned int i = 0; i < mask.size(); i++)
      {
        vmask.push_back(mask[i]);
      }

    LSIHImg(pn1, pn2, vmask, norm, offset);
  }

#undef FNAME
//--------------------------------------------------------------------

  void lsiv_std(const Image& pn1, const Image& pn2, const vector<int>& mask,
                int norm, int offset)
  {
    int dx = pn1->xsize;
    int dy = pn1->ysize;
    int sy = mask.size();
    int maxval = pn2.maxval;
    int sy1 = sy / 2;

    for (int x = 0; x < dx; x++)   // alle spalten
      {
        int y = 0;

        while (y < sy1)
          {
            PutValUnchecked(pn2, x, y, offset);
            y++;
          }

        while (y < dy - sy1)
          {
            int y1 = y - sy1;
            int y2 = y + sy1 + 1;
            int i = 0;
            int gsum = 0;

            while (y1 < y2)
              {
                gsum += mask[i] * GetValUnchecked(pn1, x, y1);
                y1++;
                i++;
              }

            gsum = limited(gsum / norm + offset, 0, maxval);
            PutValUnchecked(pn2, x, y, gsum);
            y++;
          }

        while (y < dy)
          {
            PutValUnchecked(pn2, x, y, offset);
            y++;
          }
      }
  }

  template<class T>
  void lsiv(const Image& pn1, const Image& pn2, const vector<int>& mask,
            int norm, int offset)
  {
    int dx = pn1->xsize;
    int dy = pn1->ysize;
    int sy = mask.size();
    int maxval = pn2.maxval;
    int sy1 = sy / 2;

    const T** p1 = (const T**)pn1->getDataPtr();
    T** p2 = (T**)pn2->getDataPtr();

    for (int x = 0; x < dx; x++)   // alle spalten
      {
        int y = 0;

        while (y < sy1)   // oberer Rand
          {
            p2[y][x] = offset;
            y++;
          }

        while (y < dy - sy1)
          {
            int y1 = y - sy1;
            int y2 = y + sy1 + 1;
            int i = 0;
            int gsum = 0;

            while (y1 < y2)
              {
                gsum += mask[i] * p1[y1][x];
                y1++;
                i++;
              }

            p2[y][x] = limited(gsum / norm + offset, 0, maxval);
            y++;
          }

        while (y < dy)   // unterer Rand
          {
            p2[y][x] = offset;
            y++;
          }
      }
  }

#define FNAME "LSIVImg"
  void LSIVImg(const Image& pn1, const Image& pn2, const vector<int>& mask,
               int norm, int offset)
  {
    int dx, dy;
    int sy = mask.size();

    if ((sy < 1) || ((sy & 1) != 1) || (norm == 0))
      throw IceException(FNAME, M_WRONG_PARAM);

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, dx, dy));

    if (pn1->getDataPtr() == pn2->getDataPtr())
      {
        Image src = NewImg(pn1, true);
        LSIVImg(src, pn2, mask, norm, offset);
      }

    int typ1 = pn1->ImageType();
    int typ2 = pn2->ImageType();

    if (typ1 != typ2)
      {
        lsiv_std(pn1, pn2, mask, norm, offset);
      }
    else
      {
        switch (typ1)
          {
          case 1:
            lsiv<PixelType1>(pn1, pn2, mask, norm, offset);
            break;
          case 2:
            lsiv<PixelType2>(pn1, pn2, mask, norm, offset);
            break;
          case 3:
            lsiv<PixelType3>(pn1, pn2, mask, norm, offset);
            break;
          default:
            lsiv_std(pn1, pn2, mask, norm, offset);
            break;
          }
      }
  }

  void LSIVImg(const Image& pn1, const Image& pn2, const IVector& mask,
               int norm, int offset)
  {
    vector<int> vmask;

    for (unsigned int i = 0; i < mask.size(); i++)
      {
        vmask.push_back(mask[i]);
      }

    LSIVImg(pn1, pn2, vmask, norm, offset);
  }

#undef FNAME
}
