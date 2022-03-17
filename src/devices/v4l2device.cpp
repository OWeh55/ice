#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <assert.h>

#include <cmath>

#include <getopt.h> /* getopt_long() */

#include <fcntl.h> /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h> /* for videodev2.h */

#include <linux/videodev2.h>

#include <string>
#include <vector>
#include <iostream>

#include "base.h"
#include "ColorValue.h"
#include "buffer.h"
#include "v4l2device.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

namespace ice
{
  void errno_exit(const std::string& s)
  {
    std::cerr << s << " error " << errno << " : " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  int xioctl(int fd, int request, void* arg)
  {
    int r;

    do
      {
        r = ioctl(fd, request, arg);
      }
    while (-1 == r && EINTR == errno);

    return r;
  }

  V4L2Device::V4L2Device(const std::string& n, int iom):
    devName(n), io(iom), state(invalid)
  {
    // open device
    struct stat st;

    if (-1 == stat(devName.c_str(), &st))
      {
        std::cerr << "Cannot identify '" << devName << "': " << errno;
        std::cerr << ", " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
      }

    if (!S_ISCHR(st.st_mode))
      {
        std::cerr << devName << " is no device" << std::endl;
        exit(EXIT_FAILURE);
      }

    fd = open(devName.c_str(), O_RDWR | O_NONBLOCK, 0);

    if (-1 == fd)
      {
        std::cerr << "Cannot open '" << devName << "': " << errno;
        std::cerr << ", " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
      }

    struct v4l2_capability cap;
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap))
      {
        if (EINVAL == errno)
          {
            std::cerr << devName << " is no V4L2 device" << std::endl;
            exit(EXIT_FAILURE);
          }
        else
          {
            errno_exit("VIDIOC_QUERYCAP");
          }
      }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
      {
        std::cerr << devName << " is no video capture device" << std::endl;
        exit(EXIT_FAILURE);
      }

    switch (io)
      {
      case IO_METHOD_READ:
        if (!(cap.capabilities & V4L2_CAP_READWRITE))
          {
            std::cerr << devName << " does not support read i/o" << std::endl;
            exit(EXIT_FAILURE);
          }

        break;

      case IO_METHOD_MMAP:
      case IO_METHOD_USERPTR:
        if (!(cap.capabilities & V4L2_CAP_STREAMING))
          {
            std::cerr << devName << " does not support streaming i/o" << std::endl;
            exit(EXIT_FAILURE);
          }
        break;

      case IO_METHOD_AUTO:
        if (cap.capabilities & V4L2_CAP_STREAMING)
          {
            io = IO_METHOD_MMAP;
          }
        else if (cap.capabilities & V4L2_CAP_READWRITE)
          {
            io = IO_METHOD_READ;
          }
        else
          {
            std::cerr << devName << " no i/o method found" << std::endl;
            exit(EXIT_FAILURE);
          }
        break;
      }

