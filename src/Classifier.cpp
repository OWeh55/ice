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
#include "matrixtools.h"
#include "message.h"
#include "numbase.h"
#include "macro.h"
#include "Classifier.h"

namespace ice
{
#define FNAME "Classifier::Init"
  void Classifier::Init()
  {
    if (state == invalid)
      {
        Message(FNAME, M_NOT_INITIALISED, WRONG_PARAM);
        return;
      }
    _reset();
  }

  void Classifier::Init(int classes, int dimension)
  {
    if ((dimension < 1) || (classes < 2))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }

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
      {
        Message(FNAME, M_NOT_INITIALISED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((int)s.features.size() != nFeatures)
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (s.classNr < 0 || s.classNr >= nClasses)
      {
        Message(FNAME, M_INVALID_CLASSNUMBER, WRONG_PARAM);
        return WRONG_PARAM;
      }
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
      v[i] = feat[i];
    return Train(cl, v);
  }

  // Train classifier from classified sample list
  int Classifier::Train(const Matrix& m)
  {
    if ((m.rows() < 1) || (m.cols() != nFeatures + 1))
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (state != training)
      {
        Message(FNAME, M_NOT_INITIALISED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int i = 0; i < m.rows(); i++)
      Train((int)Round(m[i][nFeatures]), m[i](0, nFeatures - 1));

    return OK;
  }

  int Classifier::Train(const std::vector<ClassSample>& sampleVector)
  {
    for (unsigned int i = 0; i < sampleVector.size(); i++)
      Train(sampleVector[i]);

    return OK;
  }

  int Classifier::Train(const Matrix& m, const IVector& classnr)
  {
    if (state != training)
      {
        Message(FNAME, M_NOT_INITIALISED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((m.rows() < 1) || (m.cols() != nFeatures) ||
        (m.rows() != classnr.Size()))
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return WRONG_PARAM;
      }

    for (int i = 0; i < m.rows(); i++)
      Train(classnr[i], m[i]);

    return OK;
  }
#undef FNAME

#define FNAME "Classifier::Finish"
  int Classifier::Finish()
  {
    if (state != training)
      {
        Message(FNAME, M_NOT_INITIALISED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    bool allTrained = true;
    for (int i = 0; i < nClasses && allTrained; i++)
      if (classTrained[i] == 0)
        allTrained = false;

    if (!allTrained)
      {
        Message(FNAME, M_NOT_TRAINED, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return -3;
      }

    if (nFeatures != (int)feat.size())
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return -3;
      }
    std::vector<double> prob;
    return _classify(feat, prob);
  }

  int Classifier::Classify(const std::vector<double>& feat,
                           std::vector<double>& prob) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return -3;
      }

    if (nFeatures != (int)feat.size())
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return -3;
      }
    prob.resize(nClasses);
    prob[0] = -1;
    int classNr = _classify(feat, prob);
    if (prob[0] < 0) // Classifier did not set probabilities
      {
        for (int i = 0; i < nClasses; ++i)
          prob[i] = 0;
        if (classNr >= 0)
          prob[classNr] = 1.0;
      }
    return classNr;
  }

  int Classifier::Classify(const Vector& feat) const
  {
    std::vector<double> v(feat.size());
    for (unsigned int i = 0; i < feat.size(); i++)
      v[i] = feat[i];
    return Classify(v);
  }

// classify list of feature vectors and deliver "class" integer matrix
  int Classifier::Classify(const Matrix& m, IVector& cl) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (nFeatures != m.cols())
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
    IfFailed(Classify(m, cl))
    {
      Message(FNAME, M_0, ERROR);
      return ERROR;
    }
    m = m || (Vector)cl;
    return OK;
  }
#undef FNAME

#define FNAME "Classifier::Read"
  int Classifier::read(const std::string& fn)
  {
    std::ifstream is(fn.c_str());
    if (!is.good())
      {
        Message(FNAME, M_FILE_OPEN, WRONG_FILE);
        return WRONG_FILE;
      }
    return read(is);
  }

  int Classifier::read(std::istream& is)
  {
    Message(FNAME, M_NOT_IMPLEMENTED, ERROR);
    return ERROR;
  }

#undef FNAME
#define FNAME "Classifier::Write"
  int Classifier::write(const std::string& fn) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, ERROR);
        return ERROR;
      }
    std::ofstream os(fn.c_str());
    if (!os.good())
      {
        Message(FNAME, M_FILE_OPEN, WRONG_FILE);
        return WRONG_FILE;
      }
    return write(os);
  }

  int Classifier::write(std::ostream& os) const
  {
    Message(FNAME, M_NOT_IMPLEMENTED, ERROR);
    return ERROR;
  }
#undef FNAME
#define FNAME "Test"
// verify classifier from classified sample list
// returns relative nunber of correct classified samples
  double Classifier::Test(const std::vector<ClassSample>& sl) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return 0.0;
      }

    if (sl.size() < 1)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return 0.0;
      }

    if (nFeatures != (int)sl[0].features.size())
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return 0.0;
      }

    int cc = 0; // count correct classified samples

    for (unsigned int i = 0; i < sl.size(); ++i)
      {
        if (sl[i].classNr == Classify(sl[i].features))
          cc++;
      }

    return ((double)cc) / ((double)sl.size());
  }

  double Classifier::Test(const Matrix& m) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return 0.0;
      }

    if (nFeatures != m.cols() - 1)
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return 0.0;
      }

    int cc = 0; // count correct classified samples

    for (int i = 0; i < m.rows(); i++)
      {
        if (RoundInt(m[i][m.cols()]) == Classify(m[i](0, nFeatures - 1)))
          cc++;
      }

    return ((double)cc) / ((double)m.rows());
  }

// returns relative nunber of correct classified samples
  double Classifier::Test(const Matrix& m, const IVector& clnr) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return 0.0;
      }

    if ((nFeatures != m.cols()) || (m.rows() != clnr.Size()))
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return 0.0;
      }

    int cc = 0; // count correct classified samples

    for (int i = 0; i < m.rows(); i++)
      {
        if (clnr[i] == Classify(m[i]))
          cc++;
      }

    return ((double)cc) / ((double)m.rows());
  }
#undef FNAME
#define FNAME "Classifier::Test"
  double Classifier::Test(const Matrix& m, Matrix& r) const
  {
    if (state != ready)
      {
        Message(FNAME, M_NOT_FINISHED, WRONG_PARAM);
        return 0.0;
      }

    if (nFeatures != m.cols() - 1)
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return 0.0;
      }

    int cc = 0; // counter for correct classifications

    IVector anz(nFeatures);
    anz.Set(0);

    r = Matrix(nFeatures, nFeatures);
    r.Set(0.0);

    for (int i = 0; i < m.rows(); i++)
      {
        int cls = RoundInt(m[i][m.cols()]);
        anz[cls] += 1;
        int cli = Classify(m[i](0, nFeatures - 1));

        if (cli >= 0) // ignore rejections
          {
            r[cls][cli] += 1.0;

            if (cli == cls) cc++;
          }
      }

    for (int i = 0; i < nFeatures; i++)
      for (int j = 0; j < nFeatures; j++)
        r[i][j] = r[i][j] / anz[i];

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
