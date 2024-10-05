#include <fstream>
#include <sstream>
#include "povexport.h"

using namespace std;

void getCenter(const Image& depth, const Image& depthvalid, int& xc, int& yc, double& zc)
{
  int xs = depth->xsize;
  int ys = depth->ysize;

  int xmin = depth->xsize;
  int xmax = 0;
  int ymin = depth->ysize;
  int ymax = 0;
  int zmin = depth->maxval;
  int zmax = 0;

  for (int y = 0; y < ys - 1; y++)
    for (int x = 0; x < xs - 1; x++)
      {
        if (GetVal(depthvalid, x, y) == 0)
          {
            int z = GetVal(depth, x, y);
            if (x < xmin) xmin = x;
            if (x > xmax) xmax = x;
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
            if (z < zmin) zmin = z;
            if (z > zmax) zmax = z;
          }
      }
  xc = (xmax + xmin) / 2;
  yc = (ymax + ymin) / 2;
  zc = (zmax + zmin) / 2.0;
}

void povExportDepth(const Image& depth, const Image& depthvalid, const string& fn, double factor)
{
  int xs = depth->xsize;
  int ys = depth->ysize;

  int xc, yc;
  double zc;
  getCenter(depth, depthvalid, xc, yc, zc);

  zc /= factor;

  ofstream os(fn.c_str());

  for (int y = 0; y < ys - 1; y++)
    for (int x = 0; x < xs - 1; x++)
      {
        if (
          GetVal(depthvalid, x, y) == 0 &&
          GetVal(depthvalid, x + 1, y) == 0 &&
          GetVal(depthvalid, x, y + 1) == 0
        )
          {
            os << "triangle {" ;
            os << "<" << x - xc << "," << (GetVal(depth, x, y) / factor) - zc << "," << y - yc << ">" ;
            os << "<" << (x + 1 - xc) << "," << (GetVal(depth, x + 1, y) / factor) - zc << "," << y - yc << ">" ;
            os << "<" << x - xc << "," << (GetVal(depth, x, y + 1) / factor) - zc << "," << (y + 1) - yc << ">" ;
            os << "}" << endl;
          }

        if (
          GetVal(depthvalid, x + 1, y) == 0 &&
          GetVal(depthvalid, x, y + 1) == 0 &&
          GetVal(depthvalid, x + 1, y + 1) == 0
        )
          {
            os << "triangle {" ;
            os << "<" << (x + 1 - xc) << "," << (GetVal(depth, x + 1, y) / factor) - zc << "," << y - yc << ">" ;
            os << "<" << x - xc << "," << (GetVal(depth, x, y + 1) / factor) - zc << "," << (y + 1) - yc << ">" ;
            os << "<" << (x + 1 - xc) << "," << (GetVal(depth, x + 1, y + 1) / factor) - zc << "," << (y + 1) - yc << ">" ;
            os << "}" << endl;
          }
      }
}

void cvExportDepth(const Image& depth, const Image& depthvalid, const string& fn, double factor)
{
  int xs = depth->xsize;
  int ys = depth->ysize;

  int xc, yc;
  double zc;
  getCenter(depth, depthvalid, xc, yc, zc);

  zc /= factor;

  ofstream os2(fn.c_str());

  os2 << "def obj" << endl;

  for (int y = 0; y < ys - 1; y++)
    for (int x = 0; x < xs - 1; x++)
      {
        if (
          GetVal(depthvalid, x, y) == 0 &&
          GetVal(depthvalid, x + 1, y) == 0 &&
          GetVal(depthvalid, x, y + 1) == 0 &&
          GetVal(depthvalid, x + 1, y + 1) == 0
        )
          {
            int d1 = GetVal(depth, x, y) / factor - zc;
            int d2 = GetVal(depth, x + 1, y) / factor - zc;
            int d3 = GetVal(depth, x + 1, y + 1) / factor - zc;
            int d4 = GetVal(depth, x, y + 1) / factor - zc;
            int min = d1;
            if (d2 < min) min = d2;
            if (d3 < min) min = d3;
            if (d4 < min) min = d4;

            int max = d1;
            if (d2 > max) max = d2;
            if (d3 > max) max = d3;
            if (d4 > max) max = d4;

            if (max - min < 18)
              {
                os2 << "polygon" << endl ;
                os2 << "<" << (x - xc) << "," << (GetVal(depth, x, y) / factor) - zc << "," << (y - yc) << ">" << endl ;
                os2 << "<" << (x + 1 - xc) << "," << (GetVal(depth, x + 1, y) / factor) - zc << "," << y - yc << ">" << endl ;
                os2 << "<" << (x + 1 - xc) << "," << (GetVal(depth, x + 1, y + 1) / factor) - zc << "," << (y + 1 - yc) << ">" << endl ;
                os2 << "<" << x - xc << "," << (GetVal(depth, x, y + 1) / factor) - zc << "," << (y + 1 - yc) << ">" << endl ;
                os2 << "endpolygon" << endl;
              }
          }
      }
  os2 << "enddef" << endl;
  os2 << "obj NULL, -90, X" << endl;
}


void cvVector(ostream& os, const Vector3d& v)
{
  os << "<" << v.x << "," << v.y << "," << v.z << ">";
}

void povVector(ostream& os, const Vector3d& v)
{
  os << "<" << v.x << "," << v.y << "," << -v.z << ">";
}

void povTriangle(ostream& os, const TRIANGLE& t, Vector3d center)
{
  os << "triangle {";
  povVector(os, t.p[0] - center);
  povVector(os, t.p[1] - center);
  povVector(os, t.p[2] - center);
  os << "}" << endl;
}

void cvTriangle(ostream& os, const TRIANGLE& t, Vector3d center)
{
  os << "triangle ";
  cvVector(os, t.p[0] - center);
  os << ",";
  cvVector(os, t.p[1] - center);
  os << ",";
  cvVector(os, t.p[2] - center);
  os << endl;
}

Vector3d Center(const vector<TRIANGLE>& tri)
{
  Vector3d center(0, 0, 0);
  for (unsigned int i = 0; i < tri.size(); i++)
    center = center + tri[i].p[0];
  return center * (1.0 / tri.size());
}

void cvExport(const vector<TRIANGLE>& tri, const string& fn)
{
  Vector3d center = Center(tri);

  ofstream os2(fn.c_str());
  os2 << "light <0.6,0.6,0.6>,<0,-150,-500>" << endl; // hoffentlich ausserhalb
  os2 << "def obj" << endl;
  for (unsigned int i = 0; i < tri.size(); i++)
    {
      const TRIANGLE& t = tri[i];
      // triangle <7.1099,0.3629,1.48993>,<7.1304,0.371005,1.50436>,<7.1441,0.360581,1.47347>
      cvTriangle(os2, t, center);
    }
  os2 << "enddef" << endl;
  os2 << "obj NULL, -90, X" << endl;
  // os2 << "obj NULL, 180, Y" << endl;
}

void tvExport(const vector<TRIANGLE>& tri, const string& fn)
{
  Vector3d center = Center(tri);
  ofstream os(fn.c_str());
}

void povExport(const vector<TRIANGLE>& tri, const string& fn)
{
  Vector3d center = Center(tri);

  ofstream os2(fn.c_str());
  for (unsigned int i = 0; i < tri.size(); i++)
    {
      const TRIANGLE& t = tri[i];
      povTriangle(os2, t, center);
    }
}
