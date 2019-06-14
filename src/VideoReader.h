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
