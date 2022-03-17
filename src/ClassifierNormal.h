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
#ifndef CLASSIFIER_NORMAL_H
#define CLASSIFIER_NORMAL_H

#include <iostream>

#include "range.h"
#include <vector>

#include "Classifier.h"
//
// abstract classifier class with normalisation
// Ortmann 07/2014
//
namespace ice
{
  class ClassifierNormal : public Classifier
  {
  public:
    static const int normalizeCenter = 1;
    static const int normalizeScaling = 2;
    static const int normalizeIsotropic = 4;

    ClassifierNormal(): Classifier() {};
    ClassifierNormal(int classes, int dimension);
    virtual ~ClassifierNormal() {};

    void writeNormalization(std::ostream& os) const;
    void readNormalization(std::istream& is);

  protected:
    // Methods called by base class
    // Train classifier from single feature vector
    virtual int _train(const ClassSample& s) override;

    void normalize(std::vector<double>& feat) const;
    void doNormalization(int mode = normalizeCenter | normalizeScaling | normalizeIsotropic);

    int normalizationMode;
    std::vector<double> featureMean;
    std::vector<double> featureSD;
    std::vector<double> scaleFactor;
    std::vector<ClassSample> samples;
  };
}
#endif
