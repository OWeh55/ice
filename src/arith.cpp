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
//
//  Bildarithmetik
//

#include <iostream>
#include <stdlib.h>

#include "IceException.h"
#include "defs.h"

#include "macro.h"

#include "base.h"
#include "util.h"

#include "arith.h"

//
//    Grundsätzliches:

//    Operationen haben die Form

//    pxxxx(p1,p2,p3) - p3 := p1 op p2

//    Rückgabewert ist FALSE bei Fehlern

//    - Fenster für die Operationen ergeben sich aus den Quellbildern p1 und
//    p2, wenn vorhanden auch aus p3
//    - Maximale Werte im Zielbild sind die Vorgabewerte aus Bild p3 (wenn
//    vorhanden), sonst entsprechend der angewandten Operationen

//
// Hilfsmakros
//
namespace ice
{
#define LOOP for (y=0;y<dy;y++)     \
    for (x=0;x<dx;x++)

#define VAL1 GetVal(pn1,x,y)
#define VAL2 GetVal(pn2,x,y)

#define NORM(val) MulDiv(val,gmax,gmax1)

//
// Bildaddition
//
#define FNAME "addImg"
  void addImg(const Image& pn1, const Image& pn2, const Image& pn3, int mode)
  {
    int dx, dy;
    int x, y;
    int gmax, gmax1;
    int val;
    int gzahl;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, pn3, dx, dy));

    gmax1 = pn1.maxval + pn2.maxval; /*maximaler wert bei addition */
    gmax = pn3.maxval; /* maximalwert aus Zielbild ermitteln */

    // jetzt addieren
    if (gmax == gmax1)   /* keine Normierung noetig */
      {
        LOOP { PutVal(pn3, x, y, VAL1 + VAL2); }
      }
    else
      {
        gzahl = gmax + 1;
        LOOP
        {
          val = VAL1 + VAL2;

          switch (mode)
            {
            case MD_NORMALIZE:
              val = NORM(val);
              break;
            case MD_LIMIT:
              val = limited(val, 0, gmax);
              break;
            case MD_MODULO:
              val = val % gzahl;
              break;
            default:
              throw IceException(FNAME, M_WRONG_MODE);
            }

          PutVal(pn3, x, y, val);
        }
      }
  }

#undef FNAME
//
// Bild-Subtraktion
//
#define FNAME "subImg"
  void subImg(const Image& pn1, const Image& pn2, int smode, const Image& pn3, int mode)
//
//  Subtraktion zweier Bilder, ergebnis muss immer positiv sein
//  smode   rechnung
//  SMD_ABSOLUTE:  pn3=abs(pn1-pn2)
//  SMD_POSITIVE:  pn3=max(pn1-pn2,0)
//  SMD_SHIFT:     pn3=pn1-pn2+pn2.maxval
//
  {
    int dx, dy;
    int x, y;
    int gmax, gmax1;
    int gzahl, gnull;
    int val;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, pn3, dx, dy));

    /* maximalwerte des Ergebnisses ermitteln */
    gzahl = pn3.maxval + 1;
    gnull = gzahl / 2;

    switch (smode)
      {
      case SMD_ABSOLUTE:
        gmax1 = Max(pn1.maxval, pn2.maxval);
        break;
      case SMD_POSITIVE:
        gmax1 = pn1.maxval;
        break;
      case SMD_SHIFT:
        gmax1 = Max(pn1.maxval, pn2.maxval) * 2 + 1;
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }

    gmax = pn3.maxval;

    //
    // jetzt subtrahieren
    //
    if (gmax == gmax1)   // Überlaufbehandlung ist überflüssig
      {
        switch (smode)
          {
          case SMD_ABSOLUTE:
            LOOP { PutVal(pn3, x, y, abs(VAL1 - VAL2)); };
            break;
          case SMD_POSITIVE:
            LOOP { PutVal(pn3, x, y, Max(VAL1 - VAL2, 0)); };
            break;
          case SMD_SHIFT:
            LOOP { PutVal(pn3, x, y, VAL1 - VAL2 + pn2.maxval); };
            break;
          }/*switch (mode) */
      }/*if*/
    else
      {
        gzahl = gmax + 1;
        gnull = gzahl / 2;
        LOOP
        {
          val = VAL1 - VAL2;

          switch (smode)
            {
            case SMD_ABSOLUTE:
              val = abs(val);
              break;
            case SMD_POSITIVE:

              if (val < 0)
                {
                  val = 0;
                }

              break;
            case SMD_SHIFT:

              if (mode == MD_NORMALIZE)
                {
                  val = val + pn2.maxval;
                }
              else
                {
                  val = val + gnull;
                }

              break;
            }

          switch (mode)
            {
            case MD_NORMALIZE:
              val = NORM(val);
              break;
            case MD_LIMIT:
              val = limited(val, 0, gmax);
              break;
            case MD_MODULO:

              while (val < 0)
                {
                  val += gzahl;
                }

              val = val % gzahl;
              break;
            default:
              throw IceException(FNAME, M_WRONG_MODE);
            }

          PutVal(pn3, x, y, val);
        }/*switch (mode) */
      }/*else*/
  }

  void subImg(const Image& pn1, const Image& pn2, const Image& pn3,
              int smode, int mode)
  {
    subImg(pn1, pn2, smode, pn3, mode);
  }
