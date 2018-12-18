#include <image3d.h>
#include <matrix3d.h>
#include <util.h>

#include "fft3d.h"
#include "sdr.h"

using namespace std;
using namespace ice;

void Expand(const Matrix3d &m1, int faktor, Matrix3d &m2)
{
  int ox = m1.sizeX();
  int nx = ox * faktor;
  int ax = 1;
  if (ox == 1)
    {
      nx = 1;
      ax = 0;
    }

  int oy = m1.sizeY();
  int ny = oy * faktor;
  int ay = 1;
  if (oy == 1)
    {
      ny = 1;
      ay = 0;
    }

  int oz = m1.sizeZ();
  int nz = oz * faktor;
  int az = 1;
  if (oz == 1)
    {
      nz = 1;
      az = 0;
    }


  // cout << "old " << ox << "," << oy << "," << oz << endl;
  // cout << "new " << nx << "," << ny << "," << nz << endl;

  m2.resize(nx, ny, nz);
  m2.set(0.0);

  int px = (nx - ox) / 2;
  int py = (ny - oy) / 2;
  int pz = (nz - oz) / 2;
  // cout << "offs: " << px << "," << py << "," << pz << endl;

  for (int z = az; z < oz; z++)
    for (int y = ay; y < oy; y++)
      for (int x = ax; x < ox; x++)
        /*
          for (int z = 1; z < oz; z++)
          for (int y = 1; y < oy; y++)
          for q(int x = 1; x < ox; x++)
        */
        {
          // cout << ". " << x << "," << y << "," << z << endl;
          // cout << "-> " << x+px << "," << y+py << "," << z+pz << endl;
          double val = m1(x, y, z);
          m2(x + px, y + py, z + pz) = val;
        }
}

bool InvConvolution(const Image3d<Image> &img1, const Image3d<Image> &img2,
                    int x, int y, int z,
                    int wsizex, int wsizey, int wsizez,
                    double beta, int resfac,
                    Matrix3d &res)
{

  int xsize, ysize, zsize;
  img1.getSize(xsize, ysize, zsize);

  int x1 = limited(x - wsizex / 2, 0, xsize - 1);
  int y1 = limited(y - wsizey / 2, 0, ysize - 1);
  int z1 = limited(z - wsizez / 2, 0, zsize - 1);
  int x2 = x1 + wsizex - 1;
  int y2 = y1 + wsizey - 1;
  int z2 = z1 + wsizez - 1;
  //cout << "(" << x1 << "," << y1 << "," << z1 << ")" << endl;
  //  cout << "(" << x2 << "," << y2 << "," << z2 << ")" << endl;
  //  cout << ">" << xsize << "," << ysize << "," << zsize << "<" << endl;
  if (x1 >= 0 && y1 >= 0 && z1 >= 0 &&
      x2 < xsize && y2 < ysize && z2 < zsize)
    {
      Window3d win(x1, y1, z1, x2, y2, z2);
      Image3d<Image> subimg1(img1, win);
      Image3d<Image> subimg2(img2, win);

      Matrix3d m1, m2; // "complex" matrix

      Image2Matrix(subimg1, m1);

      //                  cout << "to Matrix2" << endl;
      Image2Matrix(subimg2, m2);
      //cout << "fft" << endl;

      fft(m1, m2, true);

      Matrix3d md1(wsizex, wsizey, wsizez), md2(wsizex, wsizey, wsizez); // "complex" matrix of deconvolution
      for (int z = 0; z < wsizez; z++)
        {
          int nz = (wsizez - z) % wsizez;
          for (int y = 0; y < wsizey; y++)
            {
              int ny = (wsizey - y) % wsizey;
              for (int x = 0; x < wsizex; x++)
                {
                  int nx = (wsizex - x) % wsizex;

                  double rr = m1(x, y, z);
                  double ir = m2(x, y, z);

                  double rq = m1(nx, ny, nz);
                  double iq = m2(nx, ny, nz);

                  double r1 = (rr + rq) / 2;
                  double im1 = (ir - iq) / 2;
                  double r2 = (ir + iq) / 2;
                  double im2 = (rq - rr) / 2;

                  // cout << "A: " << r1 << ", " << im1 << endl;
                  // cout << "B: " << r2 << ", " << im2 << endl;

                  double b1 = r1 * r1 + im1 * im1 + beta * beta;

                  double r3 = (r2 * r1 + im2 * im1) / b1;
                  double im3 = (r1 * im2 - r2 * im1) / b1;

                  md1(x, y, z) = r3;
                  md2(x, y, z) = im3;
                  // cout << r3 << ", " << im3 << endl;
                }
            }
        }

      // zero frequency 0
      md1(wsizex / 2, wsizey / 2, wsizez / 2) = 0;
      md2(wsizex / 2, wsizey / 2, wsizez / 2) = 0;

      // "expand" matrix
      Matrix3d md2e; // expanded "complex" matrix of deconvolution

      Expand(md1, resfac, res);
      Expand(md2, resfac, md2e);

      fft(res, md2e, false);
      return true;
    }
  else // not inside
    return false;
}

bool InvConvolution(const Image3d<Image> &img1, const Image3d<Image> &img2,
                    const IVector3d &p,
                    int wsizex, int wsizey, int wsizez,
                    double beta, int resfac,
                    Matrix3d &res)
{
  cout << "[" << p.x << "," << p.y << "," << p.z << "]" << endl;

  int xp, yp, zp;
  img1.d2i(p.x, p.y, p.z, xp, yp, zp);
  return InvConvolution(img1, img2, xp, yp, zp, wsizex, wsizey, wsizez, beta, resfac, res);
}
