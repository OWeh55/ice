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
#include <string>

#include "IceException.h"
#include "numbase.h"
#include "macro.h"
#include "ClassifierNormal.h"

using namespace std;

namespace ice
{
#define FNAME "ClassifierNormal::ClassifierNormal()"
  ClassifierNormal::ClassifierNormal(int classes, int dimension):
    Classifier(classes, dimension)
  {
    try
      {
        Init(classes, dimension);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Train"
  // Train classifier from single feature vector
  int ClassifierNormal::_train(const ClassSample& s)
  {
    // store values for offline learning
    samples.push_back(s);
    return OK;
  }
#undef FNAME

  // normalizes features in vector
  // must be called in classify
  void ClassifierNormal::normalize(std::vector<double>& feat) const
  {
    if (normalizationMode & normalizeCenter)
      {
        for (int i = 0; i < nFeatures; ++i)
          {
            feat[i] -= featureMean[i];
          }
      }

    if (normalizationMode & normalizeScaling)
      {
        for (int i = 0; i < nFeatures; ++i)
          {
            feat[i] = feat[i] * scaleFactor[i];
          }
      }
  }

  void ClassifierNormal::writeNormalization(std::ostream& os) const
  {
    os << normalizationMode << std::endl;
    if (normalizationMode & normalizeCenter)
      {
        for (int i = 0; i < nFeatures; ++i)
          {
            os << featureMean[i] << " ";
          }
      }

    if (normalizationMode & normalizeScaling)
      {
        for (int i = 0; i < nFeatures; ++i)
          {
            os << scaleFactor[i] << endl;
          }
      }
  }

  void ClassifierNormal::readNormalization(std::istream& is)
  {
    is >> normalizationMode;
    if (normalizationMode & normalizeCenter)
      {
        featureMean.resize(nFeatures);
        for (int i = 0; i < nFeatures; ++i)
          {
            is >> featureMean[i];
          }
      }

    if (normalizationMode & normalizeScaling)
      {
        scaleFactor.resize(nFeatures);
        for (int i = 0; i < nFeatures; ++i)
          {
            is >> scaleFactor[i];
          }
      }
  }

  // function to be called during _finish
  // uses collected sampes to calculate a normalization
  // and normalizes collection
  void ClassifierNormal::doNormalization(int mode)
  {
    if (mode & normalizeIsotropic)
      {
        mode |= normalizeScaling;
      }
    normalizationMode = mode;

    int nSamples = samples.size();

    // calculate mean
    vector<double> sum(nFeatures, 0.0);
    for (int i = 0; i < nSamples; ++i)
      {
        for (int k = 0; k < nFeatures; ++k)
          {
            sum[k] += samples[i].features[k];
          }
      }

    featureMean.resize(nFeatures);

    for (int k = 0; k < nFeatures; ++k)
      {
        featureMean[k] = sum[k] / nSamples;
      }

    // calculate standard deviation SD
    vector<double> sum2(nFeatures, 0.0);
    for (int i = 0; i < nSamples; ++i)
      {
        for (int k = 0; k < nFeatures; ++k)
          {
            double val = samples[i].features[k] - featureMean[k];
            sum2[k] += val * val;
          }
      }

    featureSD.resize(nFeatures);

    for (int k = 0; k < nFeatures; ++k)
      {
        featureSD[k] = sqrt(sum2[k] / nSamples);
      }

    if (mode & normalizeScaling)
      {
        scaleFactor.resize(nFeatures);
        if (mode & normalizeIsotropic)
          {
            double sum = 0.0;
            for (int i = 0; i < nFeatures; ++i)
              {
                sum += featureSD[i] * featureSD[i];
              }
            double factor = 1.0 / sqrt(sum / nFeatures);
            if (factor == 0.0)   // do nothing
              {
                factor = 1.0;
              }
            for (int i = 0; i < nFeatures; ++i)
              {
                scaleFactor[i] = factor;
              }
          }
        else
          {
            for (int i = 0; i < nFeatures; ++i)
              {
                if (featureSD[i] != 0.0)
                  {
                    scaleFactor[i] = 1.0 / featureSD[i];
                  }
                else
                  {
                    scaleFactor[i] = 1.0;
                  }
              }
          }
      }

    for (int i = 0; i < nSamples; ++i)
      {
        normalize(samples[i].features);
      }
  }
}
