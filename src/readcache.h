#ifndef READCACHE_H
#define READCACHE_H
/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
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

#include "base.h"
#include "ColorImage.h"

namespace ice
{
  template<typename Treader>
  class ReadColorImageCache
  {

  public:
    // pseudo frame numbers
    static const int next = -1001;
    static const int prev = -1002;
    // errorcodes
    static const int no_error = 0;
    static const int before = 1;
    static const int past_eof = 2;

#define FNAME "ReadColorImageCache::ReadColorImageCache"
  public:
    ReadColorImageCache(Treader& tr, int xs, int ys, int mv, int csize):
      reader(tr), vr(csize), vg(csize), vb(csize), bsize(csize),
      begin_bidx(0), end_bidx(1), first_frame(0), last_frame(0), framenr(-1), error(no_error)
    {
      try
        {
          if (csize < 2)
            {
              throw 1;
            }

          for (int i = 0; i < csize; i++)
            {
              vr[i] = NewImg(xs, ys, mv);
              vg[i] = NewImg(xs, ys, mv);
              vb[i] = NewImg(xs, ys, mv);
            }

          // ersten Frame lesen
          if (!reader.read(vr[0], vg[0], vb[0]))
            {
              throw 2;
            }
        }
      catch (int error)
        {
          switch (error)
            {
            case 1:
              throw IceException(FNAME, M_WRONG_PARAM);
              break;
            case 2:
              throw IceException(FNAME, M_FILE_OPEN);
            }
        }
      catch (...)
        {
          std::cout << "UNBEKANNTE EXCEPTION" << std::endl;
        }
    }
#undef FNAME

    virtual ~ReadColorImageCache() {};

#define FNAME "ReadColorImageCache::read"
    virtual bool read(const Image& imgr,
                      const Image& imgg,
                      const Image& imgb, int frame = next)
    {
      error = no_error;

      if (frame == next)
        {
          frame = framenr + 1;
        }
      else if (frame == prev)
        {
          frame = framenr - 1;
        }

      if (frame < first_frame)
        {
          error = before;
          return false;
        }

      if (frame > last_frame)
        {
          fillbuffer(frame);
        }

      if (error == no_error)
        {
          int bidx = buffer_index(frame);
          CopyImg(vr[bidx], imgr);
          CopyImg(vg[bidx], imgg);
          CopyImg(vb[bidx], imgb);
          framenr = frame;
        }

      return error == no_error;
    }
#undef FNAME

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
    Treader& reader;

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

    virtual void fillbuffer(int frame_nr)
    {
      while (error == no_error && last_frame < frame_nr)
        {
          if (!reader.read(vr[end_bidx], vg[end_bidx], vb[end_bidx]))
            {
              error = past_eof;
            }
          else
            {
              last_frame++;

              if (end_bidx == begin_bidx)
                {
                  first_frame++;
                  next_bidx(begin_bidx);
                }

              next_bidx(end_bidx);
            }
        }
    }

  };
//====================================================================
  template<typename Treader>
  class ReadImageCache
  {

  public:
    // pseudo frame numbers
    static const int next = -1001;
    static const int prev = -1002;
    // errorcodes
    static const int no_error = 0;
    static const int before = 1;
    static const int past_eof = 2;

#define FNAME "ReadImageCache::ReadImageCache"
  public:
    ReadImageCache(Treader& tr, int xs, int ys, int mv, int csize):
      reader(tr), buffer(csize), bsize(csize),
      begin_bidx(0), end_bidx(1), first_frame(0), last_frame(0), framenr(-1), error(no_error)
    {
      try
        {
          if (csize < 2)
            {
              throw 1;
            }

          for (int i = 0; i < csize; i++)
            {
              buffer[i] = NewImg(xs, ys, mv);
            }

          // ersten Frame lesen
          if (!reader.read(buffer[0]))
            {
              throw 2;
            }
        }
      catch (int error)
        {
          switch (error)
            {
            case 1:
              throw IceException(FNAME, M_WRONG_PARAM);
              break;
            case 2:
              throw IceException(FNAME, M_FILE_OPEN);
            }
        }
      catch (...)
        {
          std::cout << "UNBEKANNTE EXCEPTION" << std::endl;
        }
    }
#undef FNAME

    virtual ~ReadImageCache() {};

#define FNAME "ReadImageCache::read"
    virtual bool read(const Image& img, int frame = next)
    {
      error = no_error;

      if (frame == next)
        {
          frame = framenr + 1;
        }
      else if (frame == prev)
        {
          frame = framenr - 1;
        }

      if (frame < first_frame)
        {
          error = before;
          return false;
        }

      if (frame > last_frame)
        {
          fillbuffer(frame);
        }

      if (error == no_error)
        {
          int bidx = buffer_index(frame);
          CopyImg(buffer[bidx], img);
          framenr = frame;
        }

      return error == no_error;
    }
#undef FNAME

    virtual int FrameNumber() const
    {
      return framenr;
    }
    virtual int getError() const
    {
      return error;
    }

  private:
    Treader& reader;

    std::vector<Image> buffer;

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

    virtual void fillbuffer(int frame_nr)
    {
      while (error == no_error && last_frame < frame_nr)
        {
          if (!reader.read(buffer[end_bidx]))
            {
              error = past_eof;
            }
          else
            {
              last_frame++;

              if (end_bidx == begin_bidx)
                {
                  first_frame++;
                  next_bidx(begin_bidx);
                }

              next_bidx(end_bidx);
            }
        }
    }

  };
} // namespace ice
#endif