    // not sure, if this should be done:
    //    cropping is not used and errors are ignored
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap))
      {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect; /* reset to default */

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop))
          {
            switch (errno)
              {
              case EINVAL:
                /* Cropping not supported. */
                break;
              default:
                /* Errors ignored. */
                break;
              }
          }
      }
    else
      {
        /* Errors ignored. */
      }

    // collect all modes
    struct v4l2_fmtdesc fmt;
    CLEAR(fmt);

    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    Mode mode;

    // all pixel formats
    while (xioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0)
      {
        // only supported formats
        if (fmt.pixelformat == V4L2_PIX_FMT_RGB24 ||
            fmt.pixelformat == V4L2_PIX_FMT_YUYV)
          {
            mode.fmt = fmt.pixelformat;
            struct v4l2_frmsizeenum fs;
            CLEAR(fs);
            fs.index = 0;
            fs.pixel_format = mode.fmt;
            // all framesizes
            while ((xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fs)) == 0)
              {
                if (fs.type == V4L2_FRMSIZE_TYPE_DISCRETE)
                  {
                    mode.width = fs.discrete.width;
                    mode.height = fs.discrete.height;
                  }
                else
                  {
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
                    // use only max values for the moment
                    // no device to test this
                    mode.width = fs.stepwise.max_width;
                    mode.height = fs.stepwise.max_height;
                  }

                v4l2_frmivalenum fr;
                CLEAR(fr);
                fr.index = 0;
                fr.pixel_format = mode.fmt;
                fr.width = mode.width;
                fr.height = mode.height;
                // all framerates
                while ((xioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fr)) == 0)
                  {
                    if (fr.type == V4L2_FRMIVAL_TYPE_DISCRETE)
                      {
                        mode.framerate = fr.discrete;
                        modes.push_back(mode);
                      }
                    else
                      {
                        // only use max framerate = min timeperframe
                        // no device to test this
                        mode.framerate = fr.stepwise.min;
                        modes.push_back(mode);
                      }
                    ++fr.index;
                  }
                ++fs.index;
              }
          }
        fmt.index++;
      }

    if (modes.empty())
      {
        std::cerr << "No supported modes found" << std::endl;
      }

    int maxsize = 0;
    int mwidth = 0;
    int mheight = 0;
    for (unsigned int i = 0; i < modes.size(); ++i)
      {
        int isize = modes[i].width * modes[i].height;
        if (isize > maxsize)
          {
            mwidth = modes[i].width;
            mheight = modes[i].height;
          }
      }
    setFormat(mwidth, mheight);
  }

  //  bool V4L2Device::setMode(const Mode &mode)
  bool V4L2Device::setMode(Mode mode)
  {
    if (state == streaming)
      {
        return false;
      }

    free_buffers(); // remove previously set up buffers

    if (state == setUp && io == IO_METHOD_MMAP)
      {
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = 0;
        req.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory  = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
          {
            if (EINVAL == errno)
              {
                std::cerr << devName << " does not support memory mapping i/o" << std::endl;
                exit(EXIT_FAILURE);
              }
            else
              {
                errno_exit("VIDIOC_REQBUFS(0)");
              }
          }

      }

    // set mode
    struct v4l2_format fmt;
    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = mode.width;
    fmt.fmt.pix.height = mode.height;
    fmt.fmt.pix.pixelformat = mode.fmt;

    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
      {
        errno_exit("VIDIOC_S_FMT");
      }

    if (mode.fmt != V4L2_PIX_FMT_YUYV &&
        mode.fmt != V4L2_PIX_FMT_RGB24)
      {
        std::cerr << "Requested pixel format not supported." << std::endl;
        exit(EXIT_FAILURE);
      }

    if ((int)fmt.fmt.pix.height != mode.height ||
        (int)fmt.fmt.pix.width != mode.width)
      {
        std::cerr << "size " << mode.width << " x " << mode.height << " not supported, ";
        std::cerr << "you may use " << fmt.fmt.pix.width << " x " << fmt.fmt.pix.height << std::endl;
        exit(EXIT_FAILURE);
      }

    v4l2_streamparm streampara;
    CLEAR(streampara);
    streampara.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    streampara.parm.capture.timeperframe = mode.framerate;

    if (xioctl(fd, VIDIOC_S_PARM, &streampara) != 0)
      {
        errno_exit("VIDIOC_S_PARM");
      }

    currentMode = mode;

    // prepare buffers
    linelength =  fmt.fmt.pix.bytesperline;
    bufferSize = fmt.fmt.pix.sizeimage;

    switch (io)
      {
      case IO_METHOD_READ:
        //      std::cout << "prepare_buffers_read" << std::endl;
        prepare_buffers_read(bufferSize);
        break;

      case IO_METHOD_MMAP:
        //      std::cout << "prepare_buffers_mmap" << std::endl;
        prepare_buffers_mmap();
        break;

      case IO_METHOD_USERPTR:
        //      std::cout << "prepare_buffers_userp" << std::endl;
        prepare_buffers_userp(bufferSize);
        break;
      }
    state = setUp;
    return true;
  }

  bool V4L2Device::setFormat(int width, int height, double frate)
  {
    // try to find best mode:
    // mode.width>=width
    // mode.height>=height
    // mode.width*mode.height -> min
    // | mode.framerate - frate | -> min
    std::vector<Mode> possibleModes;
    for (unsigned int i = 0; i < modes.size(); ++i)
      {
        if (modes[i].width >= width && modes[i].height >= height)
          {
            possibleModes.push_back(modes[i]);
          }
      }

    if (possibleModes.empty())
      {
        return false;
      }

    int minsize = possibleModes[0].width * possibleModes[0].height;
    for (unsigned int i = 1; i < possibleModes.size(); ++i)
      {
        int isize = possibleModes[i].width * possibleModes[i].height;
        if (isize < minsize)
          {
            minsize = isize;
          }
      }

    std::vector<Mode> bestModes;
    for (unsigned int i = 0; i < possibleModes.size(); ++i)
      {
        int isize = possibleModes[i].width * possibleModes[i].height;
        if (isize == minsize)
          {
            bestModes.push_back(possibleModes[i]);
          }
      }

    double fratediff = fabs(frate - bestModes[0].frameRate());
    int minidx = 0;
    for (unsigned int i = 1; i < bestModes.size(); ++i)
      {
        double adiff = fabs(frate - bestModes[i].frameRate());
        if (adiff < fratediff)
          {
            minidx = i;
            fratediff = adiff;
          }
      }

    return setMode(bestModes[minidx]);
  }

  V4L2Device::~V4L2Device()
  {
    // destructor of buffer destroys buffer
    if (-1 == close(fd))
      {
        errno_exit("close");
      }
  }

  int V4L2Device::getFd() const
  {
    return fd;
  }

  /*static*/ int V4L2Device::clamp8(int x)
  {
    if (x < 0)
      {
        return 0;
      }
    if (x > 255)
      {
        return 255;
      }
    return x;
    //    return (x < 0) ? 0 : ((x > 255) ? 255 : x);
  }

  /*static*/ void V4L2Device::yuyv2rgb(const yuyv8& s,
                                       rgb8& d1, rgb8& d2)
  {
    int v = s.v - 128;
    int u = s.u - 128;

    int vr = v * 701 / 500;
    int ug = - u * 138 / 401;
    int vg = -v * 697 / 976;
    int ub = u * 342 / 193;

    d1.r = clamp8(s.y1 + vr);
    d1.g = clamp8(s.y1 + ug + vg);
    d1.b = clamp8(s.y1 + ub);
    d2.r = clamp8(s.y2 + vr);
    d2.g = clamp8(s.y2 + ug + vg);
    d2.b = clamp8(s.y2 + ub);
  }

