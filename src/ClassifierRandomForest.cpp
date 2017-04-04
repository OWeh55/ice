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

#include <set>
#include "macro.h"
#include "IceException.h"
#include "vectortools.h"
#include "ClassifierRandomForest.h"

namespace ice
{
  ClassifierRandomForest::ClassifierRandomForest(int classes, int dimension,
      int nTrees,
      int depth,
      int nUsep):
    Classifier(classes, dimension)
  {
    Init(nClasses, dimension, nTrees, depth, nUsep);
  }

#define FNAME "RandomForest::Init"
  void ClassifierRandomForest::Init(int classes, int dimension,
                                    int nTrees,
                                    int depth,
                                    int nUsep)
  {
    try
      {
        Classifier::Init(classes, dimension);
        this->nTrees = nTrees;
        classifiers.resize(nTrees);
        nUse = nUsep;
        if (nUse > nTrees)
          {
            throw IceException(FNAME, M_WRONG_PARAMETER, WRONG_PARAM);
          }
        for (unsigned int i = 0; i < classifiers.size(); ++i)
          {
            classifiers[i].Init(classes, dimension, depth);
          }
      }
    RETHROW;
  }

  void ClassifierRandomForest::Init(int classes, int dimension)
  {
    Init(classes, dimension, 5, 10, 2);
  }

#undef FNAME
  int ClassifierRandomForest::_train(const ClassSample& s)
  {
    std::set<int> useTrees;
    while ((int)useTrees.size() < nUse)
      {
        useTrees.insert(rand() % nTrees);
      }
    for (int n : useTrees)
      {
        classifiers[n].Train(s);
      }
    return OK;
  }

  bool ClassifierRandomForest::_finish()
  {
    bool finishOk = true;
    for (int n = 0; finishOk && n < nTrees; ++n)
      {
        if (classifiers[n].Finish() != OK)
          {
            finishOk = false;
          }
      }

    return finishOk;
  }

  int ClassifierRandomForest::_classify(const std::vector<double>& f,
                                        std::vector<double>& prob) const
  {
    prob.resize(nClasses);
    int ct = 0;
    for (int i = 0; i < nTrees; ++i)
      {
        int cl = classifiers[i].Classify(f);
        if (cl >= 0)
          {
            ct++;
            prob[cl] += 1.0;
          }
      }

    for (int i = 0; i < nClasses; ++i)
      {
        prob[i] /= ct;
      }
    return maxIndex(prob);
  }
}
