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

#include "graph.h"

namespace ice
{

  void node::DeleteEdgeList()
  {
    edgerefp p1 = edgelist;

    while (p1 != NULL)
      {
        edgerefp p2 = p1->next;
        delete p1;
        p1 = p2;
      }
  }

  nodep graph::AddNode(int pid)
  {
    if (pid < 0)
      {
        pid = maxnodeid++;
      }

    nodep n = new node(pid);
    return AddNode(nodetree, n);
  }

  nodep graph::AddNode(const Vector& f, int pid)
  {
    if (pid < 0)
      {
        pid = maxnodeid++;
      }

    nodep n = new node(f, pid);
    return AddNode(nodetree, n);
  }

  nodep graph::AddNode(nodep& nt, nodep n)
  {
    if (nt == NULL)
      {
        return nt = n;
      }

    if (nt->id < n->id)
      {
        return AddNode(nt->left, n);
      }
    else
      {
        return AddNode(nt->right, n);
      }
  }

  void graph::DelNodeTree(nodep& nt)
  {
    if (nt != NULL)
      {
        DelNodeTree(nt->left);
        DelNodeTree(nt->right);
        delete nt;
      }
  }

  void graph::DelEdgeTree(edgep& et)
  {
    if (et != NULL)
      {
        DelEdgeTree(et->left);
        DelEdgeTree(et->right);
        delete et;
      }
  }

  graph::~graph()
  {
    DelEdgeTree(edgetree);
    DelNodeTree(nodetree);
  }
} // namespace ice
