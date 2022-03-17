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

#ifndef OCRTOOLSINCLUDE
#define OCRTOOLSINCLUDE

#include "fit.h"
#include "fourier.h"
#include "icefunc.h"
#include "threshld.h"

namespace ice
{
  /** liefert umschreibendes Rechteck mittels Quantilen in re[] zurück */
  int get_quantil_border(const Image& pic,
                         const Contur& c, int threshold, Window& win,
                         int seg_flag, int status, int& x_position);

  /** liefert umschreibendes Rechteck mittels Quantilen in re[] zurück
  * Beachte: kein support fuer seg_flag!
  * */
  int get_quantil_border(const Image& pic, int threshold, Window& win);

  /** Binarisiert das Eingabebild */
  void binImgAdaptive(Image& img);

  /** liefert ein für Kennzeichen normiertes Bild zurück */
  int normalizeSeg(const Image& segImgInput, Image normedSeg,
                   int gitter1, int gitter2);
}

#endif
