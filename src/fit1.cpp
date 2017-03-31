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
 *   Anpassen von geometr. Elementen (Linien,
 *    Kreisbögen) an Kurven (Konturen)
 *
 *   FitLine()              Gerade anpassen
 *
 *   M. Schubert 15.12.92
 *   W. Ortmann 9/2006,10/2011  Modernisierung
 */

#include <vector>

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "fit.h"
#include "lineseg.h"
#include "circle.h"
#include "analygeo.h"

#include "matrix_function.h"
#include "rooteig.h"

using namespace std;

namespace ice
{

#define M_2PI (2*M_PI)

  /***************************************************************/
#define FNAME "FitLine"
  static double new_weights(const vector<Point>& pl, vector<double>& weight,
                            const LineSeg& line)
  {
    double sd = 0;
    double mse;

    vector<double> da(pl.size());

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        double d = da[i] = line.Distance(pl[i]);
        sd += d * d;
      }

    if (sd == 0)
      {
        return 0.0;
      }

    sd /= pl.size();

    mse = sqrt(sd);

    for (unsigned int i = 0; i < pl.size(); i++)
      {
        weight[i] = exp(-da[i] * da[i] / sd);
      }

    return mse;
  }

  static LineSeg calc_line(double n, double x, double y, double xx, double xy, double yy)
  {
    x /= n;
    y /= n;
    xx /= n;
    yy /= n;
    xy /= n;

    double dx2 = -x * x + xx;
    double dy2 = -y * y + yy;

    double dd = dx2 - dy2;
    double dxy = (-x * y + xy) * 2;

    double phi1 = atan2(dxy, dd) / 2;
    double phi2 = phi1 + M_PI_2;

    double s1 = (Sqr(cos(phi1)) * dx2)
                + (sin(phi1) * cos(phi1) * dxy)
                + (Sqr(sin(phi1)) * dy2);
    double s2 = (Sqr(cos(phi2)) * dx2)
                + (sin(phi2) * cos(phi2) * dxy)
                + (Sqr(sin(phi2)) * dy2);

    double p, fi;

    if (s1 < s2)
      {
        fi = phi1;
      }
    else
      {
        fi = phi2;
      }

    p = x * cos(fi) + y * sin(fi);

    if (p < 0)
      {
        fi += M_PI;
        p = -p;
      }

    return LineSeg(p, fi);
  }

// fitting point list to line
  LineSeg FitLine(const vector<Point>& pl)
  {
    unsigned int nr = pl.size();

    if (nr < 2)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return LineSeg();
      }

    double n = pl.size();
    double x = 0.0;
    double y = 0.0;
    double xx = 0.0;
    double yy = 0.0;
    double xy = 0.0;

    for (unsigned int i = 0; i < nr; i++)
      {
        const double& px = pl[i].x;
        const double& py = pl[i].y;

        x  += px;
        y  += py;

        xx += px * px;
        yy += py * py;
        xy += px * py;
      }

    return calc_line(n, x, y, xx, xy, yy);
  }

// fitting point list with weights to line
  LineSeg FitLine(const vector<Point>& pl, const vector<double>& weight)
  {
    unsigned int nr = pl.size();

    if (nr < 2)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return LineSeg();
      }

    if (nr != weight.size())
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return LineSeg();
      }

    double n = 0.0;
    double x = 0.0;
    double y = 0.0;
    double xx = 0.0;
    double yy = 0.0;
    double xy = 0.0;

    for (unsigned int i = 0; i < nr; i++)
      {
        const double& pw = weight[i];

        const double& px = pl[i].x;
        const double& py = pl[i].y;

        n += pw;
        x += px * pw;
        y += py * pw;

        xx += px * px * pw;
        yy += py * py * pw;
        xy += px * py * pw;
      }

    if (n < 1e-10)   // sum of weights too small
      {
        throw IceException(FNAME, M_WRONG_POINTLIST, WRONG_PARAM);
        return LineSeg();
      }

    return calc_line(n, x, y, xx, xy, yy);
  }

// iterative fitting point list to line
  LineSeg FitLine(const vector<Point>& pl, vector<double>& weight, int n)
  {
    unsigned int nr = pl.size();

    if (nr < 2)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return LineSeg();
      }

    if (n < 0)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return LineSeg();
      }

    if (nr != weight.size())
      {
        throw IceException(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return LineSeg();
      }

    if (n == 0)
      {
        return FitLine(pl, weight);
      }

    LineSeg res;
    double mse = 1.0;

    for (int i = 0; i < n && mse > 1e-3; i++)
      {
        res = FitLine(pl, weight);

        if (i < n - 1)
          {
            mse = new_weights(pl, weight, res);
          }
      }

    return res;
  }

  LineSeg FitLine(const vector<Point>& pl, int n)
  {
    vector<double> weight(pl.size(), 1.0);
    return FitLine(pl, weight, n);
  }

  LineSeg FitLine(const Matrix& m, int step)
  {
    if (m.cols() < 2)
      {
        throw IceException(FNAME, M_WRONG_POINTLIST, WRONG_PARAM);
        return LineSeg();
      }

    if (m.rows() < 2)
      {
        throw IceException(FNAME, M_TOO_LESS_POINTS, WRONG_PARAM);
        return LineSeg();
      }

    vector<Point> pl;
    Matrix2pl(m, pl);

    vector<double> weight;
    Matrix2w(m, weight);

    return FitLine(pl, weight, step);
  }

  int FitLine(const Matrix& m, double& p, double& fi, int step)
  {
    LineSeg fitted_line;
    RETURN_ERROR_IF_FAILED(fitted_line = FitLine(m, step));
    fitted_line.CalcHesse(p, fi);
    return OK;
  }

  int FitLine(const Matrix& pl, double* par, int step)
  {
    double p, fi;
    RETURN_ERROR_IF_FAILED(FitLine(pl, p, fi, step));
    par[0] = p;
    par[1] = fi;
    return OK;
  }

#undef FNAME
}
