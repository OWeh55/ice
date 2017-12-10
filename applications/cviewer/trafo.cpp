#include "numbase.h"
#include "trafo.h"
#include "arith.h"

void Trafo::glGet(GLenum which)
{
  GLfloat gm[16];
  glGetFloatv(which, gm);
  for (int i = 0; i < 3; i++)
    for (int k = 0; k < 3; k++)
      m[k][i] = gm[i * 4 + k];
  for (int k = 0; k < 3; k++)
    s[k] = gm[12 + k];
}

void Trafo::glSet(GLenum which) const
{
  GLfloat gm[16];
  for (int i = 0; i < 3; i++)
    {
      for (int k = 0; k < 3; k++)
        gm[i * 4 + k] = m[k][i];
      gm[i * 4 + 3] = 0.0f;
    }
  for (int k = 0; k < 3; k++)
    gm[12 + k] = s[k];
  gm[15] = 1.0f;

  GLint mode;
  glGetIntegerv(GL_MATRIX_MODE, &mode);
  if (static_cast<GLenum>(mode) != which)
    {
      glMatrixMode(which);
      glLoadMatrixf(gm);
      glMatrixMode(mode);
    }
}

vector3 Trafo::Transform(const vector3& p) const
{
  vector3 res;
  mul(m, p, res);
  add(res, s, res);
  return res;
}

void Trafo::Shift(float dx, float dy, float dz)
{
  s.v[0] += dx;
  s.v[1] += dy;
  s.v[2] += dz;
}

void Trafo::Shift(const vector3& d)
{
  s = s + d;
}

void Trafo::Rotate(const vector3& dir, float phi)
{
  // 3D-Rotation um einen Strahl durch den Null-Punkt in Richtung dir
  // um phi GRAD

  double sind = Sin(phi);
  double cosd = Cos(phi);

  double cosd1 = 1 - cosd;

  double l = dir.Length();

  double dirx = dir.X() / l;
  double diry = dir.Y() / l;
  double dirz = dir.Z() / l;

  matrix3 mr;
  mr[0][0] = dirx * dirx * cosd1 + cosd;
  mr[0][1] = dirx * diry * cosd1 + dirz * sind;
  mr[0][2] = dirx * dirz * cosd1 - diry * sind;

  mr[1][0] = diry * dirx * cosd1 - dirz * sind;
  mr[1][1] = diry * diry * cosd1 + cosd;
  mr[1][2] = diry * dirz * cosd1 + dirx * sind;

  mr[2][0] = dirz * dirx * cosd1 + diry * sind;
  mr[2][1] = dirz * diry * cosd1 - dirx * sind;
  mr[2][2] = dirz * dirz * cosd1 + cosd;

  Trafo t(*this); // Kopie wegen Multiplikation der Matrizen "in sich"
  mul(mr, t.m, m);
  mul(mr, t.s, s);
}

void Trafo::Rotate(const vector3& o, const vector3& dir, float phi)
{
  Shift(-o);
  Rotate(dir, phi);
  Shift(o);
}

void Trafo::Append(const Trafo& tr)
{
  Trafo t(*this); // Kopie wegen Multiplikation der Matrizen "in sich"
  mul(tr.m, t.m, m);
  mul(tr.m, t.s, s);
  add(s, tr.s, s);
}

string ToString(const Trafo& tr)
{
  return tr.toString();
}

string Trafo::toString() const
{
  string res;
  for (int i = 0; i < 3; i++)
    {
      string line = "<";
      for (int j = 0; j < 3; j++)
        {
          line += ToString(m[i][j]);
          if (j < 2) line += ",";
        }
      res += line + ">\n";
    }

  res += "\n<";
  for (int j = 0; j < 2; j++)
    {
      res += ToString(s[j]);
      res += ",";
    }

  res += ToString(s[2]);
  res += ">";
  return res;
}
