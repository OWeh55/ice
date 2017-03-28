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
