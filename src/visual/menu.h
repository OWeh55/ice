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
