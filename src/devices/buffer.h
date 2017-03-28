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
      if (owner) // only if owner
        {
          if (mapped)  // unmap or delete
            {
              munmap(start, length);
            }
          else
            delete [] start;
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
