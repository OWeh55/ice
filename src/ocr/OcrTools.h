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

  /** Binarisiert das Eingabebild*/
  int binImg(Image& img);

  /** liefert ein für Kennzeichen normiertes Bild zurück */
  int normalizeSeg(const Image& segImgInput, Image normedSeg,
                   int gitter1, int gitter2);
}

#endif