#undef FNAME
//
// Maximal-Bild zweier Bilder
//
#define FNAME "maxImg"
  void maxImg(const Image& pn1, const Image& pn2, const Image& pn3, int mode)
  {
    int dx, dy;
    int x, y;
    int gmax, gmax1;
    int val;
    int gzahl;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, pn3, dx, dy));

    gmax1 = Max(pn1.maxval, pn2.maxval); /*maximaler wert*/
    gmax = pn3.maxval; /* maximalwert aus Zielbild ermitteln */

    /* jetzt Maximum-Bild erzeugen */
    if (gmax == gmax1)   /* keine Normierung noetig */
      {
        LOOP { PutVal(pn3, x, y, Max(VAL1, VAL2)); }
      }
    else
      {
        LOOP
        {
          gzahl = gmax + 1;
          val = Max(VAL1, VAL2);

          switch (mode)
            {
            case MD_NORMALIZE:
              val = NORM(val);
              break;
            case MD_LIMIT:
              val = limited(val, 0, gmax);
              break;
            case MD_MODULO:
              val = (val + gzahl) % gzahl;
              break;
            default:
              throw IceException(FNAME, M_WRONG_MODE);
            }

          PutVal(pn3, x, y, val);
        }
      }
  }
#undef FNAME
//
// Minimal-Bild zweier Bilder
//
#define FNAME "minImg"
  void minImg(const Image& pn1, const Image& pn2, const Image& pn3, int mode)
  {
    int dx, dy;
    int x, y;
    int gmax, gmax1;
    int val, gzahl;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, pn3, dx, dy));

    gmax1 = Min(pn1.maxval, pn2.maxval); /*maximaler wert*/
    gmax = pn3.maxval; /* maximalwert aus Zielbild ermitteln */

    /* jetzt Minimum-Bild erzeugen */
    if (gmax == gmax1)   /* keine Normierung noetig */
      {
        LOOP { PutVal(pn3, x, y, Min(VAL1, VAL2)); }
      }
    else
      {
        LOOP
        {
          gzahl = gmax + 1;
          val = Min(VAL1, VAL2);

          switch (mode)
            {
            case MD_NORMALIZE:
              val = NORM(val);
              break;
            case MD_LIMIT:
              val = limited(val, 0, gmax);
              break;
            case MD_MODULO:
              val = (val + gzahl) % gzahl;
              break;
            default:
              throw IceException(FNAME, M_WRONG_MODE);
            }

          PutVal(pn3, x, y, val);
        }
      }
  }
#undef FNAME
  /***************************************************************/
  /* Bild mit neuen Abmessungen generieren                       */
  /***************************************************************/
