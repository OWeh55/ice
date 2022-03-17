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

#include "IceException.h"
#include "numbase.h"
#include "macro.h"

#include "vectortools.h"

#include "ClassifierNearestNeighbor.h"

using namespace std;

namespace ice
{
#define FNAME "ClassifierNearestNeighbor::ClassifierNearestNeighbor()"
  ClassifierNearestNeighbor::ClassifierNearestNeighbor(int classes,
      int dimension,
      int nNeighbors,
      bool norm):
    ClassifierNormal(classes, dimension),
    nNeighbors(nNeighbors), norm(norm)
  {
    try
      {
        Init(classes, dimension, nNeighbors, norm);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "ClassifierNearestNeighbor::Init"
  void ClassifierNearestNeighbor::Init(int classes, int dimension,
                                       int nNeighbors, bool norm)
  {
    try
      {
        ClassifierNormal::Init(classes, dimension);
        this->nNeighbors = nNeighbors;
        this->norm = norm;
      }
    RETHROW;
  }
#undef FNAME
#define FNAME "ClassifierNearestNeighbor::Finish"
  bool ClassifierNearestNeighbor::_finish()
  {
    if (norm)
      {
        doNormalization(normalizeScaling);
      }
    tree.create(samples);
    //    tree.statistics();
    samples.clear();
    return true;
  }
#undef FNAME
#define FNAME "Classify"
// classify single feature Vector
// returns:  classnumber if classification is successfull
//           -1  in case of error
  int ClassifierNearestNeighbor::_classify(const std::vector<double>& feat,
      std::vector<double>& probs) const
  {
    std::vector<double> fn(feat);
    if (norm)
      {
        normalize(fn);
      }

    if (nNeighbors == 1)
      {
        return _classify1(fn, probs);
      }
    else
      {
        return _classifyk(fn, probs);
      }
  }

  int ClassifierNearestNeighbor::_classify1(const std::vector<double>& fn,
      std::vector<double>& probs) const
  {
    const ClassSample* rp = tree.findNearest(fn);
    if (rp == nullptr)
      {
        throw IceException(FNAME, M_NOT_INITIALIZED);
      }
    return rp->classNr;
  }

  int ClassifierNearestNeighbor::_classifyk(const std::vector<double>& fn,
      std::vector<double>& probs) const
  {
    vector<double> distances;
    vector<const ClassSample*> neighbors;
    tree.findKNearest(fn, nNeighbors, neighbors, distances);

    // probabilities are always calculated
    probs.clear();
    probs.resize(nClasses, 0); // set all to zero

    double sum = 0.0;
    for (unsigned int i = 0; i < neighbors.size(); ++i)
      {
        int cnr = neighbors[i]->classNr;
        if (cnr >= 0)
          {
            double cval = consider_distances ? exp(-distances[i]) : 1.0;
            sum += cval;
            probs[cnr] += cval;
          }
      }

    double max = 0.0;
    int maxClass = -1; // defaults to rejection
    for (int i = 0; i < nClasses; ++i)
      {
        probs[i] /= sum;
        if (probs[i] > max)
          {
            max = probs[i];
            if (max > rejection_threshold)
              {
                maxClass = i;
              }
          }
      }

    return maxClass;
  }
#undef FNAME
#define FNAME "ClassifierNearestNeighbor::read"
  int ClassifierNearestNeighbor::read(std::istream& source)
  {
    string id;
    source >> id;
    if (id != "ClassifierNearestNeighbor")
      throw IceException(FNAME, M_WRONG_FILEFORMAT);
    source >> nFeatures;
    source >> nClasses;
    source >> nNeighbors;
    source >> norm;
    if (norm)
      {
        readNormalization(source);
      }
    Init(nClasses, nFeatures, nNeighbors, norm);
    tree.read(source);
    state = ready;
    return OK;
  }
#undef FNAME
#define FNAME "ClassifierNearestNeighbor::write"
  int ClassifierNearestNeighbor::write(std::ostream& dest) const
  {
    dest << "ClassifierNearestNeighbor" << endl;
    dest << nFeatures << " " ;
    dest << nClasses << " ";
    dest << nNeighbors << " ";
    dest << norm << endl ;
    if (norm)
      {
        writeNormalization(dest);
      }
    tree.write(dest);
    return OK;
  }
#undef FNAME

} // namespace ice
