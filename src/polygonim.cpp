/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
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
//
//   functions for the work with polygons
//
//   (C) m.schubert 1/96
//       Ortmann 10/99, 10/06, 02/09
//
//   All modern functions should use datatype Matrix as list of points, not
//   the type PointList

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "macro.h"
#include "analygeo.h"
#include "drawline.h"
#include "paint.h"
#include "Contur.h"
#include "simplex.h"
#include "distance.h"
#include "lists.h"
#include "fit.h"

#include "polygonim.h"

using namespace std;

namespace ice
{
  /******************************************************/
  /***oeffentliche Funktionen****************************/
  /******************************************************/
#define FNAME "isPolygon"
  bool isPolygon(const Matrix& m)
  {
    RETURN_IF_FAILED(CheckPolygon(m), false);

    int points = m.rows();

    Point dummypoint;

    for (int i = 0; i < points - 2; i++)
      {
        int i1 = i + 1;

        for (int j = i + 2; j < points; j++)
          {
            int j1 = (j + 1) % points;

            if (j1 != i)
              {
                if (Intersect(Point(m[i]), Point(m[i1]),
                              Point(m[j]), Point(m[j1]),
                              dummypoint))
                  return false;
              }
          }
      }

    return true;
  }
#undef FNAME

  /******************************************************/
#define FNAME "InsidePolygon"
  bool InsidePolygon(const Matrix& m, const Point& p)
  {
    RETURN_IF_FAILED(CheckPolygon(m), false);
    int res = false;
    PointList pl;
    IF_FAILED(pl = Matrix2PointList(m))
    {
      if (pl != NULL) FreePointList(pl);

      Message(FNAME, M_0, ERROR);
      return false;
    }
    double pp[2];
    pp[0] = p.x;
    pp[1] = p.y;
    IF_FAILED(res = InsidePolygon(pp, pl))
    {
      FreePointList(pl);
      Message(FNAME, M_0, ERROR);
      return false;
    }
    FreePointList(pl);
    return res;
  }
#undef FNAME
  /******************************************************/
#define FNAME "PolygonContur"
  Contur PolygonContur(const Matrix& pl)
  {
    int i;
    Contur c;
    RETURN_IF_FAILED(CheckPolygon(pl), c);
    c.SetStart(RoundInt(pl[0][0]), RoundInt(pl[0][1]));

    for (i = 1; i < pl.rows(); i++)
      {
        c.Add(RoundInt(pl[i][0]), RoundInt(pl[i][1]));
      }

    c.Add(RoundInt(pl[0][0]), RoundInt(pl[0][1]));
    return c;
  }
#undef FNAME
  /******************************************************/
#define FNAME "DistancePointEdge"
  double DistancePointEdge_core(double x, double y, const Matrix& pl, int i1, int i2)
  {
    // point (x,y)
    // edge pl[i1] -- pl[i2]
    // no error check here
    //    cout << "*" << endl;
    //    LineSeg ls( pl[i1][0], pl[i1][1], pl[i2][0], pl[i2][1]);
    //    return ls.Distance(Point(x,y));
    return DistancePointLinesegment(x, y, pl[i1][0], pl[i1][1], pl[i2][0], pl[i2][1]);
  }

  double DistancePointEdge(double x, double y, const Matrix& pl, int i1, int i2)
  {
    // point (x,y)
    // edge pl[i1] -- pl[i2]
    RETURN_IF_FAILED(CheckPolygon(pl), 0.0);
    int ms = pl.rows();
    i1 = (i1 % ms + ms) % ms;
    i2 = (i2 % ms + ms) % ms;
    return DistancePointEdge_core(x, y, pl, i1, i2);
  }

  double DistancePointEdge(double x, double y, const Matrix& pl, int i)
  {
    // point (x,y)
    // edge pl[i] -- pl[i+1]
    RETURN_IF_FAILED(CheckPolygon(pl), 0.0);
    int ms = pl.rows();
    int i1 = (i % ms + ms) % ms;
    int i2 = ((i + 1) % ms + ms) % ms;
    return DistancePointEdge_core(x, y, pl, i1, i2);
  }

