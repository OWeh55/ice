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

  void ObjectMatcher::setObject(int which, const Contur& c)
  {
    vector<Point>& p1 = (which == 0) ? pl1 : pl2 ;
    p1.clear();
    /*
    for (int i = 0; i < c.Number(); ++i)
      {
        p1.push_back(c.getPoint(i));
      }
    */
    // since "normal" contur point are inner points of the object,
    // we use pairs of inner and outer points and place the
    // limiting polygon between these.

    vector<IPoint> pi, po;
    c.getPairs(pi, po);
    for (int i = 0; i < pi.size(); i++)
      {
        // extraction and type conversion of points
        Point iPoint = pi[i];
        Point oPoint = po[i];
        p1.push_back((iPoint + oPoint) * 0.5);
      }

    state |= (which == 0) ? st_first : st_second ;
    state &= st_first | st_second ;
  }

  void ObjectMatcher::setObject(int which, const IMatrix& m)
  {
    vector<Point>& p1 = (which == 0) ? pl1 : pl2 ;
    p1.clear();
    for (int i = 0; i < m.rows(); ++i)
      {
        p1.push_back(Point(m[i][0], m[i][1]));
      }
    state |= (which == 0) ? st_first : st_second ;
    state &= st_first | st_second ;
  }

  void ObjectMatcher::setObject(int which, const Matrix& m)
  {
    vector<Point>& p1 = (which == 0) ? pl1 : pl2 ;
    p1.clear();
    for (int i = 0; i < m.rows(); ++i)
      {
        p1.push_back(Point(m[i][0], m[i][1]));
      }
    state |= (which == 0) ? st_first : st_second ;
    state &= st_first | st_second ;
  }

  void ObjectMatcher::setObject(int which, const vector<IPoint>& pl)
  {
    vector<Point>& p1 = (which == 0) ? pl1 : pl2 ;
    p1.clear();
    for (int i = 0; i < (int)pl.size(); ++i)
      {
        p1.push_back(Point(pl[i]));
      }
    state |= (which == 0) ? st_first : st_second ;
    state &= st_first | st_second ;
  }

  void ObjectMatcher::setObject(int which, const vector<Point>& pl)
  {
    vector<Point>& p1 = (which == 0) ? pl1 : pl2 ;
    p1.clear();
    for (int i = 0; i < (int)pl.size(); ++i)
      {
        p1.push_back(pl[i]);
      }
    state |= (which == 0) ? st_first : st_second ;
    state &= st_first | st_second ;
  }

#define FNAME "ObjectMatcher::interpolObject"
  void interpolatePointList(vector<Point>& pl, double distance = 1.0)
  {
    bool changed = true;
    while (changed)
      {
        changed = false;
        vector<Point> pl_new;
        for (int i = 0; i < (int)pl.size(); ++i)
          {
            int i1 = i + 1;
            if (i1 >= (int)pl.size())
              {
                i1 = 0;
              }
            pl_new.push_back(pl[i]);

            if ((pl[i1] - pl[i]).r() > distance)
              {
                pl_new.push_back(0.5 * (pl[i] + pl[i1]));
                changed = true;
              }
          }
        swap(pl, pl_new);
      }
  }

  void ObjectMatcher::interpolObject(int which, double distance)
  {
    if (which == 0)
      {
        if ((state & st_first) == 0)
          throw IceException(FNAME, M_EMPTY_POINTLIST);
        interpolatePointList(pl1, distance);
        state &= ~st_references;
      }
    else
      {
        if ((state & st_second) == 0)
          throw IceException(FNAME, M_EMPTY_POINTLIST);
        interpolatePointList(pl2, distance);
        state &= ~st_references;
      }
  }
#undef FNAME

#define FNAME "ObjectMatcher::calcDistances"
  void ObjectMatcher::calcDistances(matrix<double>& dm) const
  {
    if ((state & (st_first | st_second)) != (st_first | st_second))
      throw IceException(FNAME, M_EMPTY_POINTLIST);

    matrix<double> hu_p1;

    if (ObjectPointFeatures(pl1, hu_p1, trmode) != 0)
      throw IceException(FNAME, M_OBJECT_FEATURE);

    matrix<double> hu_p2;
    if (ObjectPointFeatures(pl2, hu_p2, trmode) != 0)
      throw IceException(FNAME, M_OBJECT_FEATURE);

    int n1 = hu_p1.rows();
    int n2 = hu_p2.rows();

    dm.resize(n1, n2);
    // Nun in die Matrix dm die "Abstände" zwischen den Hu-Invarianten
    // aller Punkte eintragen

    DistanceMatrix(hu_p1, hu_p2, dm, distance_measure);
  }
