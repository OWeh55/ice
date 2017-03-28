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
#include <assert.h>
#include <map>
#include <set>
#include <limits>
#include "matrixtools.h"
#include "matrixarith.h"
#include "mateigen.h"
#include "treecl.h"

namespace ice
{
  /*
   * class TreeCl = oblique tree for classifiers
   */

  void TreeCl::create(CTNode*& ctree, const std::vector<ClassSample>& sl,
                      int maxdepth, int nClasses, int nFeatures)
  {
    if (maxdepth > 0)
      {
        // do the split here
        std::vector<double> splitvector;
        if (findSplit(sl, splitvector, nClasses, nFeatures))
          {
            ctree = new CTNode(CTNode::tFork);
            ctree->value = splitvector;
            std::vector<ClassSample> leftSamples;
            std::vector<ClassSample> rightSamples;
            for (unsigned int i = 0; i < sl.size(); ++i)
              {
                if (xProd(splitvector, sl[i].features) < 0)
                  {
                    leftSamples.push_back(sl[i]);
                  }
                else
                  {
                    rightSamples.push_back(sl[i]);
                  }
              }

            assert(leftSamples.size() > 0);
            assert(rightSamples.size() > 0);
            create(ctree->left, leftSamples, maxdepth - 1,
                   nClasses, nFeatures);
            create(ctree->right, rightSamples, maxdepth - 1,
                   nClasses, nFeatures);
          }
        else
          {
            createLeaf(ctree, sl, nClasses);
          }
      }
    else
      createLeaf(ctree, sl, nClasses);
  }


  double TreeCl::evaluateSplit(const std::vector<ClassSample>& samples,
                               const std::vector<double>& weights)
  {
    // returns the effective entropy per sample, if there
    // are samples at both sides
    // returns numeric_limits<int>::max(), if one side is "empty"
    std::map<int, int> counterLeft;
    std::map<int, int> counterRight;
    int nLeft = 0;
    int nRight = 0;
    for (unsigned int i = 0; i < samples.size(); ++i)
      {
        if (xProd(weights, samples[i].features) < 0)
          {
            counterLeft[samples[i].classNr]++;
            ++nLeft;
          }
        else
          {
            counterRight[samples[i].classNr]++;
            ++nRight;
          }
      }

    if (nLeft == 0 || nRight == 0)
      return std::numeric_limits<int>::max();

    double entropyLeft = 0;
    for (auto& d : counterLeft)
      {
        double p = static_cast<double>(d.second) / nLeft;
        entropyLeft -= p * log2(p);
      }

    double entropyRight = 0;
    for (auto& d : counterRight)
      {
        double p = static_cast<double>(d.second) / nRight;
        entropyRight -= p * log2(p);
      }

    return (nLeft * entropyLeft + nRight * entropyRight) / (nLeft + nRight);
  }

  bool TreeCl::findSplit(const std::vector<ClassSample>& samples,
                         std::vector<double>& weights,
                         int nClasses, int nFeatures)
  {
    std::vector<int> nElements(nClasses, 0);
    std::vector<std::vector<double>> classFeatureSum(nClasses, std::vector<double>(nFeatures, 0));

    assert(samples.size() > 0);

    // # of elements and center of classes
    for (unsigned int i = 0; i < samples.size(); ++i)
      {
        int classNr = samples[i].classNr;
        ++nElements[classNr];
        classFeatureSum[classNr] += samples[i].features;
      }

    int nc = 0;
    for (int i = 0; i < nClasses; ++i)
      if (nElements[i] > 0)
        ++nc;

    assert(nc > 0);

    if (nc < 2)
      {
        // only one class - no split
        return false;
      }

    int max, max2;
    if (nElements[0] > nElements[1])
      {
        max = 0;
        max2 = 1;
      }
    else
      {
        max = 1;
        max2 = 0;
      }

    for (int i = 2; i < nClasses; ++i)
      {
        if (nElements[i] > nElements[max])
          {
            max2 = max;
            max = i;
          }
        else if (nElements[i] > nElements[max2])
          max2 = i;
      }

    std::vector<double> v1 = classFeatureSum[max] * (1.0 / nElements[max]);
    std::vector<double> v2 = classFeatureSum[max2] * (1.0 / nElements[max2]);

    std::vector<double> dv = v2 - v1;
    double norm = length(dv);

    if (norm < 1e-10) // !!!! besseren Wert finden
      return false;

    weights = dv * (1 / norm);

    std::vector<double> center = (v1 + v2) * 0.5;

    double threshold = weights * center;
    weights.push_back(-threshold);

    return true;
  }

  void TreeCl::createLeaf(CTNode*& tree,
                          const std::vector<ClassSample>& sl,
                          int nClasses)
  {
    tree = new CTNode(CTNode::tLeaf);
    tree->value.resize(nClasses);
    std::vector<int> classCount(nClasses, 0);
    int nSamples = sl.size();
    for (int i = 0; i < nSamples; ++i)
      classCount[sl[i].classNr]++;

    for (int i = 0; i < nClasses; ++i)
      tree->value[i] = classCount[i] * 1.0 / nSamples;
  }

}