#if 0
  void V4L2Device::process_image(const void* p, unsigned char* buffer)
  {
    // if buffer==NULL ignore data
    if (buffer != NULL)
      {
        const unsigned char* ptr = (const unsigned char*)p;
        unsigned char* pos = buffer;
        if (currentMode.fmt ==  V4L2_PIX_FMT_YUYV)
          {

            yuyv8 s;
            rgb8 d1;
            rgb8 d2;

            for (int y = 0; y < currentMode.height; y++)
              for (int x = 0; x < currentMode.width; x += 2)
                {
                  s.y1 = *(ptr++);// - 16;
                  s.u = *(ptr++);
                  s.y2 = *(ptr++);// - 16;
                  s.v = *(ptr++);

                  yuyv2rgb(s, d1, d2);

                  *(pos++) = d1.r;
                  *(pos++) = d1.g;
                  *(pos++) = d1.b;

                  *(pos++) = d2.r;
                  *(pos++) = d2.g;
                  *(pos++) = d2.b;
                }
          }
        else if (currentMode.fmt ==  V4L2_PIX_FMT_RGB24)
          {

            for (int y = 0; y < currentMode.height; y++)
              for (int x = 0; x < currentMode.width; x++)
                {
                  *(pos++) = *(ptr++);
                  *(pos++) = *(ptr++);
                  *(pos++) = *(ptr++);
                }
          }
        else
          {
            std::cerr << "process_image: pixel format not supported" << std::endl;
          }
      }
  }

  int V4L2Device::read_frame_method_read(unsigned char* buffer)
  {
    if (-1 == read(fd, buffers[0].getStart(), buffers[0].getLength()))
      {
        switch (errno)
          {
          case EAGAIN:
            return 0;

          case EIO:
          /* Could ignore EIO, see spec. */
          /* fall through */
          default:
            errno_exit("read");
          }
      }

    process_image(buffers[0].getStart(), buffer);
    return 1;
  }

  int V4L2Device::read_frame_method_mmap(unsigned char* buffer)
  {
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))   // get filled buffer from driver
      {
        switch (errno)
          {
          case EAGAIN:
            return 0;

          case EIO:
          /* Could ignore EIO, see spec. */
          /* fall through */

          default:
            errno_exit("VIDIOC_DQBUF");
          }
      }

    assert(buf.index < buffers.size());

    process_image(buffers[buf.index].getStart(), buffer);

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))   // enqueue buffer again
      {
        errno_exit("VIDIOC_QBUF");
      }
    return 1;
  }

  int V4L2Device::read_frame_method_userptr(unsigned char* buffer)
  {
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
      {
        switch (errno)
          {
          case EAGAIN:
            return 0;

          case EIO:
          /* Could ignore EIO, see spec. */
          /* fall through */
          default:
            errno_exit("VIDIOC_DQBUF");
          }
      }

    unsigned int i;
    for (i = 0; i < buffers.size(); ++i)
      if (buf.m.userptr == (unsigned long) buffers[i].getStart()
          && buf.length == buffers[i].getLength())
        {
          break;
        }

    assert(i < buffers.size());

    process_image((void*) buf.m.userptr, buffer);

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
      {
        errno_exit("VIDIOC_QBUF");
      }
    return 1;
  }

  int V4L2Device::read_frame(unsigned char* buffer)
  {
    switch (io)
      {
      case IO_METHOD_READ:
        return read_frame_method_read(buffer);
        break;

      case IO_METHOD_MMAP:
        return read_frame_method_mmap(buffer);
        break;

      case IO_METHOD_USERPTR:
        return read_frame_method_userptr(buffer);
        break;
      }

    return 1;
  }

  void V4L2Device::getImg(unsigned char* buffer)
  {
    if (state != streaming)
      {
        std::cerr << "streaming not started" << std::endl;
        return;
      }
    bool frame_read = false;
    while (!frame_read)   // select() loop
      {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = ::select(fd + 1, &fds, NULL, NULL, &tv);

        if ((r == -1) && (EINTR != errno))
          {
            errno_exit("select");
          }

        if (r == 0)
          {
            std::cerr << "select timeout" << std::endl;
            exit(EXIT_FAILURE);
          }

        frame_read = read_frame(buffer);
      }
  }
