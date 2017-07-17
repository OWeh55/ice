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

#include "defs.h"
#include "macro.h"
#include "IceException.h"

#include "contools.h"
#include "simplex.h"

#include "lmdif.h"

#include "MatrixAlgebra.h"
#include "vectortools.h"
#include "geo.h"

using namespace std;

//--------------------------------------------
namespace ice
{
#define FNAME "MakeTransform"
  Trafo CTransform(const Vector& param)
  {
    // erzeugt eine 3D-2D-Trafo (Kamera)
    // Parameter in param
    Trafo c(3, 3); // Null-Transformation
    c.rotateZ(param[0]); // Rotation (Eulersche Winkel)
    c.rotateY(param[1]);
    c.rotateX(param[2]);
    c.shift(Vector(param[3], param[4], param[5])); // Position im Raum
    c.Projective();      // 3d-2d
    c.scale(0, 0, param[6], param[6]*param[7]); // anisotrope Skalierung
    c.shearX(param[8]);  // Scherung
    c.shift(param[9], param[10]); // Hauptpunkt-Verschiebung
    return c;
  }

  Trafo CCTransform(const Vector& param)
  {
    // erzeugt eine 2D-2D-Trafo, wie sie zwischen 2
    // Bildern auftreten kann, die mit der *gleichen* Kamera
    // vom *gleichen* Punkt aus aufgenommen wurden

    Trafo R(3, 3); // "Externe Kamera-Parameter"
    // translation entfällt
    R.rotateZ(param[0]); // Rotation (Eulersche Winkel)
    R.rotateY(param[1]);
    R.rotateX(param[2]);
    Matrix m = Matrix(R.getMatrix())(0, 0, 2, 2); // Rotationsmatrix

    Matrix c(3, 3); // Kamera-Matrix (interne Parameter)
    c[0][0] = param[3];
    c[0][1] = param[5];
    c[0][2] = param[6];
    c[1][0] = 0.0;
    c[1][1] = param[3] * param[4];
    c[1][2] = param[7];
    c[2][0] = 0.0;
    c[2][1] = 0.0;
    c[2][2] = 0.0;

    return Trafo(c * m * Inverse(c));
  }

#undef FNAME
#define FNAME "MatchPointlists"
  Trafo MatchProjective(const Matrix& p1, const Matrix& p2,
                        const Vector& weights)
  {
    try
      {
        int nPoints = p1.rows();
        int dim1 = p1.cols();
        int dim2 = p2.cols();

        matrix<double> a(nPoints * dim2 + 1, (dim1 + 1) * (dim2 + 1), 0);
        vector<double> r(nPoints * dim2 + 1);
        vector<double> rv;

        for (int j = 0; j < nPoints; j++)
          {
            double weight = weights[j];

            for (int k = 0; k < dim2; k++)
              {
                for (int i = 0; i < dim1; i++)
                  {
                    a[j * dim2 + k][k * (dim1 + 1) + i] = p1[j][i] * weight;
                  }

                a[j * dim2 + k][k * (dim1 + 1) + dim1] = 1.0 * weight;

                for (int i = 0; i < dim1; i++)
                  {
                    a[j * dim2 + k][dim2 * (dim1 + 1) + i] = -p1[j][i] * p2[j][k] * weight;
                  }

                a[j * dim2 + k][(dim1 + 1) * (dim2 + 1) - 1] = -p2[j][k] * weight;
                r[j * dim2 + k] = 0.0;
              }
          }

        for (int i = 0; i < dim1 + 1; i++)
          {
            a[nPoints * dim2][dim2 * (dim1 + 1) + i] = 1.0;
          }

        r[nPoints * dim2] = 1.0;

        //    cout << a << endl;

        rv = solveLinearEquation(a, r);

        matrix<double> tmatrix(dim1 + 1, dim2 + 1);
        for (int k = 0; k < dim2 + 1; k++)
          for (int i = 0; i < dim1 + 1; i++)
            {
              tmatrix[k][i] = rv[k * (dim1 + 1) + i];
            }

        return tmatrix;
      }
    RETHROW;
  }

