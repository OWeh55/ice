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
    virtual void Init(int classes, int dimension) override;
    virtual void Init(int classes, int dimension,
                      int nTrees,
                      int depth = std::numeric_limits<int>::max(),
                      int nUsep = 2);

#if 0
    // store classifier to file
    using Classifier::write;
    virtual int write(std::ostream&) const override;
    // restore classifier from file
    using Classifier::read;
    virtual int read(std::istream&) override;
#endif

  protected:
    // Train classifier from single feature vector
    virtual int _train(const ClassSample& s) override;
    // classify single feature vector
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const override;

    virtual bool _finish() override;

    std::vector<ClassifierTree> classifiers;
    int nTrees;
    int depth;
    int nUse;
  };
}
#endif
