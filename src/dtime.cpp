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

#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#ifndef _WIN32
#include <sys/times.h>
#endif

#if 0
#ifndef _MSC_VER
#include <sys/time.h>
#include <unistd.h>
#endif

#ifndef _WIN32
#include <sys/times.h>
#include <sys/resource.h>
#endif
#endif

#include "defs.h"
#include "dtime.h"
#include "IceException.h"

namespace ice
{
  /*
  * Zeitmessung
  */

  void Delay(double t)
  {
    double t1;
    t1 = TimeD() + t; // end time=start time + delay

    while (TimeD() < t1) ;
  }

  double TimeD1()
  {
    struct timeval tt;
    struct timezone tz;
    int i;
    i = gettimeofday(&tt, &tz);

    if (i == -1)
      {
        throw IceException("TimeD()", "keine Zeit"); /* kann nicht vorkommen ?! */
      }

    return (tt.tv_sec + ((double)tt.tv_usec) / 1000000);
  }

#ifndef _WIN32
  double TimeD2(bool child, bool user)
  {
    double time;
    struct tms mtimes;
    times(&mtimes);

    if (child)
      {
        time = (double)mtimes.tms_cutime;

        if (!user)
          {
            time += (double)mtimes.tms_cstime;
          }
      }
    else
      {
        time = (double)mtimes.tms_utime;

        if (!user)
          {
            time += (double)mtimes.tms_stime;
          }
      }

    time /= (double)sysconf(_SC_CLK_TCK);
    return time;
  }
#endif

#define FNAME "TimeD"
  double TimeD(int mode)
  {
#ifdef _WIN32
    return TimeD1();
#else

    switch (mode)
      {
      case TM_WORLD:
        return TimeD1();
        break;
      case TM_PROCESS:
        return TimeD2(false, false);
        break;
      case TM_USER:
        return TimeD2(false, true);
        break;
      case TM_CHILD:
        return TimeD2(true, false);
        break;
      case TM_CHILDUSER:
        return TimeD2(true, true);
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
        break;
      }

    return WRONG_PARAM;
#endif
  }
#undef FNAME
}
