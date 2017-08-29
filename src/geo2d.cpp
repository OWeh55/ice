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

/*
 * functions for 2d-2d-transformations
 */
#include "defs.h"
#include "macro.h"
#include "IceException.h"

#include "contools.h"
#include "simplex.h"

#include "lmdif.h"

#include "MatrixAlgebra.h"
#include "vectortools.h"
#include "geo.h"
#include "matrixtools.h"

using namespace std;

//--------------------------------------------
namespace ice
{
#define FNAME "checkLists"
  int checkLists(const vector<Point>& p1,
                 const vector<Point>& p2,
                 const vector<double>& weights)
  {
    int nPoints = p1.size();
    if (p2.size() != nPoints || weights.size() != nPoints)
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);
    return nPoints;
  }
#undef FNAME

#define FNAME "matchPointlistsProjectiveLinear"
  Trafo matchPointListsProjectiveLinear(const vector<Point>& p1,
                                        const vector<Point>& p2,
                                        const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);

        matrix<double> a(nPoints * 2 + 1, (2 + 1) * (2 + 1), 0);
        vector<double> r(nPoints * 2 + 1);
        vector<double> rv;

        int equNr = 0;
        for (int j = 0; j < nPoints; j++)
          {
            double weight = weights[j];

            a[equNr][0] = p1[j].x * weight;
            a[equNr][1] = p1[j].y * weight;
            a[equNr][2] = 1.0 * weight;

            a[equNr][6] = -p1[j].x * p2[j].x * weight;
            a[equNr][7] = -p1[j].y * p2[j].x * weight;
            a[equNr][8] = -p2[j].x * weight;
            r[equNr] = 0.0;

            equNr++;

            a[equNr][3] = p1[j].x * weight;
            a[equNr][4] = p1[j].y * weight;
            a[equNr][5] = 1.0 * weight;

            a[equNr][6] = -p1[j].x * p2[j].y * weight;
            a[equNr][7] = -p1[j].y * p2[j].y * weight;
            a[equNr][8] = -p2[j].y * weight;
            r[equNr] = 0.0;

            equNr++;
          }

        //
        a[equNr][6] = 1.0;
        a[equNr][7] = 1.0;
        a[equNr][8] = 1.0;
        r[equNr] = 1.0;

        rv = solveLinearEquation(a, r);

        matrix<double> tmatrix(3, 3);
        for (int k = 0; k < 3; k++)
          for (int i = 0; i < 3; i++)
            tmatrix[k][i] = rv[k * 3 + i];

        return tmatrix;
      }
    RETHROW;
  }

  Trafo matchPointListsProjectiveLinear(const vector<Point>& p1,
                                        const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsProjectiveLinear(p1, p2, weights);
  }
#undef FNAME

  // auxilary functions for Levenberg-Marquard-Algorithm
  static void trafo2vector(const Trafo& tr, vector<double>& v)
  {
    matrix<double> m = tr.getMatrix();
    int i = 0;
    for (int k = 0; k < m.rows(); ++k)
      for (int l = 0; l < m.cols(); ++l)
        {
          v[i++] = m[k][l];
        }
  }

  static void vector2trafo(const vector<double>& v, Trafo& tr)
  {
    matrix<double> m = tr.getMatrix();
    int i = 0;
    for (int k = 0; k < m.rows(); ++k)
      for (int l = 0; l < m.cols(); ++l)
        {
          m[k][l] = v[i++];
        }
    tr = Trafo(m);
  }

  class LMProjective2d: public LMFunctionObject
  {
  private:
    const vector<Point>& p1;
    const vector<Point>& p2;
    const vector<double>& w;
    vector<double>& trpara;
  public:
    LMProjective2d(const vector<Point>& pl1,
                   const vector<Point>& pl2,
                   const vector<double>& weights,
                   vector<double>& trp):
      p1(pl1), p2(pl2), w(weights), trpara(trp)
    {
      checkLists(p1, p2, weights);
    };

    int funcdim() const
    {
      return p2.size() * 2;
    }

    int operator()(Vector& result) const
    {
      Trafo tr(2, 2);
      // create transformation from parameters
      vector2trafo(trpara, tr);
      // transform pointlist p1
      vector<Point> pointsTransformed;
      transform(tr, p1, pointsTransformed);
      // error function == distance vectors of transformed points to pointlist p2
      int k = 0;
      for (int i = 0; i < (int)p2.size(); ++i)
        {
          result[k++] = (pointsTransformed[i].x - p2[i].x) * w[i];
          result[k++] = (pointsTransformed[i].y - p2[i].y) * w[i];
        }
      return 1;
    }
  };

