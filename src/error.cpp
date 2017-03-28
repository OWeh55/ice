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
// Standard-Fehlerbehandlung für Programme ohne spezielles
// Alpha-Fenster
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <string>

#include "message.h"

using namespace std;

namespace ice
{
  int PrintErrorStderr(const string& msg, int code, int mode)
  {
    int c = 0;
    fprintf(stderr, "Error: %s (%d)\n", msg.c_str(), code);

    if (mode == 1)
      {
        // can be ignored
        while ((c != 'A') && (c != 'I'))
          {
            fprintf(stderr, "Enter 'A' to abort or 'I' to ignore!\n");
            c = toupper(getchar());
          }

        return (c == 'I');
      }

    if (mode == 0) // fatal error
      {
        fprintf(stderr, "Press <Enter> to abort !\n");
        c = getchar();
        return false;
      }

    return true;
  }
}


