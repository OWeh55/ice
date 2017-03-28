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

#include "strtool.h"

#include "VideoFile.h"

namespace ice
{
//******* Videofile ***********************************************
  VideoFile::VideoFile():
    reader(nullptr),
    writer(nullptr),
    filename("")
  {
  }

#define FNAME "VideoFile::VideoFile"
  VideoFile::VideoFile(const string& fn, ios_base::openmode mode):
    reader(nullptr),
    writer(nullptr),
    filename(fn)
  {
    if (mode == ios_base::in)
      {
        try
          {
            reader = new VideoReader(fn);
          }
        catch (int error)
          {
            switch (error)
              {
              case 1:
                Message(FNAME, M_FILE_OPEN, fn, WRONG_FILE);
                break;
              case 2:
              case 3:
                Message(FNAME, M_WRONG_FILETYPE, fn, WRONG_FILE);
                break;
              case 4:
              case 5:
                Message(FNAME, M_UNSUPPORTED_FILE, fn, WRONG_FILE);
                break;
              default:
                Message(FNAME, M_FILE_OPEN, fn, WRONG_FILE);
              }

            reader = nullptr;
          }
      }
    else
      {
        try
          {
            writer = new VideoWriter(fn);
          }
        catch (int error)
          {
            Message(FNAME, M_FILE_OPEN + NumberString(error), WRONG_FILE);
            writer = nullptr;
          }
      }
  }
#undef FNAME

  VideoFile::~VideoFile()
  {
    if (reader)
      delete reader;
    if (writer)
      delete writer;
  }

  int VideoFile::close()
  {
    if (reader)
      delete reader;

    reader = nullptr;

    if (writer)
      delete writer;

    writer = nullptr;
    return true;
  }

#define FNAME "VideoFile::reset"
  int VideoFile::reset()
  {
    if (writer)
      {
        Message(FNAME, "Cannot reset in write mode", WRONG_PARAM);
        return WRONG_PARAM;
      }

    close();

    // reopen videofile
    try
      {
        reader = new VideoReader(filename);
      }
    catch (int error)
      {
        switch (error)
          {
          case 1:
            Message(FNAME, M_FILE_OPEN, filename, WRONG_FILE);
            break;
          case 2:
          case 3:
            Message(FNAME, M_WRONG_FILETYPE, filename, WRONG_FILE);
            break;
          case 4:
          case 5:
            Message(FNAME, M_UNSUPPORTED_FILE, filename, WRONG_FILE);
            break;
          default:
            Message(FNAME, M_FILE_OPEN, filename, WRONG_FILE);
          }

        reader = nullptr;
      }

    return OK;
  }
#undef FNAME

#define FNAME "VideoFile::open"
  int VideoFile::open(const string& fn, ios_base::openmode mode)
  {
    if (reader || writer)
      {
        Message(FNAME, M_ALREADY_OPEN, WRONG_STATE);
        return WRONG_STATE;
      }

    if (mode == ios_base::in)
      {
        try
          {
            reader = new VideoReader(fn);
          }
        catch (int error)
          {
            switch (error)
              {
              case 1:
                Message(FNAME, M_FILE_OPEN, fn, WRONG_FILE);
                break;
              case 2:
              case 3:
                Message(FNAME, M_WRONG_FILETYPE, fn, WRONG_FILE);
                break;
              case 4:
              case 5:
                Message(FNAME, M_UNSUPPORTED_FILE, fn, WRONG_FILE);
                break;
              default:
                Message(FNAME, M_FILE_OPEN, fn, WRONG_FILE);
              }

            reader = nullptr;
            return ERROR;
          }

        return OK;
      }
    else
      {
        try
          {
            writer = new VideoWriter(fn);
          }

        catch (int error)
          {
            switch (error)
              {
              case 1:
              case 20:
              case 21:
                Message(FNAME, M_UNSUPPORTED_FILE, fn, WRONG_FILE);
                break;
              default:
                Message(FNAME, M_FILE_OPEN, fn, WRONG_FILE);
              }

            reader = nullptr;
          }

        return OK;
      }

    return ERROR;
  }
#undef FNAME

#define FNAME "VideoFile::read"
  bool VideoFile::read(const Image& img, int ch)
  {
    if (!reader)
      {
        Message(FNAME, M_NOT_OPEN, WRONG_STATE);
        return false;
      }

    Image r = NewImg(img->xsize, img->ysize, img->maxval);
    Image g = NewImg(img->xsize, img->ysize, img->maxval);
    Image b = NewImg(img->xsize, img->ysize, img->maxval);

    bool ok = false;
    ReturnIfFailed(ok = reader->read(r, g, b), false);

    if (!ok)
      return false;

    IPoint ip;
    for (ip.y = 0; ip.y < img->ysize; ip.y++)
      for (ip.x = 0; ip.x < img->xsize; ip.x++)
        {
          int rv = GetVal(r, ip);
          int gv = GetVal(g, ip);
          int bv = GetVal(b, ip);

          switch (ch)
            {
            case 0:
              PutVal(img, ip, rv);
              break;
            case 1:
              PutVal(img, ip, gv);
              break;
            case 2:
              PutVal(img, ip, bv);
              break;
            case 3:
              PutVal(img, ip, GRAYVALUE(rv, gv, bv));
              break;
            }
        }

    return true;
  }

