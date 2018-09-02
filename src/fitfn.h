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
 * class functions with fitting
 *
 * 1d functions
 *         FunctionWithFitting (abstract)
 *         Constant
 *         Polynom1o
 *         Polynom2o
 *         Polynom
 * 2d functions
 *       Constant
 *       Polynom2d1o
 *       Polynom2d2o
 *       Polynom2d
 */
#ifndef FITFN_H
#define FITFN_H

#include "Vector.h"
#include "base.h"
#include "based.h"
#include "Point.h"
#include "function.h"
#include "fitgauss.h"

namespace ice
{
  class FunctionWithFitting : public Function
  {
  public:
    FunctionWithFitting() {}
    virtual ~FunctionWithFitting() {}

    // Methods for fitting
    virtual void FitInit() = 0;
    virtual void FitVal(double x, double g, double w = 1.0) = 0;
    virtual void FitFinish() = 0;

    virtual int Fit(const Matrix& m);
    virtual double Variance() const = 0;
  };

  class FunctionWithFittingG: public FunctionWithFitting
  {
  private:
    GaussFit fit;
  public:
    FunctionWithFittingG(): fit(1), co(1) {}
    FunctionWithFittingG(int n): fit(n), co(n) {}
    virtual ~FunctionWithFittingG() {};

    virtual void FitInit() override
    {
      fit.Init();
    };

    virtual void FitInit(int n)
    {
      fit.Init(n);
      co.resize(n);
    };

    using FunctionWithFitting::FitVal;
    virtual void FitVal(const ice::Vector& p, double v, double w)
    {
      fit.Value(p, v, w);
    };

    virtual void FitVal(const std::vector<double>& p, double v, double w)
    {
      fit.Value(p, v, w);
    };

    virtual void FitFinish() override
    {
      fit.Finish();
      fit.getResult(co);
    }

    virtual double Variance() const override
    {
      return fit.Variance();
    }

    using FunctionWithFitting::getCoefficient;
    virtual void getCoefficient(std::vector<double>& v)
    {
      v = co;
    }

    virtual int setCoefficient(const std::vector<double>& v, int idx = 0) override
    {
      for (unsigned int i = 0; i < co.size(); i++)
        {
          co[i] = v[idx++];
        }
      return idx;
    }

  protected:
    std::vector<double> co;
  };

  class Constant : public FunctionWithFittingG
  {
  public:
    Constant(): FunctionWithFittingG(1) {};
    Constant(double pa): FunctionWithFittingG(1)
    {
      co[0] = pa;
    }
    Constant(const std::vector<double>& v);
    Constant(const Constant& f): FunctionWithFittingG(1)
    {
      co = f.co;
    }

    double operator()(double x) const override
    {
      return co[0];
    }

    using FunctionWithFittingG::FitVal;
    virtual void FitVal(double x, double g, double w = 1.0) override;

    using FunctionWithFittingG::getCoefficient;
    void getCoefficient(double& pa) const
    {
      pa = co[0];
    }

    using FunctionWithFittingG::setCoefficient;
    void setCoefficient(double pa)
    {
      co[0] = pa;
    }
  };

  class Polynom1o : public FunctionWithFittingG
  {
  public:
    Polynom1o(): FunctionWithFittingG(2) {};
    Polynom1o(double a, double b): FunctionWithFittingG(2)
    {
      co[0] = a;
      co[1] = b;
    }
    Polynom1o(const std::vector<double>& v);
    Polynom1o(const Polynom1o& f): FunctionWithFittingG(2)
    {
      co = f.co;
    }
    virtual ~Polynom1o() {};

    using FunctionWithFittingG::FitVal;
    virtual void FitVal(double x, double g, double w = 1.0) override;

    double operator()(double x) const override
    {
      return x * co[1] + co[0];
    }

    using FunctionWithFittingG::getCoefficient;
    using FunctionWithFittingG::setCoefficient;

    void getCoefficient(double& pa, double& pb) const
    {
      pa = co[0];
      pb = co[1];
    }
    void setCoefficient(double pa, double pb)
    {
      co[0] = pa;
      co[1] = pb;
    }
  };

  class Polynom2o : public FunctionWithFittingG
  {
  public:
    Polynom2o(): FunctionWithFittingG(3) {};
    Polynom2o(double pa, double pb, double pc): FunctionWithFittingG(3)
    {
      co[0] = pa;
      co[1] = pb;
      co[2] = pc;
    };
    Polynom2o(const ice::Vector& v);
    Polynom2o(const Polynom2o& f): FunctionWithFittingG(3)
    {
      co = f.co;
    };

    virtual ~Polynom2o() {};

    using FunctionWithFittingG::FitVal;
    virtual void FitVal(double x, double g, double w = 1.0) override;

    double operator()(double x) const override
    {
      return (co[2] * x + co[1]) * x + co[0];
    }

    using FunctionWithFittingG::getCoefficient;
    int getCoefficient(double& pa, double& pb, double& pc) const;
  };

