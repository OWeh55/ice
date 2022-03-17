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

#include "matrixtemplate.h"
#include "MatrixAlgebra.h"
#include "LsiFilter.h"

namespace ice
{
// functions for generation of LsiFilters

#define FNAME "makePolynomFilter"
  static int powi(int b, int e)
  {
    if (e < 1)
      {
        return 1;
      }
    else
      {
        int p = powi(b, e / 2);
        p = p * p;

        if (e % 2 == 0)
          {
            return p;
          }
        else
          {
            return b * p;
          }
      }
  }

  static int kidx(int n, int i, int j)
  {
    int idx = 0;

    for (int k = 0; k < j; k++)
      {
        idx += n + 1 - k;
      }

    return idx + i;
  }

  LsiFilter makePolynomFilter(const int size, int grad, int ii, int jj)
  {
    // Parametertest
    if (
      ((size & 1) == 0) ||
      (ii + jj > grad) ||
      (grad < 0) ||
      (ii < 0) ||
      (jj < 0)
    )
      throw IceException(FNAME, M_WRONG_PARAMETER);

    Matrix f(size * size, (grad + 1) * (grad + 2) / 2);

    for (int k = 0; k < size; k++)
      for (int l = 0; l < size; l++)
        {
          int zidx = k * size + l;

          for (int i = 0; i <= grad; i++)
            for (int j = 0; j + i <= grad; j++)
              {
                f[zidx][kidx(grad, i, j)] = powi(k - size / 2, i) * powi(l - size / 2, j);
              }
        }

    Matrix ft = !f;
    Matrix ftf = ft * f;
    Matrix iftf = Inverse(ftf);
    Matrix az = iftf * ft;
    int zidx = kidx(grad, ii, jj);
    Matrix mask(size, size);

    for (int j = 0; j < size; j++)
      for (int i = 0; i < size; i++)
        {
          mask[i][j] = az[zidx][j * size + i];
        }

    return LsiFilter(mask);
  }
#undef FNAME

// these filters generate a (quasi boolean) huge matrix and
// reduce size later by combining xfak*xfak elements

  const int sizeFactor = 13;

  static void reducematrix(const matrix<double>& huge, matrix<double>& small)
  {
    int ysize = small.rows();
    int xsize = small.cols();

    // size of matrix huge must be sizeFactor*ysize,sizeFactor*ysize!!
    for (int yi = 0; yi < ysize; yi++)
      {
        for (int xi = 0; xi < xsize; xi++)
          {
            double sum = 0;

            for (int xyi = yi * sizeFactor; xyi < (yi + 1)*sizeFactor; xyi++)
              for (int xxi = xi * sizeFactor; xxi < (xi + 1)*sizeFactor; xxi++)
                {
                  sum += huge[xyi][xxi];
                }
            //std::cout << sum << std::endl;
            small[yi][xi] = sum;
          }
      }
  }

  LsiFilter makeOrientedSmearFilter(int size, double dir, double len, double width)
  {
    // generate huge (int-)matrix (quasi boolean)
    int xsize = sizeFactor * size;
    double xwidth = 0.5 * width * sizeFactor;
    double xlen = 0.5 * len * sizeFactor;
    matrix<double> hm(xsize, xsize);

    int mitte = (xsize - 1) / 2;

    double cc = cos(-dir);
    double ss = sin(-dir);

    int ct = 0;

    for (int yi = 0; yi < xsize; yi++)
      {
        double yr = yi - mitte;

        for (int xi = 0; xi < xsize; xi++)
          {
            double xr = xi - mitte;
            double yrot = xr * ss + yr * cc;
            double xrot = xr * cc - yr * ss;

            if (fabs(yrot) < xwidth && fabs(xrot) < xlen)
              {
                hm[yi][xi] = 1;
                ct++;
              }
            else
              {
                hm[yi][xi] = 0;
              }
          }
      }

    for (int yi = 0; yi < xsize; yi++)
      for (int xi = 0; xi < xsize; xi++)
        hm[yi][xi] /= ct;

    // convert huge matrix to size of filter
    matrix<double> mask(size, size);
    reducematrix(hm, mask);
    return LsiFilter(mask);
  }

