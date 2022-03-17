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
#include "matrixtools.h"
#include "IceException.h"
#include "numbase.h"
#include "macro.h"
#include "Classifier.h"

namespace ice
{
#define FNAME "Classifier::Init"
  void Classifier::Init()
  {
    if (state == invalid)
      throw IceException(FNAME, M_NOT_INITIALIZED);
    _reset();
  }

  void Classifier::Init(int classes, int dimension)
  {
    if ((dimension < 1) || (classes < 2))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    nFeatures = dimension;
    nClasses = classes;
    classTrained.resize(classes);
    _reset();
  }
#undef FNAME

#define FNAME "Train"
  int Classifier::Train(const ClassSample& s)
  {
    if (state != training)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((int)s.features.size() != nFeatures)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    if (s.classNr < 0 || s.classNr >= nClasses)
      throw IceException(FNAME, M_INVALID_CLASSNUMBER);
    classTrained[s.classNr]++;
    return _train(s);
  }

  int Classifier::Train(int cl, const std::vector<double>& feat)
  {
    return Train(ClassSample(cl, feat));
  }

  int Classifier::Train(int cl, const ice::Vector& feat)
  {
    std::vector<double> v(feat.size());
    for (int i = 0; i < (int)feat.size(); i++)
      {
        v[i] = feat[i];
      }
    return Train(cl, v);
  }

  // Train classifier from classified sample list
  int Classifier::Train(const Matrix& m)
  {
    if ((m.rows() < 1) || (m.cols() != nFeatures + 1))
      throw IceException(FNAME, M_MATRIXFORMAT);

    if (state != training)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    for (int i = 0; i < m.rows(); i++)
      {
        Train((int)Round(m[i][nFeatures]), m[i](0, nFeatures - 1));
      }

    return OK;
  }

  int Classifier::Train(const std::vector<ClassSample>& sampleVector)
  {
    for (unsigned int i = 0; i < sampleVector.size(); i++)
      {
        Train(sampleVector[i]);
      }

    return OK;
  }

  int Classifier::Train(const Matrix& m, const IVector& classnr)
  {
    if (state != training)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((m.rows() < 1) || (m.cols() != nFeatures) ||
        (m.rows() != classnr.Size()))
      throw IceException(FNAME, M_MATRIXFORMAT);

    for (int i = 0; i < m.rows(); i++)
      {
        Train(classnr[i], m[i]);
      }

    return OK;
  }
#undef FNAME

#define FNAME "Classifier::Finish"
  int Classifier::Finish()
  {
    if (state != training)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    bool allTrained = true;
    for (int i = 0; i < nClasses && allTrained; i++)
      if (classTrained[i] == 0)
        {
          allTrained = false;
        }

    if (!allTrained)
      throw IceException(FNAME, M_NOT_TRAINED);

    if (_finish())
      {
        state = ready;
        return OK;
      }

    return ERROR;
  }
#undef FNAME

#define FNAME "Classify"
// classify single feature vector
// returns:  classnumber if classification is successfull
//           -1  in case of rejection
//           -2  in case of ambiguity
//           <-2 in case of error

  int Classifier::Classify(const std::vector<double>& feat) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if (nFeatures != (int)feat.size())
      throw IceException(FNAME, M_WRONG_DIMENSION);
    std::vector<double> prob;
    return _classify(feat, prob);
  }

  int Classifier::Classify(const std::vector<double>& feat,
                           std::vector<double>& prob) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if (nFeatures != (int)feat.size())
      throw IceException(FNAME, M_WRONG_DIMENSION);
    prob.resize(nClasses);
    prob[0] = -1;
    int classNr = _classify(feat, prob);
    if (prob[0] < 0)   // Classifier did not set probabilities
      {
        for (int i = 0; i < nClasses; ++i)
          {
            prob[i] = 0;
          }
        if (classNr >= 0)
          {
            prob[classNr] = 1.0;
          }
      }
    return classNr;
  }

  int Classifier::Classify(const Vector& feat) const
  {
    std::vector<double> v(feat.size());
    for (unsigned int i = 0; i < feat.size(); i++)
      {
        v[i] = feat[i];
      }
    return Classify(v);
  }

