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
#include <vector>
#include <iomanip>
#include <algorithm>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "vectordistance.h"
#include "numbase.h"
#include "analygeo.h"
#include "Vector.h"
#include "Matrix.h"
#include "geo.h"
#include "momente.h"
#include "moments.h"
#include "simplex.h"
#include "lists.h"

#include "omatch.h"
using namespace std;
namespace ice
{

  inline int MyMod(int a, int b)
  {
    return ((a % b) + b) % b;
    /*
    // very special (optimized) modulo.
    // works only, if b > 0
    // and a within [-b..2b)
    if (a < 0)
      {
        return a + b;
      }
    if (a >= b)
      {
        return a - b;
      }
    return a;
    */
  }

  template<class Tmat, class Tref>
  double CheapestWayLeftToRight(const Tmat& distance,
                                int maindir, Tref& ref)
  {
    // find cheapest way
    // distance.cols() must be greater than distance.rows()

    int ny = distance.cols();
    int nx = distance.rows();

    matrix<signed char> dir(nx, ny);

    vector<double> min(ny);

    for (int y = 0; y < ny; y++)   // first column, all rows
      {
        min[y] = distance[0][y];
      }

// possible changes of y
    int dy1 = (maindir > 0) ? -1 : 1;
    int dy2 = (maindir > 0) ? -2 : 2;

    for (int x = 1; x < nx; ++x)   // all columns
      {
        vector<double> next_min(ny);
        for (int y = 0; y < ny; y++)   // all rows
          {
            int y1 = MyMod(y + dy1, ny);
            int y2 = MyMod(y + dy2, ny);

            next_min[y] = min[y1];
            int dy = dy1;

            if (min[y] < next_min[y])
              {
                next_min[y] = min[y];
                dy = 0;
              }

            if (min[y2] < next_min[y])
              {
                next_min[y] = min[y2];
                dy = dy2;
              }

            next_min[y] += distance[x][y];

            dir[x][y] = dy;
          }

        swap(min, next_min);
      }

    double minimum = min[0];
    int miny = 0;

    for (int y = 1; y < ny; y++)
      {
        if (min[y] < minimum)
          {
            minimum = min[y];
            miny = y;
          }
      }

    ref.resize(nx, 2);

    for (int x = nx - 1; x >= 0; x--)
      {
        ref[x][0] = x;
        ref[x][1] = miny;
        int dy = dir[x][miny]; // "direction" to go
        miny = MyMod(miny + dy, ny); // -> next point (x-1,miny)
      }

    return minimum;
  }

  int HomoAffinHuInvar(Moments m,
                       std::vector<double>& hu, Point p, int tmode)
  {
    hu.resize(9);

    tmode = tmode | TRM_REFLECTION; // its impossible to block reflections here

    // Aenderung des Umlaufsinnes, wenn m00 < 0
    m = m.NormalizeSign();

    // Berechnung der Momente bezüglich des aktuellen Punktes
    m = m.Translate(-p);

    if ((tmode == TRM_PROJECTIVE) || (tmode == TRM_AFFINE))
      {
        // Normalisierung der Scherung
        m = m.NormalizeXShearing();
      }

    if ((tmode == TRM_PROJECTIVE) || (tmode == TRM_AFFINE))
      {
        // anisotrope Skalierungsnormierung
        m = m.NormalizeScaling();
      }
    else
      {
        if (tmode == TRM_SIMILARITY)
          {
            // isotrope Skalierung
            m.NormalizeScaling(Moments::isotropic);
          }
      }

    hu = m.HuInvariants();
    return OK;
  }

  int ObjectPointFeatures(const std::vector<Point>& pl,
                          matrix<double>& feat,
                          int tmode)
  {
    feat.resize(pl.size(), 9);

    Moments m(pl);

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        vector<double> ff;
        if (HomoAffinHuInvar(m, ff, pl[i], tmode) != 0)
          {
            return 1;
          }
        for (int k = 0; k < 9; ++k)
          {
            feat[i][k] = ff[k];
          }
      }

    return 0;
  }

