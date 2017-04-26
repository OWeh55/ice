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
// Scan-Module "program"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;
#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "macro.h"
#include "strtool.h"

#include "ibuffer.h"
#include "picio.h"


#include "visual/scan.h"
#include "visual/scan_program.h"

//#include <iostream>

namespace ice
{
#define DD(i) ((ScanProgramDesc*)Channel[i].driverdata)

  int init_program(int ch, string config)
  {
    string hs = Split(config, ",", false);
    // cout << "hs:" << hs << endl;
    if (hs != "program")
      {
        return false;
      }

    Split(config, ",");

    Channel[ch].driverdata = new ScanProgramDesc;

    DD(ch)->scan = Split(config, ",");
    DD(ch)->filename = Split(config, ",");
    //    cout << config << endl;
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
    Channel[ch].info = info_program;
    Channel[ch].dialog = NULL;
    Channel[ch].preview = NULL;
    Channel[ch].grab = grab_program;
    Channel[ch].getproperties = getproperties_program;
    Channel[ch].setproperties = setproperties_program;

    Channel[ch].setdriverextdata = NULL;

    Channel[ch].getdriverextdata = NULL;

    return true;
  }


  int add_program(ScanChannelDesc* desc, string& config)
  {

    config = string("program,") +

             ((ScanProgramDesc*)(desc->driverdata))->scan + string(",") +

             ((ScanProgramDesc*)(desc->driverdata))->filename;
    if (desc->xsize > 0)
      {
        config += "," + NumberString(desc->xsize);
      }
    if (desc->ysize > 0)
      {
        config += "," + NumberString(desc->ysize);
      }
    if (desc->maxval > 0)
      {
        config += "," + NumberString(desc->maxval);
      }

    return true;

  }

  int info_program(int ch, int& x, int& y, int& maxval,
                   int& flags, string& descr)
  {
    x = Channel[ch].xsize;
    y = Channel[ch].ysize;
    maxval = Channel[ch].maxval;
    flags = SC_SCAN | SC_RGB;
    descr = string("program:") + DD(ch)->scan;
    return OK;
  }

#define FNAME "grab_program"
  int grab_program(int ch, ibuffer& ib)
  {
    RETURN_ERROR_IF_FAILED(system(DD(ch)->scan.c_str()));
    if (ib.planes == 1)
      {
        Image pi;
        RETURN_ERROR_IF_FAILED(pi = ReadImg(DD(ch)->filename, pi));
        unlink(DD(ch)->filename.c_str());
        Image2Buffer(pi, ib);
      }
    else
      {
        Image pir;
        Image pig;
        Image pib;
        //      cout << "1" << endl;
        RETURN_ERROR_IF_FAILED(ReadImg(DD(ch)->filename, pir, pig, pib));
        //      cout << "2" << endl;
        unlink(DD(ch)->filename.c_str());
        //      cout << "3" << endl;
        Image2Buffer(pir, pig, pib, ib);
        //      cout << "4" << endl;
      }
    return OK;
  }
#undef FNAME
  int getproperties_program(int ch, int& brightness, int& contrast,
                            int& saturation, int& hue)
  {
    return OK;
  }

  int setproperties_program(int ch, int brightness, int contrast,
                            int saturation, int hue)
  {
    return OK;
  }
}
