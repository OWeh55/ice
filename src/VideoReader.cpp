/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2016 FSU Jena, Digital Image Processing Group
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

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "defs.h"
#include "macro.h"

#include "VideoReader.h"

namespace ice
{
  VideoReader::VideoReader(const std::string& fn): framenr(0)
  {
    pr.open("|ffmpeg -i " + fn + " -y -codec:v ppm -f rawvideo -an -sn -v error - ");
  }

  void VideoReader::freeall()
  {
  }

  VideoReader::~VideoReader()
  {
  }

  void VideoReader::getPara(int& xsize, int& ysize, int& maxval, int& fps) const
  {
    int nChannels;
    pr.getInfo(xsize, ysize, maxval, nChannels);
    fps = 25; // unknown, dummy value !!!
  }
#define FNAME "VideoReader::read"
  bool VideoReader::read(const Image& r, const Image& g, const Image& b)
  {
    try
      {
        pr.getImage(r, g, b);
        pr.nextImage();
        framenr++;
        return true;
      }
    catch (const exception& ex)
      {
        // read failed (eof..)
        return false;
      }
  }

  bool VideoReader::read(const ColorImage& img)
  {
    return read(img.redImage(), img.greenImage(), img.blueImage());
  }

  bool VideoReader::read()
  {
    pr.getImage();
    pr.nextImage();
    framenr++;
    return true;
  }
#undef FNAME
}

// ffmpeg -i xyz.mp4 -y -codec:v ppm -f rawvideo -an -sn -   (color)
// input force-overwrite  videocodec fileformat noaudio nosubtitle to_stdout
