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

#include "message.h"
#include "numbase.h"
#include "defs.h"

namespace ice
{
// Let x and y go through every point inside the active area of Img, where Img
// could be of type Image or ImageD
#define wloop(Img, x, y) \
  for (y = 0; y < (Img).ysize; y++) \
  for (x = 0; x < (Img).xsize; x++)

// Let x and y go through every point inside the active area of image Img, with an
// increment of Step
#define wloopstep(Img, x, y, Step) \
  for (y = GetActiveArea ().getTop(); y <= Img->GetActiveArea().getBottom(); y += Step) \
    for (x = Img->GetActiveArea ().getLeft(); x <= Img->GetActiveArea().getRight(); x += Step)

// since the guys at Microsoft found it nice to declare some min and max macros
// in windef.h, we have to undefine these if we want to provide our own min and max templates
// (they are in the VC++ 6.0 documentation, but they aren't part of their STL)

#ifdef WIN32
# ifdef min
#   undef min
# endif
# ifdef max
#   undef max
# endif
#endif

  template<typename T>
  inline T min(T First, T Second)
  {
    if (First < Second)
      return First;

    return Second;
  }

  template<typename T>
  inline T max(T First, T Second)
  {
    if (First > Second)
      return First;

    return Second;
  }

  inline double ARCUS(double x)
  {
    return x * M_PI / 180;
  }
  inline double DEGREE(double x)
  {
    return x * 180 / M_PI;
  }

  // macro for fast rewriting of Message(...) to throw
#define THROW_LOGIC(fname,message) throw logic_error(string(fname)+" - "+string(message))
#define THROW_RUNTIME(fname,message) throw runtime_error(string(fname)+" - "+string(message))

// Fehlerbehandlung in Prozeduren
#define IF_FAILED(action) OffMessage(); action; OnMessage(); if(GetError()!=OK)

#define RETURN_IF_FAILED(action,ret) { IF_FAILED(action) \
{ Message(FNAME,M_0,ERROR); return ret; }}

#define RETURN_VOID_IF_FAILED(action) { IF_FAILED(action) \
{ Message(FNAME,M_0,ERROR); return; }}

#define RETURN_NULL_IF_FAILED(action) { RETURN_IF_FAILED(action,NULL) }
#define RETURN_ERROR_IF_FAILED(action) { RETURN_IF_FAILED(action,GetError()) }

// Makro-untypische Namen
#define IfFailed IF_FAILED
#define ReturnIfFailed RETURN_IF_FAILED
#define ReturnNullIfFailed RETURN_NULL_IF_FAILED
#define ReturnErrorIfFailed RETURN_ERROR_IF_FAILED

  /* Für neuen Grafiktreiber unter Windows */
#ifdef WIN32
#ifdef _WIN32
#define GetPtr(hwnd)         (ImageWndw)GetWindowLong((hwnd), 0)
#define SetPtr(hwnd, pgwnd)  (ImageWndw)SetWindowLong((hwnd), 0, (LONG)(pgwnd))
#else
#define GetPtr(hwnd)         (ImageWndw)GetWindowWord((hwnd), 0)
#define SetPtr(hwnd, pgwnd)  (ImageWndw)SetWindowWord((hwnd), 0, (WORD)(pgwnd))
#endif
#endif
}

#endif /*IFDEF _MACRO_H */


