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
#ifndef CLASSRFOREST_H
#define CLASSRFOREST_H

//
// RandomForest classifier
//  Ortmann 7/2015
//

#include <vector>
#include "ClassifierTree.h"

namespace ice
{
  class ClassifierRandomForest : public Classifier
  {
  public:
    ClassifierRandomForest(): Classifier() {};
    ClassifierRandomForest(int classes, int dimension,
                           int nTrees = 5,
                           int depth = std::numeric_limits<int>::max(),
                           int nUsep = 2);

    virtual ~ClassifierRandomForest() {};

    using Classifier::Init;
    virtual void Init(int classes, int dimension);
    virtual void Init(int classes, int dimension,
                      int nTrees,
                      int depth = std::numeric_limits<int>::max(),
                      int nUsep = 2);

#if 0
    // store classifier to file
    using Classifier::write;
    virtual int write(std::ostream&) const;
    // restore classifier from file
    using Classifier::read;
    virtual int read(std::istream&);
#endif

  protected:
    // Train classifier from single feature vector
    virtual int _train(const ClassSample& s);
    // classify single feature vector
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const;

    virtual bool _finish();

    std::vector<ClassifierTree> classifiers;
    int nTrees;
    int depth;
    int nUse;
  };
}
#endif