#define FNAME "TimeWarp"
  double CheapestWayLeftToRight(const Matrix& distance,
                                int maindir, IMatrix& ref)
  {
    // find cheapest way
    // distance.cols() must be greater than distance.rows()

    int ny = distance.cols();
    int nx = distance.rows();

    vector<vector<signed char> > dir(nx, vector<signed char>(ny));

    int minarray;

    vector<double> min1(ny);
    vector<double> min2(ny);

    //    int y1, y2;
    int dy1, dy2;

    for (int y = 0; y < ny; y++)   // first column, all rows
      {
        min1[y] = distance[0][y];
      }

    minarray = 1;

    if (maindir > 0)
      {
        dy1 = -1;
        dy2 = -2;
      }
    else
      {
        dy1 = 1;
        dy2 = 2;
      }

    for (int x = 1; x < nx; x++)   // all columns
      {
        for (int y = 0; y < ny; y++)   // all rows
          {
            int y1 = MyMod(y + dy1, ny);
            int y2 = MyMod(y + dy2, ny);

            min2[y] = min1[y1];
            int dy = dy1;

            if (min1[y] < min2[y])
              {
                min2[y] = min1[y];
                dy = 0;
              }

            if (min1[y2] < min2[y])
              {
                min2[y] = min1[y2];
                dy = dy2;
              }

            min2[y] += distance.at(x).at(y);

            dir[x][y] = dy;
          }

        minarray = 2;
        x++;

        if (x < nx)
          {
            for (int y = 0; y < ny; y++)   // all rows
              {
                int y1 = MyMod(y + dy1, ny);
                int y2 = MyMod(y + dy2, ny);

                min1[y] = min2[y1];
                int dy = dy1;

                if (min2[y] < min1[y])
                  {
                    min1[y] = min2[y];
                    dy = 0;
                  }

                if (min2[y2] < min1[y])
                  {
                    min1[y] = min2[y2];
                    dy = dy2;
                  }

                min1[y] += distance.at(x).at(y);

                dir[x][y] = dy;
              }

            minarray = 1;
          }
      }

    if (minarray == 1)
      {
        for (int i = 0; i < ny; i++)
          {
            min2[i] = min1[i];
          }
      }

    double minimum = min2[0];
    int miny = 0;

    for (int y = 1; y < ny; y++)
      {
        if (min2[y] < minimum)
          {
            minimum = min2[y];
            miny = y;
          }
      }

    ref = IMatrix(nx, 2);

    for (int x = nx - 1; x >= 0; x--)
      {
        ref[x][0] = x;
        ref[x][1] = miny;
        int dy = dir[x][miny]; // "direction" to go
        miny = MyMod(miny + dy, ny); // -> next point (x-1,miny)
      }

    return minimum;
  }

  double TimeWarp(const Matrix& feat1, const Matrix& feat2,
                  IMatrix& ref)
  {
    double ret = -1;
    int n1, n2;
    n1 = feat1.rows();
    n2 = feat2.rows();

    if (n1 < n2)
      {
        // first list is shorter than second
        // call TimeWarp again with exchanged parameters
        ret = TimeWarp(feat2, feat1, ref);

        // correct reference list
        for (int i = 0; i < ref.rows(); i++)
          {
            std::swap(ref[i][0], ref[i][1]);
          }

        return ret;
      }

    if (feat1.cols() != feat2.cols())
      throw IceException(FNAME, M_MATRIXFORMAT);

    int dim = feat1.cols();
    Matrix distance(n1, n2);
    double max = 0.0;
    double dist;

    Vector am(dim);

    // calculating distance field
    for (int i = 0; i < n1; ++i)
      {
        am = feat1.at(i);

        for (int j = 0; j < n2; ++j)
          {
            dist = 0.0;

            for (int k = 0; k < dim; k++)
              {
#ifdef EUCLIDEAN
                dist += Sqr(am.at(k) - feat2.at(j).at(k));
              }
            dist = sqrt(dist);
#else
                dist += fabs(am.at(k) - feat2.at(j).at(k));
              }
#endif
            distance.at(i).at(j) = dist;

            if (dist > max)
              {
                max = dist;
              }
          }
      }

    IMatrix ref1, ref2;
    double min1, min2;

    // search minimal way
    // lo -> ru
    min1 = CheapestWayLeftToRight(distance, 1, ref1);
    // lu -> ro
    min2 = CheapestWayLeftToRight(distance, -1, ref2);

    if (min1 < min2)
      {
        ret = min1;
        ref = ref1;
      }
    else
      {
        ret = min2;
        ref = ref2;
      }

    return ret;
  }
}
#undef FNAME
