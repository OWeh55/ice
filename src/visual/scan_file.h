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
#ifndef SCAN_FILE_H
#define SCAN_FILE_H

#include "visual/scan.h"

namespace ice
{
  typedef struct scan_file_desc_
  {
    string filename;
  } ScanFileDesc;

  int init_file(int ch, string config);
  int add_file(ScanChannelDesc* desc, string& config);
  int info_file(int ch, int& x, int& y, int& maxval,
                int& flags, string& descr);
  int preview_file(int ch, int on);
  int dialog_file(int ch);
  int grab_file(int ch, ibuffer& ib);
  int getproperties_file(int ch, int& brightness, int& contrast,
                         int& saturation, int& hue);
  int setproperties_file(int ch, int brightness, int contrast,
                         int saturation, int hue);
}
#endif

