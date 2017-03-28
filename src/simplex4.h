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
#ifndef _SIMPLEX4_H_
#define _SIMPLEX4_H_

#include <cstdlib>
#include <cstdio>
#include <sys/types.h>
#include <string.h>
#include <malloc.h>

namespace ice
{

#undef MALLOC
#define MALLOC(name,len,type) {           \
    if ( ( (name) = (type *)malloc( (len) * sizeof(type) ) ) == NULL  \
   && (len)>0) {              \
      printf( "MALLOC(" #name "," #len "=%d," #type ")"     \
        ": cannot allocate space", len);        \
      exit(1);                \
    }                 \
  }

#undef CALLOC
#define CALLOC(name,len,type) {           \
    if ( ( (name) = (type *)calloc( (len) , sizeof(type) ) ) == NULL  \
   && (len)>0) {              \
      printf( "CALLOC(" #name "," #len "=%d," #type ")"     \
        ": cannot allocate space", len);        \
      exit(1);                \
    }                 \
  }

#undef REALLOC
#define REALLOC(name,len,type) {          \
    if (((name) = (type *)realloc((name),(len)*sizeof(type))) == NULL \
  && (len)>0) {             \
      printf( "REALLOC(" #name "," #len "=%d," #type ")"    \
        ": cannot reallocate space", len);      \
      exit(1);                \
    }                 \
  }

#undef FREE
#define FREE(name) {        \
    if ( (name) != nullptr ) free( (name) ); \
    (name) = nullptr;        \
  }

}
#endif








