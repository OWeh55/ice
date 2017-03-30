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
/* VideoFile.h
   Classes VideoCile and VideoFileCached
*/
#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <string>

#include "base.h"
#include "ColorImage.h"

#include "VideoReader.h"
#include "VideoWriter.h"

namespace ice
{
  class VideoFile
  {
  public:
    VideoFile();
    VideoFile(const std::string& fn,
              std::ios_base::openmode mode = std::ios_base::in);
    virtual ~VideoFile();

    virtual int open(const std::string& fn,
                     std::ios_base::openmode mode = std::ios_base::in);

    virtual int close();
    virtual int reset();

    virtual bool isOpen() const
    {
      return reader != nullptr || writer != nullptr;
    }

    virtual void getPara(int& xs, int& ys, int& mv, int& fps) const;
    virtual void setPara(int xs, int ys,
                         int mv = 255, int fps = 0, int brate = 0);

    virtual void setCPara(const std::string& p)
    {
      if (writer) writer->setCPara(p);
    }

    virtual bool read(const Image& img, int ch = 3);
    virtual bool read(const Image& ir, const Image& ig, const Image& ib);
    virtual bool read();

    virtual bool read(ColorImage& irgb)
    {
      return read(irgb.redImage(), irgb.greenImage(), irgb.blueImage());
    }

    virtual bool write(const Image& ir, const Image& ig, const Image& ib);

    virtual bool write(const Image& img)
    {
      return write(img, img, img);
    }

    virtual bool write(const ColorImage& irgb)
    {
      return write(irgb.redImage(), irgb.greenImage(), irgb.blueImage());
    }

    virtual int FrameNumber() const;

  private:
    VideoReader* reader;
    VideoWriter* writer;

    bool writemode; // write=true, read=false
    std::string filename;
  };

  class VideoFileCached
  {
  public:
    // pseudo frame numbers
    static const int next = -1001;
    static const int prev = -1002;
    // errorcodes
    static const int no_error = 0;
    static const int before = 1;
    static const int past_eof = 2;

    VideoFileCached(const std::string& fn, int size);
    virtual ~VideoFileCached();

    virtual void getPara(int& xs, int& ys, int& mv, int& fps) const;

    virtual bool read(const Image& ir, const Image& ig, const Image& ib,
                      int frame = next);
    virtual bool read(ColorImage& irgb, int frame = next)
    {
      return read(irgb.redImage(), irgb.greenImage(), irgb.blueImage(), frame);
    }

    virtual int FrameNumber() const
    {
      return framenr;
    }

    virtual int getError() const
    {
      return error;
    }

  private:
    VideoFile* videofile;
    std::vector<Image> vr;
    std::vector<Image> vg;
    std::vector<Image> vb;

    int bsize;

    int begin_bidx; // first buffer index (with content)
    int end_bidx; // buffer index after last frame

    int first_frame; // first frame in buffer
    int last_frame; // first frame in buffer

    int framenr; // number of last read frame
    int error;

    void next_bidx(int& idx) const
    {
      idx = (idx + 1) % bsize;
    }

    int buffer_index(int frame_nr) const
    {
      return (begin_bidx - first_frame + frame_nr) % bsize;
    }

    void fillbuffer(int frame_nr);

  };
}
#endif
