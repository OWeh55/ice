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
#include <iostream>
#include <fstream>

#include "message.h"
#include "numbase.h"
#include "macro.h"

#include "vectortools.h"

#include "ClassifierKNearestNeighbor.h"

using namespace std;

namespace ice
{
#define FNAME "ClassifierKNN::ClassifierKNN()"
  ClassifierKNN::ClassifierKNN(int classes, int dimension,
                               int nNeighbors, bool norm):
    ClassifierNormal(classes, dimension),
    nNeighbors(nNeighbors), norm(norm)
  {
    IF_FAILED(Init(classes, dimension, nNeighbors))
    {
      // if initialisation fails
      Message(FNAME, M_0, ERROR);
    }
  }
#undef FNAME

#define FNAME "ClassifierKNN::Init"
  void ClassifierKNN::Init(int classes, int dimension,
                           int nNeighbors, bool norm)
  {
    RETURN_VOID_IF_FAILED(ClassifierNormal::Init(classes, dimension));
    this->nNeighbors = nNeighbors;
    this->norm = norm;
  }
#undef FNAME
#define FNAME "ClassifierKNN::Finish"
  bool ClassifierKNN::_finish()
  {
    if (norm)
      {
        doNormalization(normalizeScaling);
      }
    tree.create(samples);
    samples.clear();
    return true;
  }
#undef FNAME
#define FNAME "Classify"
// classify single feature Vector
// returns:  classnumber if classification is successfull
//           -1  in case of error
  int ClassifierKNN::_classify(const std::vector<double>& feat,
                               std::vector<double>& probs) const
  {
    vector<double> nf(feat);
    if (norm)
      {
        normalize(nf);
      }

    vector<double> distances;
    vector<const ClassSample*> neighbors;
    tree.findKNearest(nf, nNeighbors, neighbors, distances);

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
} // namespace ice
