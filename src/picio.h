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
/* Bildein- und Ausgabe mit verschiedenen Formaten */
#ifndef _PICIO_H
#define _PICIO_H

#include "ibuffer.h"
#include "jpegio.h"
#include "tifio.h"
#include "pcxio.h"
#include "tgaio.h"
#include "psdio.h"

namespace ice
{
  extern std::string ReadImgFileComment;
  extern std::string WriteImgFileComment;

// Public functions for image load and save
// Read image file
  Image ReadImg(const std::string& fname, Image& img, int flag = IB_SCALE);
  Image ReadImg(const std::string& fname);
  int ReadImg(const std::string& fname,
              Image& ir, Image& ig, Image& ib, int flag = IB_SCALE);

// Write image file
  int WriteImg(const Image& img, const std::string& fname);
  int WriteImg(const Image& ir, const Image& ig, const Image& ib,
               const std::string& fname);
// get type of file
  int ImageFileType(std::string& fname);
  int ImageFileType(std::string& fname, std::string& defext);
  std::string ImageTypePrefix(int typ);
// get file info
  int InfImgFile(const std::string& fname, int& xsize, int& ysize, int& maxval);
  int InfImgFile(const std::string& fname, int& xsize, int& ysize,
                 int& maxval, int& nr);

// Tool-functions for internal use of image load and save functions
// extern unsigned char red[256], green[256], blue[256];

  void bmp8img(unsigned char* bmp, int w, int h, Image& img);
  void bmp16img(unsigned char* bmp, int w, int h, Image& img);
}
#endif
