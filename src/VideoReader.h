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
/*************************************************************************/
/*
   class VideoReader
*/
#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <string>

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif

#include "base.h"
#include "ColorImage.h"
#include "PbmReader.h"
#include "ibuffer.h"

namespace ice
{
  class VideoReader
  {
  public:
    VideoReader(const std::string& fn);
    virtual ~VideoReader();
    virtual void getPara(int& xs, int& ys, int& mv, int& fps) const;
    virtual bool read(const Image& ir, const Image& ig, const Image& ib);
    virtual bool read(const ColorImage& img);
    virtual bool read();

    virtual int FrameNumber() const
    {
      return framenr;
    }

    VideoReader(const VideoReader& v) = delete;

  private:
    int framenr; // number of frame to read
    PbmReader pbmReader;
    int width, height, maxval, fps;
    void freeall();
  };
}
#endif
