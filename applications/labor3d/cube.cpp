// Funktionen für marching cubes
#include "cube.h"

int cube::dx[8] = {0, 1, 1, 0, 0, 1, 1, 0};
int cube::dy[8] = {0, 0, 1, 1, 0, 0, 1, 1};
int cube::dz[8] = {0, 0, 0, 0, 1, 1, 1, 1};

cube::cube(const ice::Image3d<ice::Image> &img, int x, int y, int z): x(x), y(y), z(z)
{
  for (int idx = 0; idx < 8; idx++)
    {
      img.i2d(x + dx[idx], y + dy[idx], z + dz[idx],
              p[idx].x, p[idx].y,    p[idx].z);

      val[idx] = img.getPixel(x + dx[idx], y + dy[idx], z + dz[idx]);
    }
}