#define FNAME "renormImg"
  void renormImg(const Image& p, const Image& dest)
  {
    int ax, ay, av;
    int nx, ny, nv;

    if ((!IsImg(p)) || (!IsImg(dest)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    nx = dest->xsize;
    ny = dest->ysize;
    nv = dest.maxval;

    ax = p->xsize;
    ay = p->ysize;
    av = p.maxval;

    for (int y = 0; y < dest.ysize; y++)
      for (int x = 0; x < dest.xsize; x++)
        {
          int xo = MulDiv(x, ax, nx);
          int yo = MulDiv(y, ay, ny);
          PutVal(dest, x, y, MulDiv(GetVal(p, xo, yo), nv, av));
        }
  }
#undef FNAME
  /*********************************************************************/
  /* invertiertes Bild                                                 */
  /*********************************************************************/
#define FNAME "invertImg"
  static void invertImg_core_std(const Image& pn1, const Image& pn2)
  {
    int gmax1 = pn1.maxval; /*maximaler wert*/
    int gmax2 = pn2.maxval;  /* maximalwert aus Zielbild ermitteln */

    /* jetzt inverses Bild erzeugen */
    if (gmax1 == gmax2)   /* keine Normierung noetig */
      {
        for (int y = 0; y < pn1.ysize; y++)
          for (int x = 0; x < pn1.xsize; x++)
            {
              int go = GetValUnchecked(pn1, x, y);
              PutValUnchecked(pn2, x, y, gmax1 - go);
            }
      }
    else
      {
        for (int y = 0; y < pn1.ysize; y++)
          for (int x = 0; x < pn1.xsize; x++)
            {
              int go = GetValUnchecked(pn1, x, y);
              PutValUnchecked(pn2, x, y, MulDiv(gmax1 - go, gmax2, gmax1));
            }
      }
  }

  template<class T>
  void invertImg_core(const Image& pn1, const Image& pn2)
  {
    int gmax = pn1.maxval; /*maximaler wert*/

    T** dp1 = (T**)pn1->getDataPtr();
    T** dp2 = (T**)pn2->getDataPtr();

    for (int y = 0; y < pn1.ysize; y++)
      for (int x = 0; x < pn1.xsize; x++)
        {
          dp2[y][x] = gmax - dp1[y][x];
        }
  }

  void invertImg(const Image& pn1, const Image& pn2)
  {
    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2));

    int gmax1 = pn1.maxval; /* maximalwert Quellbild */
    int gmax2 = pn2.maxval; /* maximalwert Zielbild */

    if (gmax1 != gmax2)
      {
        return invertImg_core_std(pn1, pn2);
      }

    int pt1 = pn1->ImageType();
    int pt2 = pn2->ImageType();

    if (pt1 != pt2)
      {
        return invertImg_core_std(pn1, pn2);
      }

    switch (pt1)
      {
      case 1:
        invertImg_core<PixelType1>(pn1, pn2);
        break;
      case 2:
        invertImg_core<PixelType2>(pn1, pn2);
        break;
      case 3:
        invertImg_core<PixelType3>(pn1, pn2);
        break;
      default:
        invertImg_core_std(pn1, pn2);
        break;
      }
  }

  void invertImg(const Image& pn)
  {
    invertImg(pn, pn);
  }
#undef FNAME
  /*********************************************************************/
  /* Binär-Bild                                                        */
  /*********************************************************************/
#define FNAME "binImg"
  template<typename SrcType, typename DestType>
  void _bin(const Image& src, const Image& dest, int bin, int val)
  {
    const SrcType** Pixels = (const SrcType**)src->getDataPtr();
    DestType** Pixeld = (DestType**)dest->getDataPtr();

    for (int y = 0; y < src.ysize; y++)
      for (int x = 0; x < src.xsize; x++)
        {
          if (Pixels[y][x] < (unsigned int)bin)
            {
              Pixeld[y][x] = 0;
            }
          else
            {
              Pixeld[y][x] = val;
            }
        }
  }

  void _bin_std(const Image& src, const Image& dest, int bin, int val)
  {
    IPoint p;
    for (p.y = 0; p.y < src->ysize; p.y++)
      for (p.x = 0; p.x < src->xsize; p.x++)
        {
          if (GetValUnchecked(src, p) < bin)
            {
              PutValUnchecked(dest, p, 0);
            }
          else
            {
              PutValUnchecked(dest, p, val);
            }
        }
  }

  void binImg(const Image& src, const Image& dest, int bin, int val)
  {
    RETURN_ERROR_IF_FAILED(MatchImg(src, dest));

    if (val < 0)
      {
        val = dest.maxval;
      }

    if ((bin < 1) || (bin > src.maxval) || (val < 0) || (val > dest.maxval))
      throw IceException(FNAME, M_WRONG_PARAM);

    switch ((src->ImageType() << 4) + dest->ImageType())
      {
      case 17:
        _bin<unsigned char, unsigned char>(src, dest, bin, val);
        break;
      case 18:
        _bin<unsigned char, unsigned short>(src, dest, bin, val);
        break;
      case 19:
        _bin<unsigned char, unsigned int>(src, dest, bin, val);
        break;
      case 33:
        _bin<unsigned short, unsigned char>(src, dest, bin, val);
        break;
      case 34:
        _bin<unsigned short, unsigned short>(src, dest, bin, val);
        break;
      case 35:
        _bin<unsigned short, unsigned int>(src, dest, bin, val);
        break;
      case 49:
        _bin<unsigned int, unsigned char>(src, dest, bin, val);
        break;
      case 50:
        _bin<unsigned int, unsigned short>(src, dest, bin, val);
        break;
      case 51:
        _bin<unsigned int, unsigned int>(src, dest, bin, val);
        break;
      default:
        _bin_std(src, dest, bin, val);
        break;
      }
  }

  void binImg(const Image& imgs, int thr, const Image& imgd)
  {
    binImg(imgs, imgd, thr);
  }

  void binImg(const Image& imgs, int thr)
  {
    binImg(imgs, imgs, thr);
  }