#define FNAME "iterateProjective"
  Trafo iterateProjective(const Trafo& tr,
                          const vector<Point>& p1, const vector<Point>& p2,
                          const vector<double>& weights)
  {
    matrix<double> m = tr.getMatrix();
    vector<double> trpara(m.cols()*m.rows());
    trafo2vector(tr, trpara);

    vector<double*> pp(trpara.size() - 1);
    for (int i = 0; i < (int)pp.size(); ++i)
      {
        pp[i] = &trpara[i];
      }

    int inumber;
    LMDif(pp,      // vector of pointers to parameters
          LMProjective2d(p1, p2, weights, trpara), // functor for error function
          10000,      // maximum number of iterations
          inumber);  // number of iterations executed

    Trafo res(2, 2);
    // create transformation from parameters
    vector2trafo(trpara, res);
    return res;
  }

  Trafo iterateProjective(const Trafo& tr,
                          const vector<Point>& p1, const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return iterateProjective(tr, p1, p2, weights);
  }
#undef FNAME

  class LMEuclidean2d: public LMFunctionObject
  {
  private:
    const vector<Point>& p1;
    const vector<Point>& p2;
    const vector<double>& w;
    vector<double>& trpara;
  public:
    LMEuclidean2d(const vector<Point>& pl1,
                  const vector<Point>& pl2,
                  const vector<double>& weights,
                  vector<double>& trp):
      p1(pl1), p2(pl2), w(weights), trpara(trp)
    {
      checkLists(p1, p2, weights);
    };

    int funcdim() const
    {
      return p2.size() * 2;
    }

    int operator()(Vector& result) const
    {
      Trafo tr(2, 2);
      // create transformation from parameters
      tr.rotate(0, 0, trpara[0]);
      tr.shift(trpara[1], trpara[2]);
      // transform pointlist p1
      vector<Point> pointsTransformed;
      transform(tr, p1, pointsTransformed);
      // error function == distance vectors of transformed points to pointlist p2
      int k = 0;
      for (int i = 0; i < (int)p2.size(); ++i)
        {
          result[k++] = (pointsTransformed[i].x - p2[i].x) * w[i];
          result[k++] = (pointsTransformed[i].y - p2[i].y) * w[i];
        }
      return 1;
    }
  };