  double DistancePointEdge(const Matrix& pl, int i, int i1, int i2)
  {
    // point pl[i]
    // edge pl[i1] -- pl[i2]
    RETURN_IF_FAILED(CheckPolygon(pl), 0.0);
    int ms = pl.rows();
    i = (i % ms + ms) % ms;
    i1 = (i1 % ms + ms) % ms;
    i2 = (i2 % ms + ms) % ms;
    return DistancePointEdge(pl[i][0], pl[i][1], pl, i1, i2);
  }
#undef FNAME
#define FNAME "DistancePointPolygon"
  double DistancePointPolygon(double x, double y, const Matrix& pl, int& mine)
  {
    double mind = DBL_MAX;
    RETURN_IF_FAILED(CheckPolygon(pl), mind);
    mine = 0;

    for (int i = 0; i < pl.rows(); i++)
      {
        double d = DistancePointEdge(x, y, pl, i);

        if (d < mind)
          {
            mind = d;
            mine = i;
          }
      }

    return mind;
  }

  double DistancePointPolygon(double x, double y, const Matrix& pl)
  {
    int mine;
    return DistancePointPolygon(x, y, pl, mine);
  }
#undef FNAME
#define FNAME "DistancePolygonPolygonDir"
  double DistancePolygonPolygonDir(const Matrix& p1, const Matrix& p2, int mode)
  {
    RETURN_IF_FAILED(CheckPolygon(p1), 0.0);
    RETURN_IF_FAILED(CheckPolygon(p2), 0.0);

    double dist = DistancePointPolygon(p1[0][0], p1[0][1], p2);

    for (int i = 1; i < p1.rows(); i++)
      {
        double pdist = DistancePointPolygon(p1[i][0], p1[i][1], p2);

        switch (mode)
          {
          case DPP_MIN:

            if (pdist < dist) dist = pdist;

            break;
          case DPP_MAX:

            if (pdist > dist) dist = pdist;

            break;
          case DPP_MEAN:
            dist += pdist;
            break;
          default:
            Message(FNAME, M_WRONG_MODE, WRONG_PARAM);
            return 0.0;
          }
      }

    if (mode == DPP_MEAN) dist /= p1.rows();

    return dist;
  }
#undef FNAME

#define FNAME "DistancePolygonPolygon"
  double DistancePolygonPolygon(const Matrix& p1, const Matrix& p2, int mode, int pmode)
  {
    double dist1;
    IF_FAILED(dist1 = DistancePolygonPolygonDir(p1, p2, pmode))
    {
      Message(FNAME, M_WRONG_MODE, WRONG_PARAM);
      return 0.0;
    }

    if (mode == DPP_DIR) return dist1;

    double dist2;
    IF_FAILED(dist2 = DistancePolygonPolygonDir(p2, p1, pmode))
    {
      Message(FNAME, M_WRONG_MODE, WRONG_PARAM);
      return 0.0;
    }

    switch (mode)
      {
      case DPP_MIN:

        if (dist1 < dist2) return dist1;
        else return dist2;

      case DPP_MAX:

        if (dist1 > dist2) return dist1;
        else return dist2;

      case DPP_MEAN:
        return 0.5 * (dist1 + dist2);
      default:
        Message(FNAME, M_WRONG_MODE, WRONG_PARAM);
        return 0.0;
      }
  }
#undef FNAME
#define FNAME "DistanceConturPolygon"
  double DistanceConturPolygon(const Contur& c, const Matrix& p, int mode, int pmode)
  {
    Matrix pc;
    RETURN_IF_FAILED(pc = ConturPointlist(c, 1, false), 0.0);
    return DistancePolygonPolygon(pc, p, mode, pmode);
  }
#undef FNAME
#define FNAME "DrawPolygon"
  int DrawPolygon(const Matrix& pl, int val, const Image& img,
                  int marker, int size, int color)
  {
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(CheckPointlist(pl));

    for (int i = 0, j = 1; i < pl.rows(); i++, j++)
      {
        if (j >= pl.rows()) j = 0;

        int x1 = RoundInt(pl[i][0]);
        int y1 = RoundInt(pl[i][1]);
        int x2 = RoundInt(pl[j][0]);
        int y2 = RoundInt(pl[j][1]);
        RETURN_ERROR_IF_FAILED(Line(x1, y1, x2, y2, val, 0, img));

        if (img.inside(x1, y1) && (marker > 0))
          {
            int mcolor = color;

            if (mcolor < 0) mcolor = val;

            RETURN_ERROR_IF_FAILED(Marker(marker, x1, y1, mcolor, size, img));
          }
      }

    return OK;
  }
#undef FNAME

#define FNAME "ReducePolygon"
#if 1
  // helper classes
  class reduced_polym
  {
    // describes a polygon which consists of a subset of corners of
    // another polygon

