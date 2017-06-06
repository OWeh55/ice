/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 1992..2014 FSU Jena, Digital Image Processing Group
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
#ifndef KD_INDEX_TREE_H
#define KD_INDEX_TREE_H

#include <assert.h>

#include <utility>
#include <limits>
#include <set>
#include <vector>

#include "matrixtools.h"
#include "Classifier.h"

namespace ice
{
  /*
   * KDIndexTree
   */
  template<typename T>
  class KDIndexTree final
  {
  private:
    class Node
    {
    public:
      Node(): dataIdx(-1), idx(-1), left(nullptr), right(nullptr) {}
      Node(int s): dataIdx(s), idx(-1), left(nullptr), right(nullptr) {}
      Node(int s, int idx):
        dataIdx(s), idx(idx), left(nullptr), right(nullptr) {}

      ~Node()
      {
        delete left;
        delete right;
      }

      void unlink()
      {
        left = nullptr;
        right = nullptr;
      }

      int dataIdx;
      int idx;
      Node* left;
      Node* right;
    };

  public:
    KDIndexTree(const std::vector<T>& vList):
      vList(vList), tree(nullptr), nFeatures(0)
    {
      create();
    }

    void create()
    {
      clear();
      std::vector<int> indexList(vList.size());
      for (int i = 0; i < indexList.size(); ++i)
        {
          indexList[i] = i;
        }
      create(tree, indexList);
    }

    void clear()
    {
      delete tree;
      tree = nullptr;
    }

    void create(Node*& tree, const std::vector<int>& s)
    {
      int nSamples = s.size();
      if (nSamples == 0)
        {
          tree = nullptr;
        }
      else
        {
          nFeatures = vList[0].size();
          if (nSamples == 1)
            {
              // leaf
              tree = new Node(s[0]);
            }
          else
            {
              int sidx;
              int vidx;
              if (findSplit(s, sidx, vidx))
                {
                  assert(vidx < nFeatures);
                  int splitDataIdx = s[sidx];
                  tree = new Node(splitDataIdx, vidx);
                  double splitValue = vList[splitDataIdx][vidx];
                  std::vector<int> lv;
                  std::vector<int> rv;
                  for (int i = 0; i < nSamples; ++i)
                    {
                      if (i != sidx)
                        {
                          if (vList[s[i]][vidx] < splitValue)
                            {
                              lv.push_back(s[i]);
                            }
                          else
                            {
                              rv.push_back(s[i]);
                            }
                        }
                    }
                  create(tree->left, lv);
                  create(tree->right, rv);
                }
              else
                {
                  // no split possible - all values equal
                  // we use the first sample as leaf value
                  tree = new Node(s[0]);
                }
            }
        }
    }

    double distance2(const T& d, const std::vector<double>& v) const
    {
      double dd = d[0] - v[0];
      double d2 =  dd * dd;
      for (int i = 1; i < nFeatures; ++i)
        {
          dd = d[i] - v[i];
          d2 += dd * dd;
        }
      return d2;
    }

    void evaluate(const Node* tree, const std::vector<double>& v,
                  double& minDistance2, const Node*& minNode) const
    {
      double d2 = distance2(vList[tree->dataIdx], v);
      if (d2 < minDistance2)
        {
          // this is new minimum
          minDistance2 = d2;
          minNode = tree;
        }
    }

    void findNearest(const Node* tree,
                     const std::vector<double>& v,
                     double& minDistance2,
                     const Node*& minNode) const
    {
      if (tree == nullptr)
        {
          return;
        }

      if (tree->idx < 0)   // leaf
        {
          evaluate(tree, v, minDistance2, minNode);
        }
      else
        {
          int splitIdx = tree->idx;
          assert(v.size() == vList[tree->dataIdx].size());
          assert(splitIdx < v.size());
          // signed(!) distance to hyperplane
          double hd = v[splitIdx] - vList[tree->dataIdx][splitIdx];
          double hd2 = hd * hd;

          if (hd < 0)   // which side to search first
            {
              findNearest(tree->left, v, minDistance2, minNode);

              if (minDistance2 > hd2)   // suche auch auf der anderen Seite
                {
                  // check this node
                  evaluate(tree, v, minDistance2, minNode);
                  if (minDistance2 > hd2)   // suche auch auf der anderen Seite
                    {
                      findNearest(tree->right, v, minDistance2, minNode);
                    }
                }
            }
          else
            {
              findNearest(tree->right, v, minDistance2, minNode);

              if (minDistance2 > hd2)   // suche auch auf der anderen Seite
                {
                  // check this node
                  evaluate(tree, v, minDistance2, minNode);
                  if (minDistance2 > hd2)   // suche auch auf der anderen Seite
                    {
                      findNearest(tree->left, v, minDistance2, minNode);
                    }
                }
            }
        }
    }

