#ifndef NUMBASE_H
#define NUMBASE_H
#include <cmath>
#include <algorithm>

inline float Sin(float phi)
{
  return sin(phi * M_PI / 180);
}

inline float Cos(float phi)
{
  return cos(phi * M_PI / 180);
}

void CSInit(int n, float& sin, float& cos, float& dsin, float& dcos);
inline void CSReInit(float& sin, float& cos)
{
  sin = 0;
  cos = 1;
}
inline void CSNext(float& sin, float& cos, float dsin, float dcos)
{
  float h = sin * dcos + cos * dsin;
  cos = cos * dcos - sin * dsin;
  sin = h;
}

inline float Max(float p1, float p2)
{
  return std::max(p1, p2);
}

inline float Min(float p1, float p2)
{
  return std::min(p1, p2);
}
#endif