  public:
    enum initmode_t {all, nothing, two};

  private:
    const Matrix& base;  // base polygon
    int basenr; // number of base corners

    vector<int> idx; // indices of subset corners


    vector<double> maxdist1; // maximal "distance" of an edge of red. poly
    vector<int> maxi1; // point with maximal distance to an edge
    double maxdist; // maximal maximal distance

    vector<double> maxdist2; // maximal "distance" of two combined edges

    int bnormal(int i) const
    {
      return (i % basenr + basenr) % basenr;
    }

    int bnormal2(int i, int refi) const
    {
      return bnormal(i - refi) + refi;
    }

    int rnormal(int i) const
    {
      int m = idx.size();
      return (i % m + m) % m;
    }

    double maxdistedge(int ii1, int ii2, int& maxind) const
    {
      int i1 = idx[rnormal(ii1)];
      int i2 = idx[rnormal(ii2)];

      double emax = 0;
      maxind = 0;

      int i2n = bnormal2(i2, i1);
      maxind = i1;

      for (int i = i1 + 1; i < i2n; i++)
        {
          int inormal = bnormal(i);

          double d = DistancePointEdge(base, inormal, i1, i2);

          if (d > emax)
            {
              emax = d;
              maxind = inormal;
            }
        }

      return emax;
    }

    double maxdistedge(int ii1, int ii2) const
    {
      int dummy;
      return maxdistedge(ii1, ii2, dummy);
    }

    void calcdist1(int i)
    {
      i = rnormal(i);
      double dist = maxdistedge(i, i + 1, maxi1[i]);
      maxdist1[i] = dist;

      if (dist > maxdist) maxdist = dist;
    }

    void calcdist2(int i)
    {
      i = rnormal(i);
      maxdist2[i] = maxdistedge(i, i + 2);
    }

    void init_distances()
    {
      maxdist = 0.0;
      maxdist1.resize(idx.size());
      maxi1.resize(idx.size());

      maxdist2.resize(idx.size());

      for (unsigned int i = 0; i < idx.size(); i++)
        {
          calcdist1(i);
          calcdist2(i);
        }
    }

    void recalcmaxdist()
    {
      maxdist = 0.0;

      for (unsigned int i = 0; i < idx.size(); i++)
        if (maxdist1[i] > maxdist)
          maxdist = maxdist1[i];
    }

    void insertcorner(int pos, int baseindex)
    {
      // inserts new corner _after_ pos
      pos = rnormal(pos);

      if (pos == (int)idx.size() - 1)
        {

          idx.push_back(baseindex);

          maxdist1.push_back(0);
          maxi1.push_back(0);

          maxdist2.push_back(0);

        }
      else
        {
          idx.insert(idx.begin() + pos + 1, baseindex);
          maxdist1.insert(maxdist1.begin() + pos + 1, 0);
          maxi1.insert(maxi1.begin() + pos + 1, 0);
          maxdist2.insert(maxdist2.begin() + pos + 1, 0);
        }

      calcdist1(pos);
      calcdist2(pos);

      calcdist2(pos - 1);

      calcdist1(pos + 1);
      calcdist2(pos + 1);
      // distances normally decrease here
      // we need to determine new maximum
      recalcmaxdist();
    }

