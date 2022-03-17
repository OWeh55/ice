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
#ifndef LSI_FILTER_REPRESENTATION_D_H
#define LSI_FILTER_REPRESENTATION_D_H

namespace ice
{
  class LsiRepresentationD: public LsiRepresentation
  {
  public:

    LsiRepresentationD(const LsiRepresentationD& r);
    LsiRepresentationD(const Matrix& m);
    LsiRepresentationD(const matrix<double>& m);

    virtual void sumPlusSumMinus(double& sump, double& summ) const override;

    virtual void negateMask() override;
    virtual void normalize() override;
    virtual void reflectMask() override;

    virtual void filter(const Image& src, const Image& dst, int offset) const override;
    virtual void filter(const Image& src, ImageD dst) const override;
    virtual void filter(ImageD src, ImageD dst) const override;

    virtual ~LsiRepresentationD()
    {
      //      delete [] mask;
    }

    // several forms of access to the filter mask
    virtual double getMask(int x, int y) const override
    {
      //      return mask[idx(x, y)];
      return mask[y][x];
    };

    virtual const matrix<double>& getMask() const override
    {
      return mask;
    }

    virtual void getMask(matrix<double>& m) const override;

  protected:
    matrix<double> mask;
    //    double* mask;
  };
}
#endif
