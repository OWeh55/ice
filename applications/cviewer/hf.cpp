#include "triangle.h"
#include "pbmio.h"
#include "hf.h"

void heightfield::InitHF(const string &name)
{
  int nr;
  if (InfPBMFile(name, xsize, ysize, maxval, nr))
    {
      if (maxval < 256)
        {
          unsigned char *buf;
          if (!ReadPBM8(name, buf, xsize, ysize))
            {
              cerror("Kann " + name + " nicht lesen");
            }
          else
            {
              h.clear();
              h.resize(ysize, std::vector<int>(xsize));
              // buf => h
              unsigned char *bufp = buf;
              for (int y = ysize - 1; y >= 0; y--)
                for (int x = 0; x < xsize; x++)
                  {
                    h[y][x] = *(bufp++);
                  }

              delete [] buf;
            }
        }
      else
        {
          unsigned short *buf;
          //    cout << sizeof(unsigned short)<<endl;
          if (!ReadPBM16(name, buf, xsize, ysize))
            {
              cerror("Kann " + name + " nicht lesen");
            }
          else
            {
              h.clear();
              h.resize(ysize, std::vector<int>(xsize));
              // buf => h
              unsigned short *bufp = buf;
              for (int y = ysize - 1; y >= 0; y--)
                for (int x = 0; x < xsize; x++)
                  {
                    h[y][x] = *(bufp++);
                  }

              delete [] buf;
            }
        }

      v3d.clear();
      v3d.resize(ysize, vector<vector3>(xsize));
      n3d.clear();
      n3d.resize(ysize, vector<vector3>(xsize));

    }
  else
    {
      cerror("Kann " + name + " nicht lesen");
    }
}

void heightfield::CalcV3D() const
{
  vector3 v0 = pos->Value();
  vector3 vx = DirX();
  vector3 vy = DirY();
  vector3 vz = DirZ();

  vx = vx * (1.0 / xsize);
  vy = vy * (1.0 / ysize);
  vz = vz * (1.0 / maxval);

  vector3 vla, vra;
  // Berechnung der 3D-Vektoren
  for (int y = 0 ; y < ysize; y++)
    {
      vector3 vy0 = vy * y + v0;
      for (int x = 0; x < xsize; x++)
        {
          v3d[y][x] = vx * x + vy0 + vz * h[y][x];
        }
    }

  // Berechnung der Normalen
  vector3 vl, vr, vo, vu, norm;
  for (int y = 0 ; y < ysize; y++)
    {
      for (int x = 0; x < xsize; x++)
        {
          if (x > 0)
            vl = v3d[y][x - 1];
          else
            vl = v3d[y][x];
          if (x < xsize - 1)
            vr = v3d[y][x + 1];
          else
            vr = v3d[y][x];
          if (y > 0)
            vo = v3d[y - 1][x];
          else
            vo = v3d[y][x];
          if (y < ysize - 1)
            vu = v3d[y + 1][x];
          else
            vu = v3d[y][x];
          norm = (vo - vu).cross(vr - vl);
          norm.Normalize();
          n3d[y][x] = norm;
        }
    }
}

void heightfield::Vertex(int x, int y) const
{
  glNormal3fv(n3d[y][x].carray());
  glVertex3fv(v3d[y][x].carray());
}

void heightfield::MyOptimize()
{
  DEBUGOUT("opt. heightfield ");
  objekt::MyOptimize();
  if (dirx) optvector(dirx);
  if (diry) optvector(diry);
  if (dirz) optvector(dirz);
  DEBUGOUT(endl);
  constant = pos->Constant();
  if (dirx)
    constant &= dirx->Constant();
  if (diry)
    constant &= diry->Constant();
  if (dirz)
    constant &= dirz->Constant();
  if (constant)
    CalcV3D();
};

void heightfield::myDraw() const
{
  if (!constant)
    CalcV3D();

  for (int y = 0 ; y < ysize - 1; y++)
    {
      glBegin(GL_TRIANGLE_STRIP);
      for (int x = 0; x < xsize; x++)
        {
          Vertex(x, y + 1);
          Vertex(x, y);
        }
      glEnd();
    }
}
