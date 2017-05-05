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

// Stephan Wagner, Feb. 2005
// Wolfgang Ortmann, 10/05, 12/11

#ifndef WATERSHED_H
#define WATERSHED_H

#include <vector>
#include <deque>
#include <set>

#include "base.h"

namespace ice
{

  const int INIT = 0;
  const int MASK = -1;
  const int INQUEUE = -2;
  const int WSHD = -3;
  const int DONE = -4;

// Makrodefinition fuer 8-er Nachbarschaft
// jetzt 12 Zuweisungen, vorher 18, 8 Vergleiche
// normaler 8-er Nachbarschaftsansatz: 8*2=16 Zuweisungen und 8*4=32 Vergleiche
#define ForAll8Nbrs(p,x,y,x_nb,y_nb,code) \
  {           \
    if (x>p->wxi){        \
      y_nb=y;         \
      x_nb=x-1;         \
      code;         \
      if (y>p->wyi) { y_nb=y-1; code;}    \
      if (y<p->wya) { y_nb=y+1; code;}    \
    }           \
    if (x<p->wxa) {       \
      y_nb=y;         \
      x_nb=x+1;         \
      code;         \
      if (y>p->wyi) { y_nb=y-1; code;}    \
      if (y<p->wya) { y_nb=y+1; code;}    \
    }           \
    if (y>p->wyi) { y_nb=y-1; x_nb=x; code;}  \
    if (y<p->wya) { y_nb=y+1; x_nb=x; code;}  \
  }


// Makrodefinition fuer 4-er Nachbarschaft
// jetzt 4 Zuweisungen, 4 Vergleiche
#define ForAll4Nbrs(p,x,y,x_nb,y_nb,code)   \
  {             \
    if (x>p->wxi) { y_nb=y; x_nb=x-1; code;}    \
    if (x<p->wxa) { y_nb=y; x_nb=x+1; code;}  \
    if (y>p->wyi) { y_nb=y-1; x_nb=x; code;}    \
    if (y<p->wya) { y_nb=y+1; x_nb=x; code;}    \
  }


// setM : Makro zum Setzen der Werte des Arrays auf einen Initialwert
#define setM(m,x,y,cols,rows,color) for(y=0;y<rows;y++) for(x=0;x<cols;x++) m[y][x]=color;

// Wert aus Array lesen und Wert setzen
#define PutValM(mat,x,y,wert) mat[y][x]=wert
#define GetValM(mat,x,y)      mat[y][x]

// Wasserscheidentransformation:
// WST ist Aufruf-Wrapper für (interne) Funktionen WST1, WST2
// WST1 - klassischer Flutungsansatz und WST2 - Markeransatz

  enum class WstMode { classic, marker};
  void WST(Image& i, Image& WSImg, WstMode mode = WstMode::classic);

//int WST1(Image i, Image &o);
//int WST2(Image i, Image &o);

  bool PixelIsMinimum(const Image& i, int x, int y);
  int getImgMinimas(const Image& i, Image& io);

  int PixelIsMinimum2(const Image& i, int x, int y);
  int getImgMinimas2(const Image& i, Image& io);

  bool PixelIsMaximum(const Image& i, int x, int y);
  int getImgMaximums(const Image& i, Image& io);

  int NbRegions(const Image& i, int x, int y);

// Bild in wird in nach unten vollstaendiges (Lower Complete) Bild transformiert
  int LowerCompleteImg(const Image& in, Image& lcImg);

// Marker in Bild i1 loeschen, die aus weniger als threshold Pixeln bestehen
  int deleteSmallMarkers(const Image& i1, int threshold);

// Startregionen mit Label versehen
  int fillStartRegion(Image& m1, Image& m2, int x, int y, int currLab);

// Wasserscheidenlinien loeschen und durch gemittelten Grauwert der angrenzenden Regionen ersetzen
  int delWSHDPixels(const Image& GrwImg, Image& WSHEDDeleted);

//
  int dGRWHistogramm(const Image& Origial, const Image& WSImg, Image& GrwImg);
}
#endif
