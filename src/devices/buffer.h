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
#ifndef BUFFER_H
#define BUFFER_H

#include <sys/mman.h>

//#include <algorithm>

namespace ice
{

  class buffer
  {
  public:
    buffer(): owner(false), mapped(false), length(0), start(nullptr) {}

    buffer(size_t buffer_size): owner(true), mapped(false), length(buffer_size)
    {
      start = new char[buffer_size];
    }

    buffer(size_t buffer_size, int fd, off_t offset):
      owner(true), mapped(true), length(buffer_size)
    {
      start = (char*) mmap(nullptr /* start anywhere */,
                           length,
                           PROT_READ | PROT_WRITE,
                           MAP_SHARED,
                           fd, offset
                          );
    }

    buffer(size_t buffer_size, void* ptr, bool owner):
      owner(owner), mapped(false), length(buffer_size), start((char*)ptr) {}

    buffer(const buffer& sec) = default;

    ~buffer() {} // destroy must be called to free memory before deconstruction

    void destroy()
    {
      if (owner)   // only if owner
        {
          if (mapped)   // unmap or delete
            {
              munmap(start, length);
            }
          else
            {
              delete [] start;
            }
        }
    }

    size_t getLength() const
    {
      return length;
    }

    const void* getStart() const
    {
      return start;
    }

    void* getStart()
    {
      return start;
    }

  private:
    bool owner;
    bool mapped;
    size_t length;
    char* start;
  };

}
#endif
