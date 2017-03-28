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

#include "matrixarith.h"
#include "macro.h"
#include "message.h"
#include "numbase.h"
#include "MatrixAlgebra.h"
#include "ClassifierBayes2.h"

namespace ice
{
#define FNAME "ClassifierBayes2::ClassifierBayes2()"
  ClassifierBayes2::ClassifierBayes2(int classes, int dimension,
                                     double ap1, double ap2, // probability
                                     double ac12, double ac21) // costs
  {
    IfFailed(Init(classes, dimension, ap1, ap2, ac12, ac21))
    {
      // if initialisation fails
      Message(FNAME, M_0, ERROR);
    }
  }
#undef FNAME

//--------------------------------------------------------
#define FNAME "ClassifierBayes2::Init"
  void ClassifierBayes2::Init(int classes, int dimension,
                              double ap1, double ap2, // probability
                              double ac12, double ac21) // costs
  {
    RETURN_VOID_IF_FAILED(Classifier::Init(classes, dimension));
    p1 = ap1;
    p2 = ap2;
    c12 = ac12;
    c21 = ac21;
    st1.Init(nFeatures);
    st2.Init(nFeatures);
  }
#undef FNAME
#define FNAME "Train"
// Train classifier from single feature vector
  int ClassifierBayes2::_train(const ClassSample& s)
  {
    if (s.classNr == 0)
      Put(st1, s.features);
    else
      Put(st2, s.features);
    return OK;
  }
#undef FNAME
#define FNAME "Classify"
// classify single feature vector
// returns:  classnumber if classification is successfull
//           -1  in case of error
  int ClassifierBayes2::_classify(const std::vector<double>& feat,
                                  std::vector<double>& prob) const
  {
    std::vector<double> dif1 = feat - my1;
    std::vector<double> dif2 = feat - my2;

    double ent = dif2 * (sigma2 * dif2) - dif1 * (sigma1 * dif1) - limit;

    if (ent > 0)  return 0;
    else return 1;
  }
#undef FNAME
#define FNAME "ClassifierBayes2::Finish"
  bool ClassifierBayes2::_finish()
  {
    sigma1 = st1.getCovariance();
    sigma2 = st2.getCovariance();
    my1 = st1.getMean();
    my2 = st2.getMean();
    limit = 2 * log(Determinant(sigma2) / Determinant(sigma1)) -
            log((c21 * p2) / (c12 * p1));
    sigma1 = Inverse(sigma1);
    sigma2 = Inverse(sigma2);

    return true;
  }
#undef FNAME
#define FNAME "ClassifierBayes2::Write"
// store classifier to file
  int ClassifierBayes2::write(std::ostream& dest) const
  {
    dest << "ClassifierBayes2" << std::endl;
    // write parameters of classifier
    dest << nFeatures << std::endl;
    dest << p1 << ' ' << p2  << ' ' << c12 << ' ' << c21 << std::endl;
    // write statistics of classes
    dest << st1 << std::endl;
    dest << st2 << std::endl;
    return OK;
  }
#undef FNAME
// restore classifier from file
#define FNAME "ClassifierBayes2::Read"
  int ClassifierBayes2::read(std::istream& source)
  {
    std::string id;
    source >> id;
    if (id != "ClassifierBayes2")
      {
        Message(FNAME, M_WRONG_FILE, WRONG_FILE);
        return WRONG_FILE;
      }

    source >> nFeatures >> p1 >> p2;
    source >> c12 >> c21;

    Init(2, nFeatures, p1, p2, c12, c21);

    classTrained[0] = 10;
    classTrained[1] = 10;

    source >> st1;
    source >> st2;

    if (source.fail() || source.bad())
      {
        Message(FNAME, M_WRONG_FILE, WRONG_FILE);
        return WRONG_FILE;
      }

    Finish();
    return OK;
  }
#undef FNAME
}