#undef FNAME
  /*********************************************************************/
  /* Bild - Skalierung                                                 */
  /*********************************************************************/
#define FNAME "scaleImg"
  void scaleImg(const Image& pn1, int a, int b, const Image& pn2)
  {

    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(pn1, pn2, dx, dy));

    int gmax = pn2.maxval; /* maximalwert aus Zielbild ermitteln */

    for (int y = 0; y < dy; y++)      \
      for (int x = 0; x < dx; x++)
        {
          int v = (VAL1 * a + b) / 10;

          if (v < 0)
            {
              PutVal(pn2, x, y, 0);
            }
          else if (y > gmax)
            {
              PutVal(pn2, x, y, gmax);
            }
          else
            {
              PutVal(pn2, x, y, v);
            }
        }
  }
#undef FNAME

#define FNAME "equalImg"
  template <typename ValueType>
  bool equalImg(const Image& img1, const Image& img2)
  {
    if ((img1->xsize != img2->xsize) ||
        (img1->ysize != img2->ysize))
      {
        return false;
      }

    ValueType** const Pixel1 = (ValueType** const)(img1->getDataPtr());
    ValueType** const Pixel2 = (ValueType** const)(img2->getDataPtr());

    for (int y = 0; y < img1->ysize; y++)
      for (int x = 0; x < img1->xsize; x++)
        if (Pixel1[y][x] != Pixel2[y][x])
          {
            return false;
          }

    return true;
  }

  static bool equalImg_std(const Image& img1, const Image& img2)
  {
    if ((img1->xsize != img2->xsize) ||
        (img1->ysize != img2->ysize))
      {
        return false;
      }

    for (int y = 0; y < img1->ysize; y++)
      for (int x = 0; x < img1->xsize; x++)
        if (img1.getPixelUnchecked(x, y) != img2.getPixelUnchecked(x, y))
          {
            return false;
          }

    return true;
  }

  bool equalImg(const Image& img1, const Image& img2)
  {
    try
      {
        MatchImg(img1, img2);

        if (img1->ImageType() == img2->ImageType())
          {
            switch (img1->ImageType())
              {
              case 1:
                return equalImg<unsigned char>(img1, img2);
              case 2:
                return equalImg<unsigned short>(img1, img2);
              case 3:
                return equalImg<unsigned int>(img1, img2);
              default:
                return equalImg_std(img1, img2);
              }
          }

        return equalImg_std(img1, img2);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "findMax"
  int findMax(const Image& img, int& PosX, int& PosY)
  {
    if (! IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int max = 0;

    int xSize = img.xsize;
    int ySize = img.ysize;

    for (int y = 0; y < ySize; y++)
      for (int x = 0; x < xSize; x++)
        {
          int g = img.getPixelUnchecked(x, y);
          if (g > max)
            {
              max = g;
              PosX = x;
              PosY = y;
            }
        }
    return max;
  }

  int findMax(const Image& img, IPoint& p)
  {
    return findMax(img, p.x, p.y);
  }

  int findMax(const Image& img)
  {
    int x, y;
    return findMax(img, x, y);
  }
#undef FNAME
#define FNAME "findMin"
  int findMin(const Image& img, int& PosX, int& PosY)
  {
    int tmp = 0;

    if (! IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    tmp = img.maxval + 1;
    int xSize = img->xsize;
    int ySize = img->ysize;

    for (int y = 0; y < ySize; y++)
      for (int x = 0; x < xSize; x++)
        if (img.getPixelUnchecked(x, y) < tmp)
          {
            tmp = img.getPixelUnchecked(x, y);
            PosX = x;
            PosY = y;
          }

    return tmp;
  }

  int findMin(const Image& img, IPoint& p)
  {
    return findMin(img, p.x, p.y);
  }

  int findMin(const Image& img)
  {
    int x, y;
    return findMin(img, x, y);
  }
#undef FNAME
}