#undef FNAME
#define FNAME "ObjectMatcher::calcReferences"
  void ObjectMatcher::calcReferences() const
  {
    if ((state & (st_first | st_second)) != (st_first | st_second))
      throw IceException(FNAME, M_EMPTY_POINTLIST);

    if (pl1.size() >= pl2.size())
      {
        calcReferences(pl1, pl2);
      }
    else
      {
        calcReferences(pl2, pl1);
        for (int i = 0; i < (int)references.rows(); ++i)
          {
            std::swap(references[i][0], references[i][1]);
          }
      }
    state |= st_references;
  }

  void ObjectMatcher::calcReferences(const vector<Point>& p1,
                                     const vector<Point>& p2) const
  {
    // private version - no parameter check
    // expects p1 to be the longer list

    matrix<double> hu_p1;
    if (ObjectPointFeatures(p1, hu_p1, trmode) != 0)
      throw IceException(FNAME, M_OBJECT_FEATURE);

    matrix<double> hu_p2;
    if (ObjectPointFeatures(p2, hu_p2, trmode) != 0)
      throw IceException(FNAME, M_OBJECT_FEATURE);

    int n1 = hu_p1.rows();
    int n2 = hu_p2.rows();

    // auf pl1 steht die längere Liste
    matrix<double> distance(n1, n2);

    // Nun in die Matrix distance die "Abstände" zwischen den Hu-Invarianten
    // aller Punkte eintragen

    DistanceMatrix(hu_p1, hu_p2, distance, distance_measure);

    // minimalen Weg suchen
    // lo -> ru
    double min1 = CheapestWayLeftToRight(distance, 1, references);

    if (reflection)
      {
        // lu -> ro
        matrix<int> ref2;
        double min2 = CheapestWayLeftToRight(distance, -1, ref2);
        if (min2 < min1)
          {
            min1 = min2;
            references = ref2;
          }
      }
    eval = min1;
    state |= st_references;
  }
#undef FNAME
#define FNAME "ObjectMatcher::getEvaluation"
  double ObjectMatcher::getEvaluation() const
  {
    if ((state & (st_first | st_second)) != (st_first | st_second))
      throw IceException(FNAME, M_EMPTY_POINTLIST);
    if ((state & st_references) == 0)
      {
        calcReferences();
      }
    return eval;
  }
#undef FNAME
#define FNAME "ObjectMatcher::getReferences"
  void ObjectMatcher::getReferences(std::vector<int>& i1, std::vector<int>& i2) const
  {
    if ((state & (st_first | st_second)) != (st_first | st_second))
      throw IceException(FNAME, M_EMPTY_POINTLIST);
    if ((state & st_references) == 0)
      {
        calcReferences();
      }
    i1.resize(references.rows());
    i2.resize(references.rows());
    for (int i = 0; i < (int)references.rows(); ++i)
      {
        i1[i] = references[i][0];
        i2[i] = references[i][1];
      }
  }
#undef FNAME
#define FNAME "ObjectMatcher::getOrderedLists"
  void ObjectMatcher::getOrderedLists(vector<Point>& opl1, vector<Point>& opl2) const
  {
    if ((state & (st_first | st_second)) != (st_first | st_second))
      throw IceException(FNAME, M_EMPTY_POINTLIST);
    if ((state & st_references) == 0)
      {
        calcReferences();
      }
    opl1.resize(references.rows());
    opl2.resize(references.rows());
    for (int i = 0; i < (int)references.rows(); ++i)
      {
        opl1[i] = pl1[references[i][0]];
        opl2[i] = pl2[references[i][1]];
      }
  }
#undef FNAME
#define FNAME "ObjectMatcher::getTrafo"
  Trafo ObjectMatcher::getTrafo(int optmode) const
  {
    try
      {
        Trafo res;
        vector<Point> OrderedPointList1;
        vector<Point> OrderedPointList2;
        getOrderedLists(OrderedPointList1, OrderedPointList2);

        switch (optmode)
          {
          case MM_LINEAR:
            res = MatchPointlistsLinOpt(OrderedPointList1, OrderedPointList2,
                                        trmode);
            break;
          case MM_SQUARE:
            res = matchPointLists(OrderedPointList1, OrderedPointList2,
                                  trmode);
            break;
          default:
            throw IceException(FNAME, M_WRONG_MODE);
          }

        return res;
      }
    RETHROW;
  }
}
#undef FNAME
