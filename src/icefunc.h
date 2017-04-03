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
/*********************************************************************

  Weitere ICE-Funktionen - 18.08.1997 - ttbb

  GetFreeman
  Skeleton
  Thinning
  Shading
  HistogramEqual
  RelaxImg

**********************************************************************/

#ifndef _ICE_FUNC_EXTENDED_
#define _ICE_FUNC_EXTENDED_

#include "base.h"

namespace ice
{

  void RotateImg(Image& img, short typ);

  /*********************************************************************

      Skeleton

      Skeletierung des Binaerbildes img nach _imgd.
      Ist _imgd==NULL wird das Ergebnisbild neu angelegt.
      Rueckgabe: NULL bei Fehler, sonst Zeiger auf das Ergebnisbild.
      mode3 gibt die Richtung (VERT,HORZ oder HORZ_VERT) an.
      Mit mode wird die Behandlung der Minima angegeben:
          Eintragen des ersten Minima eines Plateus: FIRST_EXTREMA
          letztes:                                   LAST_EXTREMA
          mittlerer:                                 MID_EXTREMA
          Gesamtes Plateau:                          ALL_EXTREMA
      Dito mode2 fuer die Maxima.
      thresh gibt die Grauwertdifferenz an, um die sich zwei
      aufeinanderfolgende Extrema unterscheiden mussen.
      diff ist nur im Zusammenhang mit dem Modus ALL_EXTREMA fuer
      die Maxima interessant. diff muss aus [0,1] sein und bestimmt
      die relative zulaessige Grauwertdifferenz fuer Pixel neben dem
      Plateau, die noch zum Plateau zugehoerig zaehlen.


  **********************************************************************/


#define FIRST_EXTREMA 1
#define MID_EXTREMA   2
#define LAST_EXTREMA  3
#define ALL_EXTREMA   4
#define NO_EXTREMA    0
#define HORZ_VERT     (HORZ+VERT+1)

  void Skeleton(const Image& img,
                int mode, int mode2, int mode3,
                int thresh, double diff, const Image& imgd);

  void SkeletonImg(const Image& pic, const Image& skelett, int lvl);


  /**********************************************************************

      Thinning nach JAIN

  ***********************************************************************/

  void Thinning(const Image& img1, const Image& img2);


  /**********************************************************************

      Kantenverstaerkung durch Relaxation

  ***********************************************************************/

  void RelaxImg(const Image& img, const Image& imgd, int n = 3);


}
#endif
