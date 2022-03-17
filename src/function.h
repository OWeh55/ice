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
 * classes for functions
 * 1d function
 *      Function
 * 2d function
 *      Function2d
 */
#ifndef FUNCTION_H
#define FUNCTION_H

#include <vector>

#include <base.h>
#include <based.h>

#include "IceException.h"

namespace ice
{
  class Function
  {
  public:
    Function() {}
    virtual ~Function() {}

    virtual double operator()(double x) const = 0;
    virtual double inverse(double v) const
    {
      throw IceException("Function::inverse", M_NOT_IMPLEMENTED);
    }
    virtual void getCoefficient(std::vector<double>& v) const {}
    virtual int setCoefficient(const std::vector<double>& v, int idx = 0)
    {
      return idx;
    }
  };

//===========================================

  class Function2d
  {
  public:
    Function2d() {}
    virtual ~Function2d() {}

    virtual double operator()(double x, double y) const = 0;
    virtual double operator()(const Point& p) const
    {
      return (*this)(p.x, p.y);
    };

    virtual void getCoefficient(std::vector<double>& v) const {}

    virtual int setCoefficient(const std::vector<double>& v, int idx = 0)
    {
      return idx;
    }
  };

  class ImageFunction: public Function2d
  {
  public:

    ImageFunction(const Image& imgp, int im = INTERPOL): img(imgp), mode(im) { }

    using Function2d::operator();
    virtual double operator()(double x, double y) const override
    {
      switch (mode)
        {
        case DEFAULT:
          return GetValClipped(img, RoundInt(x), RoundInt(y));
        case INTERPOL:
          return GetInterpolVal(img, x, y);
        }
      return 0;
    }

    virtual void setMode(int m)
    {
      mode = m;
    }
    virtual int getMode() const
    {
      return mode;
    }

    virtual Image& getImage()
    {
      return img;
    }

    virtual const Image& getImage() const
    {
      return img;
    }
  protected:
    Image img;
    int mode;
  };

  class ImageDFunction: public Function2d
  {

  public:

    ImageDFunction(const ImageD& imgp, int im = INTERPOL): img(imgp), mode(im) { }
    using Function2d::operator();
    virtual double operator()(double x, double y) const override
    {
      if (Inside(img, (int)x, (int)y))
        {
          switch (mode)
            {
            case DEFAULT:
              return GetValD(img, RoundInt(x), RoundInt(y));

            case INTERPOL:
              return GetInterpolValD(img, x, y);
            }
        }

      return 0;
    }

    virtual void setMode(int m)
    {
      mode = m;
    }
    virtual int getMode() const
    {
      return mode;
    }

    virtual ImageD& getImageD()
    {
      return img;
    }

    virtual const ImageD& getImageD() const
    {
      return img;
    }

  protected:
    ImageD img;
    int mode;
  };

  void setImg(const Image& img, const Function2d& fn);
}
#endif
