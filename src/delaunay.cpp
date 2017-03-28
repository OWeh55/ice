/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#include "delaunay.h"
#include "draw.h"

using namespace std;

namespace ice
{
  bool Delaunay::DTriangle::inside(Point pp) const
  {
    if (!nonDegenerate)
      return false;
    // Compute vectors
    Point v0 = p2 - p1;
    Point v1 = p3 - p1;
    Point vp = pp - p1;

    // Compute dot products
    double dot00 = v0 * v0;
    double dot01 = v0 * v1;
    double dot0p = v0 * vp;
    double dot11 = v1 * v1;
    double dot1p = v1 * vp;

    // Compute barycentric coordinates
    double invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot0p - dot01 * dot1p) * invDenom;
    double v = (dot00 * dot1p - dot01 * dot0p) * invDenom;

    // Check if point is in triangle
    return (u >= 0) && (v >= 0) && (u + v <= 1);
  }

  bool Delaunay::DTriangle::isInsideCC(Point point) const
  {
    // returns if point is in the triangle's circumcircle.
    // a point exactly on the circle is also considered to be in the circle.

    double dist2 = (point - CCCenter).r2();
    return dist2 <= CCR2;
  }

  static void perpendicularBisector(Point p1, Point p2, Point& m, Point& s)
  {
    m = (p1 + p2) * 0.5; // mittelpunkt
    Point h12 = p2 - p1;
    s = Point(-h12.y, h12.x); // senkrechte richtung
  }

  void Delaunay::DTriangle::initCC()
  {
    Point m12, v12;
    perpendicularBisector(p1, p2, m12, v12);

    Point m13, v13;
    perpendicularBisector(p1, p3, m13, v13);

    double a1 = v12.x;
    double b1 = -v13.x;
    double i1 = m13.x - m12.x;
    double a2 = v12.y;
    double b2 = -v13.y;
    double i2 = m13.y - m12.y;

    double k1, k2;
    nonDegenerate = Solve2(a1, b1, i1, a2, b2, i2, k1, k2);
    if (nonDegenerate)
      {
        CCCenter = k1 * v12 + m12;

        CCR2 = (CCCenter - p1).r2();
        CCR = sqrt(CCR2);
      }
  }

  Delaunay::Delaunay(const vector<Point>& points)
  {
    prepareNodes(points);
    triangulate();
  }

  Delaunay::Delaunay(const PolygonalCurve& curv)
  {
    vector<Point> points(curv.size());

    for (int i = 0; i < (int)curv.size(); ++i)
      points[i] = curv[i];

    prepareNodes(points);
    triangulate();
  }

  bool PointEqual(const Point& n1, const Point& n2)
  {
    return n1 == n2;
  }

  void Delaunay::prepareNodes(vector<Point> points)
  {
    nodes.resize(points.size());
    for (int i = 0; i < (int)points.size(); ++i)
      nodes[i] = Node(points[i], i);

    sort(nodes.begin(), nodes.end());

    // remove duplicate points
    vector<Node>::iterator newEnd = std::unique(nodes.begin(), nodes.end());
    nodes.erase(newEnd, nodes.end());
  }

#if 0
  void Delaunay::cleanEdges(const vector<Edge>& edges,
                            vector<Edge>& edges2, bool both) const
  {
    // remove edges, that exist in list two times (in both directions)

    // marke edges to delete
    vector<bool> toDelete(edges.size(), false);

    for (int i = 0; i < ((int)edges.size()) - 1; ++i)
      {
        if (!toDelete[i]) // not deleted yet
          {
            Edge inverseEdge = edges[i].inversed();
            int foundIndex = -1;
            for (int k = i + 1; k < (int)edges.size() && foundIndex < 0 ; ++k)
              {
                if (edges[k] == inverseEdge)
                  foundIndex = k;
              }

            if (foundIndex >= 0)
              {
                // delete edge _and_ inverse edge ?
                if (both)
                  {
                    toDelete[i] = true;
                  }
                toDelete[foundIndex] = true;
              }
          }
      }

    edges2.clear();
    for (int i = 0; i < (int)edges.size(); ++i)
      {
        if (!toDelete[i])
          {
            edges2.push_back(edges[i]);
          }
      }
  }
