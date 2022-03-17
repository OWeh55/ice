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
#ifndef KDTREE_H
#define KDTREE_H

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
   * KDTree
   */
  template<typename T>
  class KDTree final
  {
  private:
    class Node
    {
    public:
      Node(): idx(-1), left(nullptr), right(nullptr) {}
      Node(const T& s): data(s), idx(-1), left(nullptr), right(nullptr) {}
      Node(const T& s, int idx):
        data(s), idx(idx), left(nullptr), right(nullptr) {}

      ~Node()
      {
        delete left;
        delete right;
      }

      void write(std::ostream& os) const
      {
        os << idx << " " << data;
      }

      void read(std::istream& is)
      {
        is >> idx;
        is >> data;
      }

      void unlink()
      {
        left = nullptr;
        right = nullptr;
      }

      T data;
      int idx;
      Node* left;
      Node* right;
    };

  public:
    KDTree(): tree(nullptr), nFeatures(0) {}

    void clear()
    {
      delete tree;
      tree = nullptr;
    }

    void create(Node*& tree, const std::vector<T>& s)
    {
      int nSamples = s.size();
      if (nSamples == 0)
        {
          tree = nullptr;
        }
      else
        {
          nFeatures = s[0].size();
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
                  tree = new Node(s[sidx], vidx);
                  double splitValue = s[sidx][vidx];
                  std::vector<T> lv;
                  std::vector<T> rv;
                  for (int i = 0; i < nSamples; ++i)
                    {
                      if (i != sidx)
                        {
                          if (s[i][vidx] < splitValue)
                            {
                              lv.push_back(s[i]);
                            }
                          else
                            {
                              rv.push_back(s[i]);
                            }
                        }
                    }
                  // std::cout<<lv.size()<<" "<<rv.size()<<" "<<splitValue<<std::endl;
                  create(tree->left, lv);
                  create(tree->right, rv);
                }
              else
                {
                  // no split possible
                  // we use the first sample as leaf value
                  // and ignore the others (which are the same)
                  tree = new Node(s[0]);
                }
            }
        }
    }

    void create(const std::vector<T>& s)
    {
      if (tree != nullptr)
        delete tree;

      create(tree, s);
    }

    static double distance2(const T& d, const std::vector<double>& v)
    {
      int size = v.size();
      double dd = d[0] - v[0];
      double d2 =  dd * dd;
      for (int i = 1; i < size; ++i)
        {
          dd = d[i] - v[i];
          d2 += dd * dd;
        }
      return d2;
    }

    void check(const Node* node, const std::vector<double>& v,
               double& minDistance2, const Node*& minNode) const
    {
      //      checkCount++;
      double d2 = distance2(node->data, v);
      if (d2 < minDistance2)
        {
          // this is new minimum
          minDistance2 = d2;
          minNode = node;
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
          check(tree, v, minDistance2, minNode);
        }
      else
        {
          int splitIdx = tree->idx;
          assert((int)v.size() == tree->data.size());
          assert(splitIdx < (int)v.size());
          double hd = v[splitIdx] - tree->data[splitIdx]; // distance to hyperplane
          double hd2 = hd * hd;

          if (hd < 0)
            {
              findNearest(tree->left, v, minDistance2, minNode);

              if (minDistance2 > hd2)   // suche auch auf der anderen Seite
                {
                  // check this node
                  check(tree, v, minDistance2, minNode);
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
                  check(tree, v, minDistance2, minNode);
                  if (minDistance2 > hd2)   // suche auch auf der anderen Seite
                    {
                      findNearest(tree->left, v, minDistance2, minNode);
                    }
                }
            }
        }
    }

    const T* findNearest(const std::vector<double>& v) const
    {
      // checkCount=0;
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      const Node* minNode;
      double minDistance2 = std::numeric_limits<double>::max();
      findNearest(tree, v, minDistance2, minNode);
      // std::cout << checkCount << std::endl;
      return &(minNode->data);
    }

    const T* findNearest(const T& s) const
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
                      std::vector<const Node*>& minNodes,
                      std::vector<double>& dist,
                      int k) const
    {
      if (tree == nullptr)
        {
          return;
        }

      double d2 = distance2(tree->data, v);

      bool newMin = false;
      if (int(minNodes.size()) < k)
        {
          newMin = true;
          dist.push_back(d2);
          minNodes.push_back(tree);
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
          minNodes[maxidx] = tree;
          dist[maxidx] = d2;
          newMin = true;
        }

      if (newMin)
        {
          minDistance = dist[0];
          for (unsigned int i = 1; i < dist.size(); ++i)
            {
              if (dist[i] > minDistance)
                {
                  minDistance = dist[i];
                }
            }
        }

      double hd = v[tree->idx] - tree->data[tree->idx]; // distance to split plane
      double hd2 = hd * hd;

      if (hd < 0)
        {
          findKNearest(tree->left, v, minDistance, minNodes, dist, k);

          if (minDistance > hd2)   // suche auch auf der anderen Seite
            {
              findKNearest(tree->right, v, minDistance, minNodes, dist, k);
            }
        }
      else
        {
          findKNearest(tree->right, v, minDistance, minNodes, dist, k);

          if (minDistance > hd2)   // suche auch auf der anderen Seite
            {
              findKNearest(tree->left, v, minDistance, minNodes, dist, k);
            }
        }
    }

    void findKNearest(const std::vector<double>& v, int k,
                      std::vector<const T*>& neighbors, std::vector<double>& distances) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<const Node*> minNodes;
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
          neighbors[i] = &(minNodes[idx]->data);
          distances[i] = sqrt(dist2[idx]);
        }
    }

    void findKNearest(const T& s, int k,
                      std::vector<const T*>& neighbors, std::vector<double>& distances) const
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
                       std::vector<const Node*>& minNodes) const
    {
      if (tree == nullptr)
        {
          return;
        }

      double d2 = distance2(tree->data, v);

      if (d2 < dist2)
        {
          minNodes.push_back(tree);
        }

      if (tree->idx >= 0)
        {
          double hd = v[tree->idx] - tree->data[tree->idx]; // distance to split plane
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
                       std::vector<const T*>& neighbors) const
    {
      if (tree == nullptr)
        {
          throw "empty tree";
        }
      std::vector<const Node*> minNodes;
      findNeighbors(tree, v, distance * distance, minNodes);
      int nFound = minNodes.size();
      neighbors.resize(nFound);
      for (int i = 0; i < nFound; ++i)
        {
          neighbors[i] = &(minNodes[i]->data);
        }
    }

    void findNeighbors(const T& s, double distance,
                       std::vector<const T*>& neighbors) const
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
      if (tree == nullptr)
        {
          return 0;
        }
      int leftMaxDepth = getMaxDepth(tree->left);
      int rightMaxDepth = getMaxDepth(tree->right);
      return rightMaxDepth > leftMaxDepth ? rightMaxDepth + 1 : leftMaxDepth + 1;
    }

    /***
     * get minimal depth of leaf nodes
     */
    int getMinDepth(const Node* tree) const
    {
      if (tree == nullptr)
        {
          return 0;
        }
      int leftMinDepth = getMinDepth(tree->left);
      int rightMinDepth = getMinDepth(tree->right);
      return rightMinDepth < leftMinDepth ? rightMinDepth + 1 : leftMinDepth + 1;
    }

    void statistics() const
    {
      if (tree != nullptr)
        {
          int nNodes = getNNodes(tree);
          std::cout << "nodes: " << nNodes ;
          std::cout << "  maxDepth: " << getMaxDepth(tree) ;
          std::cout << "  minDepth: " << getMinDepth(tree) ;
          std::cout << std::endl;
        }
    }

  private:

    bool findSplit(const std::vector<T>& sl,
                   int& sampleIdx, int& featureIdx) const
    {
      T max(sl[0]);
      T min(sl[0]);

      // find ranges for features
      for (unsigned int k = 0; k < sl.size(); ++k)
        {
          for (int i = 0; i < nFeatures; ++i)
            {
              double val = sl[k][i];
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

      // select feature with highest deviation
      featureIdx = -1;
      double maxdif = 0;
      for (int i = 0; i < nFeatures; ++i)
        {
          // std::cout << "--   " << i << ": " << min[i] << " .. " << max[i] << std::endl;
          double dif = max[i] - min[i];
          if (dif > maxdif)
            {
              featureIdx = i;
              maxdif = dif;
            }
        }

      if (featureIdx < 0)
        {
          return false;
        }

      // use center value as splitvalue
      double diff2 = maxdif / 2;
      double sValue = min[featureIdx] + diff2;
      // assert(max[featureIdx]>sValue);
      // assert(sValue>min[featureIdx]);

      // std::cout << min[featureIdx] << " " << sValue << " " << max[featureIdx] << std::endl;
      sampleIdx = 0; // sample with splitvalue

      // is split possible (numeric limits)
      if ((max[featureIdx] > sValue) && (sValue > min[featureIdx]))
        {
          // find higher value nearest to sample value
          double higherValue = std::numeric_limits<double>::max();
          int higherIdx = -1;

          for (unsigned int k = 0; k < sl.size(); ++k)
            {
              double avalue = sl[k][featureIdx];
              //std::cout << k << ": " << lowerValue << ".." << higherValue;
              // std::cout << " " << avalue << " ";
              // std::cout << avalue-sValue <<  std::endl;
              if (avalue >= sValue)
                if (avalue < higherValue)
                  {
                    higherValue = avalue;
                    higherIdx = k;
                  }
            }

          // at least the max element should be >= sValue than
          assert(higherIdx >= 0);

          sampleIdx = higherIdx;
          return true;
        }
      else
        {
          return false;
        }
    }

    Node* tree;
    int nFeatures;
    //  public:
    //    mutable int checkCount;
  };
}
#endif
