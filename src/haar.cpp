/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002-2017 FSU Jena, Digital Image Processing Group
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
#include <vector>

#include "message.h"
#include "defs.h"

#include "macro.h"

#include "haar.h"

namespace ice
{
  void getColumn(const Image&, int, std::vector<int>& v);
  void setColumn(Image&, int, const std::vector<int>& v);
  void getRow(const Image&, int, std::vector<int>& v);
  void setRow(Image&, int, const std::vector<int>& v);

  int Haar(const Image&, Image&, int);
  int HaarInv(const Image&, Image&, int);

  /*******************************************************************
  HaarImg.c, suesse 2/92, ortmann 2/2017
  Funktion HaarImg(pic1,pic2,p,mode) als Haartransformation des
  Bildes pic1 in das Bild pic2 mit der Auflösungsstufe p.
  Dimensionen der Bilder müssen Vielfache von 2^p sein
  ********************************************************************/
#define FNAME "HaarImg"

  static bool check2p(int n, int p)
  {
    bool ok = true;
    for (int i = 0; i < p && ok; i++)
      {
        if (n % 2 != 0)
          ok = false;

        n /= 2;
      }
    return ok;
  }

  int HaarImg(const Image& pic1, Image pic2, int depth = 1, int mode = NORMAL)
  {
    if (depth < 1)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int nx = pic1.xsize;
    int ny = pic1.ysize;

    /* F e h l e r b e h a n d l u n g e n   */
    // n? =  2 ^ depth * k?

    if (!check2p(nx, depth) || !check2p(ny, depth))
      {
        Message(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
        return (WRONG_PARAM);
      }

    if (mode != NORMAL && mode != INVERS)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return (WRONG_PARAM);
      }

    if (mode == NORMAL)
      Haar(pic1, pic2, depth);
    else
      HaarInv(pic1, pic2, depth);

    return OK;
  }

  void haar(std::vector<int>& v, int steps)
  {
    int hsize = v.size() / 2;
    for (int i = 0; i < steps; i++)
      {
        std::vector<int> temp(v);
        int is = 0;
        for (int l = 0; l < hsize; l++)
          {
            temp[l] = v[is] + v[is + 1];
            temp[hsize + l] = v[is] - v[is + 1];
            is += 2;
          }
        v = temp;
        hsize /= 2;
      }
  }

  /*****************************************************************/
  int Haar(const Image& pic1, Image& pic2, int p)
  {
    // Es werden erst die Spalten des Bildes transformiert
    for (int j = 0; j < pic1.xsize; j++)
      {
        std::vector<int> hCol(pic1.ysize);
        getColumn(pic1, j, hCol);
        haar(hCol, p);
        setColumn(pic2, j, hCol);
      }


    //Jetzt zeilenweise transformieren
    for (int i = 0; i < pic1.ysize; i++)
      {
        std::vector<int> hRow(pic2.xsize);
        getRow(pic2, i, hRow);
        haar(hRow, p);
        setRow(pic2, i, hRow);
      }
    return OK;
  };

  /********************************************************************/
  /* Inverse Haartransformation HaarInv(pic1,pic2,p) zur
     Transformation von Bild pic1 ins Bild pic2 mit der
     Auflösungsstufe p
  */

  void haarInverse(std::vector<int>& v, int steps)
  {
    int hsize = v.size();
    for (int i = 0; i < steps; i++)
      hsize /= 2;

    for (int i = 0; i < steps; i++)
      {
        std::vector<int> temp(v);
        int k = 0;
        for (int l = 0; l < hsize; l++)
          {
            int sum = v[l];
            int dif = v[l + hsize];
            temp[k] = (sum + dif) / 2;
            temp[k + 1] = (sum - dif) / 2;
            k += 2;
          }
        v = temp;
        hsize *= 2;
      }
  }

  int HaarInv(const Image& pic1, Image& pic2, int p)
  {
    // we need a temporary image here
    // we must not destroy pic1, but have an image with big values
    Image h;
    h.create(pic1);

    // Zuerst wird das Bild zeilenweise transformiert
    for (int i = 0; i < pic1.ysize; i++)
      {
        std::vector<int> hRow(pic1.xsize);
        getRow(pic1, i, hRow);
        haarInverse(hRow, p);
        setRow(h, i, hRow);
      }

    // Jetzt werden die Spalten transformiert
    for (int j = 0; j < pic1.xsize; j++)
      {
        std::vector<int> hCol(pic1.ysize);
        getColumn(h, j, hCol);
        haarInverse(hCol, p);
        setColumn(pic2, j, hCol);
      }

    return OK;
  }

  /* getColumn - spalte j von bild holen) */
  void getColumn(const Image& bild, int c, std::vector<int>& v)
  {
    int grwnull = (bild.maxval + 1) / 2;
    for (int i = 0; i < bild.ysize; i++)
      {
        v[i] = bild.getPixel(c, i) - grwnull;
      }
  };

  /* getRow - zeile i von bild holen */
  void getRow(const Image& bild, int r, std::vector<int>& v)
  {
    int grwnull = (bild.maxval + 1) / 2;
    for (int i = 0; i < bild.xsize; i++)
      {
        v[i] = bild.getPixel(i, r) - grwnull;
      }
  }

  /* setColumn - spalte j auf bild schreiben */
  void setColumn(Image& bild, int c, const std::vector<int>& v)
  {
    int grwnull = (bild.maxval + 1) / 2;

    for (int i = 0; i < bild.ysize; i++)
      {
        bild.setPixelLimited(c, i, v[i] + grwnull);
      }
  }

  /* setRow - zeile i auf bild schaffen */
  void setRow(Image& bild, int r, const std::vector<int>& v)
  {
    int grwnull = (bild.maxval + 1) / 2;

    for (int i = 0; i < bild.xsize; i++)
      {
        bild.setPixelLimited(i, r, v[i] + grwnull);
      }
  }
}
