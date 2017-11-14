#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <string>

//using namespace std;

class vector3
{
public:
  float v[3];

public:
  // Konstruktoren
  vector3()
  {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
  }
  vector3(float x, float y, float z)
  {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }

  // Destruktor
  ~vector3() {}

  void clear()
  {
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
  }
  // Elementzugriff

  float X() const
  {
    return v[0];
  }
  float Y() const
  {
    return v[1];
  }
  float Z() const
  {
    return v[2];
  }

  const float &operator[](int i) const
  {
    return v[i];
  }
  float &operator[](int i)
  {
    return v[i];
  }

  float *carray()
  {
    return v;
  }
  const float *carray() const
  {
    return v;
  }

  // Operatoren

  vector3 operator+(const vector3 &sec) const
  {
    return vector3(v[0] + sec.v[0], v[1] + sec.v[1], v[2] + sec.v[2]);
  }

  vector3 operator-(const vector3 &sec) const
  {
    return vector3(v[0] - sec.v[0], v[1] - sec.v[1], v[2] - sec.v[2]);
  }

  vector3 operator-() const
  {
    return vector3(-v[0], -v[1], -v[2]);
  }

  float operator*(const vector3 &p2) const
  {
    return v[0] * p2.v[0] + v[1] * p2.v[1] + v[2] * p2.v[2];
  }

  vector3 operator*(float d) const
  {
    return vector3(v[0] * d, v[1] * d, v[2] * d);
  }

  vector3 cross(const vector3 &b) const
  {
    return vector3(v[1] * b.v[2] - v[2] * b.v[1],
                   v[2] * b.v[0] - v[0] * b.v[2],
                   v[0] * b.v[1] - v[1] * b.v[0]);
  }

  friend vector3 rotate(const vector3 &p,
                        const vector3 &o, const vector3 &dir, float phi);
  friend vector3 rotate(const vector3 &p,
                        const vector3 &dir, float phi);

  friend void add(const vector3 &s1, const vector3 &s2, vector3 &s3);

  //
  float Length() const
  {
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  }

  void Normalize()
  {
    float l = Length();
    if (l != 0)
      {
        v[0] /= l;
        v[1] /= l;
        v[2] /= l;
      }
  }

  friend vector3 Normalize(const vector3 &);
  friend std::string ToString(const vector3 &v);
  friend std::ostream &operator << (std::ostream &os, const vector3 &v);


  friend vector3 operator*(float d, const vector3 &p1)
  {
    return vector3(p1.v[0] * d, p1.v[1] * d, p1.v[2] * d);
  }
};



#endif
