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

void scaleFD(std::vector<ice::Point> fk, double s,
             std::vector<ice::Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  for (int i = 0; i < fk.size(); i++)
    {
      Point p = fk[i];
      if (i != i0)
        p *= s;
      fd[i] = p;
    }
}

void shiftFD(std::vector<ice::Point> fk, ice::Point sh,
             std::vector<ice::Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  int n = fk.size();
  fd.resize(n);
  for (int i = 0; i < n; i++)
    {
      Point p = fk[i];
      if (i == i0)
        p += sqrt(n) * sh;
      fd[i] = p;
    }
}

void rotateFD(std::vector<ice::Point> fk, double phi,
              std::vector<ice::Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  int n = fk.size();
  fd.resize(n);
  for (int i = 0; i < n; i++)
    {
      Point p = fk[i];
      double r = p.r();
      double fi = p.phi();
      if (i != i0)
        fi += phi;
      fd[i] = Point(r * cos(fi), r * sin(fi));
    }
}

void normalizeFDScaling(vector<Point> fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  int n = fk.size();
  fd.resize(n);
  double snorm = sqrt(n) / fk[i1].length();
  scaleFD(fk, snorm, fd);
}

void normalizeFDShift(vector<Point> fk, vector<Point>& fd)
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

void normalizeFDIShift(vector<Point> fk, vector<Point>& fd)
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

void normalizeFDRotation(vector<Point> fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  double beta = -fk[i2].phi();
  rotateFD(fk, beta, fd);
#if 0
  for (int i = 0; i < fk.size(); i++)
    {
      Point p = fk[i];
      double r = p.r();
      double fi = p.phi();
      if (i != i0)
        fi += beta;
      fd[i] = Point(r * cos(fi), r * sin(fi));
    }
#endif
}

void normalizeFDIShiftRotation(vector<Point> fk, vector<Point>& fd)
{
  int i0, i1, i2;
  getIndices(fk, i0, i1, i2);
  fd.resize(fk.size());
  double beta, dfi; // rotation and shift of initial point
#if 1
  double a1 = 1;
  double b1 = 1;
  double c1 = -fk[i1].phi();
  double a2 = 1;
  double b2 = 2;
  double c2 = -fk[i2].phi();
  Solve2(a1, b1, c1, a2, b2, c2, beta, dfi);
#else
  beta = -0.5 * (fk[i0 + 1].phi() + fk[i0 - 1].phi());
  dfi = -0.5 * (fk[i0 + 1].phi() - fk[i0 - 1].phi());
#endif
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

void normalizeFDEuclidian(std::vector<ice::Point> fk,
                          std::vector<ice::Point>& fd)
{
  normalizeFDShift(fk, fd);
  normalizeFDIShiftRotation(fd, fd);
}
