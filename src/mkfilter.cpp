/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "matrixtemplate.h"
#include "MatrixAlgebra.h"
#include "lsifilter.h"

namespace ice
{
// functions for generation of LSIFilters

#define FNAME "makePolynomFilter"
#if 0
  static int powi(int b, int e)
  {
    int p = 1;

    for (int i = 0; i < e; i++)
      {
        p *= b;
      }

    return p;
  }
#else
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
#endif
  static int kidx(int n, int i, int j)
  {
    int idx = 0;

    for (int k = 0; k < j; k++)
      {
        idx += n + 1 - k;
      }

    return idx + i;
  }

  LSIFilter makePolynomFilter(int size, int grad, int ii, int jj)
  {
    // Parametertest
    if (
      ((size & 1) == 0) ||
      (ii + jj > grad) ||
      (grad < 0) ||
      (ii < 0) ||
      (jj < 0)
    )
      throw IceException(FNAME, M_WRONG_PARAM);

    int m = size;
    Matrix f(m * m, (grad + 1) * (grad + 2) / 2);

    for (int k = 0; k < m; k++)
      for (int l = 0; l < m; l++)
        {
          int zidx = k * m + l;

          for (int i = 0; i <= grad; i++)
            for (int j = 0; j + i <= grad; j++)
              {
                f[zidx][kidx(grad, i, j)] = powi(k - m / 2, i) * powi(l - m / 2, j);
              }
        }

    Matrix ft = !f;
    Matrix ftf = ft * f;
    Matrix iftf = Inverse(ftf);
    Matrix az = iftf * ft;
    int zidx = kidx(grad, ii, jj);
    Matrix mask(m, m);

    for (int j = 0; j < m; j++)
      for (int i = 0; i < m; i++)
        {
          mask[i][j] = az[zidx][j * m + i];
        }

    return LSIFilter(mask);
  }
#undef FNAME

// these filters generate a (quasi boolean) huge matrix and
// reduce size later by combining elements by "counting"

  const int xfak = 13;

  static void reducematrix(const matrix<int>& huge, matrix<int>& small)
  {
    int ysize = small.rows();
    int xsize = small.cols();

    // size of matrix huge must be xfak*ysize,xfak*ysize!!
    for (int yi = 0; yi < ysize; yi++)
      {
        for (int xi = 0; xi < xsize; xi++)
          {
            int sum = 0;

            for (int xyi = yi * xfak; xyi < (yi + 1)*xfak; xyi++)
              for (int xxi = xi * xfak; xxi < (xi + 1)*xfak; xxi++)
                {
                  sum += huge[xyi][xxi];
                }

            small[yi][xi] = sum;
          }
      }
  }

  LSIFilter makeOrientedSmearFilter(int size, double dir, double len, double width)
  {
    // generate huge (int-)matrix (quasi boolean)
    int xsize = xfak * size;
    double xwidth = 0.5 * width * xfak;
    double xlen = 0.5 * len * xfak;
    matrix<int> hm(xsize, xsize);

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

    // convert huge matrix to size of filter
    matrix<int> mask(size, size);
    reducematrix(hm, mask);
    return LSIFilter(mask, ct);
  }

  LSIFilter makeOrientedDoBFilter(int size, double dir, double len, double width)
  {
    // generate huge (int-)matrix (quasi boolean)
    int xsize = xfak * size;
    double xwidth = 0.5 * width * xfak;
    double xlen = 0.5 * len * xfak;
    matrix<int> hm(xsize, xsize);

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

    // convert huge matrix to size of filter
    matrix<int> mask(size, size);
    reducematrix(hm, mask);

    int size2 = size * size;

    for (int yi = 0; yi < size; yi++)
      for (int xi = 0; xi < size; xi++)
        {
          int mv = mask[yi][xi];
          mv = mv * size2 - ct;
          mask[yi][xi] = mv;
        }

    return LSIFilter(mask, ct * size2);
  }

  LSIFilter makeOrientedEdgeFilter(int size, double dir, double rad)
  {
    int xsize = xfak * size;
    double xrad = 0.5 * rad * xfak;
    double xrad2 = xrad * xrad;

    matrix<int> hm(xsize, xsize);
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

            if (xrot * xrot + yrot * yrot < xrad2)
              {
                ct++;

                if (xrot < 0)
                  {
                    hm[yi][xi] = -1;
                  }
                else if (xrot > 0)
                  {
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

    matrix<int> mask(size, size);
    reducematrix(hm, mask);

    return LSIFilter(mask, ct);
  }
}
