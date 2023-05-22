#ifndef BINFILE_H
#define BINFILE_H

#include <fstream>

template<typename T>
void read(std::istream& is, T& var)
{
  char buf[sizeof(T)];
  for (unsigned int i = 0; i < sizeof(T); i++)
    buf[i] = is.get();

  var = *(T*)buf;
}

template<typename T>
T read(std::istream& is)
{
  unsigned char buf[sizeof(T)];
  for (unsigned int i = 0; i < sizeof(T); i++)
    {
      buf[i] = is.get();
      // std::cout << int(buf[i]) << " ";
    }
  // std::cout << std::endl;
  return *(T*)buf;
}

template<typename T>
void write(std::ostream& is, const T& var)
{
  char buf[sizeof(T)];
  *(T*)buf = var;
  for (unsigned int i = 0; i < sizeof(T); i++)
    is.put(buf[i]);


}
#endif