// classify list of feature vectors and deliver "class" integer matrix
  int Classifier::Classify(const Matrix& m, IVector& cl) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if (nFeatures != m.cols())
      throw IceException(FNAME, M_MATRIXFORMAT);

    cl = IVector(m.rows());

    for (int i = 0; i < m.rows(); i++)
      {
        cl[i] = Classify(m[i]);
      }

    return OK;
  }

  // classify list of feature vectors and extend matrix with "class" column
  int Classifier::Classify(Matrix& m) const
  {
    IVector cl;
    Classify(m, cl);
    m = m || (Vector)cl;
    return OK;
  }
#undef FNAME

#define FNAME "Classifier::Read"
  int Classifier::read(const std::string& fn)
  {
    std::ifstream is(fn.c_str());
    if (!is.good())
      throw IceException(FNAME, M_FILE_OPEN);
    return read(is);
  }

  int Classifier::read(std::istream& is)
  {
    throw IceException(FNAME, M_NOT_IMPLEMENTED);
  }
#undef FNAME
#define FNAME "Classifier::Write"
  int Classifier::write(const std::string& fn) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);
    std::ofstream os(fn.c_str());
    if (!os.good())
      throw IceException(FNAME, M_FILE_OPEN);
    return write(os);
  }

  int Classifier::write(std::ostream& os) const
  {
    throw IceException(FNAME, M_NOT_IMPLEMENTED);
  }
#undef FNAME
#define FNAME "Test"
// verify classifier from classified sample list
// returns relative nunber of correct classified samples
  double Classifier::Test(const std::vector<ClassSample>& sl) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if (sl.size() < 1)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (nFeatures != (int)sl[0].features.size())
      throw IceException(FNAME, M_MATRIXFORMAT);

    int cc = 0; // count correct classified samples

    for (unsigned int i = 0; i < sl.size(); ++i)
      {
        if (sl[i].classNr == Classify(sl[i].features))
          {
            cc++;
          }
      }

    return ((double)cc) / ((double)sl.size());
  }

  double Classifier::Test(const Matrix& m) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if (nFeatures != m.cols() - 1)
      throw IceException(FNAME, M_MATRIXFORMAT);

    int cc = 0; // count correct classified samples

    for (int i = 0; i < m.rows(); i++)
      {
        if (RoundInt(m[i][m.cols()]) == Classify(m[i](0, nFeatures - 1)))
          {
            cc++;
          }
      }

    return ((double)cc) / ((double)m.rows());
  }

// returns relative nunber of correct classified samples
  double Classifier::Test(const Matrix& m, const IVector& clnr) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if ((nFeatures != m.cols()) || (m.rows() != clnr.Size()))
      throw IceException(FNAME, M_MATRIXFORMAT);

    int cc = 0; // count correct classified samples

    for (int i = 0; i < m.rows(); i++)
      {
        if (clnr[i] == Classify(m[i]))
          {
            cc++;
          }
      }

    return ((double)cc) / ((double)m.rows());
  }
#undef FNAME
#define FNAME "Classifier::Test"
  double Classifier::Test(const Matrix& m, Matrix& r) const
  {
    if (state != ready)
      throw IceException(FNAME, M_NOT_FINISHED);

    if (nFeatures != m.cols() - 1)
      throw IceException(FNAME, M_MATRIXFORMAT);

    int cc = 0; // counter for correct classifications

    IVector anz(nFeatures);
    anz.set(0);

    r = Matrix(nFeatures, nFeatures);
    r.set(0.0);

    for (int i = 0; i < m.rows(); i++)
      {
        int cls = RoundInt(m[i][m.cols()]);
        anz[cls] += 1;
        int cli = Classify(m[i](0, nFeatures - 1));

        if (cli >= 0)   // ignore rejections
          {
            r[cls][cli] += 1.0;

            if (cli == cls)
              {
                cc++;
              }
          }
      }

    for (int i = 0; i < nFeatures; i++)
      for (int j = 0; j < nFeatures; j++)
        {
          r[i][j] = r[i][j] / anz[i];
        }

    return ((double)cc) / ((double)m.rows());
  }

  std::ostream& operator<<(std::ostream& os, const Classifier& cl)
  {
    cl.write(os);
    return os;
  }

  std::istream& operator>>(std::istream& is, Classifier& cl)
  {
    cl.read(is);
    return is;
  }
}
