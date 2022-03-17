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

#include <assert.h>
#include <algorithm>

#include "freeman.h"
#include "componenttree.h"

#include "vectortools.h"
#include "contools.h"
#include "DisjointSets.h"

#define NDEBUG 1

using namespace std;

namespace ice
{
  int ComponentTreeNode::getNComponents() const
  {
    int res = 1;
    for (auto it = children.begin(); it != children.end(); it++)
      {
        res += (*it)->getNComponents();
      }
    return res;
  }

  Region ComponentTreeNode::getTotalRegion() const
  {
    Region result = *region;
    for (auto it = children.begin(); it != children.end(); it++)
      {
        result.add((*it)->getTotalRegion());
      }
    return result;
  }

  int ComponentTreeNode::getDepth() const
  {
    int depth = 0;
    // maximum depth of childs
    for (auto it = children.begin(); it != children.end(); it++)
      {
        int hdepth = (*it)->getDepth();

        if (hdepth > depth)
          {
            depth = hdepth;
          }
      }

    return depth + 1;
  }

  int ComponentTreeNode::setHeight(int referenceValue)
  {
    height = 0;
    for (auto it = children.begin(); it != children.end(); it++)
      {
        int cHeight = (*it)->setHeight(value);

        if (cHeight > height)
          {
            height = cHeight;
          }
      }
    return height + abs(value - referenceValue);
  }

  int ComponentTreeNode::getHeight(int referenceValue) const
  {
    return height + abs(value - referenceValue);
  }

  void ComponentTreeNode::absorbChildren()
  {
    for (auto it = children.begin(); it != children.end(); it++)
      {
        (*it)->absorbChildren();
        region->add(*(*it)->region);
      }
    children.clear();
  }

  void ComponentTreeNode::reduceSmallLeaves(int minheight)
  {
    int maxheight = 0;
    unsigned int keepchild = 0;
    if (!children.empty())
      {
        vector<int> cheight(children.size());
        for (unsigned int i = 0; i < children.size(); i++)
          {
            cheight[i] = children[i]->getHeight(value);
            if (cheight[i] > maxheight)
              {
                maxheight = cheight[i];
                keepchild = i;
              }
          }

        for (unsigned int i = 0; i < children.size(); i++)
          {
            if (cheight[i] < minheight && i != keepchild)
              {
                children[i]->absorbChildren();
                region->add(*children[i]->region);
                children[i] = nullptr;
              }
          }

        // Liste bereinigen, nullptr eliminieren
        nodelist_t::iterator new_end =
          remove_if(children.begin(), children.end(),
                    bind2nd(equal_to<ComponentTreeNode*>(), (ComponentTreeNode*) nullptr));
        children.erase(new_end, children.end());

        for (unsigned int i = 0; i < children.size(); i++)
          {
            children[i]->reduceSmallLeaves(minheight);
          }
      }
  }

  bool ComponentTreeNode::cutSmallAreas(int minarea)
  {
    // entfernt Teilstränge mit Flächen kleiner minarea
    if (area < minarea)
      {
        return true;  // "ich bin klein"
      }

    for (auto it = children.begin(); it != children.end(); it++)
      {
        // Kinder die zu klein sind löschen
        if ((*it)->cutSmallAreas(minarea))
          {
            delete *it;
            *it = nullptr;
          }
      }

    // Liste bereinigen, nullptr eliminieren
    nodelist_t::iterator new_end =
      remove_if(children.begin(), children.end(),
                bind2nd(equal_to<ComponentTreeNode*>(), (ComponentTreeNode*) nullptr));
    children.erase(new_end, children.end());

    return false; // "ich bin schon groß"
  }

  void ComponentTreeNode::print(int depth, int lvl) const
  {
    // Baum ausgeben bis zu tiefe depth (debug)
    if (depth > lvl)
      {
        for (int i = 0; i < lvl; i++)
          {
            cout << ".";
          }

        cout << " area:" << area << " value:" << value;
        cout << " children:" << children.size() << endl;
        cout << " height of children:" << height << endl;
        for (auto it = children.begin(); it != children.end(); it++)
          {
            (*it)->print(depth, lvl + 1);
          }
      }
  }

  void ComponentTreeNode::draw(Image& img, int depth) const
  {
    static int color = 1;
    // draw regions in image img up to depth

    if (depth > 0)
      {
        region->draw(img, color);
        color = (color) % (img.maxval + 1) + 1;

        for (auto it = children.begin(); it != children.end(); it++)
          {
            (*it)->draw(img, depth - 1);
          }
      }
  }

  int ComponentTreeNode::getBigRegions(vector<Region>& regions, int minarea) const
  {
    int ret = 0;

    if (area < minarea)
      {
        return ret;  // zu klein, Kinder können auch nicht größer sein, fertig
      }

    regions.push_back(*region);

    ret = 1;
    for (auto it = children.begin(); it != children.end(); it++)
      {
        ret += (*it)->getBigRegions(regions, minarea);
      }

    return ret;
  }

  void ComponentTreeNode::getLeafs(
    vector<const ComponentTreeNode*>& leaves,  // regions with extremal value
    vector<const ComponentTreeNode*>& basins   // regions of extrema
  ) const
  {
    if (children.empty())
      {
        leaves.push_back(this);
        basins.push_back(this);
      }
    else
      {
        for (auto  it = children.begin(); it != children.end(); it++)
          {
            (*it)->getLeafs(leaves, basins);
          }
        if (children.size() == 1 && basins.back() == children[0])
          {
            basins.back() = this;
          }
      }
  }

  void ComponentTreeNode::getLeafs(vector<const ComponentTreeNode*>& leaves) const
  {
    if (children.empty())
      {
        leaves.push_back(this);
      }
    else
      {
        for (auto  it = children.begin(); it != children.end(); it++)
          {
            (*it)->getLeafs(leaves);
          }
      }

  }

