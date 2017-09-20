#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>
#include <string>

//using namespace std;

class Vector3
{
public:
  float v[3];

public:
  // Konstruktoren
  Vector3()
  {
    clear();
  }

  Vector3(float x, float y, float z)
  {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }

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

  const float& operator[](int i) const
  {
    return v[i];
  }
  float& operator[](int i)
  {
    return v[i];
  }

  float* carray()
  {
    return v;
  }

  const float* carray() const
  {
    return v;
  }

  // Operatoren

  Vector3 operator+(const Vector3& sec) const
  {
    return Vector3(v[0] + sec.v[0], v[1] + sec.v[1], v[2] + sec.v[2]);
  }

  void operator+=(const Vector3& sec)
  {
    v[0] += sec.v[0];
    v[1] += sec.v[1];
    v[2] += sec.v[2];
  }

  Vector3 operator-(const Vector3& sec) const
  {
    return Vector3(v[0] - sec.v[0], v[1] - sec.v[1], v[2] - sec.v[2]);
  }

  Vector3 operator-() const
  {
    return Vector3(-v[0], -v[1], -v[2]);
  }

  float operator*(const Vector3& p2) const
  {
    return v[0] * p2.v[0] + v[1] * p2.v[1] + v[2] * p2.v[2];
  }

  Vector3 operator*(float d) const
  {
    return Vector3(v[0] * d, v[1] * d, v[2] * d);
  }

  Vector3 cross(const Vector3& b) const
  {
    return Vector3(v[1] * b.v[2] - v[2] * b.v[1],
                   v[2] * b.v[0] - v[0] * b.v[2],
                   v[0] * b.v[1] - v[1] * b.v[0]);
  }

  friend Vector3 rotate(const Vector3& p,
                        const Vector3& o, const Vector3& dir, float phi);
  friend Vector3 rotate(const Vector3& p,
                        const Vector3& dir, float phi);

  friend void add(const Vector3& s1, const Vector3& s2, Vector3& s3);

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

  friend Vector3 Normalize(const Vector3&);
  friend std::string ToString(const Vector3& v);
  friend std::ostream& operator << (std::ostream& os, const Vector3& v);


  friend Vector3 operator*(float d, const Vector3& p1)
  {
    return Vector3(p1.v[0] * d, p1.v[1] * d, p1.v[2] * d);
  }
};

#endif
