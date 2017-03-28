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
/************************************************/
/*            Filter                            */
/************************************************/
#ifndef _LSIFILTER_H
#define _LSIFILTER_H

#include <iostream>

#include "matrixtemplate.h"
#include "base.h"
#include "Matrix.h"

namespace ice
{
  class LsiRepresentation
  {
  public:
    LsiRepresentation(int nx, int ny);
    virtual ~LsiRepresentation() {}

    int getdimx() const
    {
      return dimx;
    }

    int getdimy() const
    {
      return dimy;
    }

    int getdx() const
    {
      return (dimx - 1) / 2;
    }
    int getdy() const
    {
      return (dimy - 1) / 2;
    }

    int proposeoffset(const Image& img) const
    {
      double p, m;
      sumPlusSumMinus(p, m);
      double range = p - m;
      return RoundInt(-m / range * img.maxval);
    }

    virtual void sumPlusSumMinus(double& sump, double& summ) const = 0;
    virtual void normalize() = 0;

    virtual int Filter(const Image& src, const Image& dst, int offset) const = 0;
    virtual int Filter(const Image& src, ImageD dst) const = 0;
    virtual int Filter(ImageD src, ImageD dst) const = 0;

    virtual double getMask(int i, int j) const = 0;
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

  class LsiRepresentationI: public LsiRepresentation
  {
  public:
    LsiRepresentationI(const LsiRepresentationI& r);
    LsiRepresentationI(const IMatrix& m, int normp);
    LsiRepresentationI(const matrix<int>& m, int normp);
    LsiRepresentationI(int* m, int sizex, int sizey, int normp);

    virtual void sumPlusSumMinus(double& sump, double& summ) const;
    virtual LsiRepresentationI normalized(int max) const;

    virtual void negateMask();
    virtual void normalize();
    virtual void reflectMask();

    virtual int Filter(const Image& src, const Image& dst, int offset) const;
    virtual int Filter(const Image& src, ImageD dst) const;
    virtual int Filter(ImageD src, ImageD dst) const;

    virtual ~LsiRepresentationI()
    {
      delete [] mask;
    }

    // several forms of access to the filter mask
    virtual const int* getMask() const
    {
      return mask;
    }

    virtual void getMask(matrix<double>& m) const;

    virtual double getMask(int x, int y) const
    {
      return static_cast<double>(mask[idx(x, y)]) / norm;
    };

    virtual int getNorm() const
    {
      return norm;
    }
  protected:
    int* mask;
    int norm;

    int ggt(int a, int b) const;
  };

  class LsiRepresentationD: public LsiRepresentation
  {
  public:

    LsiRepresentationD(const LsiRepresentationD& r);
    LsiRepresentationD(const Matrix& m);
    LsiRepresentationD(const matrix<double>& m);

    virtual void sumPlusSumMinus(double& sump, double& summ) const;

    virtual void negateMask();
    virtual void normalize();
    virtual void reflectMask();

    virtual int Filter(const Image& src, const Image& dst, int offset) const;
    virtual int Filter(const Image& src, ImageD dst) const;
    virtual int Filter(ImageD src, ImageD dst) const;

    virtual ~LsiRepresentationD()
    {
      delete [] mask;
    }

    // several forms of access to the filter mask
    virtual double getMask(int x, int y) const
    {
      return mask[idx(x, y)];
    };

    virtual const double* getMask() const
    {
      return mask;
    }

    virtual void getMask(matrix<double>& m) const;

  protected:
    double* mask;
  };


  class LSIFilter
  {
  public:
    typedef enum {it_int, it_double} impl_type;

    impl_type typ;
    LsiRepresentation* rep;

    LSIFilter(): typ(it_int), rep(nullptr) {}
    LSIFilter(const LSIFilter& f): typ(f.typ)
    {
      if (f.rep != nullptr)
        {
          switch (typ)
            {
            case it_int:
              rep = new LsiRepresentationI(*(LsiRepresentationI*)f.rep);
              break;
            case it_double:
              rep = new LsiRepresentationD(*(LsiRepresentationD*)f.rep);
              break;
            }
        }
      else
        rep = nullptr;
    }

    explicit LSIFilter(const Matrix& m, bool convolution = false): typ(it_double)
    {
      rep = new LsiRepresentationD(m);
      if (convolution)
        rep->reflectMask();
    }

    LSIFilter(const IMatrix& m, int norm, bool convolution = false): typ(it_int)
    {
      rep = new LsiRepresentationI(m, norm);
      if (convolution)
        rep->reflectMask();
    }

    explicit LSIFilter(const matrix<double>& m, bool convolution = false): typ(it_double)
    {
      rep = new LsiRepresentationD(m);
      if (convolution)
        rep->reflectMask();
    }

    LSIFilter(const matrix<int>& m, int norm, bool convolution = false): typ(it_int)
    {
      rep = new LsiRepresentationI(m, norm);
      if (convolution)
        rep->reflectMask();
    }

    LSIFilter(int* m, int sizex, int sizey, int norm, bool convolution = false): typ(it_int)
    {
      rep = new LsiRepresentationI(m, sizex, sizey, norm);
      if (convolution)
        rep->reflectMask();
    }

