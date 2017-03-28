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

#include "vectortools.h"
#include "macro.h"
#include "message.h"
#include "numbase.h"
#include "ClassifierMinimumDistance.h"

using namespace std;

namespace ice
{
#define FNAME "ClassifierMinimumDistance::ClassifierMinimumDistance()"
  ClassifierMinimumDistance::ClassifierMinimumDistance(int classes, int dimension)
  {
    IfFailed(Init(classes, dimension))
    {
      // if initialisation fails
      Message(FNAME, M_0, ERROR);
    }
  }
#undef FNAME

#define FNAME "ClassifierMinimumDistance::Init"
  void ClassifierMinimumDistance::Init(int classes, int dimension)
  {
    RETURN_VOID_IF_FAILED(Classifier::Init(classes, dimension));

    par = Matrix(nClasses, nFeatures + 2);

    for (int i = 0; i < nClasses; i++)
      par[i][nFeatures] = 0.0;
  }
#undef FNAME
#define FNAME "Train"
// Train classifier from single feature vector
  int ClassifierMinimumDistance::_train(const ClassSample& s)
  {
    double count = par[s.classNr][nFeatures];

    if (count == 0.0)
      {
        for (int i = 0; i < nFeatures; i++)
          {
            par[s.classNr][i] = s.features[i];
          }
      }
    else
      {
        for (int i = 0; i < nFeatures; i++)
          {
            double val = par[s.classNr][i];
            par[s.classNr][i] = (count * val + s.features[i]) / (count + 1.0);
          }
      }

    par[s.classNr][nFeatures] = count + 1.0;
    return OK;
  }
#undef FNAME
#define FNAME "Classify"
// classify single feature vector
// returns:  classnumber if classification is successfull
//           <0  in case of error
  int ClassifierMinimumDistance::_classify(const std::vector<double>& feat,
      std::vector<double>& prob) const
  {
    vector<double> discr(nClasses, 0.0);

    for (int i = 0; i < nClasses; i++)
      {
        // calculate discriminance function for every class
        for (int j = 0; j < nFeatures; j++)
          {
            discr[i] += feat[j] * par[i][j];
          }

        discr[i] += par[i][nFeatures + 1];
      }
    return maxIndex(discr);
  }
#undef FNAME
#define FNAME "ClassifierMinimumDistance::Write"
// store classifier to file
  int ClassifierMinimumDistance::write(std::ostream& dest) const
  {
    dest << "ClassifierMinimumDistance" << endl;
    dest << nFeatures << " " ;
    dest << nClasses << endl ;
    dest << par ;
    return OK;
  }
#undef FNAME
#define FNAME "ClassifierMinimumDistance::Read"
// restore classifier from file
  int ClassifierMinimumDistance::read(istream& source)
  {
    string id;
    source >> id;
    if (id != "ClassifierMinimumDistance")
      {
        Message(FNAME, M_WRONG_FILE, WRONG_FILE);
        return WRONG_FILE;
      }

    source >> nFeatures >> nClasses;

    source >> par ;

    classTrained.resize(nClasses);
    for (int i = 0; i < par.rows(); ++i)
      {
        classTrained[i] = par[i][nFeatures];
      }

    Finish(); // need to recalc last column
    return OK;
  }
#undef FNAME
#define FNAME "Center"
// Train classifier from single feature vector
  Vector ClassifierMinimumDistance::Center(int cl) const
  {
    Vector ret(nFeatures);

    if ((cl < 0) || (cl >= nClasses))
      {
        Message(FNAME, M_INVALID_CLASSNUMBER, WRONG_PARAM);
        return ret;
      }

    if (nClasses == 0)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ret;
      }

    if (par[cl][nFeatures] == 0)
      {
        Message(FNAME, M_NOT_TRAINED, WRONG_PARAM);
        return ret;
      }

    return par[cl](0, nFeatures - 1);
  }
#undef FNAME

#define FNAME "ClassifierMinimumDistance:Finish"
  bool ClassifierMinimumDistance::_finish()
  {
    for (int i = 0; i < nClasses; i++)
      {
        double sum = 0;

        for (int j = 0; j < nFeatures; j++)
          sum = sum + par[i][j] * par[i][j];

        par[i][nFeatures + 1] = -sum / 2.0;
      }
    return true;
  }
#undef FNAME
}