    void deletecorner(int pos)
    {
      pos = rnormal(pos);
      idx.erase(idx.begin() + pos);

      maxdist1.erase(maxdist1.begin() + pos);
      maxi1.erase(maxi1.begin() + pos);

      maxdist2.erase(maxdist2.begin() + pos);

      calcdist1(pos - 1);
      calcdist2(pos - 1);

      calcdist2(pos - 2);
      // since distances increase here calcdist1 will update maxdist implicite
    }

  public:
    reduced_polym(const Matrix& basep, initmode_t initmode): base(basep), maxdist(0.0)
    {
      basenr = base.rows();

      switch (initmode)
        {
        case all:

          for (int i = 0; i < basenr; i++)
            idx.push_back(i);

          break;
        case two:
          idx.push_back(0);
          idx.push_back(basenr / 2);
          break;
        case nothing:
          // at the moment really nothing to do
          break;
        }

      init_distances();
    }

    double precision() const
    {
      return maxdist;
    }

    int size() const
    {
      return idx.size();
    }

    void split()
    {
      // search segment to split
      double maxdist = 0;
      unsigned int maxind = 0;
      unsigned int maxedge = 0;

      for (unsigned int i = 0; i < idx.size(); i++)
        {
          if (maxdist1[i] > maxdist)
            {
              maxdist = maxdist1[i];
              maxind  = maxi1[i];
              maxedge = i;
            }
        }

      // split now
      insertcorner(maxedge, maxind);
    }

    void merge()
    {
      // search segments to merge
      unsigned int minedge = 0;
      double mindist = maxdist2[0];

      for (unsigned int i = 0; i < idx.size(); i++)
        {
          if (maxdist2[i] < mindist)
            {
              mindist = maxdist2[i];
              minedge = i;
            }
        }

      deletecorner(minedge + 1);
    }

    void getPolygon(Matrix& poly) const
    {
      poly = Matrix(0, 2);

      // construct result matrix
      for (unsigned int i = 0; i < idx.size(); i++)
        {
          int ib = idx[i];
          poly.Append(Vector(base[ib][0], base[ib][1]));
        }
    }
  };
#endif
  Matrix ReducePolygon(const Matrix& pl, int n, int mode)
  {
    int apn  = pl.rows();
    int coln = pl.cols();

    Matrix result(0, coln);
    RETURN_IF_FAILED(CheckPolygon(pl), result);

    if (n < 3 || n > apn)
      {
        // reducing to less then 3 point does not make sense
        // "reducing" to more then original points does not make sense
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return result;
      }

    // already reduced enough
    if (apn == n) return pl;

    if (mode == 2)
      {
        reduced_polym rpl(pl, reduced_polym::two);

        while (rpl.size() < n)
          rpl.split();

        while (rpl.size() < n * 5 / 3 && rpl.size() < apn)
          rpl.split();

        while (rpl.size() > n)
          rpl.merge();

        rpl.getPolygon(result);
      }
    else if (mode == 1)
      {
        reduced_polym rpl(pl, reduced_polym::all);

        while (rpl.size() > n) // reduce to wished number of edges
          {
            rpl.merge();
          }

        rpl.getPolygon(result);
      }

    return result;
  }

  Matrix ReducePolygon(const Contur& c, int n, int mode)
  {
    Matrix fpl;

    if (n < 3)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return fpl;
      }

    // make pointlist from contur
    fpl = ConturPointlist(c, 1, false);

