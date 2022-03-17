/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include "fourierdescriptor.h"
#include "FourierTrafo.h"

using namespace std;
using namespace ice;

void computeFourier(const vector<Point>& v1, vector<Point>& v2,
                    bool forward)
{
  FourierTrafo ft(v1.size(), forward, true);
  ft.setInput(v1);
  ft.getResult(v2);
}

void getZeroIndex(const vector<Point>& fd, int& f0)
{
  int size = fd.size();
  f0 = size / 2;
}

int getZeroIndex(const vector<Point>& fd)
{
  int size = fd.size();
  return size / 2;
}

#define FNAME "zeroPaddingFD"
// applies zero padding, corrects values to get interpolation after FT^-1
void zeroPaddingFD(const std::vector<ice::Point>& v1, int newsize,
                   std::vector<ice::Point>& v2)
{
  int i0;
  getZeroIndex(v1, i0);
  int oldsize = v1.size();
  int n0, n1, n2;
  if (newsize <= oldsize)
    throw IceException(FNAME, M_WRONG_SIZE);
  v2.resize(newsize);
  getZeroIndex(v2, n0);
  int di = n0 - i0;
  // padding left
  for (int i = 0; i < di; i++)
    v2[i] = Point(0.0, 0.0);
  double fac = sqrt(1.0 * newsize / oldsize);
  // data
  for (int i = 0; i < oldsize; i++)
    v2[i + di] = v1[i] * fac;
  // padding right
  for (int i = di + oldsize; i < newsize; i++)
    v2[i] = Point(0.0, 0.0);
}
#undef FNAME

void scaleFD(std::vector<ice::Point> fk, double s,
             std::vector<ice::Point>& fd)
{
  int i0;
  getZeroIndex(fk, i0);
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
  int i0;
  getZeroIndex(fk, i0);
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
  int i0;
  getZeroIndex(fk, i0);
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

void invertOrderFD(std::vector<ice::Point> fk,
                   std::vector<ice::Point>& fd)
{
  int n = fk.size();
  fd.resize(n);
  if (n % 2 == 0)
    {
      for (int i = 1; i < n; i++)
        fd[n - i] = fk[i];
    }
  else
    {
      for (int i = 0; i < n; i++)
        fd[n - 1 - i] = fk[i];
    }
}

void normalizeFDOrder(vector<Point> fk, vector<Point>& fd)
{
  int i0 = getZeroIndex(fk);
  int n = fk.size();
  fd.resize(n);
  if (fk[i0 - 1].r() > fk[i0 + 1].r())
    invertOrderFD(fk, fd);
  else
    fd = fk;
}

void normalizeFDScaling(vector<Point> fk, vector<Point>& fd)
{
  int i0;
  getZeroIndex(fk, i0);
  int n = fk.size();
  fd.resize(n);
  double snorm = sqrt(n) / fk[i0 + 1].length();
  scaleFD(fk, snorm, fd);
}

void normalizeFDShift(vector<Point> fk, vector<Point>& fd)
{
  int i0;
  getZeroIndex(fk, i0);
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
  int i0;
  getZeroIndex(fk, i0);
  fd.resize(fk.size());
  double dfi = -fk[i0 + 1].phi();
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
  int i0;
  getZeroIndex(fk, i0);
  fd.resize(fk.size());
  double beta = -fk[i0 + 2].phi();
  rotateFD(fk, beta, fd);
}

void normalizeFDIShiftRotation(vector<Point> fk, vector<Point>& fd)
{
  int i0;
  getZeroIndex(fk, i0);
  fd.resize(fk.size());
  double beta, dfi; // rotation and shift of initial point
#if 1
  double a1 = 1;
  double b1 = 1;
  double c1 = -fk[i0 + 1].phi();
  double a2 = 1;
  double b2 = 2;
  double c2 = -fk[i0 + 2].phi();
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
