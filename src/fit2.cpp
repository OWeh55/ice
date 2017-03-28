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

/*******************************************************************
 *   Anpassen von geometr. Elementen (Linien,                      *
 *    Kreisboegen) an Kurven (Konturen)                            *
 *   ConturPointList()    Punktliste aus Kontur              *
 *   FitLine()              Gerade anpassen                    *
 *   FitCircle()    Kreis anpassen         *
 *   FitQuadrFunc               Quadrat. Funktion anpassen         *
 *   FitLineSegment()           Geradensegment anpassen            *
 *   SegmConturV2()           Kontur segmentieren                *
 *   SegmConturV1()       Geradenananpassung fuer Polygonzuege *
 *     M. Schubert 15.12.92                                    *
 *     (K. Voss 05.02.92)                                      *
 *                                                             *
 *******************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "fit.h"

#include "contools.h"
#include "matrix_function.h"
#include "rooteig.h"
#include "circle.h"
#include "MatrixAlgebra.h"
#include "mateigen.h"
#include "analygeo.h"

namespace ice
{

#define FNAME "FitCircle"

  /********************************************************************/

  static double new_weights(const std::vector<Point>& pl,
                            std::vector<double>& weights,
                            const Circle& c)
  {
    double sd = 0;
    std::vector<double> da(pl.size());

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        double d = c.Distance(pl[i]);
        da[i] = d;
        sd += d * d;
      }

    sd /= pl.size();

    double mse = sqrt(sd);

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        weights[i] = exp(-(da[i]) * (da[i]) / sd);
      }

    return mse;
  }

  Circle FitCircle(const std::vector<Point>& pl, const std::vector<double>& weight)
  {
    Matrix cov(4, 4);

    unsigned int pnr = pl.size();

    if (pnr < 3)
      {
        Message(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return Circle();
      }

    if (pnr != weight.size())
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return Circle();
      }

    /* Berechnung der Kovarianzmatrix */
    cov.Set(0.0);

    for (unsigned int i = 0; i < pnr; i++)
      {
        const double& x = pl[i].x;
        const double& y = pl[i].y;
        const double& w = weight[i];

        double x2 = x * x;
        double y2 = y * y;

        cov[0][0] += (x2 + y2) * (x2 + y2) * w;
        cov[0][1] += (x2 + y2) * x      * w;
        cov[0][2] += (x2 + y2) * y      * w;
        cov[0][3] += (x2 + y2)        * w;

        cov[1][1] += x2 * w;
        cov[1][2] += x * y * w;
        cov[1][3] += x  * w;

        cov[2][2] += y2 * w;
        cov[2][3] += y  * w;

        cov[3][3] += w;
      }

    for (int i = 1; i < 4; i++)
      for (int j = 0; j < i; j++)
        cov[i][j] = cov[j][i];

    Vector eval;
    Matrix evec;

    Eigenvalue(cov, eval, evec);

    //    std::cout << "Eigenwerte " << eval <<std::endl;
    //    std::cout << evec <<std::endl;

    double v1 = evec[1][3] / evec[0][3];
    double v2 = evec[2][3] / evec[0][3];
    double v3 = evec[3][3] / evec[0][3];

    //    std::cout << v1 << " " << v2 << " " << v3 << std::endl;
    double xm = -v1 / 2;
    double ym = -v2 / 2;
    double rad = sqrt(Sqr(xm) +
                      Sqr(ym) - v3);
    //    std::cout << xm << " " << ym << " " << rad << std::endl;
    return Circle(xm, ym, rad);
  }

  Circle FitCircle(const std::vector<Point>& pl)
  {
    std::vector<double> weight(pl.size());

    for (unsigned int i = 0; i < pl.size(); i++)
      weight[i] = 1.0;

    return FitCircle(pl, weight);
  }


  Circle FitCircle(const std::vector<Point>& pl, std::vector<double>& weight, int step)
  {
    Circle res;

    if (pl.size() < 3)
      {
        Message(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return res;
      }

    if (step < 0)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return res;
      }

    double mse = 1.0;

    while ((step >= 0) && (mse > 0.0001))
      {
        res = FitCircle(pl, weight);
        step--;

        if (step >= 0)
          mse = new_weights(pl, weight, res);
      }

    return res;
  }

  Circle FitCircle(const std::vector<Point>& pl, int step)
  {
    std::vector<double> weight(pl.size(), 1.0);
    return FitCircle(pl, weight, step);
  }

  int FitCircle(const Matrix& pl,
                double& xm, double& ym, double& rad,
                int step)
  {
    Circle c;
    ReturnErrorIfFailed(c = FitCircle(pl, step));
    xm = c.getPos().x;
    ym = c.getPos().y;
    rad = c.getR();
    return OK;
  }

  int FitCircle(const Matrix& pl, double* par, int step)
  {
    Circle c;
    ReturnErrorIfFailed(c = FitCircle(pl, step));
    par[0] = c.getPos().x;
    par[1] = c.getPos().y;
    par[2] = c.getR();
    return OK;
  }

  Circle FitCircle(const Matrix& mpl, int step)
  {
    std::vector<Point> pl;
    Matrix2pl(mpl, pl);
    std::vector<double> weight;
    Matrix2w(mpl, weight);
    return FitCircle(pl, weight, step);
  }

#undef FNAME
}
