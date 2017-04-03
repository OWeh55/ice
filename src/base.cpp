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

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "util.h"
#include "base.h"
#include "ImageBase.h"
#include "ImageTemplate.h"

using namespace std;

namespace ice
{
#define FNAME "SetImg"
  int SetImg(const Image& img, int Value)
  {
    if (Value < 0)
      {
        throw IceException(FNAME, M_VALTOOSMALL, WRONG_PARAM);
        return WRONG_PARAM;
      }
    if (Value > img.maxval)
      {
        throw IceException(FNAME, M_VALTOOLARGE, WRONG_PARAM);
        return WRONG_PARAM;
      }
    return img.set(Value);
  }
#undef FNAME

//
// Bild löschen
//
#define FNAME "ClearImg"
  int ClearImg(const Image& img)
  {
    img.set(0);
    return OK;
  }
#undef FNAME

#define FNAME "GetInterpolVal"
  double GetInterpolVal(const Image& Img, double x, double y)
  {
    return Img.getPixelInterpol(x, y);
  }

  double GetInterpolVal(const Image& Img, const Point& p)
  {
    return Img.getPixelInterpol(p.x, p.y);
  }

  bool GetInterpolVal(const Image& Img, double x, double y, double& val)
  {
    return Img.getPixelInterpol(x, y, val);
  }
#undef FNAME
#define FNAME "CopyImg"
  int CopyImg(const Image& SourceImage, const Image& DestinationImage)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(SourceImage, DestinationImage, xs, ys));

    int sourcetype = SourceImage->ImageType();

    if (SourceImage->maxval == DestinationImage->maxval &&
        sourcetype > 0 && sourcetype < 4 &&
        sourcetype == DestinationImage->ImageType())
      {
        DestinationImage.copyData(SourceImage);
#ifdef CONTROLLED_REFRESH
        DestinationImage->needRefresh();
#endif
      }
    else
      {
        int MaxSelf = SourceImage->maxval;
        int MaxDest = DestinationImage->maxval;

        // Copy the contents. But first check, if we have to adjust the color intensities
        // in >Destination<. If >Destination< has the same maximum grayvalue as this,
        // our copy routine will be faster.
        if (MaxSelf == MaxDest)
          {
            for (int y = 0; y < ys; y++)
              for (int x = 0; x < xs; x++)
                {
                  PutValUnchecked(DestinationImage, x, y, GetValUnchecked(SourceImage, x, y));
                }
          }
        else
          {
            // adjust the maximal grayvalue
            for (int y = 0; y < ys; y++)
              for (int x = 0; x < xs; x++)
                {
                  PutValUnchecked(DestinationImage, x, y,
                                  ice::MulDiv(GetValUnchecked(SourceImage, x, y), MaxDest, MaxSelf));
                }
          }
      }

    return OK;
  }
#undef FNAME
#define FNAME "Inside"
  bool Inside(const Image& img, int x, int y)
  {
    return img.inside(x, y);
  }

  bool Inside(const Image& img, IPoint p)
  {
    return img.inside(p.x, p.y);
  }

  bool Inside(const Image& img, const Window& w)
  {
    return img.inside(w);
  }
#undef FNAME

  /*************** draw a border around an image ************/
  void setborder(const Image& p, int sx, int sy, int val)
  {
    // Rand fuellen
    // oberer Rand
    for (int j = 0; j < sy; j++)
      for (int i = 0; i < p->xsize; i++)
        {
          PutValUnchecked(p, i, j, val);
        }

    // linker/rechter Rand
    for (int i = sy; i < p->ysize - sy; i++)
      {
        for (int j = 0; j < sx; j++)
          {
            PutValUnchecked(p, j, i, val);
          }

        for (int j = p->xsize - sx; j < p->xsize; j++)
          {
            PutValUnchecked(p, j, i, val);
          }
      }

    // unterer Rand
    for (int j = p->ysize - sy; j < p->ysize; j++)
      for (int i = 0; i < p->xsize; i++)
        {
          PutValUnchecked(p, i, j, val);
        }

  }

  void setborder(const Image& p, int size, int val)
  {
    /**
     * Rand der Breite size im Bild mit Wert val auffüllen
     */
    setborder(p, size, size, val);
  }

  void setborder(const Image& p, int val)
  {
    for (int i = 0; i < p->xsize; i++)
      {
        PutValUnchecked(p, i, 0, val);
      }

    for (int i = 1; i < p->ysize - 1; i++)
      {
        PutValUnchecked(p, 0, i, val);
        PutValUnchecked(p, p->xsize - 1, i, val);
      }

    for (int i = 0; i < p->xsize; i++)
      {
        PutValUnchecked(p, i, p->ysize - 1, val);
      }
  }


#define FNAME "MatchImg"
  int MatchImg(const Image& i1, const Image& i2, int& xs, int& ys)
  {
    RETURN_ERROR_IF_FAILED(i1.match(i2));

    xs = i1->xsize;
    ys = i1->ysize;

    return OK;
  }

  int MatchImg(const Image& i1, const Image& i2, const Image& i3, int& xs, int& ys)
  {
    RETURN_ERROR_IF_FAILED(i1.match(i2));
    RETURN_ERROR_IF_FAILED(i1.match(i3));

    xs = i1->xsize;
    ys = i1->ysize;

    return OK;
  }

  int MatchImg(const Image& i1, const Image& i2)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(i1, i2, xs, ys));
    return OK;
  }

  int MatchImg(const Image& i1, const Image& i2, const Image& i3)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(i1, i2, i3, xs, ys));
    return OK;
  }
#undef FNAME
#define FNAME "matchImg"
  void matchImg(const Image& i1, const Image& i2, int& xs, int& ys)
  {
    try
      {
        i1.match(i2);

        xs = i1->xsize;
        ys = i1->ysize;
      }
    RETHROW;
  }

  void matchImg(const Image& i1, const Image& i2, const Image& i3, int& xs, int& ys)
  {
    try
      {
        i1.match(i2);
        i1.match(i3);
        xs = i1->xsize;
        ys = i1->ysize;
      }
    RETHROW;
  }

  void matchImg(const Image& i1, const Image& i2)
  {
    int xs, ys;
    matchImg(i1, i2, xs, ys);
  }

  void matchImg(const Image& i1, const Image& i2, const Image& i3)
  {
    int xs, ys;
    matchImg(i1, i2, i3, xs, ys);
  }
#undef FNAME
}
