#ifndef MARCHING_H
#define MARCHING_H

#include <vector>
#include <Vector3d.h>
#include "cube.h"

struct TRIANGLE
{
  ice::Vector3d p[3];         /* Vertices */
};

ice::Vector3d VertexInterp(double isolevel, ice::Vector3d p1, ice::Vector3d p2,
                           double valp1, double valp2);

int PolygoniseCube(const cube &g, double iso, std::vector<TRIANGLE> &tri);

#endif
