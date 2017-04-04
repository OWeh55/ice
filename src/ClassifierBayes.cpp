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

#include <algorithm>
#include <fstream>
#include <math.h>
#include <string>

#include "IceException.h"
#include "numbase.h"
#include "macro.h"
#include "MatrixAlgebra.h"

#include "ClassifierBayes.h"

const double epsilonNumerics = 1e-11;

using namespace std;

namespace ice
{
  const double ClassifierBayes::epsilon = 0.01; // parameter for regularization

#define FNAME "ClassifierBayes::ClassifierBayes()"

  ClassifierBayes::ClassifierBayes(int classes, int dimension,
                                   bool rejectionp, int apmp, bool _diagonal):
    Classifier(classes, dimension), rejection(rejectionp), diagonal(_diagonal),
    apm(apmp)
  {
    try
      {
        Init(classes, dimension);
      }
    RETHROW;
  }

  ClassifierBayes::ClassifierBayes(int nClasses, int dim,
                                   const vector<double>& app, bool rejection):
    Classifier(nClasses, dim), rejection(rejection), apm(APM_CONSTRUCTOR)
  {
    try
      {
        p_k = app;
        Init(nClasses, dim);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "ClassifierBayes::Init"
  void ClassifierBayes::Init(int classnr, int dimension)
  {
    try
      {
        Classifier::Init(classnr, dimension);

        int classes = nClasses;
        if (rejection)                          // add rejection class when needed
          {
            classes++;
          }

        stat_k.resize(classes);          // statistics per class
        p_k.resize(classes);             // probability per class
        mue_k.resize(classes * nFeatures); // mean vectors
        sigma_k_inv.resize(classes * nFeatures * nFeatures);
        u_constant.resize(classes);
        u_k.resize(classes);

        for (int i = 0; i < classes; i++)
          {
            stat_k[i].Init(nFeatures);
          }

        for (unsigned int i = 0; i < p_k.size(); i++)
          {
            p_k[i] = 1.0 / classes;
          }

        nSamples = 0;
      }
    RETHROW;
  }

#undef FNAME
#define FNAME "Train"
// Train classifier from single feature vector
  int ClassifierBayes::_train(const ClassSample& s)
  {
    // update class statistics
    Put(stat_k[s.classNr], s.features);

    // class independed statics is used for rejection
    if (rejection)
      {
        Put(stat_k[nClasses], s.features);
      }

    nSamples++;
    return OK;
  }
#undef FNAME

#define FNAME "ClassifierBayes::Finish"
// Finish training Process
  bool ClassifierBayes::_finish()
  {
    vector<double>::iterator sip = sigma_k_inv.begin();
    vector<double>::iterator mp = mue_k.begin();

    int classes = nClasses;
    if (rejection)
      {
        classes++;  // zusätzlich Rückweisungsklasse
      }

    u_constant.resize(classes);

    double normalization_constant = nFeatures * log(2 * M_PI);

    for (int i = 0; i < classes; i++)
      {
        Vector mean = Mean(stat_k[i]);

        for (int k = 0; k < nFeatures; k++)
          {
            *mp = mean[k];
            mp++;
          }

        if (apm == APM_TRAIN)
          {
            p_k[i] = Weight(stat_k[i]) / nSamples;
          }

        Matrix sigma_k = Covariance(stat_k[i]);

        if (diagonal)
          {
            // zero elements outside diagonale
            for (int i = 0; i < sigma_k.rows(); i++)
              for (int j = 0 ; j < sigma_k.cols() ; j++)
                if (i != j)
                  {
                    sigma_k[i][j] = 0.0;
                  }
          }

        double min = -1.0;

        // kleinstes positives Element der Hauptdiagonale
        for (int j = 0; j < nFeatures; j++)
          {
            if (sigma_k[j][j] > 0)
              {
                if (min < 0.0)   // first positive element
                  {
                    min = sigma_k[j][j];
                  }
                else if (sigma_k[j][j] < min)
                  {
                    min = sigma_k[j][j];
                  }
              }
          }

        for (int j = 0; j < nFeatures; j++)
          {
            // nichtpositive Hauptdiagonalen-Elemente positiv machen
            if (sigma_k[j][j] <= 0)
              {
                sigma_k[j][j] = 0.5 * min;
              }
          }

        double det = 0.0;
        bool ok = true;

        try
          {
            det = CholeskyDeterminant(sigma_k);
          }
        catch (IceException& ex)
          {
            ok = false;
          }

        while (!ok || (fabs(det) < epsilonNumerics))
          {
            for (int k = 0; k < nFeatures; k++)
              {
                sigma_k[k][k] += epsilon;
              }

            ok = true;
            try
              {
                det = CholeskyDeterminant(sigma_k);
              }
            catch (IceException& ex)
              {
                ok = false;
              }
          }

        Matrix Inverse = CholeskyInverse(sigma_k);

        for (int k = 0; k < Inverse.rows(); k++)
          for (int j = k; j < Inverse.cols(); j++)
            {
              if (k == j)
                {
                  *sip = Inverse[k][j];
                }
              else
                {
                  *sip = (Inverse[k][j] + Inverse[j][k]);
                }

              sip++;
            }

        // | SIGMA_K | (verzicht auf Vorfaktoren)
        double u_val = log(det);
        u_constant[i] = -2 * log(p_k[i]) + u_val + normalization_constant;
      }
    return true;
  }
#undef FNAME

  void ClassifierBayes::setRejectionFactor(double f)
  {
    epsilonRejection = f;
  }

  double ClassifierBayes::getRejectionFactor() const
  {
    return epsilonRejection;
  }

#define FNAME "Classify"
// classify single feature vector
// returns: classification result or rejection
// parameter prob is a vector with probabilities for each class

  int ClassifierBayes::_classify(const std::vector<double>& feat,
                                 std::vector<double>& prob) const
  {
    bool rejected = false;

    int mc = MinClass(feat, rejected);

    double ukmin = u_k[mc];

    int classes = nClasses;
    if (rejection)
      {
        classes++;
      }

    if (!prob.empty())   // probabilities needed ?
      {
        // Wahrscheinlichkeiten fuer jede Klasse
        double sum = 0.0;

        for (int i = 0; i < classes; i++)
          {
            // rel. Wahrscheinlichkeit der einzelnen Klassen
            // richtige Formel (für Gauss-Verteilung) liefert oft ungeeignete
            // Werte, deshalb hier verschiedene Modifikationen
            switch (probabilityMeasure)
              {
              case 1:
                // echte rel. Wahrscheinlichkeit bei Gauss-Verteilung
                u_k[i] = exp(-0.5 * (u_k[i] - ukmin));
                break;

              case 2:
              {
                double d = u_k[i] - u_constant[mc]; // Mahalanobis-Distanz

                if (fabs(d) < epsilonNumerics)
                  {
                    u_k[i] = 1e20;
                  }
                else
                  {
                    u_k[i] = 1.0 / d;
                  }

                break;
              }
              case 3:
                u_k[i] = exp(-0.5 * (sqrt(sqrt(fabs(u_k[i] - ukmin)))));
                break;

              default:
                throw IceException(FNAME, M_WRONG_MODE, WRONG_PARAM);
              }

            sum += u_k[i]; // Summenwerte für Normierung
          }

        // Normieren
        for (int i = 0; i < nClasses; i++)
          {
            u_k[i] = u_k[i] / sum;
          }

        for (int i = 0; i < nClasses; i++)
          {
            prob[i] = u_k[i];
          }
      }
    if (rejected)
      {
        return -1;
      }
    else
      {
        return mc;
      }
  }

#undef FNAME

  int ClassifierBayes::MinClass(const std::vector<double>& feat, bool& reject) const
  {
    vector<double> h(nFeatures);
    vector<double>::const_iterator m = sigma_k_inv.begin();
    vector<double>::const_iterator mp = mue_k.begin();

    reject = false;

    // erste Klasse
    {
      for (int l = 0 ; l < nFeatures ; l++)
        {
          h[l] = feat[l] - *mp;
          mp++;
        }

      double sum = 0;

      for (int j = 0; j < nFeatures; j++)
        {
          for (int k = j; k < nFeatures; k++)
            {
              sum += *m * h[k] * h[j];
              m++;
            }
        }

      u_k[0] = u_constant[0] + sum;
    }

    double min = u_k[0]; // erste Klasse ist bisheriges Minimum
    int mc = 0;

    for (int i = 1; i < nClasses; i++)
      {
        // weitere Klassen

        for (int l = 0; l < nFeatures; l++)
          {
            h[l] = feat[l] - *mp;
            mp++;
          }

        double sum = 0;

        for (int j = 0; j < nFeatures; j++)
          {
            for (int k = j; k < nFeatures; k++)
              {
                sum += *m * h[k] * h[j];
                m++;
              }
          }

        u_k[i] = u_constant[i] + sum;

        if (u_k[i] < min)
          {
            mc = i;
            min = u_k[i];
          }
      }

    if (rejection)
      {
        for (int l = 0 ; l < nFeatures ; l++)
          {
            h[l] = feat[l] - *mp;
            mp++;
          }

        double sum = 0;

        for (int j = 0; j < nFeatures; j++)
          {
            for (int k = j; k < nFeatures; k++)
              {
                sum += *m * h[k] * h[j];
                m++;
              }
          }

        u_k[nClasses] = u_constant[nClasses] + sum / epsilonRejection;
        reject = u_k[nClasses] < min;
      }

    return mc;
  }

#define FNAME "ClassifierBayes::Write"
// store classifier to file
  int ClassifierBayes::write(std::ostream& dest) const
  {
    dest << "ClassifierBayes" << endl;
    dest << nFeatures << " " << nClasses << " ";
    dest << rejection << endl;
    if (rejection)
      {
        dest << epsilonRejection << endl;
      }

    int classes = nClasses;

    if (rejection)
      {
        classes++;
      }

    for (int i = 0; i < classes; i++)
      {
        dest << stat_k[i] << endl;
      }

    for (int i = 0; i < classes; i++)
      {
        dest << p_k[i] << endl;
      }

    dest << apm << endl;

    vector<double>::const_iterator mp = mue_k.begin();

    for (int k = 0; k < classes; k++)
      for (int d = 0; d < nFeatures; d++)
        {
          dest << *(mp++) << endl;
        }

    vector<double>::const_iterator sp = sigma_k_inv.begin();

    for (int k = 0; k < classes; k++)
      for (int i = 0; i < nFeatures; i++)
        for (int j = i; j < nFeatures; j++)
          {
            dest << *(sp++) << endl;
          }

    for (int k = 0; k < classes; k++)
      {
        dest << u_constant[k] << endl;
      }

    return OK;
  }

#undef FNAME
#define FNAME "ClassifierBayes::Read"
// restore classifier from file
  int ClassifierBayes::read(std::istream& source)
  {
    string id;
    source >> id;
    if (id != "ClassifierBayes")
      {
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
      }

    source >> nFeatures;
    source >> nClasses;
    source >> rejection;

    if (rejection)
      {
        source >> epsilonRejection;
      }

    Init(nClasses, nFeatures);

    int classes = nClasses;

    if (rejection)
      {
        classes++;
      }

    nSamples = 0;

    for (int i = 0; i < classes; i++)
      {
        source >> stat_k[i];
        nSamples += (int)Weight(stat_k[i]);

        if (i < nClasses)
          if (Weight(stat_k[i]) > 0)
            {
              classTrained[i] = (int)Weight(stat_k[i]);
            }
      }

    for (int i = 0; i < classes; i++)
      {
        source >> p_k[i];
      }

    source >> apm;

    vector<double>::iterator  mp = mue_k.begin();

    for (int k = 0; k < classes; k++)
      for (int d = 0; d < nFeatures; d++)
        {
          source >> *(mp++);
        }

    vector<double>::iterator sp = sigma_k_inv.begin();

    for (int k = 0; k < classes; k++)
      for (int i = 0; i < nFeatures; i++)
        for (int j = i; j < nFeatures; j++)
          {
            source >> *(sp++);
          }

    u_constant = vector<double>(classes);

    for (int k = 0; k < classes; k++)
      {
        source >> u_constant[k];
      }

    if (source.fail() || source.bad())
      {
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
      }
    state = ready;
    return OK;
  }
#undef FNAME

  int ClassifierBayes::getMue(int cl, Vector& mue) const
  {
    if (mue.Size() != nFeatures)
      {
        mue.Resize(nFeatures);
      }

    int x;

    for (x = cl * nFeatures; x < (cl + 1)*nFeatures; x++)
      {
        mue[x - cl * nFeatures] = mue_k[x];
      }

    return 0;
  }

  int ClassifierBayes::getSigmaInv(int cl, Matrix& sigmainv) const
  {
    if (sigmainv.rows() != nFeatures || sigmainv.cols() != nFeatures)
      {
        sigmainv = Matrix(nFeatures, nFeatures);
      }

    vector<double>::const_iterator ptr = sigma_k_inv.begin() + cl * nFeatures * (nFeatures + 1) / 2;

    int row = 0;
    int col = 0;

    for (int row = 0; row < nFeatures; row++)
      {
        sigmainv[row][col] = *ptr;
        ptr++;
        for (int col = row + 1; col < nFeatures; ++col)
          {
            sigmainv[row][col] = (*ptr) * 0.5;
            ptr++;
          }
      }

    for (row = 1; row < nFeatures; row++)
      {
        for (col = 0; col < row; col++)
          {
            sigmainv[row][col] = sigmainv[col][row];
          }
      }

    return 0;
  }
}