  LsiFilter makeOrientedDoBFilter(int size, double dir, double len, double width)
  {
    // generate huge (int-)matrix (quasi boolean)
    int xsize = sizeFactor * size;
    double xwidth = 0.5 * width * sizeFactor;
    double xlen = 0.5 * len * sizeFactor;
    matrix<double> hm(xsize, xsize);

    int mitte = (xsize - 1) / 2;

    double cc = cos(-dir);
    double ss = sin(-dir);

    int ctp = 1; // small error, but avoids div by zero
    int ctm = 1;
    for (int yi = 0; yi < xsize; yi++)
      {
        double yr = yi - mitte;

        for (int xi = 0; xi < xsize; xi++)
          {
            double xr = xi - mitte;
            double yrot = xr * ss + yr * cc;
            double xrot = xr * cc - yr * ss;

            if (fabs(xrot) > xlen)
              hm[yi][xi] = 0;
            else if (fabs(yrot) < xwidth)
              {
                hm[yi][xi] = 1;
                ctp++;
              }
            else
              {
                hm[yi][xi] = -1;
                ctm++;
              }
          }
      }

    double valm = -1.0 / ctm;
    double valp = 1.0 / ctp;
    // std::cout << ctm << " " <<ctp << std::endl;
    // std::cout << valm << " " <<valp << std::endl;

    // replace +1 and -1 with values that make sum == 0
    for (int yi = 0; yi < xsize; yi++)
      for (int xi = 0; xi < xsize; xi++)
        {
          int val = hm[yi][xi];
          if (val < 0)
            hm[yi][xi] = valm;
          else if (val > 0)
            hm[yi][xi] = valp;
        }

    // sum of positive values == -(sum of negative values) == ctp * ctm

    // convert huge matrix to size of filter
    matrix<double> mask(size, size);
    reducematrix(hm, mask);
    /*
    int size2 = size * size;

    for (int yi = 0; yi < size; yi++)
      for (int xi = 0; xi < size; xi++)
        {
          int mv = mask[yi][xi];
          mv = mv * size2 - ctp;
          mask[yi][xi] = mv;
        }
    */
    return LsiFilter(mask);

  }

  LsiFilter makeOrientedEdgeFilter(int size, double alpha, double rad)
  {
    int xsize = sizeFactor * size;
    double xrad = 0.5 * rad * sizeFactor;
    double xrad2 = xrad * xrad;

    matrix<double> hm(xsize, xsize);
    int center = (xsize - 1) / 2;

    double cc = cos(-alpha);
    double ss = sin(-alpha);

    int ctp = 1;
    int ctm = 1;

    for (int yi = 0; yi < xsize; yi++)
      {
        double yr = yi - center;

        for (int xi = 0; xi < xsize; xi++)
          {
            double xr = xi - center;

            // rotation by alpha
            double yrot = xr * ss + yr * cc;
            double xrot = xr * cc - yr * ss;

            if (xrot * xrot + yrot * yrot < xrad2)
              {
                if (xrot < 0)
                  {
                    ctm++;
                    hm[yi][xi] = -1;
                  }
                else if (xrot > 0)
                  {
                    ctp++;
                    hm[yi][xi] = 1;
                  }
                else
                  {
                    hm[yi][xi] = 0;
                  }
              }
            else
              {
                hm[yi][xi] = 0;
              }
          }
      }

    double valp = 1.0 / ctp;
    double valm = -1.0 / ctm;
    for (int yi = 0; yi < xsize; yi++)
      for (int xi = 0; xi < xsize; xi++)
        {
          double val = hm[yi][xi];
          if (val > 0)
            hm[yi][xi] = valp;
          else if (val < 0)
            hm[yi][xi] = valm;
        }
    matrix<double> mask(size, size);
    reducematrix(hm, mask);

    return LsiFilter(mask);
  }

  void makeLoG(matrix<double>& f, double sigma)
  {
    int sx = f.cols();
    int sy = f.rows();
    int xm = sx / 2;
    int ym = sy / 2;
    double sigma2 = sigma * sigma;

    double sumPositive = 0;

    for (int y = 0; y < sy; y++)
      {
        double dy = y - ym;
        double dy2 = dy * dy;

        for (int x = 0; x < sx; x++)
          {
            double dx = x - xm;
            double r2 = dx * dx + dy2;

            double ex = - r2 / (2 * sigma2);
            double fac = 1 + ex;
            double c = fac * exp(ex);

            f[y][x] = c;
            if (c > 0)
              {
                sumPositive += c;
              }
          }
      }

    for (int y = 0; y < sy; y++)
      for (int x = 0; x < sx; x++)
        {
          f[y][x] /= sumPositive;
        }
  }

  void makeGauss(matrix<double>& f, double sigma)
  {
    int sx = f.cols();
    int sy = f.rows();
    int xm = sx / 2;
    int ym = sy / 2;
    double sigma2 = sigma * sigma;

    double sumPositive = 0;

    for (int y = 0; y < sy; y++)
      {
        double dy = y - ym;
        double dy2 = dy * dy;

        for (int x = 0; x < sx; x++)
          {
            double dx = x - xm;
            double r2 = dx * dx + dy2;

            double ex = -r2 / (2 * sigma2);
            double c = exp(ex);

            f[y][x] = c;
            if (c > 0)
              {
                sumPositive += c;
              }
          }
      }

    for (int y = 0; y < sy; y++)
      for (int x = 0; x < sx; x++)
        {
          f[y][x] /= sumPositive;
        }
  }

#define FNAME "makeMexicanHatFilter"
  LsiFilter makeMexicanHatFilter(int size, double sigma)
  {
    if (size < 0 || sigma < 0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (size == 0)
      {
        size = (int)(sigma * 7) | 1;
      }

    matrix<double> fc(size, size);
    makeLoG(fc, sigma);
    return LsiFilter(fc);
  }
#undef FNAME

#define FNAME "makeGaussFilter"
  LsiFilter makeGaussFilter(int size, double sigma)
  {
    if (size < 0 || sigma < 0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (size == 0)
      {
        size = (int)(sigma * 5) | 1;
      }

    matrix<double> fc(size, size);
    makeGauss(fc, sigma);
    return LsiFilter(fc);
  }
#undef FNAME
}
