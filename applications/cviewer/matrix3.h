#ifndef _MATRIX_H
#define _MATRIX_H

#include "vector3.h"

class matrix3
{
  // private:
public:
  float m[9];
public:
  matrix3()
  {
    clear();
  }

  void clear()
  {
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 1.0f;
    m[5] = 0.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 1.0f;
  }

  float *operator[](int i)
  {
    return &m[i * 3];
  }
  const float *operator[](int i) const
  {
    return &m[i * 3];
  }

  friend void mul(const matrix3 &m1, const matrix3 &m2, matrix3 &m);
  friend void mul(const matrix3 &m, const vector3 &s1, vector3 &s2);
};
#endif