  void Trafo2Vector(const Trafo& tr, vector<double>& v)
  {
    matrix<double> m = tr.getMatrix();
    int i = 0;
    for (int k = 0; k < m.rows(); ++k)
      for (int l = 0; l < m.cols(); ++l)
        {
          v[i++] = m[k][l];
        }
  }

  void Vector2Trafo(const vector<double>& v, Trafo& tr)
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

  class LMProjective: public LMFunctionObject
  {
  private:
    const Matrix& p1;
    const Matrix& p2;
    const Vector& w;
    vector<double>& trpara;
  public:
    LMProjective(const Matrix& pl1, const Matrix& pl2, const Vector& weights,
                 vector<double>& trp):
      p1(pl1), p2(pl2), w(weights), trpara(trp) {};

    int funcdim() const
    {
      return p2.rows() * p2.cols();
    }

    int operator()(Vector& result) const
    {
      Trafo tr(p1.cols(), p2.cols());
      Vector2Trafo(trpara, tr);
      Matrix pointsTransformed;
      transformList(tr, p1, pointsTransformed);
      int k = 0;
      for (int i = 0; i < (int)p2.rows(); ++i)
        for (int l = 0; l < (int) p2.cols(); ++l)
          {
            result[k++] = (pointsTransformed[i][l] - p2[i][l]) * w[i];
          }
      return 1;
    }
  };