#define FNAME "iterateEuclidean"
  Trafo iterateEuclidean(const Trafo& tr,
                         const vector<Point>& p1, const vector<Point>& p2,
                         const vector<double>& weights)
  {
    vector<double> trpara(3);

    // initial values for shift and rotation
    matrix<double> m = tr.getMatrix();
    double c = m[0][0] + m[1][1];
    double s = m[1][0] - m[0][1];

    trpara[0] = atan2(s, c);
    trpara[1] = m[0][2];
    trpara[2] = m[1][2];

    vector<double*> pp(3);
    for (int i = 0; i < 3; ++i)
      {
        pp[i] = &trpara[i];
      }

    int inumber;
    LMDif(pp,      // vector of pointers to parameters
          LMEuclidean2d(p1, p2, weights, trpara), // functor for error function
          10000,      // maximum number of iterations
          inumber);  // number of iterations executed

    // create transformation from parameters
    Trafo res(2, 2);
    res.rotate(0, 0, trpara[0]);
    res.shift(trpara[1], trpara[2]);
    return res;
  }

  Trafo iterateEuclidean(const Trafo& tr,
                         const vector<Point>& p1, const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return iterateEuclidean(tr, p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointListsShift"
  Trafo matchPointListsShift(const vector<Point>& p1,
                             const vector<Point>& p2,
                             const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);
        Point shift(0, 0);
        double weightsum = 0;

        for (int j = 0; j < nPoints; j++)
          {
            shift += (p2[j] - p1[j]) * weights[j];
            weightsum += weights[j];
          }

        shift = shift * (1.0 / weightsum);

        Trafo res;
        res.shift(shift);
        return res;
      }
    RETHROW;
  }

  Trafo matchPointListsShift(const vector<Point>& p1,
                             const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsShift(p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointListsShiftScale"
  /*
   *  | a 0 dx |
   *  | 0 a dy |
   *  | 0 0 1  |
   *  x' = a * x + dx
   *  y' = a * y + dy
   *  | x  1 0 |  | x' |
   *  | y  0 1 |  | y' |
   */
  Trafo matchPointListsShiftScale(const vector<Point>& p1,
                                  const vector<Point>& p2,
                                  const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);
        double weightsum = 0;
        Point sum1(0, 0);
        Point sum2(0, 0);

        for (int j = 0; j < nPoints; j++)
          {
            sum1 += p1[j];
            sum2 += p2[j];
            weightsum += weights[j];
          }

        double s = 0.0;
        double q = 0.0;

        for (int j = 0; j < nPoints; j++)
          {
            s += weights[j] * (p2[j] * p1[j]);
            q += weights[j] * (p1[j] * p1[j]);
          }

        double alpha = ((sum2 * sum1) / weightsum - s) / ((sum1 * sum1) / weightsum - q);
        Point shift(0, 0);

        for (int j = 0; j < nPoints; j++)
          {
            shift += (p2[j] - alpha * p1[j]) * weights[j];
          }

        shift = (1.0 / weightsum) * shift;

        Trafo res(2, 2);
        res.scale(0, 0, alpha);
        res.shift(shift);
        return res;
      }
    RETHROW;
  }

  Trafo matchPointListsShiftScale(const vector<Point>& p1,
                                  const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsShiftScale(p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointListsRotate"
  Trafo matchPointListsRotate(const vector<Point>& p1,
                              const vector<Point>& p2,
                              const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);
        Trafo trs = matchPointListsSimilarity(p1, p2, weights);
        matrix<double> m = trs.getMatrix();
        double c = m[0][0] + m[1][1];
        double s = m[1][0] - m[0][1];
        double fi = atan2(s, c);
        // cout << (fi*180/M_PI) << endl;
        Trafo tr;
        tr.rotate(0, 0, fi);
        return tr;
      }
    RETHROW;
  }

  Trafo matchPointListsRotate(const vector<Point>& p1,
                              const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsRotate(p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointListsEuclidean"
  Trafo matchPointListsEuclidean(const vector<Point>& p1,
                                 const vector<Point>& p2,
                                 const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);
        Trafo trs = matchPointListsSimilarity(p1, p2, weights);
        return iterateEuclidean(trs, p1, p2, weights);
      }
    RETHROW;
  }

  Trafo matchPointListsEuclidean(const vector<Point>& p1,
                                 const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsEuclidean(p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointListsSimilarity"
  Trafo matchPointListsSimilarity(const vector<Point>& p1,
                                  const vector<Point>& p2,
                                  const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);

        matrix<double> a(2 * nPoints, 4);
        vector<double> r(2 * nPoints);

        for (int j = 0; j < nPoints; j++)
          {
            double weight = weights[j];

            a[j * 2][0] = p1[j].x * weight;
            a[j * 2][1] = p1[j].y * weight;
            a[j * 2][2] = weight;
            a[j * 2][3] = 0;

            r[j * 2] = p2[j].x * weight;

            a[j * 2 + 1][0] = p1[j].y * weight;
            a[j * 2 + 1][1] = -p1[j].x * weight;
            a[j * 2 + 1][2] = 0;
            a[j * 2 + 1][3] = weight;

            r[j * 2 + 1] = p2[j].y * weight;
          }

        vector<double> rv = solveLinearEquation(a, r);

        // cout << rv << endl;
        matrix<double> m(3, 3);
        m[0][0] = rv[0];
        m[0][1] = rv[1];
        m[1][0] = -rv[1];
        m[1][1] = rv[0];
        m[0][2] = rv[2];
        m[1][2] = rv[3];
        m[2][0] = 0.0;
        m[2][1] = 0.0;
        m[2][2] = 1.0;
        return Trafo(m);
      }
    RETHROW;
  }

  Trafo matchPointListsSimilarity(const vector<Point>& p1,
                                  const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsSimilarity(p1, p2, weights);
  }
