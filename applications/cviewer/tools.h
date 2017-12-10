#ifndef TOOLS_H
#define TOOLS_H

#include <string>

// #define DEBUG

#ifdef DEBUG
#define DEBUGOUT(x) cerr << (x) ;
#else
#define DEBUGOUT(x) ;
#endif

void cerror(const std::string& mesg);

template<class T>
void exchange(T& i1, T& i2)
{
  T h = i1;
  i1 = i2;
  i2 = h;
}

template<class T>
void sort(T& i1, T& i2)
{
  if (i1 > i2) exchange(i1, i2);
}

#endif