  Trafo iterateProjective(const Trafo& tr,
                          const Matrix& p1, const Matrix& p2,
                          const Vector& weights)
  {
    matrix<double> m = tr.getMatrix();
    vector<double> trpara(m.cols()*m.rows());
    Trafo2Vector(tr, trpara);

    vector<double*> pp(trpara.size() - 1);
    for (int i = 0; i < (int)pp.size(); ++i)
      {
        pp[i] = &trpara[i];
      }

    int inumber;
    LMDif(pp,      // Liste von Zeigern auf zu optimierende Parameter
          LMProjective(p1, p2, weights, trpara), // Konstruktion des Funktors
          10000,      // maximale Zahl der Iterationsschritte
          inumber);  // Rückgabe der durchgeführten Iterationsschritte

    Trafo res(tr);
    Vector2Trafo(trpara, res);
    return res;
  }

  Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                        int mode, const Vector& weights)
  {
    try
      {
        int nPoints = p1.rows();
        int dim1 = p1.cols();
        int dim2 = p2.cols();
        Trafo res(dim1, dim2);

        double weightsum;

        if ((nPoints != p2.rows()) || (nPoints != weights.Size()))
          throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

        if (dim2 > dim1)
          throw IceException(FNAME, M_WRONG_PARAM);

        if ((dim1 != dim2) && (mode != TRM_AFFINE) && (mode != TRM_PROJECTIVE))
          throw IceException(FNAME, M_WRONG_PARAM);

        if ((mode == TRM_SIMILARITY_NOR) && ((dim1 != 2) || (dim2 != 2)))
          throw IceException(FNAME, M_WRONG_PARAM);

        switch (mode)
          {
          case TRM_SHIFT:
            // dim1==dim2 !!
          {
            Vector shift(dim1);
            weightsum = 0;

            for (int i = 0; i < dim1; i++)
              {
                shift[i] = 0;
              }

            for (int j = 0; j < nPoints; j++)
              {
                for (int i = 0; i < dim1; i++)
                  {
                    shift[i] += (p2[j][i] - p1[j][i]) * weights[j];
                  }

                weightsum += weights[j];
              }

            for (int i = 0; i < dim1; i++)
              {
                res.m[i][dim2] = shift[i] / weightsum;
              }
          }
          break;
          case TRM_SCALE_SHIFT:
          {
            weightsum = 0;
            Vector sum1(dim1);
            sum1.set(0);
            Vector sum2(dim2);
            sum2.set(0);

            for (int j = 0; j < nPoints; j++)
              for (int i = 0; i < dim2; i++)
                {
                  sum1[i] += weights[j] * p1[j][i];
                }

            for (int j = 0; j < nPoints; j++)
              {
                for (int i = 0; i < dim1; i++)
                  {
                    sum2[i] += weights[j] * p2[j][i];
                  }

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
            Vector shift(dim1);

            for (int i = 0; i < dim1; i++)
              {
                shift[i] = 0;
              }

            for (int j = 0; j < nPoints; j++)
              for (int i = 0; i < dim1; i++)
                {
                  shift[i] += (p2[j][i] - alpha * p1[j][i]) * weights[j];
                }

            for (int i = 0; i < dim1; i++)
              {
                res.m[i][dim2] = shift[i] / weightsum;
                res.m[i][i] = alpha;
              }
          }
          break;

          case TRM_SIMILARITY_NOR:
            // dim1 == dim2 == 2
          {
            matrix<double> a(2 * nPoints, 4);
            vector<double> r(2 * nPoints);

            for (int j = 0; j < nPoints; j++)
              {
                double weight = weights[j];

                a[j * 2][0] = p1[j][0] * weight;
                a[j * 2][1] = p1[j][1] * weight;
                a[j * 2][2] = weight;
                a[j * 2][3] = 0;

                r[j * 2] = p2[j][0] * weight;

                a[j * 2 + 1][0] = p1[j][1] * weight;
                a[j * 2 + 1][1] = -p1[j][0] * weight;
                a[j * 2 + 1][2] = 0;
                a[j * 2 + 1][3] = weight;

                r[j * 2 + 1] = p2[j][1] * weight;
              }

            vector<double> rv = solveLinearEquation(a, r);

            // cout << rv << endl;
            res.m[0][0] = rv[0];
            res.m[0][1] = rv[1];
            res.m[1][0] = -rv[1];
            res.m[1][1] = rv[0];
            res.m[0][2] = rv[2];
            res.m[1][2] = rv[3];

            break;
          }

          case TRM_AFFINE:
            // problem is separable
          {
	    /*
            Matrix a(nPoints, dim1 + 1);
            Vector r(nPoints);
            Vector rv(dim1 + 1);

            for (int j = 0; j < nPoints; j++)
              {
                for (int i = 0; i < dim1; i++)
                  {
                    a[j][i] = p1[j][i] * weights[j];
                  }

                a[j][dim1] = 1.0 * weights[j];
              }

            for (int k = 0; k < dim2; k++)
              {
                for (int j = 0; j < nPoints; j++)
                  {
                    r[j] = p2[j][k] * weights[j];
                  }

                rv = SolveLinEqu(a, r);

                for (int i = 0; i < dim1 + 1; i++)
                  {
                    res.m[k][i] = rv[i];
                  }
              }
	    */
            matrix<double> a(nPoints, dim1 + 1);
            vector<double> r(nPoints);
            vector<double> rv(dim1 + 1);

            for (int j = 0; j < nPoints; j++)
	      {
                for (int i = 0; i < dim1; i++)
                    a[j][i] = p1[j][i] * weights[j];

                a[j][dim1] = 1.0 * weights[j];
              }

            for (int k = 0; k < dim2; k++)
              {
                for (int j = 0; j < nPoints; j++)
                    r[j] = p2[j][k] * weights[j];

                rv = solveLinearEquation(a, r);

                for (int i = 0; i < dim1 + 1; i++)
                    res.m[k][i] = rv[i];
              }

          }
          break;

          case TRM_PROJECTIVE:

            // affine transformation as first approximation
            res = MatchPointlists(p1, p2, TRM_AFFINE, weights);

            // iterative refinement to projective transformation
            res = iterateProjective(res, p1, p2, weights);

            break;

          default:
            throw IceException(FNAME, M_WRONG_MODE);
          }

        return res;
      }
    RETHROW;
  }

  Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                        int mode)
  {
    Vector weights(p1.rows());
    weights.set(1.0);
    return MatchPointlists(p1, p2, mode, weights);
  }

  Trafo MatchPointlists(const PointList& pl1, const PointList& pl2, int mode)