    int findNearest(const std::vector<double>& v) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      const Node* minNode;
      double minDistance2 = std::numeric_limits<double>::max();
      findNearest(tree, v, minDistance2, minNode);
      return minNode->dataIdx;
    }

    int findNearest(const T& s) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<double> v(s.size());
      for (int k = 0; k < s.size(); ++k)
        {
          v[k] = s[k];
        }
      return findNearest(v);
    }

    void findKNearest(const Node* tree,
                      const std::vector<double>& v,
                      double& minDistance,
                      std::vector<int>& minSamples,
                      std::vector<double>& dist,
                      int k) const
    {
      if (tree == nullptr)
        {
          return;
        }

      double d2 = distance2(vList[tree->dataIdx], v);

      bool newMin = false;
      if (minSamples.size() < k)
        {
          newMin = true;
          dist.push_back(d2);
          minSamples.push_back(tree->dataIdx);
        }
      else if (d2 < minDistance)
        {
          // find max
          int maxidx = 0;
          double maxdist = dist[0];
          for (int i = 1; i < k; ++i)
            {
              if (dist[i] > maxdist)
                {
                  maxidx = i;
                  maxdist = dist[i];
                }
            }
          // replace this element
          minSamples[maxidx] = tree->dataIdx;
          dist[maxidx] = d2;
          newMin = true;
        }

      if (newMin)
        {
          minDistance = dist[0];
          for (int i = 1; i < dist.size(); ++i)
            {
              if (dist[i] > minDistance)
                {
                  minDistance = dist[i];
                }
            }
        }

      // check whether we reached a leaf node
      if (tree->idx != -1)
        {
          double hd = v[tree->idx] - vList[tree->dataIdx][tree->idx]; // distance to split plane
          double hd2 = hd * hd;

          if (hd < 0)
            {
              findKNearest(tree->left, v, minDistance, minSamples, dist, k);

              if (minDistance > hd2)   // suche auch auf der anderen Seite
                {
                  findKNearest(tree->right, v, minDistance, minSamples, dist, k);
                }
            }
          else
            {
              findKNearest(tree->right, v, minDistance, minSamples, dist, k);

              if (minDistance > hd2)   // suche auch auf der anderen Seite
                {
                  findKNearest(tree->left, v, minDistance, minSamples, dist, k);
                }
            }
        }
    }

    void findKNearest(const std::vector<double>& v, int k,
                      std::vector<int>& neighbors, std::vector<double>& distances) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<int> minNodes;
      std::vector<double> dist2;
      double minDistance2 = std::numeric_limits<double>::max();
      findKNearest(tree, v, minDistance2, minNodes, dist2, k);
      int nFound = minNodes.size();
      neighbors.resize(nFound);
      distances.resize(nFound);
      std::vector<std::pair<double, int> > vpair(nFound);
      for (int i = 0; i < nFound; ++i)
        {
          vpair[i] = std::pair<double, int>(dist2[i], i);
        }
      sort(vpair.begin(), vpair.end());

      for (int i = 0; i < nFound; ++i)
        {
          int idx = vpair[i].second;
          neighbors[i] = minNodes[idx];
          distances[i] = sqrt(dist2[idx]);
        }
    }

    void findKNearest(const T& s, int k,
                      std::vector<int>& neighbors, std::vector<double>& distances) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<double> v(s.size());
      for (int k = 0; k < s.size(); ++k)
        {
          v[k] = s[k];
        }
      findKNearest(v, k, neighbors, distances);
    }

    /*
     * find all neighbors with distance less than given distance
     */

    void findNeighbors(const Node* tree,
                       const std::vector<double>& v,
                       double dist2,
                       std::vector<int>& minNodes) const
    {
      if (tree == nullptr)
        {
          return;
        }

      double d2 = distance2(vList[tree->dataIdx], v);

      if (d2 < dist2)
        {
          minNodes.push_back(tree->dataIdx);
        }

      // check whether we reached a leaf node
      if (tree->idx != -1)
        {
          double hd = v[tree->idx] - vList[tree->dataIdx][tree->idx]; // distance to split plane
          double hd2 = hd * hd;

          if (hd < 0)
            {
              findNeighbors(tree->left, v, dist2, minNodes);

              if (dist2 > hd2)   // suche auch auf der anderen Seite
                {
                  findNeighbors(tree->right, v, dist2, minNodes);
                }
            }
          else
            {
              findNeighbors(tree->right, v, dist2, minNodes);

              if (dist2 > hd2)   // suche auch auf der anderen Seite
                {
                  findNeighbors(tree->left, v, dist2, minNodes);
                }
            }
        }
    }

    void findNeighbors(const std::vector<double>& v, double distance,
                       std::vector<int>& neighbors) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<int> minNodes;
      findNeighbors(tree, v, distance * distance, minNodes);
      int nFound = minNodes.size();
      neighbors.resize(nFound);
      for (int i = 0; i < nFound; ++i)
        {
          neighbors[i] = minNodes[i];
        }
    }

    void findNeighbors(const T& s, double distance,
                       std::vector<int>& neighbors) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<double> v(s.size());
      for (int k = 0; k < s.size(); ++k)
        {
          v[k] = s[k];
        }
      findNeighbors(v, distance, neighbors);
    }

    /***
     * get number of nodes in tree
     */
    int getNNodes(const Node* tree) const
    {
      if (tree == nullptr)
        {
          return 0;
        }
      return getNNodes(tree->left) + getNNodes(tree->right) + 1;
    }

    /***
     * get maximal depth of nodes == depth of tree
     */
    int getMaxDepth(const Node* tree) const
    {
      if (tree == nullptr || tree->left == nullptr || tree->right == nullptr)
        {
          return 0;
        }
      int leftMaxDepth = getMaxDepth(tree->left);
      int rightMaxDepth = getMaxDepth(tree->right);
      return rightMaxDepth > leftMaxDepth ? rightMaxDepth + 1 : leftMaxDepth + 1;
    }

