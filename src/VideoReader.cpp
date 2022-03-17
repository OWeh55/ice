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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <exception>
#include <string>
#include <iostream>

#include "defs.h"
#include "IceException.h"

#include "VideoReader.h"

using namespace std;

namespace ice
{
  constexpr int BUFSIZE = 2000;

  VideoReader::VideoReader(const std::string& fn): framenr(0)
  {
    // look for info first
    try
      {
        // default values
        width = -1;
        height = -1;
        fps = 25;
        maxval = 255;

        string cmd = "ffprobe -v quiet -show_streams -select_streams V \"" + fn + "\"";

        // cout << cmd << endl;
        FILE* fd = popen(cmd.c_str(), "r");
        if (fd != nullptr)
          {
            char buffer[BUFSIZE];
            while (fgets(buffer, BUFSIZE - 1, fd))
              {
                buffer[BUFSIZE - 1] = 0; // force string end here
                int size = strlen(buffer);
                if (buffer[size - 1] == '\n') // substitute linefeed with string end
                  buffer[size - 1] = 0;
                std::string input = buffer;
                size_t pos = input.find('=');
                if (pos != string::npos)  // line contains '='
                  {
                    // split in key and value
                    string key = input.substr(0, pos);
                    string value = input.substr(pos + 1);

                    if (key == "width")
                      {
                        width = stoi(value);
                      }
                    else if (key == "height")
                      {
                        height = stoi(value);
                      }
                    else if (key == "r_frame_rate")
                      {
                        // value is <numerator>/<denominator>
                        pos = value.find("/");
                        if (pos != string::npos)
                          {
                            int z = stoi(value.substr(0, pos));
                            int n = stoi(value.substr(pos + 1));
                            fps = RoundInt(1.0 * z / n);
                          }
                      }
                  }
              }
            pclose(fd);
          }
      }
    catch (exception& ex)
      {
        // we ignore errors in detection of size etc. and open video file nevertheless
      }
    // now really open file
    try
      {
        pbmReader.open("|ffmpeg -i \'" + fn + "\' -y -codec:v ppm -f rawvideo -an -sn -v quiet - ");
      }
    catch (const IceException& ex)
      {
        if (ex.message() == "Empty pbm file")
          throw IceException("VideoReader", "no video file");
        else
          throw IceException("VideoReader", ex.message());
      }
    // if use of ffprobe failed we try getInfo() instead
    if (width < 0 || height < 0)
      {
        int nChannels;
        pbmReader.getInfo(width, height, maxval, nChannels);
      }
  }

  void VideoReader::freeall()
  {
  }

  VideoReader::~VideoReader()
  {
  }

  void VideoReader::getPara(int& xsize, int& ysize, int& maxval, int& fps) const
  {
    xsize = width;
    ysize = height;
    maxval = this->maxval;
    fps = this->fps;
  }
#define FNAME "VideoReader::read"
  bool VideoReader::read(const Image& r, const Image& g, const Image& b)
  {
    try
      {
        pbmReader.getImage(r, g, b);
        pbmReader.nextImage();
        framenr++;
        return true;
      }
    catch (const std::exception& ex)
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
    try
      {
        pbmReader.getImage();
        pbmReader.nextImage();
        framenr++;
        return true;
      }
    catch (const std::exception& ex)
      {
        // read failed (eof..)
        return false;
      }
  }
#undef FNAME
}

// ffmpeg -i xyz.mp4 -y -codec:v ppm -f rawvideo -an -sn -   (color)
// input force-overwrite  videocodec fileformat noaudio nosubtitle to_stdout
