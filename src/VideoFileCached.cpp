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
/* vio.cpp  */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "defs.h"
#include "macro.h"

#include "VideoFile.h"
#include "dtime.h"

using namespace std;

namespace ice
{
//******* VideoFileCached ***********************************************
#define FNAME "VideoFileCached::VideoFileCached"
  VideoFileCached::VideoFileCached(const string& fn, int size):
    videofile(nullptr), vr(size), vg(size), vb(size), bsize(size),
    begin_bidx(0), end_bidx(1), first_frame(0), last_frame(0), framenr(-1), error(no_error)
  {
    try
      {
        if (size < 2)
          {
            throw 6;
          }

        videofile = new VideoFile(fn);
        int xs, ys, mv, fps;
        videofile->getPara(xs, ys, mv, fps);

        for (int i = 0; i < size; i++)
          {
            vr[i].create(xs, ys, mv);
            vr[i].set(mv);
            vg[i].create(xs, ys, mv);
            vg[i].set(0);
            vb[i].create(xs, ys, mv);
            vb[i].set(mv);
          }

        // ersten Frame lesen
        if (!videofile->read(vr[0], vg[0], vb[0]))
          {
            throw 3;
          }
      }
    catch (int error)
      {
        switch (error)
          {
          case 1:
            throw IceException(FNAME, M_FILE_OPEN);
            break;
          case 2:
          case 3:
            throw IceException(FNAME, M_WRONG_FILETYPE);
            break;
          case 4:
          case 5:
            throw IceException(FNAME, M_UNSUPPORTED_FILE);
            break;
          case 6:
            throw IceException(FNAME, M_WRONG_PARAMETER);
            break;
          default:
            throw IceException(FNAME, M_FILE_OPEN);
          }

        videofile = nullptr;
      }
    catch (...)
      {
        cout << "UNBEKANNTE EXCEPTION" << endl;
      }
  }
#undef FNAME

  VideoFileCached::~VideoFileCached()
  {
    if (videofile)
      {
        delete videofile;
      }
  }

  void VideoFileCached::fillbuffer(int frame_nr)
  {
    //    cout << "fill " << frame_nr << " e:" << error  << endl;
    while (error == no_error && last_frame < frame_nr)
      {
        // cout << ">" << end_bidx << endl;
        if (!videofile->read(vr[end_bidx], vg[end_bidx], vb[end_bidx]))
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

#define FNAME "VideoFileCached::read"
  bool VideoFileCached::read(const Image& imgr,
                             const Image& imgg,
                             const Image& imgb, int frame)
  {
    if (!videofile)
      throw IceException(FNAME, M_NOT_OPEN);

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

    //    cout << "read " << frame << "  " << first_frame << ".." << last_frame << endl;

    if (frame > last_frame)
      {
        fillbuffer(frame);
      }

    if (error == no_error)
      {
        int bidx = buffer_index(frame);
        //        cout << "<" << bidx << endl;
        //        double ta=TimeD();
        CopyImg(vr[bidx], imgr);
        CopyImg(vg[bidx], imgg);
        CopyImg(vb[bidx], imgb);
        //       cout << TimeD() - ta << endl;
        framenr = frame;
      }

    return error == no_error;
  }
#undef FNAME
#define FNAME "VideoFileCached::getPara"
  void VideoFileCached::getPara(int& xsize, int& ysize, int& maxval, int& fpsp) const
  {
    if (!videofile)
      throw IceException(FNAME, M_NOT_OPEN);

    videofile->getPara(xsize, ysize, maxval, fpsp);
  }
#undef FNAME
} // namespace ice