#if 0
    /***
     * write [sub-]tree to stream
     */
    void write(std::ostream& os, const Node* tree) const
    {
      if (tree != nullptr)
        {
          os << "T ";
          tree->write(os);
          os << std::endl;
          if (tree->idx >= 0)
            {
              write(os, tree->left);
              write(os, tree->right);
            }
        }
      else
        {
          os << "N" << std::endl;
        }
    }

    /***
     * write tree to stream
     */
    void write(std::ostream& os) const
    {
      write(os, tree);
    }

    void read(std::istream& is, Node*& tr)
    {
      char c;
      is >> c;
      if (c == 'N')
        {
          tr = nullptr;
        }
      else
        {
          Node h;
          h.read(is);
          tr = new Node(h);
          if (h.idx >= 0)
            {
              read(is, tr->left);
              read(is, tr->right);
            }
        }
    }

    void read(std::istream& is)
    {
      clear();
      read(is, tree);
    }
#endif
    void statistics() const
    {
      if (tree != nullptr)
        {
          int nNodes = getNNodes(tree);
          std::cout << "nodes: " << nNodes ;
          std::cout << "  maxDepth: " << getMaxDepth(tree) ;
          std::cout << std::endl;
        }
    }

  private:

    bool findSplit(const std::vector<int>& sl, int& sidx, int& vidx) const
    {
      T max(vList[sl[0]]);
      T min(vList[sl[0]]);

      for (int k = 1; k < sl.size(); ++k)
        {
          const T& sample = vList[sl[k]];
          for (int i = 0; i < nFeatures; ++i)
            {
              double val = sample[i];
              if (val > max[i])
                {
                  max[i] = val;
                }
              else if (val < min[i])
                {
                  min[i] = val;
                }
            }
        }

      double maxdif = 0;
      for (int i = 0; i < nFeatures; ++i)
        {
          double dif = max[i] - min[i];
          if (dif > maxdif)
            {
              vidx = i;
              maxdif = dif;
            }
        }

      sidx = 0;
      if (maxdif > 0)
        {
          // use center value as splitvalue
          double sValue = (max[vidx] + min[vidx]) * 0.5;
          // find lower and higher nearest sample value
          double lowerValue = - std::numeric_limits<double>::max();
          int lowerIdx = -1;
          double higherValue = std::numeric_limits<double>::max();
          int higherIdx = -1;

          for (int k = 0; k < sl.size(); ++k)
            {
              const T& sample = vList[sl[k]];
              double avalue = sample[vidx];
              if (avalue <= sValue)
                {
                  if (avalue > lowerValue)
                    {
                      lowerValue = avalue;
                      lowerIdx = k;
                    }
                }
              else
                {
                  if (avalue < higherValue)
                    {
                      higherValue = avalue;
                      higherIdx = k;
                    }
                }
            }

          assert(lowerIdx >= 0 && higherIdx >= 0); // at least min and max should be on lower or higher side

          if (sValue - lowerValue < higherValue - sValue)
            {
              sidx = lowerIdx;
            }
          else
            {
              sidx = higherIdx;
            }
          return true;
        }
      else
        {
          return false;
        }

    }

    const std::vector<T>& vList;
    Node* tree;
    int nFeatures;
  };
}
#endif
