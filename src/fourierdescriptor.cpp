#include "fourierdescriptor.h"
#include "fouriertrafo.h"

using namespace std;
using namespace ice;

void computeFourier(const vector<Point>& v1, vector<Point>& v2,
                    bool forward)
{
  FourierTrafo ft(v1.size(), forward, true);
  ft.setInput(v1);
  ft.getResult(v2);
}

void getIndices(const vector<Point>& fd, int& f0, int& f1, int& f2)
{
  int size = fd.size();
  f0 = size / 2;
  f1 = f0 + 1;
  f2 = f0 + 2;
}

void normalizeFDScaling(const vector<Point>& fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  double snorm = 1.0 / fk[i1].length();
  for (int i = 0; i < fk.size(); i++)
    {
      Point p = fk[i];
      if (i != i0)
        p *= snorm;
      fd[i] = p;
    }
}

void normalizeFDShift(const vector<Point>& fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  for (int i = 0; i < fk.size(); i++)
    {
      Point p = fk[i];
      if (i == i0)
        p = Point(0, 0);
      fd[i] = p;
    }
}

void normalizeFDIShift(const vector<Point>& fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  double dfi = -fk[i1].phi();
  for (int i = 0; i < fk.size(); i++)
    {
      int k = i - i0;
      Point p = fk[i];
      double r = p.r();
      double fi = p.phi();
      fi += dfi * k;
      fd[i] = Point(r * cos(fi), r * sin(fi));
    }
}

void normalizeFDRotation(const vector<Point>& fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  double beta = -fk[i2].phi();
  for (int i = 0; i < fk.size(); i++)
    {
      Point p = fk[i];
      double r = p.r();
      double fi = p.phi();
      if (i != i0)
        fi += beta;
      fd[i] = Point(r * cos(fi), r * sin(fi));
    }
}

void normalizeFDIShiftRotation(const vector<Point>& fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  double beta, dfi; // rotation and shift of initial point
  double a1 = 1;
  double b1 = 1;
  double c1 = -fk[i1].phi();
  double a2 = 1;
  double b2 = 2;
  double c2 = -fk[i2].phi();
  Solve2(a1, b1, c1, a2, b2, c2, beta, dfi);
  for (int i = 0; i < fk.size(); i++)
    {
      int k = i - i0;
      Point p = fk[i];
      double r = p.r();
      double fi = p.phi();
      if (i != i0)
        fi += dfi * k + beta;
      fd[i] = Point(r * cos(fi), r * sin(fi));
    }
}

void normalizeFDEuclidian(const std::vector<ice::Point>& fk,
                          std::vector<ice::Point>& fd);
