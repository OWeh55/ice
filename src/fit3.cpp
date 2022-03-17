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

/*
 * Anpassen von geometr. Elementen (Linien,
 * Kreisbögen) an Kurven (Konturen)
 *
 * M. Schubert 15.12.92
 * W. Ortmann 2006,2012 Teil-Modernisierung
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <vector>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "fit.h"
#include "matrix_function.h"
#include "rooteig.h"
#include "analygeo.h"
#include "matdef.h"
#include "lineqn.h"

namespace ice
{

#define M_2PI (2*M_PI)

  /************************************************************/
#define FNAME "FitEllipse"
  /**********************************************************************/
  double fl_new_weights_ellipse(const std::vector<Point>& pl,
                                std::vector<double>& wv, double* par)
  {
    std::vector<double> da(pl.size());
    double p[2];

    double w = 0;
    double sd = 0;
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        p[0] = pl[i].x;
        p[1] = pl[i].y;
        double d = DistPointEllipse(p, par);
        da[i] = d;
        w += wv[i];
        sd += d * d;
      }

    sd /= w;
    double mse = sqrt(sd);

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        wv[i] = exp(-(da[i]) * (da[i]) / sd);
      }

    return mse;
  }

  void normalizeShift(std::vector<Point>& pl, double& x, double& y)
  {
    double sumx = 0.0;
    double sumy = 0.0;
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        sumx += pl[i].x;
        sumy += pl[i].y;
      }
    x = sumx / pl.size();
    y = sumy / pl.size();
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        pl[i].x -= x;
        pl[i].y -= y;
      }
  }

  void normalizeScale(std::vector<Point>& pl, double& fac)
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        sum += pl[i].r2();
      }

    fac = sqrt(sum / pl.size());

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        pl[i].x /= fac;
        pl[i].y /= fac;
      }
  }

  int FitEllipse(std::vector<Point> pl, std::vector<double> wv,
                 double* par, int step, int mode)
  {
// mode = 1: (default)
// Es werden die algebraischen Distanzen minimiert bei der Restriktion
// a^2 + b^2 + c^2 + d^2 + e^2 + f^2 = 1
// Dies führt auf ein Eigenwertproblem
//
// mode = 2: Es werden die algebraischen Distanzen minimiert bei der
// Restriktion a + c = 1. Dies führt auf die Lösung eines linearen Gleichungs-
// systems
    MatrixStruct cov_2a;

    double mse;
    double feat[5], loesung[5], koef[6];
    double cov[6][6];
    double cov_2[5][5];
    double re_2[5];
    double eval[6];
    double evec[6][6];
    double tvec[6][6];
    int code, type;
    double* cptr;

    if (pl.size() < 4)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    if (step < 0  || pl.size() != wv.size())
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if (mode != 1 && mode != 2)
      throw IceException(FNAME, M_WRONG_MODE);

    // normalize points
    double xs, ys;
    normalizeShift(pl, xs, ys);
    double fac;
    normalizeScale(pl, fac);

    if (mode == 1)
      {
        mse = 1.0;

        while ((step >= 0) && (mse > 0.0001))
          {
            /* Berechnung der Kovarianzmatrix */
            SetMatrix((double*)cov, 6, 6, 0.0);

            for (unsigned int i = 0; i < pl.size(); i++)
              {
                double x = pl[i].x;
                double y = pl[i].y;
                double w = wv[i];

                double x2 = x * x;
                double y2 = y * y;
                double x3 = x2 * x;
                double y3 = y2 * y;
                double x4 = x2 * x2;
                double y4 = y2 * y2;

                cov[0][0] += (x4 * w);
                cov[0][1] += (x2 * y2 * w);
                cov[0][2] += (x3 * w);
                cov[0][3] += (x2 * y * w);
                cov[0][4] += (x3 * y * w);
                cov[0][5] += (x2 * w);

                cov[1][1] += (y4 * w);
                cov[1][2] += (x * y2 * w);
                cov[1][3] += (y3 * w);
                cov[1][4] += (x * y3 * w);
                cov[1][5] += (y2 * w);

                cov[2][2] += (x2 * w);
                cov[2][3] += (x * y * w);
                cov[2][4] += (x2 * y * w);
                cov[2][5] += (x * w);

                cov[3][3] += (y2 * w);
                cov[3][4] += (x * y2 * w);
                cov[3][5] += (y * w);

                cov[4][4] += (x2 * y2 * w);
                cov[4][5] += (x * y * w);

                cov[5][5] += w;
              }

            for (unsigned i = 1; i < 6; i++)
              for (unsigned int j = 0; j < i; j++)
                {
                  cov[i][j] = cov[j][i];
                }

            MoveMatrix((double*)cov, 6, 6, (double*)tvec);

            RETURN_ERROR_IF_FAILED(cptr = InvertMatrix((double*)tvec, 6, (double*)cov));

            if (cptr == NULL)
              throw IceException(FNAME, M_WRONG_POINTS);

            RETURN_ERROR_IF_FAILED(EigenVal((double*)cov, 6, eval, (double*)evec));

            RETURN_ERROR_IF_FAILED(FeatureQuadrFunc(evec[0], feat, &type));

            if (type != ELLIPSE)
              {
                par[0] = par[1] = par[2] = par[3] = par[4] = 0;
                return NO_ELLIPSE;
              }

            par[0] = feat[0];
            par[1] = feat[1];
            par[2] = feat[2];
            par[3] = feat[3];
            par[4] = feat[4];

            step--;

            if (step >= 0)
              {
                mse = fl_new_weights_ellipse(pl, wv, par);
              }
          }
      }

// **************************************************************************
    if (mode == 2)
      {
        mse = 1.0;

        while ((step >= 0) && (mse > 0.0001))
          {

            /* Berechnung der Kovarianzmatrix */
            SetMatrix((double*)cov_2, 5, 5, 0.0);

            for (unsigned int i = 0; i < 5; ++i)
              {
                re_2[i] = 0.0;
              }

            for (unsigned int i = 0; i < pl.size(); i++)
              {
                double x = pl[i].x;
                double y = pl[i].y;

                double w = wv[i];

                double x2 = x * x;
                double y2 = y * y;

                cov_2[0][0] += ((y2 - x2) * (y2 - x2) * w);
                cov_2[0][1] += ((y2 - x2) * x * y * w);
                cov_2[0][2] += ((y2 - x2) * x * w);
                cov_2[0][3] += ((y2 - x2) * y * w);
                cov_2[0][4] += ((y2 - x2)  * w);

                cov_2[1][1] += (x2 * y2  * w);
                cov_2[1][2] += (x2 * y  * w);
                cov_2[1][3] += (x * y2  * w);
                cov_2[1][4] += (x * y  * w);

                cov_2[2][2] += (x2  * w);
                cov_2[2][3] += (x * y  * w);
                cov_2[2][4] += (x  * w);

                cov_2[3][3] += (y2  * w);
                cov_2[3][4] += (y  * w);

                cov_2[4][4] += (w);

                re_2[0] += ((y2 - x2) * x2 * w);
                re_2[1] += (x * y * x2  * w);
                re_2[2] += (x * x2  * w);
                re_2[3] += (y * x2  * w);
                re_2[4] += (x2  * w);
              }

            for (unsigned int i = 1; i < 5; i++)
              for (unsigned int j = 0; j < i; j++)
                {
                  cov_2[i][j] = cov_2[j][i];
                }

            for (unsigned int i = 0; i < 5; ++i)
              {
                re_2[i] = -re_2[i];
              }

            cov_2a = NewMatrix(MAT_DOUBLE, 5, 5);

            for (unsigned int i = 0; i < 5; ++i)
              for (unsigned int j = 0; j < 5; ++j)
                {
                  cov_2a->data[i][j] = cov_2[i][j];
                }

            code = EquationSys(cov_2a, re_2, loesung);

            if (code != OK)
              throw IceException(FNAME, M_0);

            koef[0] = 1.0 - loesung[0];
            koef[1] = loesung[0];
            koef[2] = loesung[2];
            koef[3] = loesung[3];
            koef[4] = loesung[1];
            koef[5] = loesung[4];

            RETURN_ERROR_IF_FAILED(code = FeatureQuadrFunc(koef, feat, &type));

            if (type != ELLIPSE)
              {
                par[0] = par[1] = par[2] = par[3] = par[4] = 0;
                return NO_ELLIPSE;
              }

            par[0] = feat[0];
            par[1] = feat[1];
            par[2] = feat[2];
            par[3] = feat[3];
            par[4] = feat[4];

            step--;

            if (step >= 0)
              {
                mse = fl_new_weights_ellipse(pl, wv, par);
              }
          }
      }
    // denormalize parameters

    par[0] += xs;
    par[1] += ys;
    par[2] *= fac;
    par[3] *= fac;

    return OK;
  }

  int FitEllipse(const std::vector<Point>& pl,
                 double* par, int step, int mode)
  {
    std::vector<double> weights(pl.size(), 1.0);
    return FitEllipse(pl, weights, par, step, mode);
  }

  Ellipse FitEllipse(const std::vector<Point>& pl, int step, int mode)
  {
    try
      {
        double par[5];
        FitEllipse(pl, par, step, mode);
        return Ellipse(par[0], par[1], par[2], par[3], par[4]);
      }
    RETHROW;
  }

  Ellipse FitEllipse(const std::vector<Point>& pl, const std::vector<double>& weights,
                     int step, int mode)
  {
    try
      {
        double par[5];
        FitEllipse(pl, weights, par, step, mode);
        return Ellipse(par[0], par[1], par[2], par[3], par[4]);
      }
    RETHROW;
  }

  int FitEllipse(const Matrix& mpl, double* par, int step, int mode)
  {
    std::vector<Point> pl;
    std::vector<double> weights;
    Matrix2pl(mpl, pl);
    Matrix2w(mpl, weights);
    return FitEllipse(pl, weights, par, step, mode);
  }

  Ellipse FitEllipse(const Matrix& pl, int step, int mode)
  {
    try
      {
        double par[5];
        FitEllipse(pl, par, step, mode);
        return Ellipse(par[0], par[1], par[2], par[3], par[4]);
      }
    RETHROW;
  }

#undef FNAME
}
