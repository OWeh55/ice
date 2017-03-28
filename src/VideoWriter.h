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
/* VideoWriter.h
   class VideoWriter
*/
#ifndef VIDEOWRITER_H
#define VIDEOWRITER_H

#include <string>

#include "exfile.h"
#include "base.h"
#include "ColorImage.h"
#include "ibuffer.h"

namespace ice
{
  class VideoWriter
  {
  public:
    VideoWriter(const std::string& fn);
    virtual ~VideoWriter();

    virtual bool close();

    virtual bool setPara(int xs, int ys, int mv = 255,
                         int fps = 0, int brate = 0);

    virtual void getPara(int& xs, int& ys, int& mv, int& fps) const;

    virtual bool write(const Image& ir, const Image& ig, const Image& ib);

    virtual int FrameNumber() const
    {
      return framenr;
    }

    virtual void setCPara(const std::string& p)
    {
      format = p;
    }

    VideoWriter(const VideoWriter& v) = delete;
  private:
    std::string filename;
    int framenr; // frame number to write

    EXFILE fd;

    int xsize;
    int ysize;
    int frate;
    int brate;

    std::string format;

    bool init();
  };
}

#endif // VIDEOWRITER_H
