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
#ifndef CLASSIFIERMD_H
#define CLASSIFIERMD_H
//
// minimum distance classifier
//  Ortmann 7/1999, 02/2013
//

#include <string>
#include <iostream>

#include "Matrix.h"
#include "Classifier.h"

namespace ice
{
  class ClassifierMinimumDistance : public Classifier
  {
  public:
    ClassifierMinimumDistance(): Classifier() {};
    ClassifierMinimumDistance(int classes, int dimension);
    virtual ~ClassifierMinimumDistance() {};

    using Classifier::Init;
    void Init(int classes, int dimension) override; //

    // store classifier to file
    using Classifier::write;
    virtual int write(std::ostream& os) const override;

    // restore classifier from file
    using Classifier::read;
    virtual int read(std::istream& is) override;

    virtual ice::Vector Center(int n) const;

  protected:
    // Train classifier from single feature vector
    virtual int _train(const ClassSample& s) override;

    // classify single feature vector
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const override;

    virtual bool _finish() override;

    Matrix par; // parameters of the classifier

  };
}
#endif

