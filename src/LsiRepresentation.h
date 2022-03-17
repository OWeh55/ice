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
#ifndef _LSI_FILTER_REPRESENTATION_H
#define _LSI_FILTER_REPRESENTATION_H

#include "ImageBase.h"
#include "based.h"

namespace ice
{
  class LsiRepresentation
  {
  public:
    LsiRepresentation(int nx, int ny);
    virtual ~LsiRepresentation() {}

    int getXDimension() const
    {
      return dimx;
    }

    int getYDimension() const
    {
      return dimy;
    }

    int getDX() const
    {
      return (dimx - 1) / 2;
    }

    int getDY() const
    {
      return (dimy - 1) / 2;
    }

    int proposeOffset(const Image& img) const
    {
      double p, m;
      sumPlusSumMinus(p, m);
      double range = p - m;
      return RoundInt(-m / range * img.maxval);
    }

    virtual void sumPlusSumMinus(double& sump, double& summ) const = 0;
    virtual void normalize() = 0;

    virtual void filter(const Image& src, const Image& dst, int offset) const = 0;
    virtual void filter(const Image& src, ImageD dst) const = 0;
    virtual void filter(ImageD src, ImageD dst) const = 0;

    virtual double getMask(int i, int j) const = 0;
    virtual const matrix<double>& getMask() const = 0;
    virtual void getMask(matrix<double>& m) const = 0;

    virtual void negateMask() = 0;
    virtual void reflectMask() = 0;

  protected:
    int idx(int x, int y) const
    {
      return y * dimx + x;
    }

    int dimx;
    int dimy;
  };
}
#endif
