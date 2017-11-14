#include <math.h>

#include "numbase.h"
#if 0
float Sin(float phi)
{
  return sin(phi * M_PI / 180);
}

float Cos(float phi)
{
  return cos(phi * M_PI / 180);
}

float Max(float p1, float p2)
{
  if (p1 > p2) return p1;
  return p2;
}

float Min(float p1, float p2)
{
  if (p1 < p2) return p1;
  return p2;
}
#endif
void CSInit(int n, float &sin, float &cos, float &dsin, float &dcos)
{
  float dfi = 360.0f / n;
  sin = 0.0f;
  cos = 1.0f;
  dsin = Sin(dfi);
  dcos = Cos(dfi);
}
