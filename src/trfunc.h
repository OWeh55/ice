#ifndef TR_FUNC_H
#define TR_FUNC_H
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
 * classes for transformations of 2d functions
 * here: Function modifier = functions that get the result of a base function and modify the result
 * f(f(f(x,y)))
 */
#include "function.h"
//=============================================================================
namespace ice
{
  class Function2dModifier : public Function2d
  {
  protected:
    Function2d& basefunc;

  public:
    Function2dModifier(Function2d& funcp): basefunc(funcp) { }

    virtual double operator()(double x, double y) const  override = 0;

    virtual void getCoefficient(vector<double>& v) const override
    {
      Function2d::getCoefficient(v); // base class
      basefunc.getCoefficient(v); // source function
      // no own coefficients here
    }

    virtual int setCoefficient(const vector<double>& v, int idx = 0) override
    {
      idx = Function2d::setCoefficient(v, idx); // base class
      idx = basefunc.setCoefficient(v, idx); // source function
      // no own coefficients here
      return idx;
    }
  };

  class Function2dParameterShift : public Function2dModifier
  {
  private:
    double shiftx, shifty;

  public:
    Function2dParameterShift(Function2d& funcp, double sx, double sy):
      Function2dModifier(funcp), shiftx(sx), shifty(sy) { }

    virtual double operator()(double x, double y) const override
    {
      return basefunc(x - shiftx, y - shifty);
    }

    virtual void getCoefficient(vector<double>& v) const override
    {
      Function2dModifier::getCoefficient(v); // base class (+source function)
      v.push_back(shiftx);
      v.push_back(shifty);
    }

    virtual int setCoefficient(const vector<double>& v, int idx = 0) override
    {
      idx = Function2dModifier::setCoefficient(v, idx); // base class (+source function)

      shiftx = v[idx];
      shifty = v[idx + 1];

      return idx + 2;
    }
  };

  class Function2dParameterTransform : public Function2dModifier
  {
  private:
    Trafo tr;

    mutable bool modified;
    mutable Trafo inv;

    void makeInverse() const
    {
      if (modified)
        {
          inv = tr.inverse();
          modified = false;
        }
    }

  public:
    Function2dParameterTransform(Function2d& funcp, const Trafo& trp): Function2dModifier(funcp), tr(trp), modified(true) { }

    virtual Trafo& Tr()
    {
      modified = true;
      return tr;
    }

    virtual const Trafo& Tr() const
    {
      return tr;
    }

    virtual double operator()(double x, double y) const override
    {
      makeInverse();
      double xt, yt;
      transform(inv, x, y, xt, yt);
      return basefunc(xt, yt);
    }

    virtual void getCoefficient(vector<double>& v) const override
    {
      Function2dModifier::getCoefficient(v);

      const matrix<double>& tm = tr.getMatrix();

      for (int i = 0; i < 2; i++)
        for (int k = 0; k < 3; k++)
          {
            v.push_back(tm[i][k]);
          }

      for (int k = 0; k < 2; k++)
        {
          v.push_back(tm[2][k]);
        }
    };

    virtual int setCoefficient(const vector<double>& v, int idx = 0) override
    {
      idx = Function2dModifier::setCoefficient(v, idx);

      Matrix m(3, 3);

      for (int i = 0; i < 2; i++)
        for (int k = 0; k < 3; k++)
          {
            m[i][k] = v[idx++];
          }

      for (int k = 0; k < 2; k++)
        {
          m[2][k] = v[idx++];
        }

      m[2][2] = 1.0;

      tr = m;

      // for (int i = 0; i < 3; i++)
      //  {
      //   for (int k = 0; k < 3; k++)
      //    cout << tr.Tmatrix()[i][k] << " ";
      //  cout << endl;
      //  }

      modified = true;

      return idx;
    }
  };

// ---------------
  class Function2dValueTransform : public Function2dModifier
  {
  private:
    double a1, a0;

  public:
    Function2dValueTransform(Function2d& funcp, double a1p, double a0p):
      Function2dModifier(funcp), a1(a1p), a0(a0p) { }

    virtual void setPara(double a1p, double a0p)
    {
      a1 = a1p;
      a0 = a0p;
    }
    virtual void getPara(double& a1p, double& a0p) const
    {
      a1p = a1;
      a0p = a0;
    }

    virtual double operator()(double x, double y) const override
    {
      return a1 * basefunc(x, y) + a0;
    }

    virtual void getCoefficient(vector<double>& v) const override
    {
      Function2dModifier::getCoefficient(v);

      v.push_back(a1);
      v.push_back(a0);
    };

    virtual int setCoefficient(const vector<double>& v, int idx = 0) override
    {
      idx = Function2dModifier::setCoefficient(v, idx);

      a1 = v[idx++];
      a0 = v[idx++];
      return idx;
    }
  };
// -------------------------
  class Function2dDistortion : public Function2dModifier
  {
  private:
    Distortion* dist;

  public:
    Function2dDistortion(Function2d& funcp, Distortion& d):
      Function2dModifier(funcp), dist(&d) { }

    virtual void setDistortion(Distortion& d)
    {
      dist = &d;
    }

    virtual double operator()(double x, double y) const override
    {
      double xt, yt;
      dist->rectify(x, y, xt, yt);
      return basefunc(xt, yt);
    }

    virtual void getCoefficient(vector<double>& v) const override
    {
      Function2dModifier::getCoefficient(v);
      Vector dv = dist->makeVector();
      v.push_back(dv.Size());
      for (int i = 0; i < dv.Size(); i++)
        {
          v.push_back(dv[i]);
        }
    }

    virtual int setCoefficient(const vector<double>& v, int idx = 0) override
    {
      idx = Function2dModifier::setCoefficient(v, idx);
      Vector dv(v[idx++]);
      for (int i = 0; i < dv.Size(); i++)
        {
          dv[i] = v[idx++];
        }
      dist->set(dv);
      return idx;
    }
  };
}
#endif