    return ReducePolygon(fpl, n, mode);
  }


  Matrix ReducePolygonPrecision(const Matrix& pl, double prec, int mode)
  {
    int coln = pl.cols();

    Matrix result(0, coln);
    RETURN_IF_FAILED(CheckPolygon(pl), result);

    if (prec < 0)
      {
        // reducing to less then 3 point does not make sense
        // "reducing" to more then original points does not make sense
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return result;
      }

    if (mode == 2)
      {
        reduced_polym rpl(pl, reduced_polym::two);
        rpl.split(); // we need at least 3 corners

        while (rpl.precision() > prec)
          {
            rpl.split();
          }

        rpl.getPolygon(result);
      }
    else if (mode == 1)
      {
        reduced_polym rpl(pl, reduced_polym::all);

        while (rpl.size() > 3 && rpl.precision() < prec) // reduce to given precision
          {
            rpl.merge();
          }

        if (rpl.precision() > prec)
          rpl.split(); // undo last step

        rpl.getPolygon(result);
      }

    return result;
  }

  Matrix ReducePolygonPrecision(const Contur& c, double prec, int mode)
  {
    Matrix fpl;

    if (prec < 0.0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return fpl;
      }

    // make pointlist from contur
    fpl = ConturPointlist(c, 1, false);

    return ReducePolygonPrecision(fpl, prec, mode);
  }

#undef FNAME
#define FNAME "FindNearestCorner"
  int FindNearestCorner(double x, double y, const Matrix& pl)
  {
    RETURN_IF_FAILED(CheckPolygon(pl), 0);
    int mini = 0;
    double mdist = Distance(x, y, pl[0][0], pl[0][1]);

    for (int i = 0; i < pl.rows(); i++)
      {
        double da = Distance(x, y, pl[i][0], pl[i][1]);

        if (da < mdist)
          {
            mdist = da;
            mini = i;
          }
      }

    return mini;
  }
