/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef EX_FILE_H
#define EX_FILE_H
#include <string>

namespace ice
{
  // lese und schreibmodi
#define FWMODUS "wb"
#define FRMODUS "rb"

  enum filekind { fk_file, fk_process, fk_stdio };
  typedef struct
  {
    FILE* fd;
    enum filekind kind;
  } EXFILE;
  EXFILE exopen(const std::string& path, const char* mode);
  int exclose(EXFILE df);
}
#endif
