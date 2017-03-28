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
#ifndef CLASSIFIER_BAYES_2_H
#define CLASSIFIER_BAYES_2_H

//
// bayes classifier for 2 classes without rejection
// Ortmann 7/1999, 02/2013
//

#include <string>

#include "Matrix.h"
#include "statistics.h"
#include "Classifier.h"

namespace ice
{
  class ClassifierBayes2 : public Classifier
  {
  public:
    ClassifierBayes2(): Classifier() {};
    ClassifierBayes2(int classes, int dimension,
                     double ap1 = 0.5, double ap2 = 0.5, // probability
                     double ac12 = 1.0, double ac21 = 1.0); // costs

    virtual ~ClassifierBayes2() {};

    using Classifier::Init;
    virtual void Init(int classes, int dimension,
                      double ap1 = 0.5, double ap2 = 0.5, // probability
                      double ac12 = 1.0, double ac21 = 1.0); // costs

    // store classifier to file
    using Classifier::write;
    virtual int write(std::ostream&) const;
    // restore classifier from file
    using Classifier::read;
    virtual int read(std::istream&);

  protected:
    // Train classifier from single feature vector
    virtual int _train(const ClassSample& s);
    // classify single feature vector
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const;

    virtual bool _finish();

    double p1, p2; // a priory probability
    double c12, c21; // costs of errors

    matrix<double> sigma1;
    matrix<double> sigma2;

    std::vector<double> my1;
    std::vector<double> my2;

    double limit;

    Statistics st1, st2; // Statistics for both classes
  };
}
#endif
