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
#ifndef CLASSIFIER_TREE_H
#define CLASSIFIER_TREE_H
//
// [oblique] tree classifier
// Ortmann 6/15
//
#include "ClassifierNormal.h"
#include "treecl.h"

namespace ice
{
  class ClassifierTree : public ClassifierNormal
  {
  public:
    ClassifierTree() {};
    ClassifierTree(int classes, int dimension,
                   int depth = std::numeric_limits<int>::max());
    virtual ~ClassifierTree() {}

    virtual void Init() override; // = ReInit
    virtual void Init(int classes, int dimension,
                      int depth = std::numeric_limits<int>::max());

#if 0
    // write classifier to file
    using Classifier::write;
    virtual int write(std::ostream& os) const;

    // read classifier from file
    using Classifier::read;
    virtual int read(std::istream& is);
#endif

  protected:
    // classify single feature vector
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const override;
    virtual bool _finish() override;

  protected:
    TreeCl tree;
    int depth;
  };
}
#endif
