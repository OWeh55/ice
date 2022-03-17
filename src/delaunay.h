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
#ifndef ICE_DELAUNAY_H
#define ICE_DELAUNAY_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "Point.h"
#include "triangle.h"
#include "lineseg.h"
#include "polygonalcurve.h"

///////////////////
// vertex

namespace ice
{
  class Delaunay
  {
    class Node: public Point
    {
      static const int noIndex = -111111;
    public:
      Node(): Point(0.0, 0.0), index(noIndex) {}
      Node(Point pnt, int idx = noIndex): Point(pnt), index(idx) {}
      Node(double x, double y, int idx = noIndex): Point(x, y), index(idx) {}

      friend void swap(Node& first, Node& second)
      {
        using std::swap;
        swap(first.x, second.x);
        swap(first.y, second.y);
        swap(first.index, second.index);
      }

      int index;
    };

    class DTriangle
    {
    public:
      /** standard constructor
       */
      DTriangle(): p1(-1, 0), p2(0, 1), p3(1, 0)
      {
        initCC();
      }
      /** constructor with given Points.
      \param p1 Corner of triangle
      \param p2 Corner of triangle
      \param p3 Corner of triangle
      */
      DTriangle(const Node& pp1, const Node& pp2, const Node& pp3):
        p1(pp1), p2(pp2), p3(pp3)
      {
        initCC();
      }

      bool inside(Point pp) const;
      bool isInsideCC(Point point) const;

      Node p1;
      Node p2;
      Node p3;

      Point CCCenter;   // center of circumcircle
      double CCR;       // radius of circumcircle
      double CCR2;      // radius of circumcircle, squared
      bool nonDegenerate;

      void initCC();
    };

    ///////////////////
    // edge

    class Edge
    {
    public:
      Edge() {}
      Edge(Node node1p, Node node2p)
        : node1(node1p), node2(node2p)
      {
      }

      Edge inversed() const
      {
        return Edge(node2, node1);
      }

      bool operator==(const Edge& e) const
      {
        return node1.index == e.node1.index && node2.index == e.node2.index;
      }

      bool isRightOf(Point p) const
      {
        return LineSeg(node1, node2, LineSeg::line).RightOf(p);
      }

      double relCross(Point p) const
      {
        return LineSeg(node1, node2, LineSeg::line).relCross(p);
      }

      double Distance(Point p) const
      {
        return LineSeg(node1, node2, LineSeg::segment).Distance(p);
      }

      friend std::ostream& operator<<(std::ostream& os, const Edge& edge)
      {
        os << edge.node1 << " -- " << edge.node2;
        return os;
      }

      Node node1;
      Node node2;
    };

///////////////////
// Delaunay

  public:
    // ctor calculates Delaunay triangulation
    Delaunay(const std::vector<Point>& nodesp);
    Delaunay(const PolygonalCurve& curv);
    virtual ~Delaunay() {}
    // get result in different forms
    // triangles / edges: getTriangles* / getEdges*
    // as coordinates / as index in original pointlist * / *I

    virtual void getTriangles(std::vector<Triangle>& output,
                              double limit = -1) const ;
    virtual void getEdges(std::vector<LineSeg>& output,
                          double limit = -1) const ;

    virtual void getTrianglesI(std::vector<std::vector<int> >& output,
                               double limit = -1) const ;
    virtual void getEdgesI(std::vector<std::vector<int> >& output,
                           double limit = -1) const ;

    virtual void getRegion(Region& region, double limit = -1) const;

    virtual void draw(const Image& img, int edgeValue = 1,
                      int fillValue = -1, double limit = -1) const;

  protected:
    void prepareNodes(std::vector<Point> points);

    void getEdges(const DTriangle& t, std::vector<Edge>& edges) const;
    void getEdges(std::vector<Edge>& edges) const;

    void triangulate();

    //    void cleanEdges(const std::vector<Edge> &e1,
    //              std::vector<Edge> &e2, bool both = true) const;

    void cleanEdges(std::vector<Edge>& e, bool both = true) const;

    std::vector<DTriangle> result;
    std::vector<Node> nodes;
  };
}
#endif
