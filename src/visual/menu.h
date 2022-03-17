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
/****************************************************
 * MENU.H  -  Menufunktionen fuer ICE-Console
 ****************************************************/

#ifndef _MENU_H
#define _MENU_H

#include <vector>

#include "util.h"
#include "Vector.h"
#include "dirfunc.h"

namespace ice
{
  int Menu(const std::vector<std::string>& zz, const vector<int>& id,
           int x1 = -1, int y1 = -1, int x2 = -1, int y2 = -1,
           bool restore = false, const string& title = "");

  int Menu(const std::vector<std::string>& zz, const IVector& id,
           int x1 = -1, int y1 = -1, int x2 = -1, int y2 = -1,
           bool restore = false, const string& title = "");

  int Menu(const std::vector<std::string>& zz,
           int x1 = -1, int y1 = -1, int x2 = -1, int y2 = -1,
           bool restore = false, const string& title = "");

  std::string FileSel(const std::string& mask,
                      int x1 = -1, int y1 = -1, int x2 = -1, int y2 = -1,
                      bool restore = true,
                      const string& title = "");

  bool SelFile(std::string mask,
               std::string& filename, std::string& dirname,
               int mode = DIR_FILE, const std::string& title = "",
               int x1 = -1, int y1 = -1, int x2 = -1, int y2 = -1);

  bool SelFile(const std::string& mask, std::string& filename);
}
#endif
