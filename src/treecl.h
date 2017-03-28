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
#ifndef TREECL_H
#define TREECL_H

#include <vector>
#include "vectortools.h"
#include "matrixarith.h"
#include "Classifier.h"

namespace ice
{
  /*
   * class TreeCl - oblique tree for classifiers
   */

  class TreeCl
  {
  private:
    class CTNode
    {
    public:
      // Fork in tree, containing weight vector
      const static int tFork = 1;
      // Sample (leaf)
      const static int tLeaf = 2;

      CTNode(int type): type(type), left(nullptr), right(nullptr) {};
      virtual ~CTNode() {};

      int type;

      // probability vector (tLeaf) or weight-vector (tFork)
      std::vector<double> value;
      int classNr;

      CTNode* left;
      CTNode* right;
    };


  public:
    TreeCl(): tree(nullptr), nClasses(0), nFeatures(0), depth(std::numeric_limits<int>::max()) {}
    TreeCl(int nClasses, int nFeatures,
           int depth = std::numeric_limits<int>::max()):
      tree(nullptr), nClasses(nClasses), nFeatures(nFeatures), depth(depth) {}

    virtual void init(int nClasses, int nFeatures,
                      int depth = std::numeric_limits<int>::max())
    {
      clear();
      this->nClasses = nClasses;
      this->nFeatures = nFeatures;
      this->depth = depth;
    }

    static void clear(CTNode* tree)
    {
      if (tree != nullptr)
        {
          clear(tree->left);
          clear(tree->right);
          delete tree;
        }
    }

    virtual void clear()
    {
      clear(tree);
      tree = nullptr;
    }

    static double xProd(const std::vector<double>& weights,
                        const std::vector<double>& features)
    {
      double prod = 0;
      for (unsigned int i = 0; i < features.size(); ++i)
        prod += weights[i] * features[i];
      prod += weights.back();
      return prod;
    }

    static double evaluateSplit(const std::vector<ClassSample>& samples,
                                const std::vector<double>& weights);

    static bool findSplit(const std::vector<ClassSample>& samples,
                          std::vector<double>& weights,
                          int nClasses, int nFeatures);

    static void create(CTNode*& ctree, const std::vector<ClassSample>& sl,
                       int maxdepth, int nClasses, int nFeatures);

    virtual void create(const std::vector<ClassSample>& sl)
    {
      create(tree, sl, depth, nClasses, nFeatures);
    }

    static void createLeaf(CTNode*& tree,
                           const std::vector<ClassSample>& sl, int nClasses);

    static std::vector<double> find(const CTNode* ctree,
                                    const std::vector<double>& feat)
    {
      if (ctree == nullptr)
        {
          return std::vector<double>();
        }
      else if (ctree->type == CTNode::tFork)
        {
          if (xProd(ctree->value, feat) < 0)
            return find(ctree->left, feat);
          else
            return find(ctree->right, feat);
        }
      else // ctree->type == tLeaf
        {
          return ctree->value;
        }

    }

    virtual std::vector<double> find(const std::vector<double>& feat) const
    {
      return find(tree, feat);
    }

    static int getNNode(const CTNode* tree)
    {
      if (tree->type != CTNode::tFork)
        return 1;
      return getNNode(tree->left) + getNNode(tree->right) + 1;
    }

    int getNNode() const
    {
      return getNNode(tree);
    }

    static int getNLeafs(const CTNode* tree)
    {
      if (tree->type != CTNode::tFork)
        return 1;
      return getNLeafs(tree->left) + getNLeafs(tree->right);
    }

    int getNLeafs() const
    {
      return getNLeafs(tree);
    }

    static int getMaxDepth(const CTNode* tree)
    {
      if (tree->type != CTNode::tFork)
        return 0;
      int mdl = getMaxDepth(tree->left);
      int mdr = getMaxDepth(tree->right);
      if (mdl > mdr)
        return mdl + 1;
      else
        return mdr + 1;
    }

    int getMaxDepth() const
    {
      return getMaxDepth(tree);
    }

    int getDepthSum(CTNode* tree, int depth) const
    {
      if (tree->type != CTNode::tFork)
        return depth;
      return getDepthSum(tree->left, depth + 1) + getDepthSum(tree->right, depth + 1);
    }

    int getDepthSum() const
    {
      return getDepthSum(tree, 0);
    }

    void print(const CTNode* tree, int depth) const
    {
      if (tree != nullptr)
        {
          for (int i = 0; i < depth; ++i)
            std::cout << "*";
          std::cout << " ";
          if (tree->type == CTNode::tFork)
            {
              std::cout << "Fork " << tree->value << std::endl;
              print(tree->left, depth + 2);
              std::cout << std::endl;
              print(tree->right, depth + 2);
            }
          else
            {
              std::cout << "Leaf " << tree->classNr << ": " << tree->value << std::endl;
              print(tree->right, depth);
            }
        }
    }

    virtual void statistics() const
    {
      if (tree != nullptr)
        {
          int nNodes = getNNode();
          int nLeafs = getNLeafs();
          std::cout << "nodes: " << nNodes << "  leafs: " << nLeafs << std::endl;
          std::cout << "maxDepth: " << getMaxDepth() ;
          std::cout << " averageDepth: " << static_cast<double>(getDepthSum()) / nLeafs << std::endl;
        }
    }

  private:
    CTNode* tree;
    int nClasses;
    int nFeatures;
    int depth;
  };
}
#endif