#endif

  void Delaunay::cleanEdges(vector<Edge>& edges, bool both) const
  {
    // remove edges, that exist in list two times (in both directions)

    // marke edges to delete
    vector<bool> toDelete(edges.size(), false);

    for (int i = 0; i < ((int)edges.size()) - 1; ++i)
      {
        if (!toDelete[i]) // not deleted yet
          {
            Edge inverseEdge = edges[i].inversed();
            int foundIndex = -1;
            for (int k = i + 1; k < (int)edges.size() && foundIndex < 0 ; ++k)
              {
                if (edges[k] == inverseEdge)
                  foundIndex = k;
              }

            if (foundIndex >= 0)
              {
                // delete edge _and_ inverse edge ?
                if (both)
                  {
                    toDelete[i] = true;
                  }
                toDelete[foundIndex] = true;
              }
          }
      }

    int dest_idx = 0;
    for (int i = 0; i < (int)edges.size(); ++i)
      {
        if (!toDelete[i])
          {
            edges[dest_idx] = edges[i];
            ++dest_idx;
          }
      }
    edges.resize(dest_idx);
  }


  void Delaunay::triangulate()
  {
    result.clear();

    // "normal" triangles
    vector<DTriangle> Triangles;

    // "infinite Triangles" represented by the finite edge
    // we will start with 2 infinite triangles
    vector<Edge> infiniteTriangles(2);

    // this special node represents the "infinite point"
    // within edges to "infinity"
    const Node infiniteNode(0, 0, -1);

    // we use first and last point as starting configuration
    // there are no (finite) triangles, but 2 infinite triangles

    infiniteTriangles[0] = Edge(nodes[0], nodes.back());
    infiniteTriangles[1] = Edge(nodes.back(), nodes[0]);

    // insert remaining nodes
    for (int i = 1; i < (int)nodes.size() - 1; ++i)
      {
        const Node& currentNode = nodes[i];

        // remove triangles left of this x
        // we dont have to look at triangles again, that are
        // left of the current x coordinate.
        // left here means, that the circumcircle is left of this x
        // we do not remove any infinite triangle

        int dest_idx = 0;
        for (vector<DTriangle>::iterator it = Triangles.begin();
             it != Triangles.end();
             ++it)
          {
            if (currentNode.x < it->CCCenter.x - it->CCR)
              {
                // move triangle to result
                result.push_back(*it);
              }
            else
              {
                // keep triangle in  workset
                Triangles[dest_idx] = *it;
                ++dest_idx;
              }
          }
        Triangles.resize(dest_idx);

        // insert current node:
        // - remove triangles, which circumcircle contains node
        // - add new triangles with current node and edges of eliminated triangles

        // remove triangles, which circumcircle contains node
        vector<Edge> edges;

        dest_idx = 0;

        for (int i = 0; i < (int)infiniteTriangles.size(); ++i)
          {
            const Edge& current = infiniteTriangles[i];
            double relcross = current.relCross(currentNode);
            if (relcross > 0.0 || // right of edge
                (relcross == 0.0 && current.Distance(currentNode) == 0.0)) // on edge
              {
                edges.push_back(current);
                edges.push_back(Edge(current.node2, infiniteNode));
                edges.push_back(Edge(infiniteNode, current.node1));
              }
            else
              {
                infiniteTriangles[dest_idx] = current;
                ++dest_idx;
              }
          }
        infiniteTriangles.resize(dest_idx);

        dest_idx = 0;
        for (vector<DTriangle>::iterator it = Triangles.begin();
             it != Triangles.end(); ++it)
          {
            if (it->isInsideCC(currentNode))
              {
                // remove triangle
                // keep edges
                // cout << "finite " ;
                edges.push_back(Edge(it->p1, it->p2));
                edges.push_back(Edge(it->p2, it->p3));
                edges.push_back(Edge(it->p3, it->p1));
              }
            else
              {
                // keep triangle
                Triangles[dest_idx] = *it;
                ++dest_idx;
              }
          }
        Triangles.resize(dest_idx);

        cleanEdges(edges, true);

        // Create new triangles from the edges and the current Node.
        for (vector<Edge>::const_iterator it = edges.begin();
             it != edges.end(); ++it)
          {
            const Edge& currentEdge = *it;
            if (currentEdge.node1 == infiniteNode)
              infiniteTriangles.push_back(Edge(currentEdge.node2, currentNode));
            else if (currentEdge.node2 == infiniteNode)
              infiniteTriangles.push_back(Edge(currentNode, currentEdge.node1));
            else
              Triangles.push_back(DTriangle(currentEdge.node1,
                                            currentEdge.node2,
                                            currentNode));
          }
      } // all nodes handled

    // insertion finished, add workset to result

    for (vector<DTriangle>::const_iterator it = Triangles.begin();
         it != Triangles.end();
         ++it)
      {
        result.push_back(*it);
      }
  }

  // public
  void Delaunay::getTriangles(std::vector<Triangle>& output,
                              double limit) const
  {
    double limit2 = limit * limit;
    output.clear();
    for (vector<DTriangle>::const_iterator it = result.begin();
         it != result.end();
         ++it)
      {
        const DTriangle& t = *it;
        if ((limit <= 0) ||
            (
              (t.p1 - t.p2).r2() <= limit2 &&
              (t.p3 - t.p2).r2() <= limit2 &&
              (t.p1 - t.p3).r2() <= limit2
            )
           )
          output.push_back(Triangle(t.p1, t.p2, t.p3));
      }
  }

  // private
  void Delaunay::getEdges(const DTriangle& t, vector<Edge>& edges) const
  {
    // helper function: edges of one DTriangle
    edges.push_back(Edge(t.p1, t.p2));
    edges.push_back(Edge(t.p2, t.p3));
    edges.push_back(Edge(t.p3, t.p1));
  }

  // private
  void Delaunay::getEdges(vector<Edge>& edges) const
  {
    edges.clear();
    // helper function: edges of all DTriangle of result
    for (vector<DTriangle>::const_iterator it = result.begin();
         it != result.end();
         ++it)
      {
        getEdges(*it, edges);
      }
    // remove duplicates
    cleanEdges(edges, false);
  }

  // public
  void Delaunay::getEdges(std::vector<LineSeg>& output, double limit) const
  {
    double limit2 = limit * limit;
    vector<Edge> edges;
    getEdges(edges);
    output.clear();
    for (int i = 0; i < (int)edges.size(); ++i)
      if ((limit <= 0) || ((edges[i].node1 - edges[i].node2).r2() <= limit2))
        output.push_back(LineSeg(edges[i].node1, edges[i].node2));
  }

  void Delaunay::getTrianglesI(std::vector<std::vector<int> >& output,
                               double limit) const
  {
    double limit2 = limit * limit;
    output.clear();

    vector<int> triangleIndex(3); // indices of corners of an triangle

    for (vector<DTriangle>::const_iterator it = result.begin();
         it != result.end();
         ++it)
      {
        const DTriangle& t = *it;

        if (limit <= 0 ||
            ((t.p1 - t.p2).r2() <= limit2 &&
             (t.p3 - t.p2).r2() <= limit2 &&
             (t.p1 - t.p3).r2() <= limit2)
           )
          {
            triangleIndex[0] = t.p1.index;
            triangleIndex[1] = t.p2.index;
            triangleIndex[2] = t.p3.index;
            output.push_back(triangleIndex);
          }
      }

  }

  void Delaunay::getEdgesI(std::vector<std::vector<int> >& output,
                           double limit) const
  {
    double limit2 = limit * limit;
    output.clear();
    vector<int> edgeIndex(2);

    vector<Edge> edges;
    getEdges(edges);

    for (int i = 0; i < (int)edges.size(); ++i)
      {
        if (limit <= 0 || (edges[i].node1 - edges[i].node2).r2() <= limit2)
          {
            edgeIndex[0] = edges[i].node1.index;
            edgeIndex[1] = edges[i].node2.index;
            output.push_back(edgeIndex);
          }
      }
  }

  void Delaunay::getRegion(Region& region, double limit) const
  {
    vector<Triangle> triangles;
    getTriangles(triangles, limit);

    for (vector<Triangle>::const_iterator it = triangles.begin();
         it != triangles.end();
         ++it)
      {
        Region triangleRegion;
        Triangle2Region(*it, triangleRegion);
        region.add(triangleRegion);
      }
  }

#define FNAME "Delaunay::draw"
  void Delaunay::draw(const Image& img, int edgeValue, int fillValue, double limit) const
  {
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return;
      }

    if (fillValue >= 0)
      {
        Region region;
        getRegion(region, limit);
        region.draw(img, fillValue);
      }

    if (edgeValue >= 0)
      {
        vector<LineSeg> edges;
        getEdges(edges, limit);

        for (vector<LineSeg>::const_iterator it = edges.begin(); it != edges.end(); ++it)
          ice::draw(*it, img, edgeValue);

      }
  }
#undef FNAME
}
