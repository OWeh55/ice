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

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "defs.h"
#include "IceException.h"

#include "pbmio.h"

#include "VideoWriter.h"

using namespace std;

namespace ice
{
  //******* VideoWriter ****************************

  VideoWriter::VideoWriter(const string& fname):
    filename(fname),
    framenr(0),
    xsize(0), ysize(0),
    frate(25), brate(4000000),
    format("")
  {
    fd.fd=nullptr;
    // really open video before first writing
  }

  bool VideoWriter::close()
  {
    if (fd.fd != nullptr)
      {
        exclose(fd);
      }
    fd.fd = nullptr;
    framenr = 0;
    return true;
  }

  VideoWriter::~VideoWriter()
  {
    close();
  };

  void VideoWriter::getPara(int& xs, int& ys, int& mv, int& fps) const
  {
    xs = xsize;
    ys = ysize;
    mv = 255;
    fps = frate;
  }

  bool VideoWriter::setPara(int xs, int ys, int mv, int fps, int bratep)
  {
    xsize = xs;
    ysize = ys;
    // ignore mv for the moment
    if (fps > 0)
      {
        frate = fps;
      }
    if (bratep > 0)
      {
        brate = bratep;
      }
    return true;
  }

  bool VideoWriter::init()
  {
    string cmdline = "|ffmpeg -y -v error -f image2pipe -codec:v ppm ";
    if (frate > 0)
      {
        cmdline += " -r " + std::to_string(frate);
      }
    cmdline += " -i -";
    if (brate > 0)
      {
        cmdline += " -b:v " + std::to_string(brate);
      }
    if (!format.empty())
      {
        if (format[0] == '#')
          {
            // special predefined profiles
            if (format == "#h264ts")
              format = "-f mpegts -vcodec libx264 -vf format=yuv420p -profile:v high -level 4.1";
            else if (format == "#lossless")
              format = "-c:v libvpx-vp9 -pix_fmt gbrp -lossless 1";
            else
              throw IceException("VideoWriter", "Undefined profile " + format);
          }
        cmdline += " " + format + " ";
      }
    cmdline += " \'" + filename + "\'";


    // cout << cmdline << endl;

    fd = exopen(cmdline, FWMODUS);
    if (fd.fd == nullptr)
      {
        throw IceException("VideoWriter::init", M_FILE_OPEN);
      }
    return true;
  }

#define FNAME "VideoWriter::write"
  bool VideoWriter::write(const Image& ir, const Image& ig, const Image& ib)
  {
    try
      {
        int xa, ya;
        MatchImg(ir, ig, ib, xa, ya);

        if (xsize == 0)   // image size already set ?
          {
            setPara(xa, ya, 255, 0, 0);
          }

        if (framenr == 0)   // first frame
          {
            init();
          }

        WritePBMImg(ir, ig, ib, fd);

        framenr++;
        return true;
      }
    RETHROW;
  }
#undef FNAME
}

