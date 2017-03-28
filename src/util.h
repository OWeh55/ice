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
 * Util.h
 * Funktion: Hilfsroutinen fuer diverse Zwecke
 *
 * Hilfsfunktion MulDiv
 *
 * Wertebereichsbeschränkung von Zahlen (limited())
 */

#ifndef _UTIL_H   /* verhindern mehrfacher Verarbeitung in einem */
#define _UTIL_H   /* Programm */

#include <cmath>

#include "base.h"


using namespace std;

namespace ice
{

#ifndef _WINDOWS_
// int MulDiv(int a, int b, int c);

  /**************************************************************/
  /* Hilfsroutine zur doppelt genauen ganzzahligen Multiplikation/Division   */
  /**************************************************************/
  inline int MulDiv(int a, int b, int c)
  {
#if ((defined(__i386)||defined(__i486)||defined(__x86_64))&&(!defined(__PIC__)))
    int val;
    asm("movl %1,%%eax\n\t;"
        "movl %2,%%ebx\n\t"
        "imull %%ebx\n\t"
        "movl %3,%%ebx\n\t"
        "idivl %%ebx\n\t"
        "movl %%eax,%0" :
        "=g"(val) : "g"(a), "g"(b), "g"(c):"%eax", "%ebx", "%edx");
    return val;
#else
    return ((int)floor((double)a * (double)b / (double)c));
#endif
  }
#endif

  inline int limited(int v, int min, int max)
  {
    if (v < min) return min;
    if (v > max) return max;
    return v;
  }

  inline int limited(int v, int max)
  {
    if (v < 0) return 0;
    if (v > max) return max;
    return v;
  }

  int limited(int v, const Image& p);
}

#endif /* ifndef _UTIL_H */