#endif
  void V4L2Device::process_image(const void* p, const Image& imgr, const Image& imgg, const Image& imgb)
  {
    ColorValue cv2;
    const unsigned char* ptr = (const unsigned char*)p;

    yuyv8 s;
    rgb8 d1;
    rgb8 d2;

    if (imgr->ImageType() == 1 &&
        imgg->ImageType() == 1 &&
        imgb->ImageType() == 1)
      {
        PixelType1** datar = (PixelType1**)imgr->getDataPtr();
        PixelType1** datag = (PixelType1**)imgg->getDataPtr();
        PixelType1** datab = (PixelType1**)imgb->getDataPtr();

        for (int y = 0; y < imgr->ysize; y++)
          {
            ptr = (const unsigned char*)p + linelength * y;
            for (int x = 0; x < imgr->xsize; x += 2)
              {
                s.y1 = *(ptr++);// - 16;
                s.u  = *(ptr++);
                s.y2 = *(ptr++);// - 16;
                s.v  = *(ptr++);

                yuyv2rgb(s, d1, d2);

                datar[y][x] = 255 - d1.r;
                datag[y][x] = 255 - d1.g;
                datab[y][x] = 255 - d1.b;

                datar[y][x + 1] = 255 - d2.r;
                datag[y][x + 1] = 255 - d2.g;
                datab[y][x + 1] = 255 - d2.b;
              }
          }
      }
    else
      {
        // cout << "Warning: using slow pixel access - check system" << endl;
        for (int y = 0; y < imgr->ysize; y++)
          {
            ptr = (const unsigned char*)p + linelength * y;
            for (int x = 0; x < imgr->xsize; x += 2)
              {
                s.y1 = *(ptr++);// - 16;
                s.u  = *(ptr++);
                s.y2 = *(ptr++);// - 16;
                s.v  = *(ptr++);

                yuyv2rgb(s, d1, d2);

                PutValUnchecked(imgr, x, y, 255 - d1.r);
                PutValUnchecked(imgg, x, y, 255 - d1.g);
                PutValUnchecked(imgb, x, y, 255 - d1.b);

                PutValUnchecked(imgr, x + 1, y, 255 - d2.r);
                PutValUnchecked(imgg, x + 1, y, 255 - d2.g);
                PutValUnchecked(imgb, x + 1, y, 255 - d2.b);
                ptr += 4;
              }
          }
      }
  }

  int V4L2Device::read_frame_method_read(const Image& imgr,
                                         const Image& imgg,
                                         const Image& imgb)
  {
    if (-1 == read(fd, buffers[0].getStart(), buffers[0].getLength()))
      {
        switch (errno)
          {
          case EAGAIN:
            return 0;

          case EIO:
          /* Could ignore EIO, see spec. */
          /* fall through */
          default:
            errno_exit("read");
          }
      }

    process_image(buffers[0].getStart(), imgr, imgg, imgb);
    return 1;
  }

  int V4L2Device::drop_frames_method_read()
  {
    while (-1 != read(fd, buffers[0].getStart(), buffers[0].getLength()))
      {
        /* read until error because of empty buffer*/
      }
    return 1;
  }

  int V4L2Device::read_frame_method_mmap(const Image& imgr,
                                         const Image& imgg,
                                         const Image& imgb)
  {
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
      {
        switch (errno)
          {
          case EAGAIN:
            return 0;

          case EIO:
          /* Could ignore EIO, see spec. */

          /* fall through */

          default:
            errno_exit("VIDIOC_DQBUF");
          }
      }

    assert(buf.index < buffers.size());

    process_image(buffers[buf.index].getStart(), imgr, imgg, imgb);
    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
      {
        errno_exit("VIDIOC_QBUF");
      }
    return 1;
  }

  int V4L2Device::drop_frames_method_mmap()
  {
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // get filled buffers and insert as new again
    while (-1 != xioctl(fd, VIDIOC_DQBUF, &buf))
      {
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
          {
            errno_exit("VIDIOC_QBUF");
          }
      }
    return 1;
  }

  int V4L2Device::read_frame_method_userptr(const Image& imgr,
      const Image& imgg,
      const Image& imgb)
  {
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
      {
        switch (errno)
          {
          case EAGAIN:
            return 0;

          case EIO:
          /* Could ignore EIO, see spec. */
          /* fall through */
          default:
            errno_exit("VIDIOC_DQBUF");
          }
      }

    unsigned int i;
    for (i = 0; i < buffers.size(); ++i)
      if (buf.m.userptr == (unsigned long) buffers[i].getStart()
          && buf.length == buffers[i].getLength())
        {
          break;
        }

    assert(i < buffers.size());

    process_image((void*) buf.m.userptr, imgr, imgg, imgb);

    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
      {
        errno_exit("VIDIOC_QBUF");
      }
    return 1;
  }

  int V4L2Device::drop_frames_method_userptr()
  {
    struct v4l2_buffer buf;
    CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
      {
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
          {
            errno_exit("VIDIOC_QBUF");
          }
      }
    return 1;
  }

  int V4L2Device::read_frame(const Image& imgr, const Image& imgg, const Image& imgb)
  {
    switch (io)
      {
      case IO_METHOD_READ:
        return read_frame_method_read(imgr, imgg, imgb);
        break;

      case IO_METHOD_MMAP:
        return read_frame_method_mmap(imgr, imgg, imgb);
        break;

      case IO_METHOD_USERPTR:
        return read_frame_method_userptr(imgr, imgg, imgb);
        break;
      }

    return 1;
  }

  void V4L2Device::dropAllImages()
  {
    // flush buffers
    // next getImg reads new image
    switch (io)
      {
      case IO_METHOD_READ:
        drop_frames_method_read();
        break;

      case IO_METHOD_MMAP:
        drop_frames_method_mmap();
        break;

      case IO_METHOD_USERPTR:
        drop_frames_method_userptr();
        break;
      }
  }

  void V4L2Device::getImg(const Image& imgr, const Image& imgg, const Image& imgb)
  {
    if (state != streaming)
      {
        std::cerr << "streaming not started" << std::endl;
        return;
      }
    bool frame_read = false;
    while (!frame_read)   // select loop
      {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = ::select(fd + 1, &fds, nullptr, nullptr, &tv);

        if ((r == -1) && (EINTR != errno))
          {
            errno_exit("select");
          }

        if (r == 0)
          {
            std::cerr << "select timeout" << std::endl;
            exit(EXIT_FAILURE);
          }

        frame_read = read_frame(imgr, imgg, imgb);
      }
  }

  void V4L2Device::stopCapturing()
  {
    enum v4l2_buf_type type;

    switch (io)
      {
      case IO_METHOD_READ:
        /* Nothing to do. */
        break;

      case IO_METHOD_MMAP:
      case IO_METHOD_USERPTR:
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
          {
            errno_exit("VIDIOC_STREAMOFF");
          }
        break;
      }
    state = setUp;
  }

  void V4L2Device::startCapturing()
  {
    if (state != setUp)
      {
        return;
      }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    switch (io)
      {
      case IO_METHOD_READ:
        /* Nothing to do. */
        state = streaming;
        break;

      case IO_METHOD_MMAP:
        for (unsigned int i = 0; i < buffers.size(); ++i)
          {
            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
              {
                errno_exit("VIDIOC_QBUF");
              }
          }

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
          {
            errno_exit("VIDIOC_STREAMON");
          }
        state = streaming;
        break;

      case IO_METHOD_USERPTR:
        for (unsigned int i = 0; i < buffers.size(); ++i)
          {
            struct v4l2_buffer buf;

            CLEAR(buf);

            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            buf.index = i;
            buf.m.userptr = (unsigned long) buffers[i].getStart();
            buf.length = buffers[i].getLength();

            if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
              {
                errno_exit("VIDIOC_QBUF");
              }
          }

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
          {
            errno_exit("VIDIOC_STREAMON");
          }
        state = streaming;
        break;
      default:
        std::cerr << "WRONG IO MODE" << std::endl;
      }
  }

  void V4L2Device::prepare_buffers_read(unsigned int buffer_size)
  {
    buffers.clear();
    buffers.resize(1);

    buffers[0] = buffer(buffer_size);

    if (!buffers[0].getStart())
      {
        std::cerr << "Out of memory" << std::endl;
        exit(EXIT_FAILURE);
      }
  }

  void V4L2Device::prepare_buffers_mmap()
  {
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory  = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
      {
        if (EINVAL == errno)
          {
            std::cerr << devName << " does not support memory mapping i/o" << std::endl;
            exit(EXIT_FAILURE);
          }
        else
          {
            errno_exit("VIDIOC_REQBUFS");
          }
      }

    if (req.count < 2)
      {
        std::cerr << "Insufficient buffer memory on " << devName << std::endl;
        exit(EXIT_FAILURE);
      }

    buffers.resize(req.count);

    for (unsigned int bufferNr = 0; bufferNr < req.count; ++bufferNr)
      {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = bufferNr;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
          {
            errno_exit("VIDIOC_QUERYBUF");
          }

        // buffers[bufferNr] = buffer(buf.length,
        //                            mmap(nullptr /* start anywhere */,
        //                                 buf.length,
        //                                 PROT_READ | PROT_WRITE /* required */,
        //                                 MAP_SHARED /* recommended */,
        //                                 fd, buf.m.offset),
        //         false);

        buffers[bufferNr] = buffer(buf.length, fd, buf.m.offset);

        if (MAP_FAILED == buffers[bufferNr].getStart())
          {
            errno_exit("mmap");
          }
      }
  }

  void V4L2Device::free_buffers()
  {
    for (unsigned int i = 0; i < buffers.size(); ++i)
      {
        buffers[i].destroy();
      }
    buffers.clear();
  }

  void V4L2Device::prepare_buffers_userp(unsigned int buffer_size)
  {
    struct v4l2_requestbuffers req;
    unsigned int page_size = getpagesize();

    buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);

    CLEAR(req);

    req.count  = 4;
    req.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory  = V4L2_MEMORY_USERPTR;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
      {
        if (EINVAL == errno)
          {
            std::cerr << devName << " does not support user pointer i/o" << std::endl;
            exit(EXIT_FAILURE);
          }
        else
          {
            errno_exit("VIDIOC_REQBUFS");
          }
      }

    buffers.clear();
    buffers.resize(4);

    for (unsigned int n_buffers = 0; n_buffers < 4; ++n_buffers)
      {
        buffers[n_buffers] = buffer(buffer_size,
                                    memalign(page_size, buffer_size),
                                    true);

        if (!buffers[n_buffers].getStart())
          {
            std::cerr << "Out of memory" << std::endl;
            exit(EXIT_FAILURE);
          }
      }
  }

  void V4L2Device::printControls() const
  {
    struct v4l2_queryctrl qctrl;
    qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &qctrl))
      {
        /* ... */
        // V4L2_CID_CAMERA_CLASS
        std::cout << qctrl.id << ":" << qctrl.name;
        std::cout << " " << qctrl.minimum << " .. " << qctrl.maximum;
        std::cout << " (" << qctrl.step << ") " << std::endl;
        qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
      }
  }

  bool V4L2Device::checkControl(int id)
  {
    struct v4l2_queryctrl queryctrl;
    memset(&queryctrl, 0, sizeof(queryctrl));
    queryctrl.id = id;

    if (-1 == xioctl(fd, VIDIOC_QUERYCTRL, &queryctrl))
      {
        if (errno != EINVAL)
          {
            perror("VIDIOC_QUERYCTRL");
          }
        return false;
      }

    return !(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED);
  }

  bool V4L2Device::setControl(int id, int value)
  {
    struct v4l2_ext_control control;
    memset(&control, 0, sizeof(control));
    control.id = id;
    control.value = value;
    if (-1 == xioctl(fd, VIDIOC_S_CTRL, &control))
      {
        std::cerr << id << ": " << value << std::endl;
        perror("VIDIOC_S_CTRL");
        return false;
      }
    return true;
  }

  int V4L2Device::getControl(int id) const
  {
    struct v4l2_ext_controls ctrls = {0};
    struct v4l2_ext_control ctrl = {0};
    ctrl.id = id;

    ctrls.count = 1;
    ctrls.controls = &ctrl;

    int ret = xioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls);
    if (ret)
      {
        std::cerr << "control id: " << id << " failed to get value";
        std::cerr << "error " << ret << ")" << std::endl;
        return -1;
      }
    else
      {
        return ctrl.value;
      }
  }

  bool V4L2Device::setAutoFocus(int value)
  {
    return checkControl(V4L2_CID_FOCUS_AUTO) &&
           setControl(V4L2_CID_FOCUS_AUTO, value);
  }

  bool V4L2Device::setAutoExposure(int value)
  {
    return checkControl(V4L2_CID_EXPOSURE_AUTO) &&
           setControl(V4L2_CID_EXPOSURE_AUTO, value);
  }

  int V4L2Device::getAutoFocus() const
  {
    return getControl(V4L2_CID_FOCUS_AUTO);
  }

  bool V4L2Device::setFocus(int value)
  {
    return checkControl(V4L2_CID_FOCUS_ABSOLUTE) &&
           setControl(V4L2_CID_FOCUS_ABSOLUTE, value);
  }

  bool V4L2Device::setExposure(int value)
  {
    return checkControl(V4L2_CID_EXPOSURE_ABSOLUTE) &&
           setControl(V4L2_CID_EXPOSURE_ABSOLUTE, value);
  }

  int V4L2Device::getFocus() const
  {
    return getControl(V4L2_CID_FOCUS_ABSOLUTE);
  }

  int V4L2Device::getExposure() const
  {
    return getControl(V4L2_CID_FOCUS_ABSOLUTE);
  }
}