#undef FNAME

#define FNAME "matchPointListsAffine"
  Trafo matchPointListsAffine(const vector<Point>& p1,
                              const vector<Point>& p2,
                              const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);

        matrix<double> a(nPoints, 3);
        vector<double> r(nPoints);
        vector<double> rv(3);
        matrix<double> res(3, 3);

        // first equation system for x coordinate
        for (int j = 0; j < nPoints; j++)
          {
            a[j][0] = p1[j].x * weights[j];
            a[j][1] = p1[j].y * weights[j];
            a[j][2] = 1.0 * weights[j];

            r[j] = p2[j].x * weights[j];
          }

        rv = solveLinearEquation(a, r);

        for (int i = 0; i < 3; i++)
          res[0][i] = rv[i];

        // second equation system for y coordinate with unchanged coefficient matrix
        for (int j = 0; j < nPoints; j++)
          r[j] = p2[j].y * weights[j];

        rv = solveLinearEquation(a, r);

        for (int i = 0; i < 3; i++)
          res[1][i] = rv[i];

        res[2][0] = 0.0;
        res[2][1] = 0.0;
        res[2][2] = 1.0;
        return Trafo(res);

      }
    RETHROW;
  }

  Trafo matchPointListsAffine(const vector<Point>& p1,
                              const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsAffine(p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointListsProjective"
  Trafo matchPointListsProjective(const vector<Point>& p1,
                                  const vector<Point>& p2,
                                  const vector<double>& weights)
  {
    try
      {
        // affine transformation as first approximation
        Trafo res = matchPointListsAffine(p1, p2, weights);

        // iterative refinement to projective transformation
        return iterateProjective(res, p1, p2, weights);
      }
    RETHROW;
  }

  Trafo matchPointListsProjective(const vector<Point>& p1,
                                  const vector<Point>& p2)
  {
    vector<double> weights(p1.size(), 1.0);
    return matchPointListsProjective(p1, p2, weights);
  }
#undef FNAME
#define FNAME "matchPointLists"
  Trafo matchPointLists(const vector<Point>& p1,
                        const vector<Point>& p2,
                        int mode, const vector<double>& weights)
  {
    try
      {
        int nPoints = checkLists(p1, p2, weights);

        if ((nPoints != p2.size()) || (nPoints != weights.size()))
          throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

        switch (mode)
          {
          case TRM_SHIFT:
            return matchPointListsShift(p1, p2, weights);
            break;
          case TRM_SCALE_SHIFT:
            return matchPointListsShiftScale(p1, p2, weights);
            break;

          case TRM_SIMILARITY_NOR:
            return matchPointListsSimilarity(p1, p2, weights);
            break;

          case TRM_AFFINE:
            return matchPointListsAffine(p1, p2, weights);
            break;

          case TRM_PROJECTIVE:
            return matchPointListsProjective(p1, p2, weights);
            break;

          default:
            throw IceException(FNAME, M_WRONG_MODE);
          }

        return Trafo();
      }
    RETHROW;
  }

  Trafo matchPointLists(const vector<Point>& p1, const vector<Point>& p2,
                        int mode)
  {
    vector<double> weights(p1.size(), 1);
    return matchPointLists(p1, p2, mode, weights);
  }
#undef FNAME
}
