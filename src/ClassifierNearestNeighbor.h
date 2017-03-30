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
#ifndef CLASSIFIER_NEAREST_NEIGHBOR_H
#define CLASSIFIER_NEAREST_NEIGHBOR_H

#include "Matrix.h"
#include "ClassifierNormal.h"
#include "KDTree.h"

namespace ice
{
  class ClassifierNearestNeighbor : public ClassifierNormal
  {
  public:
    ClassifierNearestNeighbor(): ClassifierNormal() {};
    ClassifierNearestNeighbor(int classes, int dimension,
                              int _num_neighbors = 1,
                              bool norm = false);
    virtual ~ClassifierNearestNeighbor() {};

    using Classifier::Init;
    void Init(int classes, int dimension,
              int nNeighbors = 1, bool norm = false); // standard initialization

#if 0
    // store classifier to file
    using Classifier::write;
    virtual int write(std::ostream& os) const;

    // restore classifier from file
    using Classifier::read;
    virtual int read(std::istream& is);
#endif

    void setConsiderDistances(bool _consider_distances)
    {
      consider_distances = _consider_distances;
    };

    bool getConsiderDistances() const
    {
      return consider_distances;
    };

    void setRejectionThreshold(double _rejection_threshold)
    {
      rejection_threshold = _rejection_threshold;
    };

    double getRejectionThreshold() const
    {
      return rejection_threshold;
    };

    int getNNeighbors() const
    {
      return nNeighbors;
    };

    void setNNeighbors(int nn)
    {
      nNeighbors = nn;
    }

    using ClassifierNormal::read;
    virtual int read(std::istream& is);
    using ClassifierNormal::write;
    virtual int write(std::ostream& fn) const;

  protected:
    /** classify single feature vector*/
    virtual int _classify(const std::vector<double>& feat,
                          std::vector<double>& prob) const;

    /** classify by searching (1) nearest neighbor */
    virtual int _classify1(const std::vector<double>& feat,
                           std::vector<double>& probs) const;

    /** classify by searching k nearest neighbors */
    virtual int _classifyk(const std::vector<double>& feat,
                           std::vector<double>& probs) const;

    virtual bool _finish();

    /*
    void insert_distance(double distance, int class_nr) const;

    int vote(const std::vector<double> distances,
       std::vector<int> indicees) const;

    int vote_binary(const std::vector<double> distances,
                    std::vector<int> indicees, bool& rejected) const;
    */

    int nNeighbors;
    bool norm;

    bool consider_distances = false;

    double rejection_threshold = 0.0;
    /** sample will be erjected if probability is not bigger than threshold */

    KDTree<ClassSample> tree;
  };
}
#endif
