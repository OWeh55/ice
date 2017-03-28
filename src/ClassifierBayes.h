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
#ifndef CLASSBAYES_H
#define CLASSBAYES_H
//
// D. Lincke 12/2002
// Rodner 2006
// Ortmann 2006, 02/2013
//

#include <vector>
#include <string>
#include "Matrix.h"
#include "Classifier.h"

#include "statistics.h"

// way to define a priori probabilities
// vector of probabilities as parameter of constructor
#define APM_CONSTRUCTOR 1
// equal probabilities
#define APM_EQUAL 2
// probabilities from training
#define APM_TRAIN 3

namespace ice
{
  class ClassifierBayes : public Classifier
  {
  public:
    ClassifierBayes(): Classifier() {};

    ClassifierBayes(int classes, int dimension,
                    bool rejection = false, int apmp = APM_EQUAL, bool diagonal = false);

    ClassifierBayes(int classes, int dimension, const std::vector<double>& app, bool rejection = false);

    virtual ~ClassifierBayes() {};

    using Classifier::Init;
    virtual void Init(int classes, int dimension);

    virtual void setRejectionFactor(double f);
    virtual double getRejectionFactor() const;

    // store classifier to file
    using Classifier::write;
    virtual int write(std::ostream& os) const;

    // restore classifier from file
    using Classifier::read;
    virtual int read(std::istream& is);

    virtual int getMue(int cl, Vector& mue) const;
    virtual int getSigmaInv(int cl, Matrix& sigmainv) const;

  protected:

    //
    virtual bool _finish();

    // Train classifier from single feature vector
    virtual int _train(const ClassSample& s);

    // classify single feature vector
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const;

    static const double epsilon; // parameter for regularization

    // parameters for control of classifier
    bool rejection;  // classifier with rejection ?
    bool diagonal;

    double epsilonRejection = 0.3; // rejection parameter
    std::vector<double> p_k;       // a priori probability of classes
    int apm;                       // way to determine a priori probabilities

    // training data
    std::vector<Statistics> stat_k;       // statistik of Class k;
    int nSamples;   // total nr of samples

    // data for classification
    std::vector<double> mue_k;       // mue of Class k
    std::vector<double> sigma_k_inv; // inverse Kovarianzmatrix
    std::vector<double> u_constant;  // =log(Determinant(2*M_PI*sigma_k[i])) - 2*log(p_k[i])

    // logarithmic measure per class
    mutable std::vector<double> u_k;

    int probabilityMeasure = 1;

    int MinClass(const std::vector<double>& feat, bool& reject) const;
  };
}
#endif
