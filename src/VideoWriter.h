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
