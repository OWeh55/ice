#ifndef GAUSFIT_H
#define GAUSFIT_H
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
 * fitgauss - Fitten mit der Methode der kleinsten Quadrate nach Gauss
 *
 * Wolfgang Ortmann, 2002..2013
 */

#include <vector>

#include "Vector.h"
#include "Matrix.h"

namespace ice
{
  class GaussFit
  {
  public:
    GaussFit(int dim = 0);
    virtual ~GaussFit() {};
    //
    virtual void Init(int pdim = 0);
    virtual void Value(const Vector& p, double res, double w = 1.0);
    virtual void Value(const std::vector<double>& p, double res, double w = 1.0);
    virtual void Finish();

    virtual void Fit(const Matrix& m, const Vector& res);
    //
    virtual void getResult(std::vector<double>& v) const;
    virtual void getResult(Vector& v) const;

    virtual double Variance() const;

  protected:
    int dim;

    bool finished;

    double gsquare;
    double wsum;
    int values;

    std::vector<std::vector<double> > sum;
    double var;
    std::vector<double> gsum;
    std::vector<double> para;
  };
} // namespace ice
#endif
