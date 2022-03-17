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
#include "visual/xio.h"
#include "dtime.h"
#include "visual/timing.h"

namespace ice
{
#define TIMERNR 10
  double startt[TIMERNR];
  bool run[TIMERNR];

  void Begin(int n, const char* s)
  {
    int i, ni;
    ni = n % TIMERNR;
    for (i = 0; i < n; i++)
      {
        Printf(" ");
      }
    Printf("Begin %s\n", s);
    startt[ni] = TimeD(TM_PROCESS);
    run[ni] = true;
  }

  void Stop(int n)
  {
    int ni;
    ni = n % TIMERNR;
    if (run[ni])
      {
        startt[ni] = TimeD(TM_PROCESS) - startt[ni];
      }
    run[ni] = false;
  }

  void Cont(int n)
  {
    int ni;
    ni = n % TIMERNR;
    if (!run[ni])
      {
        startt[n] = TimeD(TM_PROCESS) - startt[n];
      }
    run[ni] = true;
  }

  void End(int n, const char* s)
  {
    int i, ni;
    ni = n % 10;
    for (i = 0; i < n; i++)
      {
        Printf(" ");
      }
    if (run[ni])
      {
        Stop(ni);
      }
    Printf("End %s (%6.2f)\n", s, startt[ni]);
  }
}
