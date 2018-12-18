#ifndef CUBES_H
#define CUBES_H
#include <ImageC.h>
#include <image3d.h>

class cube
{
public:
  cube(const ice::Image3d<ice::Image>& img, int x, int y, int z);

  ice::Vector3d p[8];
  double val[8]; // Grauwerte der Ecken

private:
  static int dx[8];
  static int dy[8];
  static int dz[8];
  static int mask[8];

  const int x, y, z; // Position Referenzecke
};


#endif
