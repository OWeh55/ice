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

#include "util.h"

namespace ice
{
  /*******************************************************************/
#if 0
  /**************************************************************/
  /* Hilfsroutine zur doppelt genauen ganzzahligen Multiplikation/Division   */
  /**************************************************************/
  int MulDiv(int a, int b, int c)
  {
#if ((defined(__i386)||defined(__i486)||defined(__x86_64))&&(!__PIC__))
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
  /*-----------------------------------------------------------
    Routinen zur Bereichsbeschraenkung von Werten
    -----------------------------------------------------------*/

#if 0
  int limited(int v, int min, int max)
  {
    if (v < min)
      {
        return min;
      }

    if (v > max)
      {
        return max;
      }

    return v;
  }
#endif

  int limited(int v, const Image& p)
  {
    if (v < 0)
      {
        return 0;
      }

    if (v > p.maxval)
      {
        return p.maxval;
      }

    return v;
  }
}
