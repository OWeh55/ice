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
#ifndef ICE_CLASSIFIER_H
#define ICE_CLASSIFIER_H
//
// classifier hierarchy
// Ortmann 7/1999, 9/2002, 3/2013, 7/2014, 6/2015
//
#include <vector>

#include "Vector.h"
#include "Matrix.h"
#include "ClassSample.h"

namespace ice
{

  /**
   * (abstract) base class for classifiers
   * implements wrapper functions, that convert different datastructures for input
   * all protected base functions deal with ClassSample or vector<double>
   * wrappers also manage training status etc
   */

  class Classifier
  {
  public:
    static const int invalid = 0;
    static const int training = 1;
    static const int ready = 2;

    // special "class numbers"
    static const int ambiguous = -2;
    static const int rejected = -1;

    Classifier(): nFeatures(0), nClasses(0), classTrained(0), state(invalid) {};
    Classifier(int classes, int dimension):
      nFeatures(dimension), nClasses(classes), classTrained(classes, 0), state(training) {};

    virtual ~Classifier() {};

    virtual void Init(); // = reinit (with already known parameters)
    virtual void Init(int classes, int dimension); // [re-]init with new parameters

    // Train classifier from single feature vector
    virtual int Train(const ClassSample& s);
    virtual int Train(int cl, const ice::Vector& feat);
    virtual int Train(int cl, const std::vector<double>& feat);

    // Train classifier from classified sample list
    // vector<ClassSample>
    virtual int Train(const std::vector<ClassSample>& sampleVector);
    // Matrix with features and classnumbers
    virtual int Train(const Matrix& features);
    // Matrix with features, IVector with classnumbers
    virtual int Train(const Matrix& features, const IVector& classnr);

    // finish training process
    // must be called once after training before classification
    virtual int Finish();

    // classify single feature vector
    virtual int Classify(const std::vector<double>& feat) const;
    virtual int Classify(const std::vector<double>& feat,
                         std::vector<double>& prob) const;

    virtual int Classify(const ice::Vector& feat) const;

    // classify list of feature vectors and extend matrix with "class" column
    virtual int Classify(Matrix& m) const;
    // classify list of feature vectors and
    // deliver integer vector with classnumbers
    virtual int Classify(const Matrix& m, IVector& cl) const;

    // verify classifier from classified sample list
    virtual double Test(const std::vector<ClassSample>& sl) const;
    virtual double Test(const Matrix& m) const;
    virtual double Test(const Matrix& m, const IVector& classnr) const;

    // verify classifier from classified sample list and return error matrix
    virtual double Test(const Matrix& m, Matrix& f) const;
    //virtual double Test(const matrix &m,const IVector &classnr,Matrix &f);

    // store classifier in file
    virtual int write(std::ostream& os) const;
    virtual int write(const std::string& fn) const;

    // restore classifier from file
    virtual int read(std::istream& is);
    virtual int read(const std::string& fn);

    // information about classifier properties
    virtual bool hasRejection() const
    {
      return false;
    }
    virtual bool hasAmbiguity() const
    {
      return false;
    }

  protected:
    // basic train function, override this in subclasses
    virtual int _train(const ClassSample& s) = 0;

    // basic finish function, override this in subclasses
    virtual bool _finish() = 0;

    // basic classify function, override this in subclasses
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const = 0;

    int nFeatures;  // number of features
    int nClasses; // number of classes

    std::vector<int> classTrained; // number of trained samples per class
    int state;

  protected:
    virtual void _reset() // reset to state "ready for training"
    {
      state = training;
      fill(classTrained.begin(), classTrained.end(), 0);
    }

    virtual void _invalidate() // makes classifier invalid
    {
      state = invalid;
      nClasses = 0;
      classTrained.clear();
    }
  };

  std::ostream& operator<<(std::ostream& os, const Classifier& cl);
  std::istream& operator>>(std::istream& is, Classifier& cl);
}
#endif
