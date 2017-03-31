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
/*****************************************************************************/
/* Macro.h                                                                   */
/* Funktion: Hilfsmakros                                                     */
/*****************************************************************************/

#ifndef _MACRO_H  /* verhindern mehrfacher Verarbeitung in einem */
#define _MACRO_H  /* Programm */

#include "IceException.h"
#include "numbase.h"
#include "defs.h"

namespace ice
{
// Let x and y go through every point inside the active area of Img, where Img
// could be of type Image or ImageD

#define wloop(Img, x, y) \
  for (y = 0; y < (Img).ysize; y++) \
  for (x = 0; x < (Img).xsize; x++)

// Fehlerbehandlung in Prozeduren
#define IF_FAILED(action) try { action; } catch(IceException ex)

#define RETURN_ERROR_IF_FAILED(action) try { action; } catch(IceException &ex) { throw IceException(ex, FNAME); }
#define RETURN_IF_FAILED(action,ret) RETURN_ERROR_IF_FAILED(action)
#define RETURN_VOID_IF_FAILED(action) RETURN_ERROR_IF_FAILED(action)
#define RETURN_NULL_IF_FAILED(action) RETURN_ERROR_IF_FAILED(action)
}

#endif /*IFDEF _MACRO_H */


