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
// Scan-Module "file"
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "macro.h"

#include "strtool.h"
#include "ibuffer.h"
#include "picio.h"

#include "visual/scan.h"
#include "visual/scan_file.h"

namespace ice
{
#define DD(i) ((ScanFileDesc*)Channel[i].driverdata)

  int init_file(int ch, string config)
  {
    string hs = Split(config, ",", false);

    if (hs != "file")
      {
        return false;
      }

    Split(config, ",");

    Channel[ch].driverdata = new ScanFileDesc;

    DD(ch)->filename = Split(config, ",");

    if (config != "")
      {
        Channel[ch].xsize = atol(Split(config, ",").c_str());
      }
    else
      {
        Channel[ch].xsize = -1;  // unknown
      }
    if (config != "")
      {
        Channel[ch].ysize = atol(Split(config, ",").c_str());
      }
    else
      {
        Channel[ch].ysize = -1;  // unknown
      }
    if (config != "")
      {
        Channel[ch].maxval = atol(Split(config, ",").c_str());
      }
    else
      {
        Channel[ch].maxval = -1;  // unknown
      }

    Channel[ch].wxi = -1;
    Channel[ch].flags = SC_SCAN | SC_RGB;
    Channel[ch].info = info_file;
    Channel[ch].dialog = nullptr;
    Channel[ch].preview = nullptr;
    Channel[ch].grab = grab_file;
    Channel[ch].getproperties = getproperties_file;
    Channel[ch].setproperties = setproperties_file;
    Channel[ch].setdriverextdata = nullptr;
    Channel[ch].getdriverextdata = nullptr;
    return true;
  }

  int add_file(ScanChannelDesc* desc, string& config)
  {
    config = string("file,") + ((ScanFileDesc*)(desc->driverdata))->filename + string(",");
    return true;
  }

#define FNAME "info_file"
  int info_file(int ch, int& x, int& y, int& maxval,
                int& flags, string& descr)
  {
    if ((Channel[ch].xsize < 0) || (Channel[ch].ysize < 0) || (Channel[ch].maxval < 0))
      {
        x = y = 0;
        FILE* f = fopen((DD(ch)->filename).c_str(), "r");
        if (f != nullptr)
          {
            fclose(f);
            int nr;
            RETURN_ERROR_IF_FAILED(InfImgFile(DD(ch)->filename, x, y, maxval, nr));
          }
      }
    else
      {
        x = Channel[ch].xsize;
        y = Channel[ch].ysize;
        maxval = Channel[ch].maxval < 0;
      }

    flags = SC_SCAN | SC_RGB;
    descr = string("file:") + DD(ch)->filename;
    return OK;
  }
#undef FNAME

#define FNAME "grab_file"
  int grab_file(int ch, ibuffer& ib)
  {
    Image pi;
    Image pig;
    Image pib;
    if (ib.planes == 1)
      {
        RETURN_ERROR_IF_FAILED(pi = ReadImg(DD(ch)->filename, pi));
        Image2Buffer(pi, ib);
      }
    else
      {
        RETURN_ERROR_IF_FAILED(ReadImg(DD(ch)->filename, pi, pig, pib));
        Image2Buffer(pi, pig, pib, ib);
      }
    return OK;
  }
#undef FNAME

  int getproperties_file(int ch, int& brightness, int& contrast,
                         int& saturation, int& hue)
  {
    return OK;
  }

  int setproperties_file(int ch, int brightness, int contrast,
                         int saturation, int hue)
  {
    return OK;
  }

}