    ~LSIFilter()
    {
      delete rep;
    }

    virtual int Filter(const Image& src, const Image& dst, int offset/*=0*/) const
    {
      return rep->Filter(src, dst, offset);
    }
    virtual int Filter(const Image& src, ImageD dst) const
    {
      return rep->Filter(src, dst);
    }
    virtual int Filter(ImageD src, ImageD dst) const
    {
      return rep->Filter(src, dst);
    }

    virtual double getMask(int x, int y) const
    {
      return rep->getMask(x, y);
    }

    virtual void getMask(matrix<double>& m) const
    {
      rep->getMask(m);
    }

    virtual void NegateMask()
    {
      rep->negateMask();
    }

    virtual LSIFilter getInverse(int sizex, int sizey) const;
    virtual LSIFilter getInverse(int size) const
    {
      return getInverse(size, size);
    }

    virtual double proposeOffset(const Image& img) const
    {
      return rep->proposeoffset(img);
    }

    virtual LSIFilter& operator=(const LSIFilter& f);
  };

  std::ostream& operator<<(std::ostream& os, const LSIFilter& f);

  // basic implementation functions - lsifilter0.cpp
  // not for public use, no parameter check
  int lsiimg(const Image& src, const Image& dest,
             int nx, int ny, int* mask, int norm, int off);
  int lsiimg(const Image& src, const Image& dest,
             int nx, int ny, double* mask, int off);

  int lsiimg(const Image& src, ImageD dest,
             int nx, int ny, int* mask, int norm);
  int lsiimg(const Image& src, ImageD dest,
             int nx, int ny, double* mask);

  int lsiimg(ImageD src, ImageD dest, int nx, int ny,
             int* mask, int norm);
  int lsiimg(ImageD src, ImageD dest,
             int nx, int ny, double* mask);

  int lsiimgcyc(const Image& src, const Image& dest,
                int nx, int ny, int* mask, int norm, int off);
  int lsiimgcyc(const Image& src, const Image& dest,
                int nx, int ny, double* mask, int off);

  // second level function - with parameter check
  // use not recommended
  int LSIImg(const Image& src, const Image& dst,
             int nx, int ny, int* mask, int norm, int off);
  int LSIImg(const Image& src, const Image& dst,
             int nx, int ny, double* mask, int off);

  int LSIImg(const Image& src, ImageD dst,
             int nx, int ny, int* mask, int norm = 1);
  int LSIImg(const Image& src, ImageD dst,
             int nx, int ny, double* mask);

  int LSIImg(ImageD src, ImageD dst,
             int nx, int ny, int* mask, int norm = 1);
  int LSIImg(ImageD src, ImageD dst,
             int nx, int ny, double* mask);

  // different parameter order (for compatibility)
  int LSIImg(const Image& imgs,
             int nx, int ny, int* mask, int norm,
             int off, const Image& imgd);
  int LSIImg(const Image& imgs,
             int nx, int ny, double* mask,
             int off, const Image& imgd);

// variants with "cyclic" interpretation of values (like hue or angles)
  int LSIImgCyc(const Image&, const Image&,
                int nx, int ny, int* mask, int norm, int off);
  int LSIImgCyc(const Image&, const Image&,
                int nx, int ny, double* mask, int off);

  // special access to pixel value through filter
  int GetVal(const Image& img, int x, int y, const LSIFilter& f);
  int GetVal(const Image& img, IPoint p, const LSIFilter& f);

  // Filtering with mask given as matrix
  int LSIImg(const Image&, const Image&,
             const IMatrix& m, int norm, int off);
  int LSIImg(const Image&, const Image&,
             const Matrix& m, int off);
  // different parameter order (for compatibility)
  int LSIImg(const Image& imgs, const IMatrix& m, int norm, int off, const Image& imgd);
  int LSIImg(const Image& imgs, const Matrix& m, int off, const Image& imgd);

  // !! Preferred filter function with LSIFilter class parameter
  // equivalent to f.Filter(src,dest)
  int LSIImg(const Image& src, const Image& dest,
             const LSIFilter& f, int offset);

  // filtering for double images
  int LSIImg(ImageD imgs, ImageD imgd,
             const Matrix& m);

  // special implementations for row or column vector
  int LSIHImg(const Image& pn1, const Image& pn2, const std::vector<int>& mask,
              int norm, int offset/*=0*/);
  int LSIHImg(const Image& pn1, const Image& pn2, const IVector& mask,
              int norm, int offset/*=0*/);
  int LSIVImg(const Image& pn1, const Image& pn2, const std::vector<int>& mask,
              int norm, int offset/*=0*/);
  int LSIVImg(const Image& pn1, const Image& pn2, const IVector& mask,
              int norm, int offset/*=0*/);

// functions for generation of LSIFilters
  LSIFilter mkOrientedSmearFilter(int n, double dir, double len, double width);
  LSIFilter mkOrientedDoBFilter(int n, double dir, double len, double width);
  LSIFilter mkOrientedEdgeFilter(int n, double dir, double rad);
  LSIFilter mkPolynomFilter(int size, int grad, int ii, int jj);
}
#endif