  void ComponentTreeNode::getLeafs(std::vector<Region>& minregions) const
  {
    vector<const ComponentTreeNode*> leaves;
    getLeafs(leaves);

    minregions.clear();

    for (auto it = leaves.begin(); it != leaves.end(); ++it)
      {
        minregions.push_back((*it)->getRegion());
      }
  }

  void ComponentTreeNode::getLeafs(std::vector<Region>& minregions,
                                   std::vector<Region>& basinregions) const
  {
    vector<const ComponentTreeNode*> leaves;
    vector<const ComponentTreeNode*> basins;
    getLeafs(leaves, basins);

    minregions.clear();
    basinregions.clear();
    for (auto it = leaves.begin(); it != leaves.end(); ++it)
      {
        minregions.push_back((*it)->getRegion());
      }
    for (auto it = basins.begin(); it != basins.end(); ++it)
      {
        basinregions.push_back((*it)->getTotalRegion());
      }
  }

  int ComponentTreeNode::setArea()
  {
    area = region->getArea();
    for (auto it = children.begin(); it != children.end(); it++)
      {
        area += (*it)->setArea();
      }
    return area;
  }

  ComponentTreeNode* makeComponentTree(const Image& img, bool minAsLeaf)
  {
    ComponentTreeNode* root;

    int xs = img.xsize;
    int ys = img.ysize;
    int ss = xs * ys;
    int mv = img.maxval;

    // Sortieren der Grauwerte

    vector<int> slink(ss); // verlinkung von Grauwerten gleichen Wertes
    vector<int> bucket(mv + 1); // "Zeiger" auf ersten Grauwert mit Wert i

    for (int i = 0; i <= mv; i++)
      {
        bucket[i] = -1;
      }

    int idx = 0;

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          // idx == y * xs + x
          int g = img.getPixelUnchecked(x, y);
          slink[idx] = bucket[g]; // link to "previous" pixel with same value
          bucket[g] = idx; // last pixel with this value
          idx++;
        }

    DisjointSets node(ss);
    DisjointSets tree(ss);

    vector<int> lowestNode(ss);
    vector<ComponentTreeNode*> nodes(ss, nullptr);
    ComponentTreeNode invalid;

    // initialisierung der Datenstrukturen
    for (int idx = 0; idx < ss; idx++)
      {
        lowestNode[idx] = idx;
      }

    int firstValue = mv;
    int endValue = -1;
    int stepOfValue = -1;
    if (minAsLeaf)
      {
        firstValue = 0;
        endValue = mv + 1;
        stepOfValue = 1;
      }

    for (int value = firstValue; value != endValue; value += stepOfValue)
      {
        int idx = bucket[value]; // last pixel in chain with value value

        while (idx >= 0)
          {
            // hier Punkt behandeln
            int px = ind2x(idx);
            int py = ind2y(idx);

            int curTree = tree.find(idx);
            assert(curTree == idx);

            int lNode = lowestNode[curTree];
            assert(lNode == idx);

            int curNode = node.find(lNode);

            // nodes[idx] = new ComponentTreeNode(value);

            // each neighbor
            for (int dir = 0; dir < 8; dir += 2)
              {
                int qx, qy;
                Freeman(dir).move(px, py, qx, qy);
                if (img.inside(qx, qy))
                  {
                    int qidx = xy2ind(qx, qy);

                    if (nodes[qidx] != nullptr)   // if already processed
                      {
                        int adjTree = tree.find(qidx);
                        int adjNode = node.find(lowestNode[adjTree]);

                        if (curNode != adjNode)
                          {
                            // if (nodes[curNode]->Value() == nodes[adjNode]->Value())
                            if (value == nodes[adjNode]->Value())
                              {
                                // curNode = MergeNodes(adjNode,curNode);
                                node.link(adjNode, curNode);
                                if (nodes[curNode] != nullptr)
                                  {
                                    nodes[adjNode]->merge(*nodes[curNode]);
                                    nodes[curNode]->unlink();
                                    delete nodes[curNode];
                                  }
                                nodes[curNode] = &invalid; // mark as handled but invalid
                                curNode = adjNode;
                              }
                            else
                              {
                                if (nodes[curNode] == nullptr)
                                  {
                                    nodes[curNode] = new ComponentTreeNode(value);
                                  }
                                nodes[curNode]->addAsChild(*nodes[adjNode]);
                              }

                            curTree = tree.link(adjTree, curTree);
                            lowestNode[curTree] = curNode;
                          }
                      }
                  }
              }
            if (curNode == idx && nodes[idx] == nullptr)
              {
                nodes[idx] = new ComponentTreeNode(value);
              }
            // nächster Punkt
            idx = slink[idx];
          }
      }

    int rootnode = lowestNode[tree.find(node.find(0))];
    root = nodes[rootnode];

    // complete remaining values in tree nodes
    //    Region region;
    //    int area;

    // create regions
    for (int y = 0; y < ys; y++)
      {
        int x1 = 0;
        int cNode = node.find(xy2ind(0, y));
        for (int x = 1; x < xs; x++)
          {
            int idx = xy2ind(x, y);
            // find corresponding node
            int nNode = node.find(idx);
            if (nNode != cNode)
              {
                ComponentTreeNode* aNode = nodes[cNode];
                aNode->getRegion().add(x1, y, x - 1, y);
                x1 = x;
                cNode = nNode;
              }
          }
        ComponentTreeNode* aNode = nodes[cNode];
        aNode->getRegion().add(x1, y, xs - 1, y);
      }
    // set value for child height
    root->setHeight(root->value);

    // set value for area in nodes
    root->setArea();

    return root;
  }
}
