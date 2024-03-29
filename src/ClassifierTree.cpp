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
#include <iostream>
#include <fstream>
#include <assert.h>

#include "vectortools.h"
#include "IceException.h"
#include "numbase.h"
#include "macro.h"
#include "ClassifierTree.h"

using namespace std;

namespace ice
{
#define FNAME "ClassifierTree::ClassifierTree()"
  ClassifierTree::ClassifierTree(int classes, int dimension, int depth)
  {
    try
      {
        Init(classes, dimension, depth);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "ClassifierTree::Init"
  void ClassifierTree::Init()
  {
    if (nClasses == 0)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    Init(nClasses, nFeatures);
  }

  void ClassifierTree::Init(int classes, int dimension, int depth)
  {
    try
      {
        Classifier::Init(classes, dimension);
        this->depth = depth;
        tree.init(classes, dimension, depth);
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "ClassifierTree::Finish"
  bool ClassifierTree::_finish()
  {
    tree.clear();
    tree.create(samples);
    // tree.statistics();
    return true;
  }
#undef FNAME

#define FNAME "ClassifierTree::Classify"
// classify single feature Vector
// returns:  classnumber if classification is successfull
//           -1  in case of error
  int ClassifierTree::_classify(const std::vector<double>& feat,
                                std::vector<double>& prob) const
  {
    //    cout << "."; cout.flush();
    prob = tree.find(feat);
    assert((int)prob.size() == nClasses);
    return maxIndex(prob);
  }
#undef FNAME
}
