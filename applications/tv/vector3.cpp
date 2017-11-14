#include <cmath>

#include <string>

#include "numbase.h"
#include "vector3.h"

void add(const Vector3& s1, const Vector3& s2, Vector3& s3)
{
  for (int i = 0; i < 3; i++)
    {
      s3.v[i] = s1.v[i] + s2.v[i];
    }
}

Vector3 Normalize(const Vector3& v)
{
//  float l=v.Length();
  float l = sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
  if (l == 0) return Vector3(0, 0, 0);
  return Vector3(v.v[0] / l, v.v[1] / l, v.v[2] / l);
}

Vector3 rotate(const Vector3& p,
               const Vector3& o, const Vector3& dir, float phi)
{
// 3D-Rotation um einen durch Punkt und Richtung gegebenen Strahl
  double sind = Sin(phi);
  double cosd = Cos(phi);

  double cosd1 = 1 - cosd;

  double dirx = dir.v[0] / dir.Length();
  double diry = dir.v[1] / dir.Length();
  double dirz = dir.v[2] / dir.Length();

  Vector3 h = p - o;
  double x = h.v[0];
  double y = h.v[1];
  double z = h.v[2];

  double zx = (dirx * dirx * cosd1 + cosd) * x
              + (dirx * diry * cosd1 + dirz * sind) * y
              + (dirx * dirz * cosd1 - diry * sind) * z;
  double zy = (diry * dirx * cosd1 - dirz * sind) * x
              + (diry * diry * cosd1 + cosd) * y
              + (diry * dirz * cosd1 + dirx * sind) * z;
  double zz = (dirz * dirx * cosd1 + diry * sind) * x
              + (dirz * diry * cosd1 - dirx * sind) * y
              + (dirz * dirz * cosd1 + cosd) * z;

  return Vector3(zx, zy, zz) + o;
}

Vector3 rotate(const Vector3& p,
               const Vector3& dir, float phi)
{
// 3D-Rotation um einen Strahl durch den Null-Punkt in Richtung dir
// um phi GRAD

  double sind = Sin(phi);
  double cosd = Cos(phi);

  double cosd1 = 1 - cosd;

  double len = dir.Length();
  double dirx = dir.v[0] / len;
  double diry = dir.v[1] / len;
  double dirz = dir.v[2] / len;

  double x = p.v[0];
  double y = p.v[1];
  double z = p.v[2];

  double zx = (dirx * dirx * cosd1 + cosd) * x
              + (dirx * diry * cosd1 + dirz * sind) * y
              + (dirx * dirz * cosd1 - diry * sind) * z;

  double zy = (diry * dirx * cosd1 - dirz * sind) * x
              + (diry * diry * cosd1 + cosd) * y
              + (diry * dirz * cosd1 + dirx * sind) * z;

  double zz = (dirz * dirx * cosd1 + diry * sind) * x
              + (dirz * diry * cosd1 - dirx * sind) * y
              + (dirz * dirz * cosd1 + cosd) * z;

  return Vector3(zx, zy, zz);
}

