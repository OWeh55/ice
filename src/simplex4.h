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

