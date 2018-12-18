#include <image.h>
#include <matrix3d.h>

#include "konfokal.h"

void CreatePSF(const Image3d<Image>& img, double delta_r)
{
  int z0 = img.zsize / 2;
  int x0 = img.xsize / 2;
  int y0 = img.ysize / 2;

  img.clear();
  #pragma omp parallel for schedule(dynamic,40)
  for (int z = 0; z < img.zsize; z++)
    {
      double rad = abs(z - z0) * delta_r;
      double rad2 = rad * rad;

      double area;
      if (rad == 0)
        area = 1;
      else
        area = M_PI * rad2;

      double value = img.maxval / area;
      cout << rad << " " << (rad2 * M_PI) << " " << value << endl;
      if (value >= 0.5)
        {
          if (z == z0)
            img.setPixel(x0, y0, z0, value);
          else
            {
              for (int y = 0; y < img.ysize; y++)
                for (int x = 0; x < img.xsize; x++)
                  {
                    int ct = 0;
                    int ctall = 0;
                    for (double dy = -0.5; dy < 0.5; dy += 0.1)
                      for (double dx = -0.5; dx < 0.5; dx += 0.1)
                        {
                          double ddx = x + dx - x0;
                          double ddy = y + dy - y0;
                          double r2 = ddx * ddx + ddy * ddy;
                          if (r2 < rad2)
                            ct++;
                          ctall++;
                        }
                    img.setPixel(x, y, z, RoundInt((value * ct) / ctall));
                  }
            }
        }
    }
}

void CreatePSF(Matrix3d& mat, double delta_r)
{
  int x0 = mat.sizeX() / 2;
  int y0 = mat.sizeY() / 2;
  int z0 = mat.sizeZ() / 2;

  mat.set(0.0);
  #pragma omp parallel for schedule(dynamic,40)
  for (int z = 0; z < mat.sizeZ(); z++)
    {
      double rad = abs(z - z0) * delta_r;
      double rad2 = rad * rad;

      double area;
      if (rad == 0)
        area = 1;
      else
        area = M_PI * rad2;

      double value = 1.0 / area;
      //      cout << rad << " " << (rad2*M_PI) << " " << value << endl;
      if (z == z0)
        mat(x0, y0, z0) = value;
      else
        {
          for (int y = 0; y < mat.sizeY(); y++)
            for (int x = 0; x < mat.sizeX(); x++)
              {
                int ct = 0;
                int ctall = 0;
                for (double dy = -0.5; dy < 0.5; dy += 0.1)
                  for (double dx = -0.5; dx < 0.5; dx += 0.1)
                    {
                      double ddx = x + dx - x0;
                      double ddy = y + dy - y0;
                      double r2 = ddx * ddx + ddy * ddy;
                      if (r2 < rad2)
                        ct++;
                      ctall++;
                    }
                mat(x, y, z) = (value * ct) / ctall;
              }
        }
    }
}