// compatibility function with struct Pointlists
  {
    try
      {

        if ((pl1 == NULL) || (pl2 == NULL))
          throw IceException(FNAME, M_WRONG_PTR);

        int pnumber = pl1->lng;

        if (pl2->lng != pnumber)
          throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

        Matrix p1(pnumber, 2);
        Matrix p2(pnumber, 2);
        Vector w(pnumber);

        for (int i = 0; i < pnumber; i++)
          {
            p1[i][0] = pl1->xptr[i];
            p1[i][1] = pl1->yptr[i];
            p2[i][0] = pl2->xptr[i];
            p2[i][1] = pl2->yptr[i];
            w[i] = pl2->wptr[i];
          }

        return MatchPointlists(p1, p2, mode, w);
      }
    RETHROW;
  }

  Trafo MatchPointlists(const vector<Point>& pl1, const vector<Point>& pl2,
                        int mode)
  {
    Trafo res;
    int nPoints = pl1.size();

    if ((int)pl2.size() != nPoints)
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

    Matrix p1(nPoints, 2);
    Matrix p2(nPoints, 2);

    for (int i = 0; i < nPoints; i++)
      {
        p1[i][0] = pl1[i].x;
        p1[i][1] = pl1[i].y;
        p2[i][0] = pl2[i].x;
        p2[i][1] = pl2[i].y;
      }

    return MatchPointlists(p1, p2, mode);
  }
#undef FNAME
#define FNAME "MatchPointlistsLinOpt"
  Trafo MatchPointlistsLinOpt(const Matrix& p1, const Matrix& p2,
                              int mode, const Vector& weights, double limit)
  {
    // wrapper function to use old style MatchPointlistsLinOpt with classes
    try
      {
        int nPoints;
        int dim1 = p1.cols();
        int dim2 = p2.cols();
        PointList pl1, pl2;
        double tr[3][3];

        Matrix tmatrix(3, 3);
        Trafo res(2, 2);

        nPoints = p1.rows();

        if ((nPoints != p2.rows()) || (nPoints != weights.Size()))
          throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

        nPoints = p1.rows();

        if ((dim2 != 2) || (dim1 != 2)) // linear opt. only 2 dimensions
          throw IceException(FNAME, M_WRONG_PARAM);

        // construct pointlist
        pl1 = NewPointList(nPoints);
        pl2 = NewPointList(nPoints);

        for (int i = 0; i < nPoints; i++)
          {
            PutPoint(pl1, i, p1[i][0], p1[i][1], weights[i]);
            PutPoint(pl2, i, p2[i][0], p2[i][1], weights[i]);
          }

        MatchPointlistsLinOpt(pl1, pl2, tr, mode, limit);

        FreePointList(pl1);
        FreePointList(pl2);

        for (int i = 0; i < 3; i++)
          for (int j = 0; j < 3; j++)
            {
              tmatrix[i][j] = tr[i][j];
            }

        return tmatrix;
      }
    RETHROW;
  }

  Trafo MatchPointlistsLinOpt(const Matrix& p1, const Matrix& p2,
                              int mode)
  {
    Vector weights(p1.rows());
    for (int i = 0; i < p1.rows(); i++)
      {
        weights[i] = 1.0;
      }

    return MatchPointlistsLinOpt(p1, p2, mode, weights);
  }

  Trafo MatchPointlistsLinOpt(const std::vector<Point>& p1, const std::vector<Point>& p2,
                              int mode)
  {
    std::vector<double> weights(p1.size());

    for (int i = 0; i < (int)p1.size(); i++)
      {
        weights[i] = 1.0;
      }

    return MatchPointlistsLinOpt(p1, p2, mode, weights);
  }

  Trafo MatchPointlistsLinOpt(const std::vector<Point>& p1, const std::vector<Point>& p2,
                              int mode, const std::vector<double>& weights,
                              double limit)
  {
    // wrapper function for old style MatchPointlistsLinOpt

    int nPoints = p1.size();

    Matrix tmatrix(3, 3, 1);
    if ((nPoints != (int)p2.size()) || (nPoints != (int)weights.size()))
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);

    PointList pl1, pl2;
    double tr[3][3];

    // construct pointlist
    pl1 = NewPointList(nPoints);
    pl2 = NewPointList(nPoints);

    for (int i = 0; i < nPoints; i++)
      {
        PutPoint(pl1, i, p1[i].x, p1[i].y, weights[i]);
        PutPoint(pl2, i, p2[i].x, p2[i].y, weights[i]);
      }

    MatchPointlistsLinOpt(pl1, pl2, tr, mode, limit);

    FreePointList(pl1);
    FreePointList(pl2);

    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        {
          tmatrix[i][j] = tr[i][j];
        }

    return tmatrix;
  }

#undef FNAME
}
