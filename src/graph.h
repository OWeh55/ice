#ifndef GRAPH_H
#define GRAPH_H
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
/*
 * datastructure graph
 * 7/2004 Ortmann
 */

#include "defs.h"
#include "Vector.h"

namespace ice
{

  class node;
  class edge;
  class edgeref;

  typedef node* nodep;
  typedef edge* edgep;
  typedef edgeref* edgerefp;

  class node
  {
  private:
    node(int pid = 0): left(nullptr), right(nullptr), edgelist(nullptr), id(pid) {}
    node(const Vector& f, int pid = 0):
      left(nullptr), right(nullptr), edgelist(nullptr), id(pid), feature(f) {}
    ~node()
    {
      DeleteEdgeList();
    }
    node(const node& n);

    void DeleteEdgeList();

  protected:
    nodep left;
    nodep right;
    edgerefp edgelist;
    int id;
    Vector feature;
    friend class graph;
  };

  class edge
  {
  private:
    edge(): id(0), left(nullptr), right(nullptr), node1(nullptr), node2(nullptr) {};
    edge(int pid, nodep p1, nodep p2):
      id(pid), left(nullptr), right(nullptr), node1(p1), node2(p2) {};
    edge(int pid, nodep p1, nodep p2, const Vector& feat):
      id(pid), left(nullptr), right(nullptr), node1(p1), node2(p2), feature(feat) {};
    edge(const edge& e);
  protected:
    int id;
    edgep left;
    edgep right;
    nodep node1;
    nodep node2;
    Vector feature;
    friend class edgeref;
    friend class graph;
  };

  class edgeref
  {
  public:
    edgeref(node* refnode, edge* e): next(nullptr)
    {
      dir = (e->node2 == refnode);
      ep = e;
    }
    node* connected_node()
    {
      if (dir)
        {
          return ep->node1;
        }
      else
        {
          return ep->node2;
        }
    }
    edgerefp next;

  protected:
    edgeref();
    edgeref(const edgeref&);

    edgep ep;
    bool dir;
    friend class graph;
  };

  class graph
  {
  public:
    graph(): nodetree(nullptr), edgetree(nullptr), maxnodeid(0), maxedgeid(0) {}
    ~graph();

    nodep AddNode(int pid = -1);
    nodep AddNode(const Vector& f, int pid = -1);

    edgep AddEdge(nodep n1, nodep n2, int pid = -1);
    edgep AddEdge(int nid1, int nid2, int pid = -1);
    edgep AddEdge(nodep n1, nodep n2, const Vector& f, int pid = -1);
    edgep AddEdge(int nid1, int nid2, const Vector& f, int pid = -1);

  protected:
    nodep AddNode(nodep& nt, nodep n);
    edgep AddEdge(edgep& et, edgep e);

    void DelNodeTree(nodep& nt);
    void DelEdgeTree(edgep& nt);

    graph(const graph&);

    nodep nodetree;
    edgep edgetree;

    int maxnodeid;
    int maxedgeid;

    int isnode(nodep n);
    int isedge(edgep e);
  };
}
#endif
