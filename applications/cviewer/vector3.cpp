#include <math.h>

#include "numbase.h"
#include "vector3.h"
#include "arith.h"

void add(const vector3& s1, const vector3& s2, vector3& s3)
{
  for (int i = 0; i < 3; i++)
    {
      s3.v[i] = s1.v[i] + s2.v[i];
    }
}

vector3 Normalize(const vector3& v)
{
//  float l=v.Length();
  float l = sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
  if (l == 0) return vector3(0, 0, 0);
  return vector3(v.v[0] / l, v.v[1] / l, v.v[2] / l);
}

vector3 rotate(const vector3& p,
               const vector3& o, const vector3& dir, float phi)
{
// 3D-Rotation um einen durch Punkt und Richtung gegebenen Strahl
  double sind = Sin(phi);
  double cosd = Cos(phi);

  double cosd1 = 1 - cosd;

  double dirx = dir.v[0] / dir.Length();
  double diry = dir.v[1] / dir.Length();
  double dirz = dir.v[2] / dir.Length();

  vector3 h = p - o;
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

  return vector3(zx, zy, zz) + o;
}

vector3 rotate(const vector3& p,
               const vector3& dir, float phi)
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

  return vector3(zx, zy, zz);
}

string ToString(const vector3& v)
{
  return "<" + ToString(v.v[0]) + ","
         + ToString(v.v[1]) + ","
         + ToString(v.v[2]) + ">";
}

std::ostream& operator << (std::ostream& os, const vector3& v)
{
  os << ToString(v);
  return os;
}
