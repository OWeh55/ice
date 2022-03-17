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
#ifndef V4L2_DEVICE_H
#define V4L2_DEVICE_H

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <assert.h>

#include <cstdint>

#include <linux/videodev2.h>

#include <string>
#include <vector>
#include <iostream>

#include "base.h"
#include "buffer.h"

namespace ice
{
  class V4L2Device
  {
  public:
    class Mode
    {
    public:
      uint32_t fmt;
      int width;
      int height;
      v4l2_fract framerate;

      Mode() {}

      Mode(uint32_t fmt, int width, int height, v4l2_fract framerate):
        fmt(fmt), width(width), height(height), framerate(framerate) {}

      std::string formatString() const
      {
        std::string fs = "1234";
        fs[0] = fmt & 0xFF;
        fs[1] = (fmt >> 8) & 0xFF;
        fs[2] = (fmt >> 16) & 0xFF;
        fs[3] = (fmt >> 24) & 0xFF;
        return fs;
      }

      void printFrameRate() const
      {
        if (framerate.numerator == 1)
          {
            std::cout << framerate.denominator;
          }
        else
          {
            std::cout << framerate.denominator << "/" << framerate.numerator;
          }
      }

      std::string modeString() const
      {
        return formatString() + " " + std::to_string(width)
               +  " x " + std::to_string(height);
      }

      void print() const
      {
        std::cout << modeString() << "  ";
        printFrameRate();
      }

      double frameRate() const
      {
        return static_cast<double>(framerate.denominator) / framerate.numerator;
      }
    };

    static const int IO_METHOD_MMAP = 0;
    static const int IO_METHOD_READ = 1;
    static const int IO_METHOD_USERPTR = 2;
    static const int IO_METHOD_AUTO = 255;

    V4L2Device(const std::string& n, int iom = IO_METHOD_AUTO);

    ~V4L2Device();

    void getImg(const Image& imgr, const Image& imgg, const Image& imgb);
    void dropAllImages();

    std::vector<Mode> getModes() const
    {
      return modes;
    }

    bool setMode(Mode mode);
    bool setFormat(int width, int height, double frate = 1e16);

    Mode getCurrentMode() const
    {
      return currentMode;
    }

    void stopCapturing();
    void startCapturing();

    void printControls() const;

  private:
    int getFd() const;

    struct yuyv8
    {
      unsigned char y1;
      unsigned char u;
      unsigned char y2;
      unsigned char v;
    };

    struct rgb8
    {
      unsigned char r;
      unsigned char g;
      unsigned char b;
    };

    static int clamp8(int x);

    static void yuyv2rgb(const yuyv8& s, rgb8& d1, rgb8& d2);

    void process_image(const void* p,
                       const Image& imgr, const Image& imgg, const Image& imgb);

    int read_frame_method_read(const Image& imgr, const Image& imgg, const Image& imgb);

    int read_frame_method_mmap(const Image& imgr, const Image& imgg, const Image& imgb);

    int read_frame_method_userptr(const Image& imgr, const Image& imgg, const Image& imgb);

    int drop_frames_method_read();

    int drop_frames_method_mmap();

    int drop_frames_method_userptr();

    int read_frame(const Image& imgr, const Image& imgg, const Image& imgb);

    void prepare_buffers_read(unsigned int buffer_size);
    void prepare_buffers_mmap();
    void prepare_buffers_userp(unsigned int buffer_size);
    void free_buffers();

  public:
    // controls
    bool checkControl(int id);

    bool setControl(int id, int value);

    int getControl(int id) const;

    bool setAutoFocus(int value);
    bool setAutoExposure(int value);

    int getAutoFocus() const;

    bool setFocus(int value);
    bool setExposure(int value);

    int getFocus() const;
    int getExposure() const;

  private:
    std::string devName;
    int fd;

    std::vector<Mode> modes;
    int io;

    int bufferSize;
    std::vector<buffer> buffers;

    Mode currentMode;
    int linelength;
    int state;

    //  device state constants
    static const int invalid = 0;
    static const int setUp = 1;
    static const int initialized = 2;
    static const int streaming = 3;
  };
}
#endif