#undef FNAME
#define FNAME "FitPolygonPointlist"
  vector<Point> FitPolygonPointlist(const vector<Point>& poly, const vector<Point>& pl,
                                    int step, int count)
  {
    // fit given "polygon" poly to pointlist pl
    vector<Point> res(poly);
    int nPolyPoint = poly.size();

    Matrix polym(nPolyPoint, 2);
    for (int i = 0; i < nPolyPoint; i++)
      {
        polym[i][0] = poly[i].x;
        polym[i][1] = poly[i].y;
      }

    Matrix edge = Matrix(nPolyPoint, 2);

    int nOriginalPoints = pl.size();
    Matrix cl(nOriginalPoints, 2);
    for (int i = 0; i < nOriginalPoints; i++)
      {
        cl[i][0] = pl[i].x;
        cl[i][1] = pl[i].y;
      }

    bool changed = true;

    IVector oldcutpoint(4);

    for (int ct = 0; changed && ct < count; ct++)
      {
        IVector cutpoints(nPolyPoint);

        for (int i = 0; i < nPolyPoint; i++)
          {
            double x = polym[i][0];
            double y = polym[i][1];
            int j = FindNearestCorner(x, y, cl);
            cutpoints[i] = j;
          }

        // check if cutpoints are ordered
        int next = cutpoints[0];

        for (int i = 0; i < cutpoints.Size(); i++)
          {
            int idx = cutpoints[i];

            while (idx < next)
              idx += nOriginalPoints;

            next = idx + 1; // every edge must have at least 2 corresponding points
          }

        if (next > cutpoints[0] + nOriginalPoints)
          {
            if (ct == 0)
              Message(FNAME, M_WRONG_START, WRONG_PARAM);

            return res;
          }

        changed = oldcutpoint != cutpoints;


        if (changed)
          {
            oldcutpoint = cutpoints;

            // fit lines to the contur segments

            for (int j = 0; j < nPolyPoint; j++) // every edge
              {
                int j1 = (j + 1) % nPolyPoint;
                Matrix pl1(0, 2);
                int c1 = cutpoints[j];
                int c2 = cutpoints[j1];

                if (c2 < c1) c2 += nOriginalPoints;

                for (int i = c1; i <= c2; i++) // points of that segment
                  {
                    int in = i % nOriginalPoints;
                    pl1.Append(Vector(cl[in][0], cl[in][1]));
                  }
                // cout << pl1.rows() << endl;
                if (step < 0)
                  FitLineLinOpt(pl1, edge[j][0], edge[j][1]);
                else
                  FitLine(pl1, edge[j][0], edge[j][1], step);
              }

            // calculate new corners as intersection of "edges"
            for (int j = 0; j < nPolyPoint; j++)
              {
                double p1, p2, phi1, phi2;
                double p[2];
                int j1 = j - 1;

                if (j1 < 0) j1 = nPolyPoint - 1;

                phi1 = edge[j][1];
                p1 = edge[j][0];
                phi2 = edge[j1][1];
                p2 = edge[j1][0];

                if (IntersecLine(p1, phi1, p2, phi2, p) != OK)
                  {
                    // should not happen !?
                    Message(FNAME, "Can´t fit", ERROR);
                    return res;
                  }

                polym[j][0] = p[0];
                polym[j][1] = p[1];
              }
          }
      } // for (ct=0...

    for (int i = 0; i < nPolyPoint; i++)
      {
        res[i].x = polym[i][0];
        res[i].y = polym[i][1];
      }
    return res;
  }


  Matrix FitPolygonContur(const Matrix& pl, const Contur& c, int step, int count)
  {
    // fit given polygon pl to contur c
    Matrix res;

    RETURN_IF_FAILED(CheckPolygon(pl), res);

    int pnr = pl.rows();

    res = pl;

    Matrix edge = Matrix(pnr, 2);

    Matrix cl = ConturPointlist(c);
    int cnr = cl.rows();

    bool changed = true;

    IVector oldcutpoint(4);

    for (int ct = 0; changed && ct < count; ct++)
      {
        IVector cutpoints;

        for (int i = 0; i < pnr; i++)
          {
            double x = res[i][0];
            double y = res[i][1];
            int j = FindNearestCorner(x, y, cl);
            cutpoints.Append(j);
          }

        // check if cutpoints are ordered
        int next = cutpoints[0];

        for (int i = 0; i < cutpoints.Size(); i++)
          {
            int idx = cutpoints[i];

            while (idx < next) idx += cnr;

            next = idx + 2; // every edge must have at least 2 corresponding contur points
          }

        if (next > cutpoints[0] + cnr)
          {
            if (ct == 0)
              Message(FNAME, M_WRONG_START, WRONG_PARAM);

            return res;
          }

        changed = oldcutpoint != cutpoints;

        oldcutpoint = cutpoints;

        if (changed)
          {
            // fit lines to the contur segments

            for (int j = 0; j < pnr; j++) // every edge
              {
                int j1 = (j + 1) % pnr;
                Matrix pl1(0, 2);
                int c1 = cutpoints[j];
                int c2 = cutpoints[j1];

                if (c2 < c1) c2 += cnr;

                for (int i = c1; i < c2; i++) // points of that segment
                  {
                    int in = i % cnr;
                    pl1.Append(Vector(cl[in][0], cl[in][1]));
                  }

                if (step < 0)
                  FitLineLinOpt(pl1, edge[j][0], edge[j][1]);
                else
                  FitLine(pl1, edge[j][0], edge[j][1], step);
              }

            // calculate new corners as intersection of "edges"
            for (int j = 0; j < pnr; j++)
              {
                double p1, p2, phi1, phi2;
                double p[2];
                int j1 = j - 1;

                if (j1 < 0) j1 = pnr - 1;

                phi1 = edge[j][1];
                p1 = edge[j][0];
                phi2 = edge[j1][1];
                p2 = edge[j1][0];

                if (IntersecLine(p1, phi1, p2, phi2, p) != OK)
                  {
                    // should not happen !?
                    Message(FNAME, "Can´t fit", ERROR);
                    return res;
                  }

                res[j][0] = p[0];
                res[j][1] = p[1];
              }
          }
      } // for (ct=0...

    return res;
  }
}
#undef FNAME