  class Polynom : public FunctionWithFittingG
  {
  public:
    Polynom(): FunctionWithFittingG(1), ord(0) {};
    Polynom(int n): FunctionWithFittingG(n + 1), ord(n) {};
    Polynom(const ice::Vector& v);
    Polynom(const Polynom1o& f);
    Polynom(const Polynom2o& f);
    Polynom(const Polynom& f);
    virtual ~Polynom() {};
    using FunctionWithFittingG::FitVal;
    virtual void FitVal(double x, double g, double w = 1.0) override;
    virtual void FitInit(int n) override;
    virtual void FitInit() override;
    double operator()(double x) const override;
  protected:
    int ord;
  };

//===========================================

  class Function2dWithFitting : public Function2d
  {
  public:
    Function2dWithFitting() {}
    virtual ~Function2dWithFitting() {}
    virtual void FitInit() = 0;

    // fitting
    virtual void FitVal(double x, double y, double g, double w = 1.0) = 0;
    virtual void FitVal(const Point& p, double g, double w = 1.0)
    {
      FitVal(p.x, p.y, g, w);
    };
    virtual void FitFinish() = 0;
    virtual int Fit(const Matrix& m);
    virtual int Fit(const Image& img);
    virtual int Fit(ImageD img);
    virtual double Variance() const = 0 ;
  };

  class Function2dWithFittingG : public Function2dWithFitting
  {
  public:
    Function2dWithFittingG(): fit(1), co(1) {}
    Function2dWithFittingG(int n): fit(n), co(n) {}
    virtual ~Function2dWithFittingG() {};

    virtual void FitInit() override
    {
      fit.Init();
      co.clear();
    }

    virtual void FitInit(int n)
    {
      fit.Init(n);
      co.resize(n);
    };

    using Function2dWithFitting::FitVal;
    virtual void FitVal(const ice::Vector& p, double v, double w)
    {
      fit.Value(p, v, w);
    };

    virtual void FitFinish() override
    {
      fit.Finish();
      fit.getResult(co);
    }

    using Function2dWithFitting::getCoefficient;
    virtual void getCoefficient(std::vector<double>& v)
    {
      v = co;
    }

    using Function2dWithFitting::setCoefficient;
    virtual int setCoefficient(const std::vector<double>& v, int idx = 0) override
    {
      for (unsigned int i = 0; i < co.size(); i++)
        {
          co[i] = v[idx++];
        }
      return idx;
    }

    virtual double Variance() const override
    {
      return fit.Variance();
    }
  protected:
    GaussFit fit;
  protected:
    std::vector<double> co;
  };

  class Constant2d : public Function2dWithFittingG
  {
  public:
    Constant2d(): Function2dWithFittingG(1) {}
    Constant2d(double a): Function2dWithFittingG(1)
    {
      co[0] = a;
    }
    Constant2d(const ice::Vector& v);
    Constant2d(const Constant2d& f): Function2dWithFittingG(1)
    {
      co = f.co;
    }

    using Function2dWithFittingG::FitVal;
    void FitVal(double x, double y, double g, double w = 1.0) override;

    double operator()(double x, double y) const override;
    using Function2dWithFittingG::getCoefficient;
    int getCoefficient(double& pa) const;
  };

  class Polynom2d1o : public Function2dWithFittingG
  {
  public:
    Polynom2d1o(): Function2dWithFittingG(3) {};
    Polynom2d1o(double pa, double pb, double pc): Function2dWithFittingG(3)
    {
      co[0] = pa;
      co[1] = pb;
      co[2] = pc;
    }
    Polynom2d1o(const ice::Vector& v);
    Polynom2d1o(const Polynom2d1o& f): Function2dWithFittingG(3)
    {
      co = f.co;
    };
    virtual ~Polynom2d1o() {};

    using Function2dWithFittingG::FitVal;
    virtual void FitVal(double x, double y, double g, double w = 1.0) override;

    virtual double operator()(double x, double y) const override;

    using Function2dWithFittingG::getCoefficient;
    virtual int getCoefficient(double& pa, double& pb, double& pc) const;

  };

  class Polynom2d2o : public Function2dWithFittingG
  {
  public:
    Polynom2d2o(): Function2dWithFittingG(6) {};
    Polynom2d2o(double a, double b, double c, double d, double e, double f):
      Function2dWithFittingG(6)
    {
      co[0] = a;
      co[1] = b;
      co[2] = c;
      co[3] = d;
      co[4] = e;
      co[5] = f;
    };
    Polynom2d2o(const ice::Vector& v);
    Polynom2d2o(const Polynom2d2o& f): Function2dWithFittingG(6)
    {
      co = f.co;
    }
    virtual ~Polynom2d2o() {};

    using Function2dWithFittingG::FitVal;
    void FitVal(double x, double y, double g, double w = 1.0) override;
    double operator()(double x, double y) const override;

    using Function2dWithFittingG::getCoefficient;
    int getCoefficient(double& pa, double& pb, double& pc,
                       double& pd, double& pe, double& pf) const;
  };

  class Polynom2d : public Function2dWithFittingG
  {
  public:
    Polynom2d();
    Polynom2d(int n);
    Polynom2d(const ice::Vector& v);
    Polynom2d(const Polynom2d& f);
    virtual ~Polynom2d() {} ;

    virtual void FitInit(int n) override;
    virtual void FitInit() override;

    using Function2dWithFittingG::FitVal;
    virtual void FitVal(double x, double y, double g, double w = 1.0) override;

    virtual double operator()(double x, double y) const override;

  protected:
    int ord;
    int vsize;
  };
}

#endif
