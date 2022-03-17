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
#include <iostream>

#include "exfile.h"

using namespace std;

namespace ice
{
  EXFILE exopen(const string& fn, const char* mode)
  {
    EXFILE rc;

    if (fn[0] == '-')
      {
        rc.kind = fk_stdio;

        if (mode[0] == 'r')
          {
            rc.fd = stdin;
          }
        else
          {
            rc.fd = stdout;
          }

        return rc;
      }

    if (fn[0] == '|')
      {
        char smode[2];
        smode[0] = mode[0];
        smode[1] = 0;
        rc.kind = fk_process;
        rc.fd = popen(fn.substr(1).c_str(), smode);
        return rc;
      }

    rc.kind = fk_file;
    rc.fd = fopen(fn.c_str(), mode);
    return rc;
  }

  int exclose(EXFILE fd)
  {
    switch (fd.kind)
      {
      case fk_stdio:
        return 0;
      case fk_process:
        return pclose(fd.fd);
      default:
        return fclose(fd.fd);
      }
  }
}