  bool VideoFile::read(const Image& imgr, const Image& imgg, const Image& imgb)
  {
    if (!reader)
      {
        Message(FNAME, M_NOT_OPEN, WRONG_STATE);
        return false;
      }

    bool ok = false;
    ReturnIfFailed(ok = reader->read(imgr, imgg, imgb), false);

    if (!ok)
      return false;

    return true;
  }

  bool VideoFile::read()
  {
    if (!reader)
      {
        Message(FNAME, M_NOT_OPEN, WRONG_STATE);
        return false;
      }

    bool ok = false;
    ReturnIfFailed(ok = reader -> read(), false);

    if (!ok)
      return false;

    return true;
  }
#undef FNAME

#define FNAME "VideoFile::getPara"
  void VideoFile::getPara(int& xsize, int& ysize, int& maxval, int& fpsp) const
  {
    if (reader)
      reader->getPara(xsize, ysize, maxval, fpsp);
    else if (writer)
      writer->getPara(xsize, ysize, maxval, fpsp);
    else
      {
        Message(FNAME, M_NOT_OPEN, WRONG_STATE);
      }
  }
#undef FNAME
#define FNAME "VideoFile::set"
  void VideoFile::setPara(int xsize, int ysize,
                          int maxval, int fpsp, int brate)
  {
    if (reader)
      {
        // ignore. Paremeters of source used
      }
    else if (writer)
      {
        writer->setPara(xsize, ysize, maxval, fpsp, brate);
      }
    else
      {
        Message(FNAME, M_NOT_OPEN, WRONG_STATE);
      }
  }
#undef FNAME
#define FNAME "VideoFile::write"
  bool VideoFile::write(const Image& ir, const Image& ig, const Image& ib)
  {
    if (!writer)
      {
        Message(FNAME, M_NOT_OPEN, WRONG_STATE);
        return false;
      }

    try
      {
        return writer->write(ir, ig, ib);
      }
    catch (int error)
      {
        switch (error)
          {
          case 30:
            Message(FNAME, "Cannot convert color space", error);
            break;
          case 31:
            Message(FNAME, "Cannot encode video", error);
            break;
          case 32:
            Message(FNAME, "Cannot write video", error);
            break;
          }
        return false;
      }
  }
#undef FNAME
#define FNAME "VideoFile::FrameNumber"
  int VideoFile::FrameNumber() const
  {
    if (writer)
      return writer->FrameNumber();
    else if (reader)
      return reader->FrameNumber();

    return -1; // "gentle" error - not open
  }
}
#undef FNAME

