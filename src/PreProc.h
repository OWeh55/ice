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
/* Vorverarbeitung von Bildern zur Unterdrückung von Fourierkreuzen
   im Hinblick auf Bildmatching */

/* ttbb 20.05.1998 */

#ifndef _PREPROC_H_
#define _PREPROC_H_

#include "base.h"

// Parameter:
// img1 das zu bearbeitende Bild
// img2 das Ergebnisbild (dieses muß die gleiche Größe wie img1 haben)
// Rückgabe: Ergebniscode

namespace ice
{
  int PreprocessImg(Image img1, Image img2);
}
#endif
