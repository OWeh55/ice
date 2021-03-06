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
/*
* macro.h
* macro for error handling, obsolete
*/
#ifndef _MACRO_H // include guard
#define _MACRO_H

#include "IceException.h"

namespace ice
{

// macro for optimized handling all neighbours

#define ForAll8Nbrs(img,x,y,x_nb,y_nb,code) \
{\
  if (x>0){\
    y_nb=y;\
    x_nb=x-1;\
    code;\
    if (y>0) { y_nb=y-1; code;}\
    if (y<img.ysize-1) { y_nb=y+1; code;}\
  }\
  if (x<img.xsize-1) {\
    y_nb=y;\
    x_nb=x+1;\
    code;\
    if (y>0) { y_nb=y-1; code;}\
    if (y<img.ysize-1) { y_nb=y+1; code;}\
  }\
  if (y>0) { y_nb=y-1; x_nb=x; code;}\
  if (y<(img.ysize-1)) { y_nb=y+1; x_nb=x; code;}\
}

#define ForAll4Nbrs(img,x,y,x_nb,y_nb,code) \
{\
  if (x>0) { y_nb=y; x_nb=x-1; code;}\
  if (x<(img.xsize-1)) { y_nb=y; x_nb=x+1; code;}\
  if (y>0) { y_nb=y-1; x_nb=x; code;}\
  if (y<(img.ysize-1)) { y_nb=y+1; x_nb=x; code;}\
}

  // error handling in functions
  // these macros should be substituted with exceptions

#define RETURN_ERROR_IF_FAILED(action) try { action; } catch(IceException &ex) { throw IceException(ex, FNAME); }
#define RETURN_NULL_IF_FAILED(action) RETURN_ERROR_IF_FAILED(action)
}

#endif /*IFDEF _MACRO_H */
