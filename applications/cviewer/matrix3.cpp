#include "matrix3.h"

void mul(const matrix3& m1, const matrix3& m2, matrix3& m)
{
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      {
        float sum = 0;
        for (int k = 0; k < 3; k++)
          sum += m1.m[i * 3 + k] * m2.m[k * 3 + j];
        m.m[i * 3 + j] = sum;
      }
}

void mul(const matrix3& m, const vector3& s1, vector3& s2)
{
  for (int i = 0; i < 3; i++)
    {
      float sum = 0;
      for (int j = 0; j < 3; j++)
        {
          sum += m.m[i * 3 + j] * s1.v[j];
        }
      s2.v[i] = sum;
    }
}
