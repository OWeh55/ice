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

/*
 * DIRFUNC.H zu ICE
 *  (c) OWeh 3/93,8/04,8/05
 */

#ifndef _DIRFUNC_H
#define _DIRFUNC_H

#include <vector>
#include <string>

namespace ice
{
  const int DIR_FILE = 1;
  const int DIR_DIR = 2;
  const int DIR_WITHPATH = 4;
  const int DIR_SELPATH = 8;

  void Directory(std::vector<std::string>& z, const std::string& mask, int mode = DIR_FILE | DIR_DIR);
  void Directory(std::vector<std::string>& z, std::string dir, const std::string& mask, int mode = DIR_FILE | DIR_DIR);
}

#endif
